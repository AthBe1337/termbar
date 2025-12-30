#include <pybind11/pybind11.h>
#include "termbar.h"

namespace py = pybind11;

PYBIND11_MODULE(termbar, m) {
    m.doc() = "Termbar: A C++ terminal progress bar library with Python bindings";

    // 导出颜色枚举
    py::enum_<termbar::Color>(m, "Color")
        .value("Default", termbar::Color::Default)
        .value("Red", termbar::Color::Red)
        .value("Green", termbar::Color::Green)
        .value("Yellow", termbar::Color::Yellow)
        .value("Blue", termbar::Color::Blue)
        .value("Magenta", termbar::Color::Magenta)
        .value("Cyan", termbar::Color::Cyan)
        .value("White", termbar::Color::White)
        .export_values();

    // 导出主类
    py::class_<termbar::ProgressBar>(m, "ProgressBar")
        .def(py::init<int, termbar::Color>(), 
             py::arg("total"), py::arg("color") = termbar::Color::Green)
        .def("update", &termbar::ProgressBar::update, py::arg("step"))
        .def("log", &termbar::ProgressBar::log, py::arg("message"))
        .def("finish", &termbar::ProgressBar::finish)
        .def("__enter__", [](termbar::ProgressBar &self) { return &self; })
        .def("__exit__", [](termbar::ProgressBar &self, py::object, py::object, py::object) {
            self.finish();
        });
}