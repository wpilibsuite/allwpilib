#include <pybind11/pybind11.h>
#include <pybind11/functional.h>

#include "wpi/util/timestamp.hpp"

namespace py = pybind11;

// Global holder for the Python callback to keep it in scope
static py::object global_now_impl;

// Helper function to massage the python callback into a C api
uint64_t now_impl_trampoline() {
    py::gil_scoped_acquire acquire;
    return global_now_impl().cast<uint64_t>();
}

void set_now_impl(py::object func) {
    global_now_impl = func;
    if (func.is_none()) {
        wpi::util::SetNowImpl(nullptr);
    } else {
        wpi::util::SetNowImpl(&now_impl_trampoline);
    }
}