# SVpwm_STM32G474

基于 STM32G474 微控制器的空间矢量脉宽调制（SVPWM）实现项目。

> **注意**:  本项目为 2025 年全国大学生电子设计竞赛的弃用方案，仅供学习和参考使用。

## 项目简介

本项目实现了在 STM32G474 平台上的三相 SVPWM 算法，适用于电机控制、逆变器等应用场景。该方案在 2025 年电赛准备过程中开发，后因技术问题而弃用，现开源供大家学习交流。

## 硬件平台

- **微控制器**: STM32G474XX
- **开发工具**:  STM32CubeMX + CMake

## 项目结构

```
.
├── Core/                   # 核心代码
├── Drivers/                # STM32 HAL 驱动库
├── build/                  # 编译输出目录
├── cmake/                  # CMake 配置文件
├── log/                    # 日志文件
├── CMakeLists.txt          # CMake 构建配置
├── CMakePresets.json       # CMake 预设配置
├── phase3_svpwm.ioc       # STM32CubeMX 工程文件
├── startup_stm32g474xx.s  # 启动文件
└── STM32G474XX_FLASH.ld   # 链接脚本
```

## 编译说明

### 前置要求

- ARM GCC 工具链
- CMake (>= 3.22)
- Make 或 Ninja 构建工具

### 编译步骤

```bash
# 配置项目
cmake -B build

# 编译
cmake --build build

# 或使用 CMake Presets
cmake --preset default
cmake --build --preset default
```

## 功能特性

- ✅ 三相 SVPWM 算法实现
- ✅ STM32G474 定时器配置
- ✅ CMake 构建系统支持

## 开发环境

推荐使用以下开发环境：

- STM32CubeIDE
- VSCode + CMake Tools
- Keil MDK (需要额外配置)

## 项目状态

⚠️ **弃用说明**: 本方案为 2025 年电赛的前期探索方案，因以下技术问题已停止开发：

- **SVPWM 谐波量问题**: 空间矢量调制产生的谐波含量较高，难以满足电赛对输出波形质量的严格要求
- **闭环控制难度**: 闭环控制系统的调试复杂度高，PID 参数整定困难，系统稳定性不佳

**经验教训**:
- SVPWM 虽然理论上优势明显，但在实际应用中需要额外的滤波措施来降低谐波
- 闭环控制需要充分的前期仿真和参数优化，时间成本较高
- 建议后续参赛者充分评估方案的可实现性和调试难度

## 许可证

本项目遵循开源许可证（请根据实际情况添加）。

## 贡献

欢迎提交 Issue 和 Pull Request！

## 联系方式

- 作者:  66Leslie
- 项目地址: [https://github.com/66Leslie/SVpwm_STM32G474](https://github.com/66Leslie/SVpwm_STM32G474)