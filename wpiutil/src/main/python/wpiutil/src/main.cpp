
#include "semiwrap_init.wpiutil._wpiutil.hpp"

void setup_stack_trace_hook(py::object fn);
void cleanup_stack_trace_hook();

void setup_safethread_gil();
void cleanup_safethread_gil();

void set_now_impl(py::object fn);
void cleanup_now_impl();

SEMIWRAP_PYBIND11_MODULE(m) {
  initWrapper(m);

  static int unused;
  py::capsule cleanup(&unused, [](void*) {
    cleanup_stack_trace_hook();
    cleanup_safethread_gil();
    cleanup_now_impl();
  });

  setup_safethread_gil();

  m.def("_setup_stack_trace_hook", &setup_stack_trace_hook);
  m.def("set_now_impl", &set_now_impl);
  m.add_object("_st_cleanup", cleanup);
}
