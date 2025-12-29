#pragma once

#include <string>
#include <mutex>

namespace termbar {

enum class Color {
    Default,
    Red,
    Green,
    Yellow,
    Blue,
    Magenta,
    Cyan,
    White
};

class ProgressBar {
private:
    std::mutex output_mutex_;
    int total_steps_;
    int current_step_;
    int term_rows_;
    bool finished_;
    std::string color_code_;
    std::string reset_code_;

    std::string get_ansi_code(Color color);
    bool supports_color();
    void update_terminal_size();
    void move_cursor(int row, int col);
    void save_cursor();
    void restore_cursor();
    void clear_line();
    std::string get_bar_string();

public:
    // 构造函数：指定总步数和颜色
    ProgressBar(int total, Color color = Color::Green);
    ~ProgressBar();

    // 更新进度条 (0 -> total)
    void update(int step);

    // 打印日志，自动处理滚动和光标
    void log(const std::string& message);

    // 显式结束进度条（析构函数也会自动调用）
    void finish();
};

} // namespace termbar
