#include "termbar.h"
#include <thread>
#include <chrono>
#include <iostream>

void simulate_work(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

int main() {
    std::cout << "Starting Installation Wizard...\n\n";

    // --- 阶段 1: 检查环境 (黄色) ---
    {
        std::cout << "Step 1/3: Checking Environment\n";
        termbar::ProgressBar bar(100, termbar::Color::Yellow);
        for (int i = 0; i <= 100; i+=2) {
            bar.update(i);
            if (i == 50) bar.log(" > Memory Check... OK");
            simulate_work(20);
        }
    } // bar 析构，进度条清除，光标回到正常位置

    std::cout << "Environment valid.\n\n";

    // --- 阶段 2: 安装核心组件 (蓝色) ---
    {
        std::cout << "Step 2/3: Installing Core Modules\n";
        termbar::ProgressBar bar(200, termbar::Color::Blue);
        for (int i = 0; i <= 200; i++) {
            bar.update(i);
            if (i % 50 == 0) bar.log(" > Extracted module_" + std::to_string(i));
            simulate_work(10);
        }
    }

    std::cout << "Core installed.\n\n";

    // --- 阶段 3: 清理临时文件 (红色) ---
    {
        std::cout << "Step 3/3: Cleanup (Sensitive Operation)\n";
        termbar::ProgressBar bar(50, termbar::Color::Red);
        for (int i = 0; i <= 50; i++) {
            bar.update(i);
            if (i == 25) bar.log(" > Removing /tmp/install_cache");
            simulate_work(30);
        }
    }

    std::cout << "\nInstallation Complete!\n";
    return 0;
}
