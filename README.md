# ESTUN Robot Interface(ERI) 🤖

这是 **Estun Automation** 提供的机器人实时外部控制接口 ([ERI](https://www.estun.com/gnxrj/170.html)) 开发工具包，包含sdk以及使用样例。

## 📦 快速开始

### 1. 环境要求
- **CMake**: v3.10+
- **Windows**: Visual Studio 2022 (MSVC)
- **Ubuntu**: GCC 11.4+

### 2. 下载二进制库
由于合规性与仓库体积限制，请前往 [Releases](https://github.com/ESTUN-R-D/Estun_ERI_SDK/releases) 页面下载对应系统的二进制压缩包，并解压至项目根目录。

### 3. 编译示例 for x64
```powershell
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

### 4. 编译示例 for Linux
```bash
rm -rf build && cmake -S . -B build && cmake --build build -j$(nproc)
```

### 5. 运行示例

运行前请先在示例源码中确认机器人控制器 IP、端口和示例点位是否符合现场设备。
`setCommunicationParam` 仅用于设置 PC 端网络参数，返回成功不代表机器人已经连接成功。
一般需要看到 SDK 日志中三个通道均连接成功，才表示与机器人建立连接：

```text
[INFO] [UDPSocket] connect success
[INFO] [ServoSocket] connect success
[INFO] [cmdSocket] connect success
```

Windows 构建完成后，可在 `build/examples/<demo>/Release/` 目录运行对应 `.exe`：

```powershell
.\build\examples\01_APOS_demo\Release\eri_apos_demo.exe
.\build\examples\02_CPOS_demo\Release\eri_cpos_demo.exe
.\build\examples\03_GETDO_demo\Release\eri_getdo_demo.exe
.\build\examples\04_STATUS_CALLBACK_demo\Release\eri_status_callback_demo.exe
```

Linux 构建完成后，请运行不带 `_bin` 后缀的启动脚本。脚本会自动设置 `LD_LIBRARY_PATH`，并切换到可执行文件目录以便 SDK 读取配置文件：

```bash
./build/examples/01_APOS_demo/eri_apos_demo
./build/examples/02_CPOS_demo/eri_cpos_demo
./build/examples/03_GETDO_demo/eri_getdo_demo
./build/examples/04_STATUS_CALLBACK_demo/eri_status_callback_demo
```

在 Ubuntu 环境中，如遇到网络、设备或配置文件访问权限问题，请使用 `sudo` 运行启动脚本：

```bash
sudo ./build/examples/01_APOS_demo/eri_apos_demo
sudo ./build/examples/02_CPOS_demo/eri_cpos_demo
sudo ./build/examples/03_GETDO_demo/eri_getdo_demo
sudo ./build/examples/04_STATUS_CALLBACK_demo/eri_status_callback_demo
```

### 6. 示例说明

- `01_APOS_demo`: 关节实时控制示例，演示 PC 端通信参数配置、`getControlRobot`、`setERIMotionParam`、`movJ`、`startServo`、`servoToAPOS`、`releaseControlOverRobot`。
- `03_GETDO_demo`: DO 读取示例，演示 `getCurrentDO` 的基本用法。
- `04_STATUS_CALLBACK_demo`: 状态回调示例，演示 `setRobotStatusCallback` 注册方式，以及如何在主线程读取最新 `RobotStatus`。

> [!WARNING]
> 运动示例中的点位仅用于展示接口调用流程。操作真实机器人前，请根据机器人型号、安装姿态、负载、工具坐标和现场安全边界重新确认点位。
