#include "termbar.h"
#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstdlib>

namespace termbar {

ProgressBar::ProgressBar(int total, Color color) 
    : total_steps_(total), current_step_(0), finished_(false) {
    
    update_terminal_size();
    color_code_ = get_ansi_code(color);
    reset_code_ = supports_color() ? "\033[0m" : "";

    std::cout << "\033[?25l" << std::flush; // 隐藏光标
}

ProgressBar::~ProgressBar() {
    finish();
}

void ProgressBar::update(int step) {
    std::lock_guard<std::mutex> lock(output_mutex_);
    if (finished_) return;

    // 每次更新检查窗口高度，应对运行时调整窗口大小
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    term_rows_ = w.ws_row;

    current_step_ = step;
    if (current_step_ > total_steps_) current_step_ = total_steps_;

    save_cursor();
    move_cursor(term_rows_, 1);
    clear_line();
    std::cout << get_bar_string() << std::flush;
    restore_cursor();
}

void ProgressBar::log(const std::string& message) {
    std::lock_guard<std::mutex> lock(output_mutex_);
    if (finished_) {
        std::cout << message << std::endl;
        return;
    }

    // 打印日志，自动换行
    std::cout << "\033[K" << message << std::endl;

    // 强制重绘底部进度条，防止被日志滚动卷走
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    term_rows_ = w.ws_row;
    
    save_cursor();
    move_cursor(term_rows_, 1);
    clear_line();
    std::cout << get_bar_string() << std::flush;
    restore_cursor();
}

void ProgressBar::finish() {
    std::lock_guard<std::mutex> lock(output_mutex_);
    if (!finished_) {
        finished_ = true;
        save_cursor();
        move_cursor(term_rows_, 1);
        clear_line(); // 清除进度条
        restore_cursor();
        std::cout << "\033[?25h" << std::flush; // 恢复光标
    }
}

std::string ProgressBar::get_ansi_code(Color color) {
    if (!supports_color()) return "";
    switch (color) {
        case Color::Red:     return "\033[31m";
        case Color::Green:   return "\033[32m";
        case Color::Yellow:  return "\033[33m";
        case Color::Blue:    return "\033[34m";
        case Color::Magenta: return "\033[35m";
        case Color::Cyan:    return "\033[36m";
        case Color::White:   return "\033[37m";
        default:             return "";
    }
}

bool ProgressBar::supports_color() {
    if (!isatty(STDOUT_FILENO)) return false;
    const char* term = std::getenv("TERM");
    if (term == nullptr) return false;
    std::string s(term);
    return s.find("xterm") != std::string::npos || 
           s.find("color") != std::string::npos ||
           s.find("256") != std::string::npos ||
           s.find("screen") != std::string::npos;
}

void ProgressBar::update_terminal_size() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    term_rows_ = w.ws_row;
}

void ProgressBar::move_cursor(int row, int col) {
    std::cout << "\033[" << row << ";" << col << "H";
}

void ProgressBar::save_cursor() { std::cout << "\0337"; }
void ProgressBar::restore_cursor() { std::cout << "\0338"; }
void ProgressBar::clear_line() { std::cout << "\033[2K"; }

std::string ProgressBar::get_bar_string() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int width = w.ws_col;
    float progress = (total_steps_ > 0) ? (float)current_step_ / total_steps_ : 0.0f;
    int bar_width = width - 8; 
    if (bar_width < 0) bar_width = 0;
    int filled = (int)(bar_width * progress);

    std::string bar = "[";
    bar += color_code_;
    for (int i = 0; i < bar_width; ++i) {
        if (i < filled) bar += "█";
        else {
            if (i == filled) bar += reset_code_;
            bar += " ";
        }
    }
    if (filled == bar_width) bar += reset_code_;
    bar += "] " + std::to_string((int)(progress * 100)) + "%";
    return bar;
}

} // namespace termbar
