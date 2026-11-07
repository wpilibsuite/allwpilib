#include "wpi/util/UsageReporting.hpp"

#include <pybind11/pybind11.h>

#include "wpi/util/string.hpp"

namespace py = pybind11;

py::object& get_report_usage_impl_ref() {
  static py::object report_usage_impl;
  return report_usage_impl;
}

void report_usage_impl_trampoline(const WPI_String* resource,
                                  const WPI_String* data) {
  py::gil_scoped_acquire acquire;
  try {
    auto& hook = get_report_usage_impl_ref();
    if (hook) {
      auto resourceView = wpi::util::to_string_view(resource);
      auto dataView = wpi::util::to_string_view(data);
      hook(py::str(resourceView.data(), resourceView.size()),
           py::str(dataView.data(), dataView.size()));
    }
  } catch (py::error_already_set& e) {
    e.discard_as_unraisable("wpiutil.report_usage_impl_trampoline");
  }
}

void set_report_usage_impl(py::object func) {
  get_report_usage_impl_ref() = func;
  if (func.is_none()) {
    wpi::util::SetReportUsageImpl(nullptr);
  } else {
    wpi::util::SetReportUsageImpl(&report_usage_impl_trampoline);
  }
}

void cleanup_report_usage_impl() {
  wpi::util::SetReportUsageImpl(nullptr);

  auto& hook = get_report_usage_impl_ref();
  if (hook) {
    hook.dec_ref();
    hook.release();
  }
}
