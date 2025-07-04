
#include <semiwrap_init.wpiutil._wpiutil.hpp>

void setup_stack_trace_hook(py::object fn);
void cleanup_stack_trace_hook();

void setup_safethread_gil();
void cleanup_safethread_gil();

#ifndef __FRC_ROBORIO__

namespace wpi::impl {
void ResetSendableRegistry();
} // namespace wpi::impl

void cleanup_sendable_registry() {
  py::gil_scoped_release unlock;
  wpi::impl::ResetSendableRegistry();
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
  });

  setup_safethread_gil();

  m.def("_setup_stack_trace_hook", &setup_stack_trace_hook);
  m.add_object("_st_cleanup", cleanup);
}