#include "termbar.h"
#include <thread>
#include <chrono>
#include <iostream>

int main() {
    std::cout << "Preparing to download 50 files...\n";
    
    // 使用默认颜色 (Green)
    termbar::ProgressBar bar(50);

    for (int i = 0; i <= 50; ++i) {
        bar.update(i);

        // 模拟偶尔出现的日志
        if (i % 10 == 0 && i > 0) {
            bar.log("[INFO] Downloaded package_" + std::to_string(i) + ".zip");
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    // 任务完成，finish() 会被析构函数自动调用，
    // 或者你可以手动调用 bar.finish() 来提前结束进度条显示。
    return 0;
}
