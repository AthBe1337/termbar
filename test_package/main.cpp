// test_package/main.cpp
#include "termbar.h" // 确保包含我们在上一个问题中修改过的头文件
#include <thread>
#include <chrono>
#include <iostream>

int main() {
    std::cout << "Starting TermBar Test..." << std::endl;

    // 创建一个进度条，总量100，颜色为青色
    termbar::ProgressBar bar(100, termbar::Color::Cyan);

    for (int i = 0; i <= 100; ++i) {
        // 更新进度
        bar.update(i);

        // 模拟一些工作 (50毫秒)
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        // 模拟中间打印日志 (测试光标保存恢复功能)
        if (i == 50) {
            bar.log("Halfway there! Keep going...");
        }
    }
    
    // 显式完成（也可以依赖析构函数）
    bar.finish();
    
    std::cout << "Job Done!" << std::endl;
    return 0;
}