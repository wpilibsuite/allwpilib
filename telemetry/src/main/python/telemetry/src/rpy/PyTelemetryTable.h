#pragma once

#include <string>
#include <string_view>

#include <pybind11/pybind11.h>
#include <pybind11/typing.h>

#include "wpi/telemetry/TelemetryTable.hpp"
#include "wpystruct.h"

namespace wpi::telemetry::python {

/**
 * Telemetry sends information from the robot program to dashboards, debug
 * tools, or log files.
 *
 * For more advanced use cases, use the NetworkTables or DataLog APIs.
 */
class PyTelemetryTable {
 public:
  explicit PyTelemetryTable(wpi::telemetry::TelemetryTable& table);

  /**
   * Gets the table path.
   *
   * @return path with trailing "/"
   */
  std::string GetPath() const;

  /**
   * Sets the table type.  TelemetryLoggable implementations can use this
   * function to communicate the type of data in the table.  Callers should
   * check the return value and not log data if false is returned.
   *
   * @param type type string
   * @return False if type mismatch.
   */
  bool SetType(std::string_view type);

  /**
   * Gets the table type.
   *
   * @return Table type as set by SetType(), or empty string if none set.
   */
  std::string GetType() const;

  /**
   * Returns if a table type is set.
   *
   * @return True if type set by SetType(), false otherwise
   */
  bool HasType() const;

  /**
   * Gets a child telemetry table.
   *
   * @param name table name
   * @return table
   */
  PyTelemetryTable GetTable(std::string_view name) const;

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

  /**
   * Logs a telemetry value.
   *
   * Sequences must pass an explicit element_type. Use bool, int, float, or str
   * for primitive arrays, object to log a string array using str() for each
   * element, or a WPIStruct class for struct arrays. type_string is only used
   * as custom type metadata for scalar str and bytes-like values (bytes,
   * bytearray, and memoryview). Wrap other buffer exporters in memoryview to
   * explicitly log their raw bytes in C order.
   */
  void Log(
      std::string_view name, pybind11::object value,
      pybind11::typing::Optional<pybind11::typing::Type<pybind11::object>>
          elementType = pybind11::none(),
      std::string_view typeString = "") const;

 private:
  using ElementType = pybind11::typing::Type<pybind11::object>;

  enum class SequenceKind {
    BOOLEAN,
    INTEGER,
    DOUBLE,
    STRING,
    FALLBACK_STRING,
  };

  static void AddStructSchemas(
      const wpi::telemetry::TelemetryTable::EntryHandle& entry,
      std::string_view typeString, const WPyStructInfo& info);

  wpi::telemetry::TelemetryTable::EntryHandle GetEntry(
      std::string_view name) const;
  static void ValidateNoElementType(
      const pybind11::typing::Optional<ElementType>& elementType);
  static void ValidateNoTypeString(std::string_view typeString);
  static void ValidateNoExplicitTelemetryType(
      const pybind11::typing::Optional<ElementType>& elementType,
      std::string_view typeString);
  static SequenceKind KindFromElementType(pybind11::handle elementType);
  void LogObject(std::string_view name, pybind11::handle value,
                 pybind11::handle logTo) const;
  void LogSequence(std::string_view name, const pybind11::sequence& value,
                   pybind11::handle valueType) const;
  void LogStruct(std::string_view name, pybind11::handle value) const;
  void LogStructSequence(std::string_view name,
                         const pybind11::sequence& value,
                         const pybind11::type& type) const;
  void LogStructSequenceImpl(
      const wpi::telemetry::TelemetryTable::EntryHandle& entry,
      const pybind11::sequence& value, const pybind11::type& type) const;

  wpi::telemetry::TelemetryTable* m_table;
};

}  // namespace wpi::telemetry::python
