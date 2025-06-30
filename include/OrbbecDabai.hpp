/**
 * @file OrbbecDabai.hpp
 * @author Your Name
 * @brief Orbbec DaBai相机库
 * @version 0.1
 * @date 2025-01-15
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef ORBBEC_DABAI_HPP
#define ORBBEC_DABAI_HPP

#include <libobsensor/ObSensor.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <memory>

class OrbbecDabai
{
public:
    OrbbecDabai();
    ~OrbbecDabai();

    /**
     * @brief 初始化相机
     */
    void init();

    /**
     * @brief 设置相机参数
     */
    void setCamera();

    /**
     * @brief 关闭相机
     */
    void close();

    /**
     * @brief 获取图像 (彩色、深度、红外)
     *
     * @return std::vector<cv::Mat> [0]彩色图像 [1]深度图像 [2]红外图像
     */
    std::vector<cv::Mat> getImg();

    /**
     * @brief 获取彩色图像
     *
     * @return cv::Mat 彩色图像
     */
    cv::Mat getColorImg();

    /**
     * @brief 获取深度图像
     *
     * @return cv::Mat 深度图像
     */
    cv::Mat getDepthImg();

    /**
     * @brief 获取红外图像
     *
     * @return cv::Mat 红外图像
     */
    cv::Mat getIRImg();

    /**
     * @brief 获取指定像素点的深度值
     *
     * @param x 像素x坐标
     * @param y 像素y坐标
     * @return float 深度值(米)，0表示无效深度
     */
    float getDepthAt(int x, int y);

    /**
     * @brief 获取对齐的彩色图像和深度图像
     *
     * @param colorImg 输出的彩色图像
     * @param depthImg 输出的深度图像
     */
    void getAlignedImages(cv::Mat &colorImg, cv::Mat &depthImg);

private:
    // Orbbec SDK相关对象
    ob::Context ctx;
    std::shared_ptr<ob::Pipeline> pipeline;
    std::shared_ptr<ob::Device> device;
    std::shared_ptr<ob::Config> config;

    // 格式转换器
    ob::FormatConvertFilter formatConverter;

    // 状态标志
    bool isInitialized;
    bool isRunning;

    // 图像参数
    int colorWidth;
    int colorHeight;
    int depthWidth;
    int depthHeight;

    // 深度缩放因子
    float depthScale;

    // 最新的帧集
    std::shared_ptr<ob::FrameSet> currentFrameset;

    /**
     * @brief 获取最新帧集
     *
     * @param timeout_ms 超时时间(毫秒)
     * @return bool 是否成功获取
     */
    bool updateFrameset(uint32_t timeout_ms = 1000);

    /**
     * @brief 转换颜色帧格式为BGR
     *
     * @param colorFrame 原始颜色帧
     * @return std::shared_ptr<ob::ColorFrame> BGR格式的颜色帧
     */
    std::shared_ptr<ob::ColorFrame> convertColorToBGR(std::shared_ptr<ob::ColorFrame> colorFrame);
};

#endif // ORBBEC_DABAI_HPP