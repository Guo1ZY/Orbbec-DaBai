/**
 * @file OrbbecDabai.cpp
 * @author Guo1ZY 132872017@qq.com
 * @brief Orbbec DaBai相机实现
 * @version 0.1
 * @date 2025-01-15
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "OrbbecDabai.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <sys/time.h>

int main()
{
    std::cout << "=== Orbbec DaBai Camera Test ===" << std::endl;

    OrbbecDabai camera;

    // 初始化相机
    std::cout << "Initializing camera..." << std::endl;
    camera.init();
    camera.setCamera();

    // 时间测量变量
    timeval tt1, tt2;

    std::cout << "\nCamera controls:" << std::endl;
    std::cout << "  ESC - Exit program" << std::endl;
    std::cout << "  Space - Save current images" << std::endl;
    std::cout << "  'd' - Show center depth value" << std::endl;
    std::cout << "  'i' - Show camera info" << std::endl;
    std::cout << "\nStarting camera loop...\n"
              << std::endl;

    int frameCount = 0;

    while (true)
    {
        gettimeofday(&tt1, NULL);

        // 获取所有类型的图像
        auto images = camera.getImg();

        if (images.size() >= 3)
        {
            frameCount++;

            // 显示彩色图像
            if (!images[0].empty())
            {
                cv::imshow("Color Image", images[0]);
            }

            // 显示深度图像（转换为可视化）
            if (!images[1].empty())
            {
                cv::Mat depthDisplay;
                images[1].convertTo(depthDisplay, CV_8UC1, 255.0 / 5000.0); // 假设最大深度5m
                cv::applyColorMap(depthDisplay, depthDisplay, cv::COLORMAP_JET);
                cv::imshow("Depth Image", depthDisplay);

                // 在深度图上绘制中心十字
                int centerX = depthDisplay.cols / 2;
                int centerY = depthDisplay.rows / 2;
                cv::line(depthDisplay, cv::Point(centerX - 20, centerY), cv::Point(centerX + 20, centerY), cv::Scalar(255, 255, 255), 2);
                cv::line(depthDisplay, cv::Point(centerX, centerY - 20), cv::Point(centerX, centerY + 20), cv::Scalar(255, 255, 255), 2);
                cv::imshow("Depth Image with Center", depthDisplay);
            }

            // 显示红外图像
            if (!images[2].empty())
            {
                cv::Mat irDisplay;
                images[2].convertTo(irDisplay, CV_8UC1, 255.0 / 65535.0);
                cv::imshow("IR Image", irDisplay);
            }
        }
        else
        {
            std::cerr << "Failed to get images from camera!" << std::endl;
        }

        // 计算帧时间
        gettimeofday(&tt2, NULL);
        long timeuse = 1000 * (tt2.tv_sec - tt1.tv_sec) + (tt2.tv_usec - tt1.tv_usec) * 1.0 / 1000;

        // 每30帧显示一次帧率信息
        if (frameCount % 30 == 0)
        {
            std::cout << "Frame " << frameCount << " - Time: " << timeuse << "ms ("
                      << (1000.0 / timeuse) << " FPS)" << std::endl;
        }

        // 键盘事件处理
        int key = cv::waitKey(1) & 0xFF;
        if (key == 27) // ESC键退出
        {
            std::cout << "ESC pressed, exiting..." << std::endl;
            break;
        }
        else if (key == 32) // 空格键保存图像
        {
            std::cout << "Saving images..." << std::endl;
            auto saveImages = camera.getImg();
            if (saveImages.size() >= 3)
            {
                // 保存彩色图像
                if (!saveImages[0].empty())
                {
                    std::string colorFile = "color_" + std::to_string(frameCount) + ".jpg";
                    cv::imwrite(colorFile, saveImages[0]);
                    std::cout << "  Color image saved as: " << colorFile << std::endl;
                }

                // 保存深度图像
                if (!saveImages[1].empty())
                {
                    std::string depthFile = "depth_" + std::to_string(frameCount) + ".png";
                    cv::imwrite(depthFile, saveImages[1]);
                    std::cout << "  Depth image saved as: " << depthFile << std::endl;

                    // 同时保存深度可视化图像
                    cv::Mat depthVis;
                    saveImages[1].convertTo(depthVis, CV_8UC1, 255.0 / 5000.0);
                    cv::applyColorMap(depthVis, depthVis, cv::COLORMAP_JET);
                    std::string depthVisFile = "depth_vis_" + std::to_string(frameCount) + ".jpg";
                    cv::imwrite(depthVisFile, depthVis);
                    std::cout << "  Depth visualization saved as: " << depthVisFile << std::endl;
                }

                // 保存红外图像
                if (!saveImages[2].empty())
                {
                    std::string irFile = "ir_" + std::to_string(frameCount) + ".png";
                    cv::imwrite(irFile, saveImages[2]);
                    std::cout << "  IR image saved as: " << irFile << std::endl;
                }
            }
        }
        else if (key == 'd' || key == 'D') // 'd'键获取中心点深度
        {
            cv::Mat depthImg = camera.getDepthImg();
            if (!depthImg.empty())
            {
                int centerX = depthImg.cols / 2;
                int centerY = depthImg.rows / 2;
                float depth = camera.getDepthAt(centerX, centerY);
                std::cout << "Center point (" << centerX << "," << centerY << ") depth: "
                          << depth << " meters" << std::endl;

                // 显示周围区域的深度值
                std::cout << "Nearby depth values:" << std::endl;
                for (int dy = -2; dy <= 2; dy++)
                {
                    for (int dx = -2; dx <= 2; dx++)
                    {
                        float nearDepth = camera.getDepthAt(centerX + dx * 10, centerY + dy * 10);
                        std::cout << std::fixed << std::setprecision(3) << nearDepth << " ";
                    }
                    std::cout << std::endl;
                }
            }
        }
        else if (key == 'i' || key == 'I') // 'i'键显示相机信息
        {
            auto testImages = camera.getImg();
            if (testImages.size() >= 3)
            {
                std::cout << "\n=== Camera Information ===" << std::endl;
                if (!testImages[0].empty())
                {
                    std::cout << "Color image: " << testImages[0].cols << "x" << testImages[0].rows
                              << " (" << testImages[0].type() << ")" << std::endl;
                }
                if (!testImages[1].empty())
                {
                    std::cout << "Depth image: " << testImages[1].cols << "x" << testImages[1].rows
                              << " (" << testImages[1].type() << ")" << std::endl;
                }
                if (!testImages[2].empty())
                {
                    std::cout << "IR image: " << testImages[2].cols << "x" << testImages[2].rows
                              << " (" << testImages[2].type() << ")" << std::endl;
                }
                std::cout << "Frame count: " << frameCount << std::endl;
                std::cout << "=========================\n"
                          << std::endl;
            }
        }
        else if (key == 'a' || key == 'A') // 'a'键测试对齐图像
        {
            std::cout << "Testing aligned images..." << std::endl;
            cv::Mat alignedColor, alignedDepth;
            camera.getAlignedImages(alignedColor, alignedDepth);

            if (!alignedColor.empty() && !alignedDepth.empty())
            {
                cv::imshow("Aligned Color", alignedColor);

                cv::Mat alignedDepthVis;
                alignedDepth.convertTo(alignedDepthVis, CV_8UC1, 255.0 / 5000.0);
                cv::applyColorMap(alignedDepthVis, alignedDepthVis, cv::COLORMAP_JET);
                cv::imshow("Aligned Depth", alignedDepthVis);

                std::cout << "Aligned images displayed in separate windows" << std::endl;
            }
            else
            {
                std::cout << "Failed to get aligned images!" << std::endl;
            }
        }
    }

    // 关闭相机和窗口
    std::cout << "Closing camera..." << std::endl;
    camera.close();
    cv::destroyAllWindows();

    std::cout << "Total frames processed: " << frameCount << std::endl;
    std::cout << "Program exited successfully!" << std::endl;

    return 0;
}