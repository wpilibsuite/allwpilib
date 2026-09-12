#include "MockTelemetryBackendFunctions.h"

#include <stdint.h>

#include <concepts>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include <pybind11/stl.h>

namespace py = pybind11;

namespace wpi::telemetry::python {

py::object ActionValueToPython(
    const wpi::telemetry::MockTelemetryBackend::Action& action) {
  py::dict result;
  result["path"] = action.path;
  result["timestamp"] = action.timestamp;
  std::visit(
      [&](const auto& value) {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::same_as<T, wpi::telemetry::MockTelemetryBackend::
                                          KeepDuplicatesValue>) {
          result["kind"] = "keep_duplicates";
          result["value"] = value.value;
        } else if constexpr (std::same_as<T,
                                          wpi::telemetry::MockTelemetryBackend::
                                              SetPropertyValue>) {
          result["kind"] = "set_property";
          result["key"] = value.key;
          result["value"] = value.value;
        } else if constexpr (std::same_as<T, bool>) {
          result["kind"] = "boolean";
          result["value"] = value;
        } else if constexpr (std::same_as<T, int16_t> ||
                             std::same_as<T, int32_t> ||
                             std::same_as<T, int64_t>) {
          result["kind"] = "integer";
          result["value"] = value;
        } else if constexpr (std::same_as<T, float> ||
                             std::same_as<T, double>) {
          result["kind"] = "double";
          result["value"] = value;
        } else if constexpr (std::same_as<T,
                                          wpi::telemetry::MockTelemetryBackend::
                                              LogStringValue>) {
          result["kind"] = "string";
          result["value"] = value.value;
          result["type_string"] = value.typeString;
        } else if constexpr (std::same_as<T,
                                          wpi::telemetry::MockTelemetryBackend::
                                              LogBooleanArrayValue>) {
          result["kind"] = "boolean[]";
          py::list list;
          for (int item : value.value) {
            list.append(item != 0);
          }
          result["value"] = std::move(list);
        } else if constexpr (std::same_as<T, std::vector<int16_t>> ||
                             std::same_as<T, std::vector<int32_t>> ||
                             std::same_as<T, std::vector<int64_t>>) {
          result["kind"] = "integer[]";
          result["value"] = value;
        } else if constexpr (std::same_as<T, std::vector<float>> ||
                             std::same_as<T, std::vector<double>>) {
          result["kind"] = "double[]";
          result["value"] = value;
        } else if constexpr (std::same_as<T, std::vector<std::string>>) {
          result["kind"] = "string[]";
          result["value"] = value;
        } else if constexpr (std::same_as<T,
                                          wpi::telemetry::MockTelemetryBackend::
                                              LogRawValue>) {
          result["kind"] = "raw";
          result["value"] =
              py::bytes{reinterpret_cast<const char*>(value.value.data()),
                        value.value.size()};
          result["type_string"] = value.typeString;
        }
      },
      action.value);
  return std::move(result);
}

py::object SchemaToPython(
    const wpi::telemetry::MockTelemetryBackend::Schema* schema) {
  if (!schema) {
    return py::none{};
  }
  py::dict result;
  result["type"] = schema->type;
  result["schema_bytes"] =
      py::bytes{reinterpret_cast<const char*>(schema->schemaBytes.data()),
                schema->schemaBytes.size()};
  result["schema_string"] = schema->schemaString;
  return std::move(result);
}

}  // namespace wpi::telemetry::python
