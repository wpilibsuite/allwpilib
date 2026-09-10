#include "TelemetryFunctions.h"

#include <utility>

#include "wpi/telemetry/Telemetry.hpp"

namespace wpi::telemetry::python {

PyTelemetryTable GetTable(std::string_view name) {
  auto& root = wpi::telemetry::GetTable();
  return PyTelemetryTable{name.empty() ? root : root.GetTable(name)};
}

void Log(std::string_view name, pybind11::object value,
         pybind11::typing::Optional<pybind11::typing::Type<pybind11::object>>
             elementType,
         std::string_view typeString) {
  PyTelemetryTable{wpi::telemetry::GetTable()}.Log(
      name, std::move(value), std::move(elementType), typeString);
}

void KeepDuplicates(std::string_view name) {
  wpi::telemetry::KeepDuplicates(name);
}

void SetProperty(std::string_view name, std::string_view key,
                 std::string_view value) {
  wpi::telemetry::SetProperty(name, key, value);
}

}  // namespace wpi::telemetry::python
