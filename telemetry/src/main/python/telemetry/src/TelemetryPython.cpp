#include "TelemetryPython.h"

#include <stdint.h>

#include <memory>
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
#include "wpi/telemetry/Telemetry.hpp"
#include "wpi/telemetry/TelemetryBackend.hpp"
#include "wpi/telemetry/TelemetryEntry.hpp"
#include "wpi/telemetry/TelemetryRegistry.hpp"
#include "wpi/telemetry/TelemetryTable.hpp"

namespace py = pybind11;

namespace {

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

  void Log(std::string_view name, py::handle value,
           std::string_view valueType = {}) const {
    if (py::isinstance<py::bool_>(value)) {
      m_table->Log(name, value.cast<bool>());
    } else if (py::isinstance<py::int_>(value)) {
      m_table->Log(name, value.cast<int64_t>());
    } else if (py::isinstance<py::float_>(value)) {
      m_table->Log(name, value.cast<double>());
    } else if (py::isinstance<py::str>(value)) {
      auto str = value.cast<std::string>();
      if (valueType.empty()) {
        m_table->Log(name, str);
      } else {
        m_table->Log(name, str, valueType);
      }
    } else if (IsBytesLike(value)) {
      auto raw = BytesLikeToString(value);
      auto data = std::span<const uint8_t>{
          reinterpret_cast<const uint8_t*>(raw.data()), raw.size()};
      if (valueType.empty()) {
        m_table->Log(name, data);
      } else {
        m_table->Log(name, data, valueType);
      }
    } else if (py::hasattr(value, "log_to")) {
      LogObject(name, value);
    } else if (PySequence_Check(value.ptr())) {
      LogSequence(name, py::reinterpret_borrow<py::sequence>(value), valueType);
    } else {
      m_table->Log(name, py::str(value).cast<std::string>());
    }
  }

 private:
  enum class SequenceKind {
    kBoolean,
    kInteger,
    kDouble,
    kString,
    kRaw,
  };

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

  static SequenceKind KindFromType(std::string_view type) {
    if (type == "boolean[]" || type == "bool[]") {
      return SequenceKind::kBoolean;
    }
    if (type == "integer[]" || type == "int[]" || type == "long[]") {
      return SequenceKind::kInteger;
    }
    if (type == "double[]" || type == "float[]") {
      return SequenceKind::kDouble;
    }
    if (type == "string[]") {
      return SequenceKind::kString;
    }
    if (type == "raw" || type == "byte[]" || type == "bytes") {
      return SequenceKind::kRaw;
    }
    throw py::type_error("unsupported telemetry array value_type");
  }

  static SequenceKind InferSequenceKind(const py::sequence& value) {
    bool allBool = true;
    bool allInt = true;
    bool allNumeric = true;
    bool allString = true;
    const size_t size = py::len(value);
    if (size == 0) {
      return SequenceKind::kString;
    }

    for (size_t i = 0; i < size; ++i) {
      py::handle item = value[static_cast<py::ssize_t>(i)];
      const bool isBool = py::isinstance<py::bool_>(item);
      const bool isInt = py::isinstance<py::int_>(item) && !isBool;
      const bool isFloat = py::isinstance<py::float_>(item);
      const bool isString = py::isinstance<py::str>(item);

      allBool &= isBool;
      allInt &= isInt;
      allNumeric &= isInt || isFloat;
      allString &= isString;
    }

    if (allBool) {
      return SequenceKind::kBoolean;
    }
    if (allInt) {
      return SequenceKind::kInteger;
    }
    if (allNumeric) {
      return SequenceKind::kDouble;
    }
    if (allString) {
      return SequenceKind::kString;
    }
    return SequenceKind::kString;
  }

  void LogObject(std::string_view name, py::handle value) const {
    auto& child = m_table->GetTable(name);
    if (py::hasattr(value, "get_telemetry_type")) {
      py::object typeObj = value.attr("get_telemetry_type")();
      if (!typeObj.is_none()) {
        auto type = typeObj.cast<std::string>();
        if (!type.empty() && !child.SetType(type)) {
          return;
        }
      }
    }

    try {
      value.attr("log_to")(PyTelemetryTable{child});
    } catch (py::error_already_set& error) {
      if (!error.matches(PyExc_TypeError)) {
        throw;
      }
      error.restore();
      PyErr_Clear();
      value.attr("log_to")(
          py::cast(&child, py::return_value_policy::reference));
    }
  }

  void LogSequence(std::string_view name, const py::sequence& value,
                   std::string_view valueType) const {
    SequenceKind kind =
        valueType.empty() ? InferSequenceKind(value) : KindFromType(valueType);
    const size_t size = py::len(value);
    switch (kind) {
      case SequenceKind::kBoolean: {
        auto data = std::make_unique<bool[]>(size);
        for (size_t i = 0; i < size; ++i) {
          data[i] = value[static_cast<py::ssize_t>(i)].cast<bool>();
        }
        m_table->Log(name, std::span<const bool>{data.get(), size});
        break;
      }
      case SequenceKind::kInteger: {
        std::vector<int64_t> data;
        data.reserve(size);
        for (size_t i = 0; i < size; ++i) {
          data.emplace_back(value[static_cast<py::ssize_t>(i)].cast<int64_t>());
        }
        m_table->Log(name, data);
        break;
      }
      case SequenceKind::kDouble: {
        std::vector<double> data;
        data.reserve(size);
        for (size_t i = 0; i < size; ++i) {
          data.emplace_back(value[static_cast<py::ssize_t>(i)].cast<double>());
        }
        m_table->Log(name, data);
        break;
      }
      case SequenceKind::kString: {
        std::vector<std::string> data;
        data.reserve(size);
        for (size_t i = 0; i < size; ++i) {
          data.emplace_back(
              py::str(value[static_cast<py::ssize_t>(i)]).cast<std::string>());
        }
        m_table->Log(name, data);
        break;
      }
      case SequenceKind::kRaw: {
        std::vector<uint8_t> data;
        data.reserve(size);
        for (size_t i = 0; i < size; ++i) {
          int item = value[static_cast<py::ssize_t>(i)].cast<int>();
          if (item < 0 || item > 255) {
            throw py::value_error(
                "raw telemetry values must be in range 0-255");
          }
          data.emplace_back(static_cast<uint8_t>(item));
        }
        if (valueType.empty()) {
          m_table->Log(name, std::span<const uint8_t>{data});
        } else {
          m_table->Log(name, std::span<const uint8_t>{data}, valueType);
        }
        break;
      }
    }
  }

  wpi::TelemetryTable* m_table;
};

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
             std::string_view valueType) {
            PyTelemetryTable{self}.Log(name, value, valueType);
          },
          py::arg("name"), py::arg("value"), py::arg("value_type") = "");

  py::class_<PyTelemetryTable>(m, "TelemetryTable")
      .def_property_readonly("path", &PyTelemetryTable::GetPath)
      .def("set_type", &PyTelemetryTable::SetType)
      .def("get_type", &PyTelemetryTable::GetType)
      .def("has_type", &PyTelemetryTable::HasType)
      .def("get_table", &PyTelemetryTable::GetTable)
      .def("keep_duplicates", &PyTelemetryTable::KeepDuplicates)
      .def("set_property", &PyTelemetryTable::SetProperty)
      .def("log", &PyTelemetryTable::Log, py::arg("name"), py::arg("value"),
           py::arg("value_type") = "");

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
          [](std::string_view name, py::object value,
             std::string_view valueType) {
            PyTelemetryTable{Telemetry::GetTable()}.Log(name, value, valueType);
          },
          py::arg("name"), py::arg("value"), py::arg("value_type") = "")
      .def_static("keep_duplicates", &Telemetry::KeepDuplicates)
      .def_static("set_property", &Telemetry::SetProperty);

  py::class_<wpi::TelemetryRegistry>(m, "TelemetryRegistry")
      .def_static("set_report_warning",
                  [](py::object func) {
                    if (func.is_none()) {
                      TelemetryRegistry::SetReportWarning(nullptr);
                    } else {
                      TelemetryRegistry::SetReportWarning(
                          [func = std::move(func)](std::string_view path,
                                                   std::string_view msg) {
                            py::gil_scoped_acquire gil;
                            func(std::string{path}, std::string{msg});
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
      .def_static("get_entry", &TelemetryRegistry::GetEntry,
                  py::return_value_policy::reference)
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
