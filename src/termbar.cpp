#include "termbar.h"
#include <iostream>
#include <cstdlib>

#ifdef _WIN32
    #ifndef NOMINMAX
    #define NOMINMAX
    #endif
    #include <windows.h>
    #include <io.h>
    #define FILENO_STDOUT _fileno(stdout)
    #define ISATTY _isatty
#else
    #include <sys/ioctl.h>
    #include <unistd.h>
    #define FILENO_STDOUT STDOUT_FILENO
    #define ISATTY isatty
#endif

namespace termbar {

ProgressBar::ProgressBar(int total, Color color)
    : total_steps_(total), current_step_(0), finished_(false), term_rows_(0), term_cols_(0) {

    setup_console();

    update_terminal_info();

    color_code_ = get_ansi_code(color);
    reset_code_ = (is_interactive() && !color_code_.empty()) ? "\033[0m" : "";

    if (is_interactive()) {
        std::cout << "\033[?25l" << std::flush;
    }
}

ProgressBar::~ProgressBar() {
    finish();
}

void ProgressBar::setup_console() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;

    // ENABLE_VIRTUAL_TERMINAL_PROCESSING = 0x0004
    dwMode |= 0x0004;
    SetConsoleMode(hOut, dwMode);

    // SetConsoleOutputCP(CP_UTF8);
#endif
}

void ProgressBar::update_terminal_info() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        term_cols_ = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        term_rows_ = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    } else {
        term_cols_ = 80;
        term_rows_ = 25;
    }
#else
    struct winsize w;
    if (ioctl(FILENO_STDOUT, TIOCGWINSZ, &w) == 0) {
        term_cols_ = w.ws_col;
        term_rows_ = w.ws_row;
    } else {
        term_cols_ = 80;
        term_rows_ = 25;
    }
#endif
}

bool ProgressBar::is_interactive() {
    return ISATTY(FILENO_STDOUT);
}


void ProgressBar::update(int step) {
    std::lock_guard<std::mutex> lock(output_mutex_);
    if (finished_) return;

    update_terminal_info();

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

    std::cout << "\033[K" << message << std::endl;

    update_terminal_info();
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
        clear_line();
        restore_cursor();
        if (is_interactive()) {
            std::cout << "\033[?25h" << std::flush;
        }
    }
}

std::string ProgressBar::get_ansi_code(Color color) {
    if (!is_interactive()) return "";

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

void ProgressBar::move_cursor(int row, int col) {
    std::cout << "\033[" << row << ";" << col << "H";
}

void ProgressBar::save_cursor() { std::cout << "\0337"; }
void ProgressBar::restore_cursor() { std::cout << "\0338"; }
void ProgressBar::clear_line() { std::cout << "\033[2K"; }

std::string ProgressBar::get_bar_string() {
    float progress = (total_steps_ > 0) ? (float)current_step_ / total_steps_ : 0.0f;
    int bar_width = term_cols_ - 8;
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
