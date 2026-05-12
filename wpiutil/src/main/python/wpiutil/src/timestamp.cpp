#include <pybind11/pybind11.h>
#include <pybind11/functional.h>

#include "wpi/util/timestamp.hpp"

namespace py = pybind11;

py::object &get_now_impl_ref() {
  static py::object get_now_impl_ref;
  return get_now_impl_ref;
}

// Helper function to massage the python callback into a C api
uint64_t now_impl_trampoline() {
    py::gil_scoped_acquire acquire;
    return get_now_impl_ref()().cast<uint64_t>();
}

void set_now_impl(py::object func) {
    get_now_impl_ref() = func;
    if (func.is_none()) {
        wpi::util::SetNowImpl(nullptr);
    } else {
        wpi::util::SetNowImpl(&now_impl_trampoline);
    }
}

void cleanup_now_impl() {
  wpi::util::SetNowImpl(nullptr);

  // release the function during interpreter shutdown
  auto &hook = get_now_impl_ref();
  if (hook) {
    hook.dec_ref();
    hook.release();
  }
}