/**
 * @file OrbbecDabai.cpp
 * @author Your Name
 * @brief Orbbec DaBai相机库实现
 * @version 0.1
 * @date 2025-01-15
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "OrbbecDabai.hpp"
#include <iostream>

/**
 * @brief 构造函数
 */
OrbbecDabai::OrbbecDabai()
    : isInitialized(false), isRunning(false), depthScale(0.001f),
      colorWidth(1280), colorHeight(720), depthWidth(640), depthHeight(480)
{
}

/**
 * @brief 析构函数
 */
OrbbecDabai::~OrbbecDabai()
{
    close();
}

/**
 * @brief 初始化相机
 */
void OrbbecDabai::init()
{
    try
    {
        // 查询设备数量
        auto deviceList = ctx.queryDeviceList();
        uint32_t deviceCount = deviceList->deviceCount();

        if (deviceCount == 0)
        {
            std::cerr << "No Orbbec device found!" << std::endl;
            return;
        }

        // 获取第一个设备
        device = deviceList->getDevice(0);
        auto deviceInfo = device->getDeviceInfo();

        // 创建pipeline
        pipeline = std::make_shared<ob::Pipeline>(device);
        config = std::make_shared<ob::Config>();

        // 配置彩色流
        auto colorProfiles = pipeline->getStreamProfileList(OB_SENSOR_COLOR);
        if (colorProfiles)
        {
            auto colorProfile = colorProfiles->getVideoStreamProfile(colorWidth, colorHeight, OB_FORMAT_ANY, 30);
            if (!colorProfile)
            {
                // 如果指定分辨率不支持，使用默认配置
                auto profile = colorProfiles->getProfile(OB_PROFILE_DEFAULT);
                colorProfile = profile->as<ob::VideoStreamProfile>();
            }
            config->enableStream(colorProfile);
            colorWidth = colorProfile->width();
            colorHeight = colorProfile->height();
        }

        // 配置深度流
        auto depthProfiles = pipeline->getStreamProfileList(OB_SENSOR_DEPTH);
        if (depthProfiles)
        {
            auto depthProfile = depthProfiles->getVideoStreamProfile(depthWidth, depthHeight, OB_FORMAT_ANY, 30);
            if (!depthProfile)
            {
                depthProfile = depthProfiles->getVideoStreamProfile();
            }
            config->enableStream(depthProfile);
            depthWidth = depthProfile->width();
            depthHeight = depthProfile->height();
        }

        // 配置红外流
        auto irProfiles = pipeline->getStreamProfileList(OB_SENSOR_IR);
        if (irProfiles)
        {
            auto irProfile = irProfiles->getVideoStreamProfile();
            config->enableStream(irProfile);
        }

        // 设置对齐模式（深度对齐到彩色）
        config->setAlignMode(ALIGN_D2C_SW_MODE);

        // 启动pipeline
        pipeline->start(config);
        isRunning = true;

        // 获取深度缩放因子
        // try
        // {
        //     depthScale = 0.001f; // 默认1mm
        // }
        // catch(...)
        // {
        //     depthScale = 0.001f; // 默认1mm
        //     std::cerr << "Using default depth scale: 0.001" << std::endl;
        // }
        depthScale = 0.001f;

            // 过滤前几帧以稳定数据
            for (int i = 0; i < 5; i++)
        {
            auto frameset = pipeline->waitForFrames(1000);
            if (!frameset)
            {
                std::cerr << "Failed to get initial frames" << std::endl;
                break;
            }
        }

        isInitialized = true;

        // 输出初始化信息
        std::cout << "Orbbec DaBai Camera: " << deviceInfo->name() << " Initialize success!" << std::endl;
        std::cout << "SN: " << deviceInfo->serialNumber() << std::endl;
        std::cout << "Color Resolution: " << colorWidth << "x" << colorHeight << std::endl;
        std::cout << "Depth Resolution: " << depthWidth << "x" << depthHeight << std::endl;
        std::cout << "Depth Scale: " << depthScale << std::endl;
    }
    catch (const ob::Error &e)
    {
        std::cerr << "Orbbec error: " << e.getMessage() << std::endl;
        isInitialized = false;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Standard exception: " << e.what() << std::endl;
        isInitialized = false;
    }
}

/**
 * @brief 设置相机参数
 */
void OrbbecDabai::setCamera()
{
    if (!isInitialized || !device)
    {
        std::cerr << "Camera not initialized!" << std::endl;
        return;
    }

    try
    {
        // 设置彩色相机参数
        device->setBoolProperty(OB_PROP_COLOR_MIRROR_BOOL, false);            // 关闭镜像
        device->setBoolProperty(OB_PROP_COLOR_AUTO_EXPOSURE_BOOL, true);      // 开启自动曝光
        device->setBoolProperty(OB_PROP_COLOR_AUTO_WHITE_BALANCE_BOOL, true); // 开启自动白平衡

        // 设置图像质量参数
        try
        {
            device->setIntProperty(OB_PROP_COLOR_BRIGHTNESS_INT, 0);  // 亮度
            device->setIntProperty(OB_PROP_COLOR_SHARPNESS_INT, 50);  // 锐度
            device->setIntProperty(OB_PROP_COLOR_CONTRAST_INT, 50);   // 对比度
            device->setIntProperty(OB_PROP_COLOR_SATURATION_INT, 50); // 饱和度
            device->setIntProperty(OB_PROP_COLOR_GAIN_INT, 100);      // 增益
        }
        catch (...)
        {
            // 某些参数可能不支持，忽略错误
        }

        // 设置深度相机参数
        device->setBoolProperty(OB_PROP_DEPTH_MIRROR_BOOL, false); // 关闭镜像

        // 设置红外相机参数
        device->setBoolProperty(OB_PROP_IR_MIRROR_BOOL, false); // 关闭镜像

        std::cout << "Camera parameters set successfully!" << std::endl;
    }
    catch (const ob::Error &e)
    {
        std::cerr << "Error setting camera parameters: " << e.getMessage() << std::endl;
    }
}

/**
 * @brief 关闭相机
 */
void OrbbecDabai::close()
{
    if (isRunning && pipeline)
    {
        try
        {
            pipeline->stop();
            isRunning = false;
            std::cout << "Camera closed successfully!" << std::endl;
        }
        catch (const ob::Error &e)
        {
            std::cerr << "Error closing camera: " << e.getMessage() << std::endl;
        }
    }
    isInitialized = false;
}

/**
 * @brief 获取最新帧集
 */
bool OrbbecDabai::updateFrameset(uint32_t timeout_ms)
{
    if (!isInitialized || !isRunning)
    {
        return false;
    }

    try
    {
        currentFrameset = pipeline->waitForFrames(timeout_ms);
        return currentFrameset != nullptr;
    }
    catch (const ob::Error &e)
    {
        std::cerr << "Error getting frameset: " << e.getMessage() << std::endl;
        return false;
    }
}

/**
 * @brief 转换颜色帧格式为BGR
 */
std::shared_ptr<ob::ColorFrame> OrbbecDabai::convertColorToBGR(std::shared_ptr<ob::ColorFrame> colorFrame)
{
    if (!colorFrame)
        return nullptr;

    // 首先转换为RGB
    if (colorFrame->format() != OB_FORMAT_RGB)
    {
        if (colorFrame->format() == OB_FORMAT_MJPG)
        {
            formatConverter.setFormatConvertType(FORMAT_MJPG_TO_RGB888);
        }
        else if (colorFrame->format() == OB_FORMAT_UYVY)
        {
            formatConverter.setFormatConvertType(FORMAT_UYVY_TO_RGB888);
        }
        else if (colorFrame->format() == OB_FORMAT_YUYV)
        {
            formatConverter.setFormatConvertType(FORMAT_YUYV_TO_RGB888);
        }
        else
        {
            return colorFrame; // 不支持的格式，直接返回
        }
        colorFrame = formatConverter.process(colorFrame)->as<ob::ColorFrame>();
    }

    // 然后转换为BGR
    formatConverter.setFormatConvertType(FORMAT_RGB888_TO_BGR);
    return formatConverter.process(colorFrame)->as<ob::ColorFrame>();
}

/**
 * @brief 获取图像 (彩色、深度、红外)
 */
std::vector<cv::Mat> OrbbecDabai::getImg()
{
    std::vector<cv::Mat> images;

    if (!updateFrameset())
    {
        return images; // 返回空vector
    }

    try
    {
        // 获取彩色图像
        auto colorFrame = currentFrameset->colorFrame();
        if (colorFrame)
        {
            colorFrame = convertColorToBGR(colorFrame);
            cv::Mat colorMat(colorFrame->height(), colorFrame->width(), CV_8UC3, colorFrame->data());
            images.push_back(colorMat.clone());
        }
        else
        {
            images.push_back(cv::Mat());
        }

        // 获取深度图像
        auto depthFrame = currentFrameset->depthFrame();
        if (depthFrame)
        {
            cv::Mat depthMat(depthFrame->height(), depthFrame->width(), CV_16UC1, depthFrame->data());
            images.push_back(depthMat.clone());
        }
        else
        {
            images.push_back(cv::Mat());
        }

        // 获取红外图像
        auto irFrame = currentFrameset->irFrame();
        if (irFrame)
        {
            cv::Mat irMat(irFrame->height(), irFrame->width(), CV_16UC1, irFrame->data());
            images.push_back(irMat.clone());
        }
        else
        {
            images.push_back(cv::Mat());
        }
    }
    catch (const ob::Error &e)
    {
        std::cerr << "Error getting images: " << e.getMessage() << std::endl;
    }

    return images;
}

/**
 * @brief 获取彩色图像
 */
cv::Mat OrbbecDabai::getColorImg()
{
    if (!updateFrameset())
    {
        return cv::Mat();
    }

    try
    {
        auto colorFrame = currentFrameset->colorFrame();
        if (colorFrame)
        {
            colorFrame = convertColorToBGR(colorFrame);
            cv::Mat colorMat(colorFrame->height(), colorFrame->width(), CV_8UC3, colorFrame->data());
            return colorMat.clone();
        }
    }
    catch (const ob::Error &e)
    {
        std::cerr << "Error getting color image: " << e.getMessage() << std::endl;
    }

    return cv::Mat();
}

/**
 * @brief 获取深度图像
 */
cv::Mat OrbbecDabai::getDepthImg()
{
    if (!updateFrameset())
    {
        return cv::Mat();
    }

    try
    {
        auto depthFrame = currentFrameset->depthFrame();
        if (depthFrame)
        {
            cv::Mat depthMat(depthFrame->height(), depthFrame->width(), CV_16UC1, depthFrame->data());
            return depthMat.clone();
        }
    }
    catch (const ob::Error &e)
    {
        std::cerr << "Error getting depth image: " << e.getMessage() << std::endl;
    }

    return cv::Mat();
}

/**
 * @brief 获取红外图像
 */
cv::Mat OrbbecDabai::getIRImg()
{
    if (!updateFrameset())
    {
        return cv::Mat();
    }

    try
    {
        auto irFrame = currentFrameset->irFrame();
        if (irFrame)
        {
            cv::Mat irMat(irFrame->height(), irFrame->width(), CV_16UC1, irFrame->data());
            return irMat.clone();
        }
    }
    catch (const ob::Error &e)
    {
        std::cerr << "Error getting IR image: " << e.getMessage() << std::endl;
    }

    return cv::Mat();
}

/**
 * @brief 获取指定像素点的深度值
 */
float OrbbecDabai::getDepthAt(int x, int y)
{
    if (!updateFrameset())
    {
        return 0.0f;
    }

    try
    {
        auto depthFrame = currentFrameset->depthFrame();
        if (depthFrame)
        {
            int width = depthFrame->width();
            int height = depthFrame->height();

            if (x >= 0 && x < width && y >= 0 && y < height)
            {
                uint16_t *depthData = (uint16_t *)depthFrame->data();
                uint16_t depthValue = depthData[y * width + x];
                return depthValue * depthScale; // 转换为米
            }
        }
    }
    catch (const ob::Error &e)
    {
        std::cerr << "Error getting depth value: " << e.getMessage() << std::endl;
    }

    return 0.0f; // 无效深度
}

/**
 * @brief 获取对齐的彩色图像和深度图像
 */
void OrbbecDabai::getAlignedImages(cv::Mat &colorImg, cv::Mat &depthImg)
{
    if (!updateFrameset())
    {
        colorImg = cv::Mat();
        depthImg = cv::Mat();
        return;
    }

    try
    {
        // 获取彩色图像
        auto colorFrame = currentFrameset->colorFrame();
        if (colorFrame)
        {
            colorFrame = convertColorToBGR(colorFrame);
            cv::Mat colorMat(colorFrame->height(), colorFrame->width(), CV_8UC3, colorFrame->data());
            colorImg = colorMat.clone();
        }
        else
        {
            colorImg = cv::Mat();
        }

        // 获取深度图像 (已经对齐到彩色图像)
        auto depthFrame = currentFrameset->depthFrame();
        if (depthFrame)
        {
            cv::Mat depthMat(depthFrame->height(), depthFrame->width(), CV_16UC1, depthFrame->data());
            depthImg = depthMat.clone();
        }
        else
        {
            depthImg = cv::Mat();
        }
    }
    catch (const ob::Error &e)
    {
        std::cerr << "Error getting aligned images: " << e.getMessage() << std::endl;
        colorImg = cv::Mat();
        depthImg = cv::Mat();
    }
}