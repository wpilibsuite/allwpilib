#include "MockTelemetryBackendFunctions.h"

#include <stdint.h>

#include <concepts>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include <pybind11/stl.h>

namespace py = pybind11;

namespace wpi::telemetry::python {
namespace {

PyObject* gMockBackendValueTypes;

}  // namespace

void InitializeMockBackendValueTypes(py::module_& module) {
  py::module_ mockBackend = py::module_::import("telemetry.mock_backend");
  py::tuple valueTypes = py::make_tuple(
      mockBackend.attr("KeepDuplicatesValue"),
      mockBackend.attr("SetPropertyValue"), mockBackend.attr("LogStringValue"),
      mockBackend.attr("LogBooleanArrayValue"), mockBackend.attr("LogRawValue"),
      mockBackend.attr("Action"));
  auto* valueTypesPtr = valueTypes.ptr();
  py::capsule cleanup{static_cast<void*>(valueTypesPtr), [](void* value) {
                        auto* valueTypes = static_cast<PyObject*>(value);
                        if (gMockBackendValueTypes == valueTypes) {
                          gMockBackendValueTypes = nullptr;
                        }
                        Py_DECREF(valueTypes);
                      }};
  valueTypes.release();
  module.attr("_mock_backend_value_types") = cleanup;
  gMockBackendValueTypes = valueTypesPtr;
}

py::object GetMockBackendValueType(MockBackendValueType type) {
  if (!gMockBackendValueTypes) {
    throw std::runtime_error(
        "mock backend value type cache is not initialized");
  }
  PyObject* valueType =
      PyTuple_GetItem(gMockBackendValueTypes, static_cast<Py_ssize_t>(type));
  if (!valueType) {
    throw py::error_already_set{};
  }
  return py::reinterpret_borrow<py::object>(valueType);
}

std::optional<ActionValue> GetLastValue(
    const wpi::telemetry::MockTelemetryBackend& backend,
    std::string_view path) {
  auto* action = backend.GetLastAction(path);
  if (!action) {
    return std::nullopt;
  }

  return std::visit(
      [](const auto& value) -> ActionValue {
        using T = std::decay_t<decltype(value)>;
        if constexpr (
            std::same_as<
                T, wpi::telemetry::MockTelemetryBackend::KeepDuplicatesValue> ||
            std::same_as<
                T, wpi::telemetry::MockTelemetryBackend::SetPropertyValue> ||
            std::same_as<
                T, wpi::telemetry::MockTelemetryBackend::LogStringValue>) {
          return ActionValue{py::cast(value.value)};
        } else if constexpr (std::same_as<T,
                                          wpi::telemetry::MockTelemetryBackend::
                                              LogBooleanArrayValue>) {
          py::list result;
          for (int item : value.value) {
            result.append(item != 0);
          }
          return ActionValue{std::move(result)};
        } else if constexpr (std::same_as<T,
                                          wpi::telemetry::MockTelemetryBackend::
                                              LogRawValue>) {
          return ActionValue{
              py::bytes{reinterpret_cast<const char*>(value.value.data()),
                        value.value.size()}};
        } else {
          return ActionValue{py::cast(value)};
        }
      },
      action->value);
}

std::optional<ObjectDict> GetSchema(
    const wpi::telemetry::MockTelemetryBackend& backend,
    std::string_view schemaName) {
  auto* schema =
      const_cast<wpi::telemetry::MockTelemetryBackend&>(backend).GetSchema(
          schemaName);
  if (!schema) {
    return std::nullopt;
  }

  ObjectDict result;
  result["type"] = schema->type;
  result["schema_bytes"] =
      py::bytes{reinterpret_cast<const char*>(schema->schemaBytes.data()),
                schema->schemaBytes.size()};
  result["schema_string"] = schema->schemaString;
  return result;
}

}  // namespace wpi::telemetry::python
