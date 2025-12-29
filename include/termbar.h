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
    int term_cols_;
    bool finished_;
    std::string color_code_;
    std::string reset_code_;

    void setup_console();
    void update_terminal_info();
    bool is_interactive();
    std::string get_ansi_code(Color color);

    void move_cursor(int row, int col);
    void save_cursor();
    void restore_cursor();
    void clear_line();
    std::string get_bar_string();

public:
    ProgressBar(int total, Color color = Color::Green);
    ~ProgressBar();

    void update(int step);

    void log(const std::string& message);

    void finish();
};

} // namespace termbar
