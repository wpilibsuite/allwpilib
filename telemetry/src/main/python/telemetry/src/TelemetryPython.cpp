#include "TelemetryPython.h"

#include <stdint.h>

#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

#include "pybind11/functional.h"
#include "pybind11/stl.h"
#include "wpi/telemetry/DiscardTelemetryBackend.hpp"
#include "wpi/telemetry/MockTelemetryBackend.hpp"
#include "wpi/telemetry/MultiTelemetryBackend.hpp"
#include "wpi/telemetry/Telemetry.hpp"
#include "wpi/telemetry/TelemetryBackend.hpp"
#include "wpi/telemetry/TelemetryEntry.hpp"
#include "wpi/telemetry/TelemetryLoggable.hpp"
#include "wpi/telemetry/TelemetryRegistry.hpp"
#include "wpi/telemetry/TelemetryTable.hpp"
#include "wpystruct.h"

namespace py = pybind11;

namespace {

bool IsWpiStruct(py::handle value) {
  return py::hasattr(py::type::of(value), "WPIStruct");
}

bool IsWpiStructType(py::handle value) {
  return PyType_Check(value.ptr()) && py::hasattr(value, "WPIStruct");
}

bool IsBuiltinType(py::handle value, const char* name) {
  return value.is(py::module_::import("builtins").attr(name));
}

bool IsNoElementType(py::handle elementType) {
  return elementType.is_none();
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

constexpr const char* kLogDoc = R"doc(Logs a telemetry value.

Sequences must pass an explicit element_type. Use bool, int, float, or str for
primitive arrays, object to log a string array using str() for each element, or
a WPIStruct class for struct arrays. type_string is only used as custom type
metadata for scalar str and bytes-like values.)doc";

}  // namespace

namespace wpi::telemetry::python {

class PyTelemetryTable {
 public:
  explicit PyTelemetryTable(wpi::TelemetryTable& table) : m_table{&table} {}

  std::string GetPath() const { return std::string{m_table->GetPath()}; }
  bool SetType(std::string_view type) { return m_table->SetType(type); }
  std::string GetType() const { return m_table->GetType(); }
  bool HasType() const { return m_table->HasType(); }
  PyTelemetryTable GetTable(std::string_view name) const {
    return PyTelemetryTable{m_table->GetTable(name)};
  }
  void KeepDuplicates(std::string_view name) { m_table->KeepDuplicates(name); }
  void SetProperty(std::string_view name, std::string_view key,
                   std::string_view value) {
    m_table->SetProperty(name, key, value);
  }

  void Log(std::string_view name, py::handle value, py::object elementType,
           std::string_view typeString) const {
    if (py::isinstance<py::bool_>(value)) {
      ValidateNoExplicitTelemetryType(elementType, typeString);
      auto entry = GetEntry(name);
      if (!entry->IsDiscard()) {
        entry->LogBoolean(value.cast<bool>());
      }
    } else if (py::isinstance<py::int_>(value)) {
      ValidateNoExplicitTelemetryType(elementType, typeString);
      auto entry = GetEntry(name);
      if (!entry->IsDiscard()) {
        entry->LogInt64(value.cast<int64_t>());
      }
    } else if (py::isinstance<py::float_>(value)) {
      ValidateNoExplicitTelemetryType(elementType, typeString);
      auto entry = GetEntry(name);
      if (!entry->IsDiscard()) {
        entry->LogDouble(value.cast<double>());
      }
    } else if (py::isinstance<py::str>(value)) {
      ValidateNoElementType(elementType);
      auto entry = GetEntry(name);
      if (!entry->IsDiscard()) {
        auto str = value.cast<std::string>();
        entry->LogString(str, typeString.empty() ? "string" : typeString);
      }
    } else if (IsBytesLike(value)) {
      ValidateNoElementType(elementType);
      auto entry = GetEntry(name);
      if (!entry->IsDiscard()) {
        auto raw = BytesLikeToString(value);
        auto data = std::span<const uint8_t>{
            reinterpret_cast<const uint8_t*>(raw.data()), raw.size()};
        entry->LogRaw(data, typeString.empty() ? "raw" : typeString);
      }
    } else if (auto logTo = GetOptionalAttr(value, "log_to")) {
      ValidateNoExplicitTelemetryType(elementType, typeString);
      LogObject(name, value, *logTo);
    } else if (IsWpiStruct(value)) {
      ValidateNoExplicitTelemetryType(elementType, typeString);
      LogStruct(name, value);
    } else if (PySequence_Check(value.ptr())) {
      ValidateNoTypeString(typeString);
      if (IsNoElementType(elementType)) {
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
        entry->LogString(py::str(value).cast<std::string>(), "string");
      }
    }
  }

 private:
  enum class SequenceKind {
    kBoolean,
    kInteger,
    kDouble,
    kString,
    kFallbackString,
  };

  static void AddStructSchemas(const wpi::TelemetryTable::EntryHandle& entry,
                               std::string_view typeString,
                               const WPyStructInfo& info) {
    if (entry.HasPublishedSchema(typeString)) {
      return;
    }
    wpi::util::ForEachStructSchema<WPyStruct>(
        [&](std::string_view schemaType, std::string_view schema) {
          wpi::TelemetryRegistry::AddSchema(entry.GetBackend(), schemaType,
                                            "structschema", schema);
        },
        info);
    entry.MarkSchemaPublished(typeString);
  }

  static bool IsBytesLike(py::handle value) {
    return PyBytes_Check(value.ptr()) || PyByteArray_Check(value.ptr()) ||
           PyMemoryView_Check(value.ptr());
  }

  static std::string BytesLikeToString(py::handle value) {
    py::object bytes =
        py::reinterpret_steal<py::object>(PyBytes_FromObject(value.ptr()));
    if (!bytes) {
      throw py::error_already_set{};
    }
    return bytes.cast<std::string>();
  }

  wpi::TelemetryTable::EntryHandle GetEntry(std::string_view name) const {
    return m_table->GetEntry(name);
  }

  static void ValidateNoElementType(py::handle elementType) {
    if (!IsNoElementType(elementType)) {
      throw py::type_error(
          "element_type is only supported for telemetry sequences");
    }
  }

  static void ValidateNoTypeString(std::string_view typeString) {
    if (!typeString.empty()) {
      throw py::type_error(
          "type_string is only supported for scalar string and raw telemetry "
          "values");
    }
  }

  static void ValidateNoExplicitTelemetryType(py::handle elementType,
                                              std::string_view typeString) {
    ValidateNoElementType(elementType);
    ValidateNoTypeString(typeString);
  }

  static SequenceKind KindFromElementType(py::handle elementType) {
    if (py::isinstance<py::str>(elementType)) {
      throw py::type_error("telemetry element_type must be a Python type");
    }
    if (IsBuiltinType(elementType, "bool")) {
      return SequenceKind::kBoolean;
    }
    if (IsBuiltinType(elementType, "int")) {
      return SequenceKind::kInteger;
    }
    if (IsBuiltinType(elementType, "float")) {
      return SequenceKind::kDouble;
    }
    if (IsBuiltinType(elementType, "str")) {
      return SequenceKind::kString;
    }
    if (IsBuiltinType(elementType, "object")) {
      return SequenceKind::kFallbackString;
    }
    throw py::type_error("unsupported telemetry element_type");
  }

  void LogObject(std::string_view name, py::handle value,
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

    if (py::isinstance<wpi::TelemetryLoggable>(value)) {
      logTo(py::cast(&child, py::return_value_policy::reference));
    } else {
      logTo(PyTelemetryTable{child});
    }
  }

  void LogSequence(std::string_view name, const py::sequence& value,
                   py::handle valueType) const {
    auto entry = GetEntry(name);
    if (entry->IsDiscard()) {
      return;
    }

    SequenceKind kind = KindFromElementType(valueType);
    const size_t size = py::len(value);
    switch (kind) {
      case SequenceKind::kBoolean: {
        auto data = std::make_unique<bool[]>(size);
        for (size_t i = 0; i < size; ++i) {
          py::handle item = value[static_cast<py::ssize_t>(i)];
          if (!py::isinstance<py::bool_>(item)) {
            throw py::type_error(
                "boolean telemetry arrays require bool values");
          }
          data[i] = item.cast<bool>();
        }
        entry->LogBooleanArray(std::span<const bool>{data.get(), size});
        break;
      }
      case SequenceKind::kInteger: {
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
        entry->LogInt64Array(std::span<const int64_t>{data});
        break;
      }
      case SequenceKind::kDouble: {
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
        entry->LogDoubleArray(std::span<const double>{data});
        break;
      }
      case SequenceKind::kString: {
        std::vector<std::string> data;
        data.reserve(size);
        for (size_t i = 0; i < size; ++i) {
          py::handle item = value[static_cast<py::ssize_t>(i)];
          if (!py::isinstance<py::str>(item)) {
            throw py::type_error("string telemetry arrays require str values");
          }
          data.emplace_back(item.cast<std::string>());
        }
        entry->LogStringArray(std::span<const std::string>{data});
        break;
      }
      case SequenceKind::kFallbackString: {
        std::vector<std::string> data;
        data.reserve(size);
        for (size_t i = 0; i < size; ++i) {
          data.emplace_back(
              py::str(value[static_cast<py::ssize_t>(i)]).cast<std::string>());
        }
        entry->LogStringArray(std::span<const std::string>{data});
        break;
      }
    }
  }

  void LogStruct(std::string_view name, py::handle value) const {
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
    entry->LogRaw(std::span<const uint8_t>{data}, typeString);
  }

  void LogStructSequence(std::string_view name, const py::sequence& value,
                         const py::type& type) const {
    auto entry = GetEntry(name);
    if (entry->IsDiscard()) {
      return;
    }

    ValidateStructSequenceType(value, type);
    LogStructSequenceImpl(entry, value, type);
  }

  void LogStructSequenceImpl(const wpi::TelemetryTable::EntryHandle& entry,
                             const py::sequence& value,
                             const py::type& type) const {
    WPyStructInfo info{type};
    std::string typeString{wpi::util::GetStructTypeString<WPyStruct>(info)};
    AddStructSchemas(entry, typeString, info);

    const size_t size = py::len(value);
    const size_t structSize = wpi::util::GetStructSize<WPyStruct>(info);
    std::vector<uint8_t> data;
    data.resize(size * structSize);
    for (size_t i = 0; i < size; ++i) {
      WPyStruct wrapped{py::reinterpret_borrow<py::object>(
          value[static_cast<py::ssize_t>(i)])};
      wpi::util::PackStruct(
          std::span<uint8_t>{data}.subspan(i * structSize, structSize), wrapped,
          info);
    }

    typeString += "[]";
    entry->LogRaw(std::span<const uint8_t>{data}, typeString);
  }

  wpi::TelemetryTable* m_table;
};

}  // namespace wpi::telemetry::python

namespace {

py::object ActionValueToPython(
    const wpi::MockTelemetryBackend::Action& action) {
  py::dict result;
  result["path"] = action.path;
  std::visit(
      [&](const auto& value) {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::same_as<
                          T, wpi::MockTelemetryBackend::KeepDuplicatesValue>) {
          result["kind"] = "keep_duplicates";
          result["value"] = value.value;
        } else if constexpr (std::same_as<
                                 T,
                                 wpi::MockTelemetryBackend::SetPropertyValue>) {
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
        } else if constexpr (std::same_as<
                                 T,
                                 wpi::MockTelemetryBackend::LogStringValue>) {
          result["kind"] = "string";
          result["value"] = value.value;
          result["type_string"] = value.typeString;
        } else if constexpr (std::same_as<T, wpi::MockTelemetryBackend::
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
        } else if constexpr (std::same_as<
                                 T, wpi::MockTelemetryBackend::LogRawValue>) {
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

py::object SchemaToPython(const wpi::MockTelemetryBackend::Schema* schema) {
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

}  // namespace

void wpi::InitTelemetryPython(py::module_& m) {
  using telemetry::python::PyTelemetryTable;

  py::class_<wpi::TelemetryTable>(m, "_NativeTelemetryTable")
      .def_property_readonly("path",
                             [](const TelemetryTable& self) {
                               return std::string{self.GetPath()};
                             })
      .def("set_type", &TelemetryTable::SetType)
      .def("get_type", &TelemetryTable::GetType)
      .def("has_type", &TelemetryTable::HasType)
      .def("get_table", &TelemetryTable::GetTable,
           py::return_value_policy::reference)
      .def("keep_duplicates", &TelemetryTable::KeepDuplicates)
      .def("set_property", &TelemetryTable::SetProperty)
      .def(
          "log",
          [](TelemetryTable& self, std::string_view name, py::object value,
             py::object elementType, std::string_view typeString) {
            PyTelemetryTable{self}.Log(name, value, std::move(elementType),
                                       typeString);
          },
          py::arg("name"), py::arg("value"), py::kw_only(),
          py::arg("element_type") = py::none(), py::arg("type_string") = "",
          kLogDoc);

  py::class_<PyTelemetryTable>(m, "TelemetryTable")
      .def_property_readonly("path", &PyTelemetryTable::GetPath)
      .def("set_type", &PyTelemetryTable::SetType)
      .def("get_type", &PyTelemetryTable::GetType)
      .def("has_type", &PyTelemetryTable::HasType)
      .def("get_table", &PyTelemetryTable::GetTable)
      .def("keep_duplicates", &PyTelemetryTable::KeepDuplicates)
      .def("set_property", &PyTelemetryTable::SetProperty)
      .def("log", &PyTelemetryTable::Log, py::arg("name"), py::arg("value"),
           py::kw_only(), py::arg("element_type") = py::none(),
           py::arg("type_string") = "", kLogDoc);

  py::class_<wpi::Telemetry>(m, "Telemetry")
      .def_static(
          "get_table",
          [](std::string_view name) {
            auto& root = Telemetry::GetTable();
            return PyTelemetryTable{name.empty() ? root : root.GetTable(name)};
          },
          py::arg("name") = "")
      .def_static(
          "log",
          [](std::string_view name, py::object value, py::object elementType,
             std::string_view typeString) {
            PyTelemetryTable{Telemetry::GetTable()}.Log(
                name, value, std::move(elementType), typeString);
          },
          py::arg("name"), py::arg("value"), py::kw_only(),
          py::arg("element_type") = py::none(), py::arg("type_string") = "",
          kLogDoc)
      .def_static("keep_duplicates", &Telemetry::KeepDuplicates)
      .def_static("set_property", &Telemetry::SetProperty);

  py::class_<wpi::TelemetryRegistry>(m, "TelemetryRegistry")
      .def_static(
          "set_report_warning",
          [](py::object func) {
            if (func.is_none()) {
              TelemetryRegistry::SetReportWarning(nullptr);
            } else {
              auto callback = std::shared_ptr<py::object>{
                  new py::object{std::move(func)}, [](auto object) {
                    py::gil_scoped_acquire gil;
                    delete object;
                  }};
              TelemetryRegistry::SetReportWarning(
                  [callback](std::string_view path, std::string_view msg) {
                    py::gil_scoped_acquire gil;
                    (*callback)(std::string{path}, std::string{msg});
                  });
            }
          })
      .def_static("report_warning", &TelemetryRegistry::ReportWarning)
      .def_static("register_backend",
                  [](std::string_view prefix,
                     std::shared_ptr<TelemetryBackend> backend) {
                    TelemetryRegistry::RegisterBackend(prefix,
                                                       std::move(backend));
                  })
      .def_static("get_backend", &TelemetryRegistry::GetBackend)
      .def_static("get_entry", &TelemetryRegistry::GetEntry)
      .def_static(
          "get_table",
          [](std::string_view path) {
            return PyTelemetryTable{TelemetryRegistry::GetTable(path)};
          },
          py::arg("path"))
      .def_static("reset", &TelemetryRegistry::Reset);

  py::class_<wpi::DiscardTelemetryBackend, py::smart_holder,
             wpi::TelemetryBackend>(m, "DiscardTelemetryBackend")
      .def(py::init<>());

  py::class_<wpi::MultiTelemetryBackend, py::smart_holder,
             wpi::TelemetryBackend>(m, "MultiTelemetryBackend")
      .def(py::init<>())
      .def(py::init<std::vector<std::shared_ptr<TelemetryBackend>>>(),
           py::arg("backends"));

  py::class_<wpi::MockTelemetryBackend, py::smart_holder,
             wpi::TelemetryBackend>(m, "MockTelemetryBackend")
      .def(py::init<>())
      .def("clear", &MockTelemetryBackend::Clear)
      .def("get_actions",
           [](const MockTelemetryBackend& self) {
             py::list actions;
             for (const auto& action : self.GetActions()) {
               actions.append(ActionValueToPython(action));
             }
             return actions;
           })
      .def("get_last_action",
           [](const MockTelemetryBackend& self, std::string_view path) {
             auto* action = self.GetLastAction(path);
             if (!action) {
               return py::object{py::none{}};
             }
             return ActionValueToPython(*action);
           })
      .def("get_last_value",
           [](const MockTelemetryBackend& self, std::string_view path) {
             auto* action = self.GetLastAction(path);
             if (!action) {
               return py::object{py::none{}};
             }
             py::dict result =
                 py::reinterpret_borrow<py::dict>(ActionValueToPython(*action));
             return py::object{result["value"]};
           })
      .def("get_schema",
           [](MockTelemetryBackend& self, std::string_view schemaName) {
             return SchemaToPython(self.GetSchema(schemaName));
           });
}
