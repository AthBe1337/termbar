#include "termbar.h"
#include <thread>
#include <vector>
#include <chrono>
#include <iostream>

// 工作线程函数，接收进度条的引用
void worker_task(termbar::ProgressBar& bar, int start_id, int count) {
    for (int i = 0; i <= count; ++i) {
        // 多个线程同时调用 update 是安全的
        // 但为了逻辑清晰，这里只让一个线程驱动进度，或者使用原子计数器
        // 这里简化演示：我们在主循环外只更新一次
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        
        if (i % 20 == 0) {
            bar.log("[Thread " + std::to_string(start_id) + "] Processed batch " + std::to_string(i));
        }
    }
}

int main() {
    const int total_items = 100;
    
    // 使用青色进度条
    termbar::ProgressBar bar(total_items, termbar::Color::Cyan);

    // 模拟工作逻辑
    for (int i = 0; i <= total_items; ++i) {
        bar.update(i);
        
        if (i == 30) bar.log("[Main] Database connection established.");
        if (i == 70) bar.log("[Main] Cache cleared.");
        
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    bar.finish();
    std::cout << "All threads joined. Exiting.\n";
    return 0;
}
