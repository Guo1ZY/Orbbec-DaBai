# Orbbec DaBai 相机库

![Orbbec DaBai Camera](https://images.unsplash.com/photo-1581091226033-d5c48150dbaa?ixlib=rb-4.0.3&ixid=M3wxMjA3fDB8MHxwaG90by1wYWdlfHx8fGVufDB8fHx8fA%3D%3D&auto=format&fit=crop&w=1200&h=600&q=80)

## 概述

Orbbec DaBai 相机库是一个高性能的C++库，用于简化Orbbec DaBai深度相机的开发。该库提供了简洁的API接口，支持彩色、深度和红外图像的获取，以及深度对齐、中心点深度测量等功能。

主要功能：
- 多模态图像获取（彩色、深度、红外）
- 深度图像对齐到彩色图像
- 实时帧率统计
- 深度值测量
- 图像保存功能
- 相机参数配置

## 功能特性

- **多图像流支持**：同时获取彩色、深度和红外图像
- **深度对齐**：将深度图像对齐到彩色图像坐标系
- **实时显示**：通过OpenCV实时显示三种图像流
- **中心点深度测量**：获取图像中心区域的深度值
- **帧率统计**：实时显示处理帧率
- **图像保存**：一键保存当前帧图像
- **相机控制**：支持镜像、曝光、白平衡等参数设置

## 依赖项

- **Orbbec SDK** (v1.8.0+)
- **OpenCV** (v4.5.0+)
- **CMake** (v3.12+)
- **C++17** 兼容编译器

## 构建与安装

### 1. 克隆仓库
```bash
git clone https://github.com/yourusername/orbbec-dabai.git
cd orbbec-dabai
```

### 2. 安装依赖
#### Ubuntu
```bash
sudo apt install build-essential cmake libopencv-dev
```

#### Windows
- 安装 [CMake](https://cmake.org/download/)
- 安装 [OpenCV](https://opencv.org/releases/)
- 安装 [Orbbec SDK](https://orbbec3d.com/develop/)

### 3. 配置项目
```bash
mkdir build
cd build
cmake .. -DOrbbecSDK_DIR=/path/to/orbbec/sdk/cmake \
         -DOpenCV_DIR=/path/to/opencv/build
```

### 4. 编译项目
```bash
cmake --build . --config Release
```

### 5. 运行示例程序
```bash
./bin/OrbbecDaBaiDemo
```

## 使用示例

### 基本使用
```cpp
#include "OrbbecDabai.hpp"

int main() {
    OrbbecDabai camera;
    camera.init();
    camera.setCamera();
    
    while (true) {
        auto images = camera.getImg();
        // 处理彩色、深度和红外图像...
    }
    
    camera.close();
    return 0;
}
```

### 键盘控制
程序运行时支持以下键盘命令：
- **ESC** - 退出程序
- **空格键** - 保存当前图像帧
- **D/d** - 显示中心点深度值
- **I/i** - 显示相机信息
- **A/a** - 显示对齐的彩色和深度图像

### 获取深度值
```cpp
// 获取图像中心点深度
float depth = camera.getDepthAt(centerX, centerY);
std::cout << "Center depth: " << depth << " meters" << std::endl;

// 获取对齐图像
cv::Mat alignedColor, alignedDepth;
camera.getAlignedImages(alignedColor, alignedDepth);
```

## 项目结构
```
orbbec-dabai/
├── CMakeLists.txt          # 项目构建配置
├── include/
│   └── OrbbecDabai.hpp     # 库头文件
├── source/
│   └── OrbbecDabai.cpp     # 库实现文件
├── main.cpp                # 示例主程序
├── build/                  # 构建目录
└── README.md               # 项目文档
```

## 贡献指南

欢迎贡献代码！请遵循以下步骤：

1. Fork 项目仓库
2. 创建新分支 (`git checkout -b feature/your-feature`)
3. 提交更改 (`git commit -am 'Add some feature'`)
4. 推送到分支 (`git push origin feature/your-feature`)
5. 创建 Pull Request

## 许可证

本项目采用 MIT 许可证 - 详见 [LICENSE](LICENSE) 文件。

## 致谢

- 感谢 Orbbec 提供优秀的3D视觉硬件
- 感谢 OpenCV 团队提供强大的计算机视觉库
- 感谢所有贡献者和用户的支持

---

**连接相机，开始您的3D视觉之旅！** 🚀
