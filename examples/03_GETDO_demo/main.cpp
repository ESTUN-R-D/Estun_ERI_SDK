#ifdef _WIN32
#include <Windows.h>
#else
#include <thread>
#include <chrono>
#include <limits.h>
#include <unistd.h>
#define Sleep(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms))
#endif

#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include "ERIParamManager.h"

static const UINT32 kMinWritableDoPort = 12;
static const UINT32 kMaxWritableDoPort = 32;

static bool switchToExecutableDir() {
#ifdef __linux__
    char exePath[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
    if (len <= 0) {
        return false;
    }
    exePath[len] = '\0';

    std::string fullPath(exePath);
    std::size_t pos = fullPath.find_last_of('/');
    if (pos == std::string::npos) {
        return false;
    }

    std::string exeDir = fullPath.substr(0, pos);
    return chdir(exeDir.c_str()) == 0;
#else
    return true;
#endif
}

void processWriteDO(ERIParamManager& eriManager, bool controlHeld) {
    if (!controlHeld) {
        std::cerr << "请先使用指令 5 获取机器人控制权，再设置 DO。" << std::endl;
        return;
    }

    UINT32 port = 0;
    UINT32 val = 0;
    const auto readUnsignedInput = [](const char* prompt, UINT32& value) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin >> std::ws, line)) return false;

        std::istringstream input(line);
        long long parsedValue = 0;
        if (!(input >> parsedValue) || parsedValue < 0 ||
            parsedValue > static_cast<long long>(std::numeric_limits<UINT32>::max())) {
            return false;
        }
        input >> std::ws;
        if (!input.eof()) return false;

        value = static_cast<UINT32>(parsedValue);
        return true;
    };

    if (!readUnsignedInput("请输入 DO 端口号 (12-32): ", port)) {
        std::cerr << "端口号无效，本次未写入 DO。" << std::endl;
        return;
    }
    if (port < kMinWritableDoPort || port > kMaxWritableDoPort) {
        std::cerr << "当前默认允许操作的 DO 区间为 DO12-DO32，本次未写入。"
                  << "如需调整区间，请同步修改机器人底层配置文件。" << std::endl;
        return;
    }
    if (!readUnsignedInput("请输入端口值 (0/1): ", val) || val > 1) {
        std::cerr << "端口值无效，请输入 0 或 1；本次未写入 DO。" << std::endl;
        return;
    }

    if (!eriManager.setDo(port, val)) {
        std::cerr << "设置 DO[" << port << "] = " << val << " 失败!" << std::endl;
        return;
    }

    std::cout << "设置 DO[" << port << "] = " << val << " 成功" << std::endl;

    // 写入后立即回读该端口，便于确认是否生效。
    UINT32 readBack = eriManager.getCurrentDO(port);
    std::cout << "回读 DO[" << port << "] = " << readBack << std::endl;
}

void processGetControl(ERIParamManager& eriManager, bool& controlHeld) {
    if (controlHeld) {
        std::cout << "当前程序已获取机器人控制权。" << std::endl;
        return;
    }
    if (!eriManager.getControlRobot()) {
        std::cerr << "获取机器人控制权失败!" << std::endl;
        return;
    }
    controlHeld = true;
    std::cout << "已获取机器人控制权。" << std::endl;
}

void processReleaseControl(ERIParamManager& eriManager, bool& controlHeld) {
    if (!controlHeld) {
        std::cout << "当前程序未记录到已获取的控制权。" << std::endl;
        return;
    }
    // 释放失败时如实提示，不声称已释放。
    // 不使用 forceReleaseControlOverRobot，其使用契约未在仓库资料中定义。
    if (eriManager.releaseControlOverRobot()) {
        controlHeld = false;
        std::cout << "控制权释放成功。" << std::endl;
    } else {
        std::cerr << "释放控制权失败，请手动确认控制权状态!" << std::endl;
    }
}

void processReadDO(ERIParamManager& eriManager) {
    UINT32 doValue = eriManager.getCurrentDO(4);
    std::cout << "当前系统 DO: " << doValue << std::endl;
}

int main() {
    if (!switchToExecutableDir()) {
        std::cerr << "警告: 切换到可执行文件目录失败，可能找不到配置文件。" << std::endl;
    }

    std::cout << "注意: 本示例支持向控制器写入物理 DO 输出。" << std::endl;
    std::cout << "当前默认允许操作的 DO 区间为 DO12-DO32；修改区间需同步调整机器人底层配置文件。" << std::endl;
    std::cout << "测试前请确认目标端口号无设备动作风险（夹爪/吸盘/工装等）。" << std::endl;
    std::cout << "请先使用指令 5 获取控制权后再写 DO；指令 6 释放控制权。" << std::endl;

    ERIParamManager eriManager;
    // 仅配置 PC 端通信参数。运行示例前请改成现场机器人控制器 IP。
    std::string robotIp = "172.31.16.227";
    uint16_t cmdPort = 61210;
    uint16_t servoPort = 61211;
    uint16_t statusPort = 61212;

    if (!eriManager.setCommunicationParam(robotIp.c_str(), cmdPort, servoPort, statusPort)) {
        std::cerr << "PC端通信参数设置失败!" << std::endl;
        return -1;
    }
    Sleep(50); // 保留短暂等待；连接成功以 SDK 三个通道的 connect success 日志为准。

    char input = '0';
    bool controlHeld = false;
    while (true) {
        std::cout << "输入指令: (1: 写DO[需先获取控制权], 3: 读取DO, 5: 获取控制权, 6: 释放控制权, q: 退出): ";
        if (!(std::cin >> input)) {
            if (std::cin.eof()) break;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cerr << "输入无效，请重新输入指令。" << std::endl;
            continue;
        }

        if (input == 'q') break;
        if (input == '1') processWriteDO(eriManager, controlHeld);
        if (input == '3') processReadDO(eriManager);
        if (input == '5') processGetControl(eriManager, controlHeld);
        if (input == '6') processReleaseControl(eriManager, controlHeld);
    }

    if (controlHeld) {
        processReleaseControl(eriManager, controlHeld);
    }

    return 0;
}
