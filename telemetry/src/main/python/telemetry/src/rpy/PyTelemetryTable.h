#pragma once

#include <string>
#include <string_view>

#include <pybind11/pybind11.h>
#include <wpi/telemetry/TelemetryTable.hpp>
#include <wpystruct.h>

namespace wpi::telemetry::python {

class PyTelemetryTable {
 public:
  explicit PyTelemetryTable(wpi::telemetry::TelemetryTable& table);

  std::string GetPath() const;
  bool SetType(std::string_view type);
  std::string GetType() const;
  bool HasType() const;
  PyTelemetryTable GetTable(std::string_view name) const;
  void KeepDuplicates(std::string_view name);
  void SetProperty(std::string_view name, std::string_view key,
                   std::string_view value);
  void Log(std::string_view name, pybind11::handle value,
           pybind11::object elementType, std::string_view typeString) const;

 private:
  enum class SequenceKind { BOOLEAN, INTEGER, DOUBLE, STRING, FALLBACK_STRING };

  static void AddStructSchemas(
      const wpi::telemetry::TelemetryTable::EntryHandle& entry,
      std::string_view typeString, const WPyStructInfo& info);
  static bool IsBytesLike(pybind11::handle value);
  static std::string BytesLikeToString(pybind11::handle value);
  wpi::telemetry::TelemetryTable::EntryHandle GetEntry(
      std::string_view name) const;
  static void ValidateNoElementType(pybind11::handle elementType);
  static void ValidateNoTypeString(std::string_view typeString);
  static void ValidateNoExplicitTelemetryType(pybind11::handle elementType,
                                              std::string_view typeString);
  static SequenceKind KindFromElementType(pybind11::handle elementType);
  void LogObject(std::string_view name, pybind11::handle value,
                 pybind11::handle logTo) const;
  void LogSequence(std::string_view name, const pybind11::sequence& value,
                   pybind11::handle valueType) const;
  void LogStruct(std::string_view name, pybind11::handle value) const;
  void LogStructSequence(std::string_view name, const pybind11::sequence& value,
                         const pybind11::type& type) const;
  void LogStructSequenceImpl(
      const wpi::telemetry::TelemetryTable::EntryHandle& entry,
      const pybind11::sequence& value, const pybind11::type& type) const;

  wpi::telemetry::TelemetryTable* m_table;
};

}  // namespace wpi::telemetry::python
