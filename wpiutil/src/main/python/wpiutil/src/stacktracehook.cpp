
#include <semiwrap.h>
#include <wpi/StackTrace.h>

py::object &get_hook_ref() {
  static py::object hook;
  return hook;
}

std::string final_py_stack_trace_hook(int offset) {
  std::string msg = "\tat <python stack trace not available due to interpreter shutdown>\n";
  msg += wpi::GetStackTraceDefault(offset);
  return msg;
}

std::string py_stack_trace_hook(int offset) {
  py::gil_scoped_acquire gil;

  try {
    auto &hook = get_hook_ref();
    if (hook) {
      return py::cast<std::string>(hook(offset));
    }
  } catch (py::error_already_set &e) {
    e.discard_as_unraisable("wpiutil._stacktrace._stack_trace_hook");
  }

  return wpi::GetStackTraceDefault(offset);
}

void setup_stack_trace_hook(py::object fn) {
  get_hook_ref() = fn;
  wpi::SetGetStackTraceImpl(py_stack_trace_hook);
}

void cleanup_stack_trace_hook() {
  wpi::SetGetStackTraceImpl(final_py_stack_trace_hook);

  // release the function during interpreter shutdown
  auto &hook = get_hook_ref();
  if (hook) {
    hook.dec_ref();
    hook.release();
  }
}