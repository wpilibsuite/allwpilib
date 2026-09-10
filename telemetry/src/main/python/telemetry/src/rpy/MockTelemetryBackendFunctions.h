#pragma once

#include <optional>
#include <string_view>
#include <utility>

#include <pybind11/pybind11.h>
#include <pybind11/typing.h>

#include "wpi/telemetry/MockTelemetryBackend.hpp"

namespace wpi::telemetry::python {

class ActionValue : public pybind11::object {
 public:
  using pybind11::object::object;

  ActionValue(pybind11::object&& value)
      : pybind11::object{std::move(value)} {}
};

using ObjectDict = pybind11::typing::Dict<pybind11::str, pybind11::object>;

enum class MockBackendValueType {
  KEEP_DUPLICATES,
  SET_PROPERTY,
  LOG_STRING,
  LOG_BOOLEAN_ARRAY,
  LOG_RAW,
  ACTION,
};

void InitializeMockBackendValueTypes(pybind11::module_& module);
pybind11::object GetMockBackendValueType(MockBackendValueType type);

std::optional<ActionValue> GetLastValue(
    const wpi::telemetry::MockTelemetryBackend& backend,
    std::string_view path);

std::optional<ObjectDict> GetSchema(
    const wpi::telemetry::MockTelemetryBackend& backend,
    std::string_view schemaName);

}  // namespace wpi::telemetry::python

namespace pybind11::detail {

template <>
struct handle_type_name<wpi::telemetry::python::ActionValue> {
  static constexpr auto name =
      const_name("telemetry.mock_backend.ActionValue");
};

}  // namespace pybind11::detail
