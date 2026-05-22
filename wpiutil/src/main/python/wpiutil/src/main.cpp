
#include <semiwrap_init.wpiutil._wpiutil.hpp>

void setup_stack_trace_hook(py::object fn);
void cleanup_stack_trace_hook();

void setup_safethread_gil();
void cleanup_safethread_gil();

void set_now_impl(py::object fn);
void cleanup_now_impl();

#ifndef __FIRST_SYSTEMCORE__

namespace wpi::util::impl {
void ResetSendableRegistry();
} // namespace wpi::util::impl

void cleanup_sendable_registry() {
  py::gil_scoped_release unlock;
  wpi::util::impl::ResetSendableRegistry();
}

#else

void cleanup_sendable_registry() {}

#endif

SEMIWRAP_PYBIND11_MODULE(m) {
  initWrapper(m);

  static int unused;
  py::capsule cleanup(&unused, [](void *) {
    cleanup_sendable_registry();
    cleanup_stack_trace_hook();
    cleanup_safethread_gil();
    cleanup_now_impl();
  });

  setup_safethread_gil();

  m.def("_setup_stack_trace_hook", &setup_stack_trace_hook);
  m.def("SetNowImpl", &set_now_impl);
  m.add_object("_st_cleanup", cleanup);
}
