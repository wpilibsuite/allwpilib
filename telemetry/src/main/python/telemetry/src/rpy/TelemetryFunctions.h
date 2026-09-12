#pragma once

#include <string_view>

#include <pybind11/pybind11.h>
#include <pybind11/typing.h>

#include "PyTelemetryTable.h"

namespace wpi::telemetry::python {

/**
 * Gets a child telemetry table (or root table if not specified)
 *
 * @param name table name
 * @return table
 */
PyTelemetryTable GetTable(std::string_view name = "");

/**
 * Logs a telemetry value.
 *
 * Sequences must pass an explicit element_type. Use bool, int, float, or str
 * for primitive arrays, object to log a string array using str() for each
 * element, or a WPIStruct class for struct arrays. type_string is only used as
 * custom type metadata for scalar str and bytes-like values (bytes,
 * bytearray, and memoryview). Wrap other buffer exporters in memoryview to
 * explicitly log their raw bytes in C order.
 */
void Log(std::string_view name, pybind11::object value,
         pybind11::typing::Optional<pybind11::typing::Type<pybind11::object>>
             elementType = pybind11::none(),
         std::string_view typeString = "");

/**
 * Indicates duplicate values should be preserved. Normally duplicate values
 * are ignored.
 *
 * @param name the name
 */
void KeepDuplicates(std::string_view name);

/**
 * Sets property for a value. Properties are stored as a key/value map.
 *
 * @param name the name
 * @param key property key
 * @param value property value
 */
void SetProperty(std::string_view name, std::string_view key,
                 std::string_view value);

}  // namespace wpi::telemetry::python
