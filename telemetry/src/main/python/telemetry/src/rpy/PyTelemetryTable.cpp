#include "PyTelemetryTable.h"

#include <stdint.h>

#include <memory>
#include <optional>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include <wpybuffer.h>

#include "pybind11/stl.h"
#include "wpi/telemetry/TelemetryBackend.hpp"
#include "wpi/telemetry/TelemetryEntry.hpp"
#include "wpi/telemetry/TelemetryLoggable.hpp"
#include "wpi/telemetry/TelemetryRegistry.hpp"

namespace py = pybind11;

namespace {

using ElementType = py::typing::Type<py::object>;

bool IsWpiStruct(py::handle value) {
  return py::hasattr(py::type::of(value), "WPIStruct");
}

bool IsWpiStructType(py::handle value) {
  return PyType_Check(value.ptr()) && py::hasattr(value, "WPIStruct");
}

std::optional<py::object> GetOptionalAttr(py::handle value, const char* name) {
#if PY_VERSION_HEX >= 0x030D0000
  PyObject* attr = nullptr;
  int result = PyObject_GetOptionalAttrString(value.ptr(), name, &attr);
  if (result < 0) {
    throw py::error_already_set{};
  }
  if (result == 0) {
    return std::nullopt;
  }
  return py::reinterpret_steal<py::object>(attr);
#else
  PyObject* attr = PyObject_GetAttrString(value.ptr(), name);
  if (attr) {
    return py::reinterpret_steal<py::object>(attr);
  }
  if (PyErr_ExceptionMatches(PyExc_AttributeError)) {
    PyErr_Clear();
    return std::nullopt;
  }
  throw py::error_already_set{};
#endif
}

void ValidateStructSequenceType(const py::sequence& value,
                                const py::type& type) {
  const size_t size = py::len(value);
  for (size_t i = 0; i < size; ++i) {
    py::handle item = value[static_cast<py::ssize_t>(i)];
    int isInstance = PyObject_IsInstance(item.ptr(), type.ptr());
    if (isInstance < 0) {
      throw py::error_already_set{};
    }
    if (isInstance == 0) {
      throw py::type_error(
          "struct telemetry arrays require values of the specified "
          "WPIStruct type");
    }
  }
}

}  // namespace

namespace wpi::telemetry::python {

PyTelemetryTable::PyTelemetryTable(wpi::telemetry::TelemetryTable& table)
    : m_table{&table} {}

std::string PyTelemetryTable::GetPath() const {
  return std::string{m_table->GetPath()};
}

bool PyTelemetryTable::SetType(std::string_view type) {
  return m_table->SetType(type);
}

std::string PyTelemetryTable::GetType() const {
  return m_table->GetType();
}

bool PyTelemetryTable::HasType() const {
  return m_table->HasType();
}

PyTelemetryTable PyTelemetryTable::GetTable(std::string_view name) const {
  return PyTelemetryTable{m_table->GetTable(name)};
}

void PyTelemetryTable::KeepDuplicates(std::string_view name) {
  m_table->KeepDuplicates(name);
}

void PyTelemetryTable::SetProperty(std::string_view name, std::string_view key,
                                   std::string_view value) {
  m_table->SetProperty(name, key, value);
}

void PyTelemetryTable::Log(std::string_view name, py::object value,
                           py::typing::Optional<ElementType> elementType,
                           std::string_view typeString) const {
  if (py::isinstance<py::bool_>(value)) {
    ValidateNoExplicitTelemetryType(elementType, typeString);
    auto entry = GetEntry(name);
    if (!entry->IsDiscard()) {
      entry->LogBoolean(value.cast<bool>(), 0);
    }
  } else if (py::isinstance<py::int_>(value)) {
    ValidateNoExplicitTelemetryType(elementType, typeString);
    auto entry = GetEntry(name);
    if (!entry->IsDiscard()) {
      entry->LogInt64(value.cast<int64_t>(), 0);
    }
  } else if (py::isinstance<py::float_>(value)) {
    ValidateNoExplicitTelemetryType(elementType, typeString);
    auto entry = GetEntry(name);
    if (!entry->IsDiscard()) {
      entry->LogDouble(value.cast<double>(), 0);
    }
  } else if (py::isinstance<py::str>(value)) {
    ValidateNoElementType(elementType);
    auto entry = GetEntry(name);
    if (!entry->IsDiscard()) {
      auto str = value.cast<std::string>();
      entry->LogString(str, typeString.empty() ? "string" : typeString, 0);
    }
  } else if (wpi::util::python::IsBytesLike(value)) {
    ValidateNoElementType(elementType);
    auto entry = GetEntry(name);
    if (!entry->IsDiscard()) {
      auto raw = wpi::util::python::RequestContiguousBuffer(
          py::reinterpret_borrow<py::buffer>(value));
      auto data =
          std::span<const uint8_t>{static_cast<const uint8_t*>(raw.ptr),
                                   static_cast<size_t>(raw.view()->len)};
      entry->LogRaw(data, typeString.empty() ? "raw" : typeString, 0);
    }
  } else if (auto logTo = GetOptionalAttr(value, "log_to")) {
    ValidateNoExplicitTelemetryType(elementType, typeString);
    LogObject(name, value, *logTo);
  } else if (IsWpiStruct(value)) {
    ValidateNoExplicitTelemetryType(elementType, typeString);
    LogStruct(name, value);
  } else if (PySequence_Check(value.ptr())) {
    ValidateNoTypeString(typeString);
    if (elementType.is_none()) {
      throw py::type_error("sequence element type must be specified");
    }
    auto sequence = py::reinterpret_borrow<py::sequence>(value);
    if (IsWpiStructType(elementType)) {
      LogStructSequence(name, sequence,
                        py::reinterpret_borrow<py::type>(elementType));
    } else {
      LogSequence(name, sequence, elementType);
    }
  } else {
    ValidateNoExplicitTelemetryType(elementType, typeString);
    auto entry = GetEntry(name);
    if (!entry->IsDiscard()) {
      entry->LogString(py::str(value).cast<std::string>(), "string", 0);
    }
  }
}

void PyTelemetryTable::AddStructSchemas(
    const wpi::telemetry::TelemetryTable::EntryHandle& entry,
    std::string_view typeString, const WPyStructInfo& info) {
  if (entry.HasPublishedSchema(typeString)) {
    return;
  }
  wpi::util::ForEachStructSchema<WPyStruct>(
      [&](std::string_view schemaType, std::string_view schema) {
        wpi::telemetry::TelemetryRegistry::AddSchema(
            entry.GetBackend(), schemaType, "structschema", schema);
      },
      info);
  entry.MarkSchemaPublished(typeString);
}

wpi::telemetry::TelemetryTable::EntryHandle PyTelemetryTable::GetEntry(
    std::string_view name) const {
  return m_table->GetEntry(name);
}

void PyTelemetryTable::ValidateNoElementType(
    const py::typing::Optional<ElementType>& elementType) {
  if (!elementType.is_none()) {
    throw py::type_error(
        "element_type is only supported for telemetry sequences");
  }
}

void PyTelemetryTable::ValidateNoTypeString(std::string_view typeString) {
  if (!typeString.empty()) {
    throw py::type_error(
        "type_string is only supported for scalar string and raw telemetry "
        "values");
  }
}

void PyTelemetryTable::ValidateNoExplicitTelemetryType(
    const py::typing::Optional<ElementType>& elementType,
    std::string_view typeString) {
  ValidateNoElementType(elementType);
  ValidateNoTypeString(typeString);
}

PyTelemetryTable::SequenceKind PyTelemetryTable::KindFromElementType(
    py::handle elementType) {
  if (py::isinstance<py::str>(elementType)) {
    throw py::type_error("telemetry element_type must be a Python type");
  }
  if (elementType.is(py::handle{reinterpret_cast<PyObject*>(&PyBool_Type)})) {
    return SequenceKind::BOOLEAN;
  }
  if (elementType.is(py::handle{reinterpret_cast<PyObject*>(&PyLong_Type)})) {
    return SequenceKind::INTEGER;
  }
  if (elementType.is(py::handle{reinterpret_cast<PyObject*>(&PyFloat_Type)})) {
    return SequenceKind::DOUBLE;
  }
  if (elementType.is(
          py::handle{reinterpret_cast<PyObject*>(&PyUnicode_Type)})) {
    return SequenceKind::STRING;
  }
  if (elementType.is(
          py::handle{reinterpret_cast<PyObject*>(&PyBaseObject_Type)})) {
    return SequenceKind::FALLBACK_STRING;
  }
  throw py::type_error("unsupported telemetry element_type");
}

void PyTelemetryTable::LogObject(std::string_view name, py::handle value,
                                 py::handle logTo) const {
  if (!m_table->ShouldLogTableValue(name)) {
    return;
  }

  auto& child = m_table->GetTable(name);
  if (auto getTelemetryType = GetOptionalAttr(value, "get_telemetry_type")) {
    py::object typeObj = (*getTelemetryType)();
    if (!typeObj.is_none()) {
      auto type = typeObj.cast<std::string>();
      if (!type.empty() && !child.SetType(type)) {
        return;
      }
    }
  }

  if (py::isinstance<wpi::telemetry::TelemetryLoggable>(value)) {
    logTo(py::cast(&child, py::return_value_policy::reference));
  } else {
    logTo(PyTelemetryTable{child});
  }
}

void PyTelemetryTable::LogSequence(std::string_view name,
                                   const py::sequence& value,
                                   py::handle valueType) const {
  auto entry = GetEntry(name);
  if (entry->IsDiscard()) {
    return;
  }

  SequenceKind kind = KindFromElementType(valueType);
  const size_t size = py::len(value);
  switch (kind) {
    case SequenceKind::BOOLEAN: {
      auto data = std::make_unique<bool[]>(size);
      for (size_t i = 0; i < size; ++i) {
        py::handle item = value[static_cast<py::ssize_t>(i)];
        if (!py::isinstance<py::bool_>(item)) {
          throw py::type_error("boolean telemetry arrays require bool values");
        }
        data[i] = item.cast<bool>();
      }
      entry->LogBooleanArray(std::span<const bool>{data.get(), size}, 0);
      break;
    }
    case SequenceKind::INTEGER: {
      std::vector<int64_t> data;
      data.reserve(size);
      for (size_t i = 0; i < size; ++i) {
        py::handle item = value[static_cast<py::ssize_t>(i)];
        if (!py::isinstance<py::int_>(item) ||
            py::isinstance<py::bool_>(item)) {
          throw py::type_error("integer telemetry arrays require int values");
        }
        data.emplace_back(item.cast<int64_t>());
      }
      entry->LogInt64Array(std::span<const int64_t>{data}, 0);
      break;
    }
    case SequenceKind::DOUBLE: {
      std::vector<double> data;
      data.reserve(size);
      for (size_t i = 0; i < size; ++i) {
        py::handle item = value[static_cast<py::ssize_t>(i)];
        if ((!py::isinstance<py::int_>(item) ||
             py::isinstance<py::bool_>(item)) &&
            !py::isinstance<py::float_>(item)) {
          throw py::type_error(
              "double telemetry arrays require int or float values");
        }
        data.emplace_back(item.cast<double>());
      }
      entry->LogDoubleArray(std::span<const double>{data}, 0);
      break;
    }
    case SequenceKind::STRING: {
      std::vector<std::string> data;
      data.reserve(size);
      for (size_t i = 0; i < size; ++i) {
        py::handle item = value[static_cast<py::ssize_t>(i)];
        if (!py::isinstance<py::str>(item)) {
          throw py::type_error("string telemetry arrays require str values");
        }
        data.emplace_back(item.cast<std::string>());
      }
      entry->LogStringArray(std::span<const std::string>{data}, 0);
      break;
    }
    case SequenceKind::FALLBACK_STRING: {
      std::vector<std::string> data;
      data.reserve(size);
      for (size_t i = 0; i < size; ++i) {
        data.emplace_back(
            py::str(value[static_cast<py::ssize_t>(i)]).cast<std::string>());
      }
      entry->LogStringArray(std::span<const std::string>{data}, 0);
      break;
    }
  }
}

void PyTelemetryTable::LogStruct(std::string_view name,
                                 py::handle value) const {
  auto entry = GetEntry(name);
  if (entry->IsDiscard()) {
    return;
  }

  WPyStructInfo info{py::type::of(value)};
  std::string typeString{wpi::util::GetStructTypeString<WPyStruct>(info)};
  AddStructSchemas(entry, typeString, info);

  std::vector<uint8_t> data(wpi::util::GetStructSize<WPyStruct>(info));
  WPyStruct wrapped{py::reinterpret_borrow<py::object>(value)};
  wpi::util::PackStruct(std::span<uint8_t>{data}, wrapped, info);
  entry->LogRaw(std::span<const uint8_t>{data}, typeString, 0);
}

void PyTelemetryTable::LogStructSequence(std::string_view name,
                                         const py::sequence& value,
                                         const py::type& type) const {
  auto entry = GetEntry(name);
  if (entry->IsDiscard()) {
    return;
  }

  ValidateStructSequenceType(value, type);
  LogStructSequenceImpl(entry, value, type);
}

void PyTelemetryTable::LogStructSequenceImpl(
    const wpi::telemetry::TelemetryTable::EntryHandle& entry,
    const py::sequence& value, const py::type& type) const {
  WPyStructInfo info{type};
  std::string typeString{wpi::util::GetStructTypeString<WPyStruct>(info)};
  AddStructSchemas(entry, typeString, info);

  const size_t size = py::len(value);
  const size_t structSize = wpi::util::GetStructSize<WPyStruct>(info);
  std::vector<uint8_t> data;
  data.resize(size * structSize);
  for (size_t i = 0; i < size; ++i) {
    WPyStruct wrapped{
        py::reinterpret_borrow<py::object>(value[static_cast<py::ssize_t>(i)])};
    wpi::util::PackStruct(
        std::span<uint8_t>{data}.subspan(i * structSize, structSize), wrapped,
        info);
  }

  typeString += "[]";
  entry->LogRaw(std::span<const uint8_t>{data}, typeString, 0);
}

}  // namespace wpi::telemetry::python
