# ESTUN Robot Interface (ERI) 🤖

English | [简体中文](README.md)

This repository contains the SDK and example programs for **Estun Automation**'s real-time external robot control interface ([ERI](https://www.estun.com/gnxrj/170.html)).

## Quick Start

### 1. Requirements
- **CMake**: v3.10+
- **Windows**: Visual Studio 2022 (MSVC)
- **Ubuntu**: GCC 11.4+

### 2. Download the Binary Package
To keep the repository compliant and lightweight, download the binary package for your target platform from the [Releases](https://github.com/ESTUN-R-D/Estun_ERI_SDK/releases) page and extract it into the project root directory.

### 3. Build the Examples on x64 Windows
```powershell
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

### 4. Build the Examples on Linux
```bash
rm -rf build && cmake -S . -B build && cmake --build build -j$(nproc)
```

### 5. Run the Examples

Before running any example, verify that the robot controller IP, port, and sample positions in the source code match your actual setup.
`setCommunicationParam` only configures the network parameters on the PC side. A successful return value does not mean the robot is already connected.
In most cases, you should wait until the SDK log shows that all three channels are connected:

```text
[INFO] [UDPSocket] connect success
[INFO] [ServoSocket] connect success
[INFO] [cmdSocket] connect success
```

After a Windows build, run the matching `.exe` file from `build/examples/<demo>/Release/`:

```powershell
.\build\examples\01_APOS_demo\Release\eri_apos_demo.exe
.\build\examples\02_CPOS_demo\Release\eri_cpos_demo.exe
.\build\examples\03_GETDO_demo\Release\eri_getdo_demo.exe
.\build\examples\04_STATUS_CALLBACK_demo\Release\eri_status_callback_demo.exe
```

After a Linux build, run the launcher script without the `_bin` suffix. The script sets `LD_LIBRARY_PATH` automatically and switches to the executable directory so the SDK can find its configuration files:

```bash
./build/examples/01_APOS_demo/eri_apos_demo
./build/examples/02_CPOS_demo/eri_cpos_demo
./build/examples/03_GETDO_demo/eri_getdo_demo
./build/examples/04_STATUS_CALLBACK_demo/eri_status_callback_demo
```

If you hit permission issues related to network access, devices, or configuration files on Ubuntu, run the launcher script with `sudo`:

```bash
sudo ./build/examples/01_APOS_demo/eri_apos_demo
sudo ./build/examples/02_CPOS_demo/eri_cpos_demo
sudo ./build/examples/03_GETDO_demo/eri_getdo_demo
sudo ./build/examples/04_STATUS_CALLBACK_demo/eri_status_callback_demo
```

### 6. Example Overview

- `01_APOS_demo`: Joint-space real-time control example showing PC-side communication setup, `getControlRobot`, `setERIMotionParam`, `movJ`, `startServo`, `servoToAPOS`, and `releaseControlOverRobot`.
- `02_CPOS_demo`: TCP circular trajectory example showing `movJ` to the initial pose, `getWorldCpos` to read the current TCP pose, `startServo` in CPOS mode, and periodic Cartesian targets sent with `servoToCPOS`.
- `03_GETDO_demo`: Digital output reading example showing the basic usage of `getCurrentDO`.
- `04_STATUS_CALLBACK_demo`: Status callback example showing how to register `setRobotStatusCallback` and read the latest `RobotStatus` in the main thread.

> [!WARNING]
> The motion waypoints in these examples are only for illustrating the API flow. Before operating a real robot, review and adjust them for the robot model, mounting posture, payload, tool coordinates, and on-site safety boundaries.
