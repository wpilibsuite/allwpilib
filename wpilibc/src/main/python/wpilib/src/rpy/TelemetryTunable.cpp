// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "TelemetryTunable.h"

#include <stdint.h>

#include <algorithm>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "pybind11/functional.h"
#include "pybind11/stl.h"
#include "wpi/backend/NetworkTablesTelemetryBackend.hpp"
#include "wpi/backend/NetworkTablesTunableBackend.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/telemetry/Telemetry.hpp"
#include "wpi/telemetry/TelemetryRegistry.hpp"
#include "wpi/telemetry/TelemetryTable.hpp"
#include "wpi/tunable/ComplexTunable.hpp"
#include "wpi/tunable/Tunable.hpp"
#include "wpi/tunable/TunableConfig.hpp"
#include "wpi/tunable/TunableRegistry.hpp"
#include "wpi/tunable/TunableTable.hpp"
#include "wpi/tunable/Tunables.hpp"

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
    } else if (py::isinstance<py::bytes>(value)) {
      auto raw = value.cast<std::string>();
      m_table->Log(
          name, std::span<const uint8_t>{
                    reinterpret_cast<const uint8_t*>(raw.data()), raw.size()});
    } else if (py::hasattr(value, "log_to")) {
      auto& child = m_table->GetTable(name);
      std::string type;
      if (py::hasattr(value, "get_telemetry_type")) {
        type = py::cast<std::string>(value.attr("get_telemetry_type")());
        if (!type.empty() && !child.SetType(type)) {
          return;
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
    } else if (py::isinstance<py::sequence>(value)) {
      LogSequence(name, value.cast<py::sequence>(), valueType);
    } else {
      throw py::type_error("unsupported telemetry value type");
    }
  }

 private:
  void LogSequence(std::string_view name, const py::sequence& value,
                   std::string_view valueType) const {
    std::string type{valueType};
    if (type.empty() && py::len(value) != 0) {
      py::handle first = value[0];
      if (py::isinstance<py::bool_>(first)) {
        type = "boolean[]";
      } else if (py::isinstance<py::int_>(first)) {
        type = "integer[]";
      } else if (py::isinstance<py::float_>(first)) {
        type = "double[]";
      } else if (py::isinstance<py::str>(first)) {
        type = "string[]";
      }
    }
    if (type == "boolean[]") {
      auto size = py::len(value);
      auto data = std::make_unique<bool[]>(size);
      for (size_t i = 0; i < size; ++i) {
        data[i] = value[static_cast<py::ssize_t>(i)].cast<bool>();
      }
      m_table->Log(name, std::span<const bool>{data.get(), size});
    } else if (type == "integer[]" || type == "int[]") {
      m_table->Log(name, value.cast<std::vector<int64_t>>());
    } else if (type == "double[]" || type == "float[]") {
      m_table->Log(name, value.cast<std::vector<double>>());
    } else if (type == "string[]" || (type.empty() && py::len(value) == 0)) {
      m_table->Log(name, value.cast<std::vector<std::string>>());
    } else {
      throw py::type_error(
          "cannot infer telemetry array type; pass value_type explicitly");
    }
  }

  wpi::TelemetryTable* m_table;
};

using TunableVariant =
    std::variant<wpi::TunableBool, wpi::TunableInt64, wpi::TunableDouble,
                 wpi::TunableString, wpi::TunableRaw, wpi::TunableBoolVector,
                 wpi::TunableInt64Vector, wpi::TunableDoubleVector,
                 wpi::TunableStringVector>;

class PyTunable {
 public:
  PyTunable(py::object value, py::object getter, py::object setter,
            py::object onTune, bool robust, bool isMutable,
            std::string valueType)
      : m_getter{std::move(getter)},
        m_setter{std::move(setter)},
        m_onTune{std::move(onTune)},
        m_value{MakeValue(value, robust, isMutable, std::move(valueType))} {}

  wpi::detail::TunableBase& GetBase() {
    return std::visit(
        [](auto& value) -> wpi::detail::TunableBase& { return value; },
        m_value);
  }

  py::object Get() const {
    return std::visit([](const auto& value) { return py::cast(value.Get()); },
                      m_value);
  }

  void Set(py::handle value) {
    std::visit(
        [&](auto& tunable) {
          using T = std::remove_cvref_t<decltype(tunable.Get())>;
          if constexpr (std::same_as<T, std::vector<uint8_t>>) {
            auto raw = py::cast<std::string>(value);
            tunable.Set(std::span<const uint8_t>{
                reinterpret_cast<const uint8_t*>(raw.data()), raw.size()});
          } else {
            tunable.Set(py::cast<T>(value));
          }
        },
        m_value);
  }

  void Refresh() {
    if (!m_getter.is_none()) {
      py::gil_scoped_acquire gil;
      Set(m_getter());
    }
  }

 private:
  wpi::TunableConfig MakeConfig(bool robust, bool isMutable) {
    return wpi::TunableConfig{
        .robust = robust,
        .isMutable = isMutable,
        .onTune = [this](wpi::detail::TunableBase&, wpi::ComplexTunable*) {
          py::gil_scoped_acquire gil;
          py::object value = Get();
          if (!m_setter.is_none()) {
            m_setter(value);
          }
          if (!m_onTune.is_none()) {
            m_onTune(value);
          }
        }};
  }

  TunableVariant MakeValue(py::handle value, bool robust, bool isMutable,
                           std::string valueType) {
    auto config = MakeConfig(robust, isMutable);
    if (valueType.empty()) {
      if (py::isinstance<py::bool_>(value)) {
        valueType = "boolean";
      } else if (py::isinstance<py::int_>(value)) {
        valueType = "integer";
      } else if (py::isinstance<py::float_>(value)) {
        valueType = "double";
      } else if (py::isinstance<py::str>(value)) {
        valueType = "string";
      } else if (py::isinstance<py::bytes>(value)) {
        valueType = "raw";
      } else if (py::isinstance<py::sequence>(value) && py::len(value) != 0) {
        py::handle first = value.cast<py::sequence>()[0];
        if (py::isinstance<py::bool_>(first)) {
          valueType = "boolean[]";
        } else if (py::isinstance<py::int_>(first)) {
          valueType = "integer[]";
        } else if (py::isinstance<py::float_>(first)) {
          valueType = "double[]";
        } else if (py::isinstance<py::str>(first)) {
          valueType = "string[]";
        }
      }
    }
    if (valueType == "boolean") {
      return wpi::TunableBool{value.cast<bool>(), config};
    }
    if (valueType == "integer" || valueType == "int") {
      return wpi::TunableInt64{value.cast<int64_t>(), config};
    }
    if (valueType == "double" || valueType == "float") {
      return wpi::TunableDouble{value.cast<double>(), config};
    }
    if (valueType == "string") {
      return wpi::TunableString{value.cast<std::string>(), config};
    }
    if (valueType == "raw") {
      auto raw = value.cast<std::string>();
      return wpi::TunableRaw{std::vector<uint8_t>{raw.begin(), raw.end()},
                             config};
    }
    if (valueType == "boolean[]") {
      return wpi::TunableBoolVector{value.cast<std::vector<bool>>(), config};
    }
    if (valueType == "integer[]" || valueType == "int[]") {
      return wpi::TunableInt64Vector{value.cast<std::vector<int64_t>>(),
                                     config};
    }
    if (valueType == "double[]" || valueType == "float[]") {
      return wpi::TunableDoubleVector{value.cast<std::vector<double>>(),
                                      config};
    }
    if (valueType == "string[]") {
      return wpi::TunableStringVector{value.cast<std::vector<std::string>>(),
                                      config};
    }
    throw py::type_error(
        "cannot infer tunable type; pass value_type explicitly");
  }

  py::object m_getter;
  py::object m_setter;
  py::object m_onTune;
  TunableVariant m_value;
};

class PyComplexTunableAdapter;

class PyTunableTable {
 public:
  PyTunableTable(wpi::TunableTable table, PyComplexTunableAdapter* owner)
      : m_table{std::move(table)}, m_owner{owner} {}

  std::string GetPath() const { return m_table.GetPath(); }
  PyTunableTable GetTable(std::string_view name) {
    return PyTunableTable{m_table.GetTable(name), m_owner};
  }
  void Publish(std::string_view name, py::object value);
  void PublishValue(std::string_view name, py::object getter, py::object setter,
                    std::string valueType, bool robust);
  void Remove(std::string_view name) { m_table.Remove(name); }

 private:
  wpi::TunableTable m_table;
  PyComplexTunableAdapter* m_owner;
};

class PyComplexTunableAdapter : public wpi::ComplexTunable {
 public:
  explicit PyComplexTunableAdapter(py::object value)
      : m_value{std::move(value)} {
    if (py::hasattr(m_value, "get_tunable_type")) {
      m_type = py::cast<std::string>(m_value.attr("get_tunable_type")());
    }
  }

  ~PyComplexTunableAdapter() override = default;

  std::string_view GetTunableType() const override { return m_type; }

  void PublishTunable(wpi::TunableTable& table) override {
    py::gil_scoped_acquire gil;
    try {
      m_value.attr("publish_tunable")(PyTunableTable{table, this});
    } catch (py::error_already_set& error) {
      if (!error.matches(PyExc_TypeError)) {
        throw;
      }
      error.restore();
      PyErr_Clear();
      m_value.attr("publish_tunable")(
          py::cast(&table, py::return_value_policy::reference));
    }
  }

  void UpdateTunable() const override {
    py::gil_scoped_acquire gil;
    for (auto&& child : m_values) {
      child->Refresh();
    }
    if (py::hasattr(m_value, "update_tunable")) {
      m_value.attr("update_tunable")();
    }
  }

  void AddValue(std::string path, std::shared_ptr<PyTunable> value) {
    m_paths.emplace_back(std::move(path));
    m_values.emplace_back(std::move(value));
  }
  void AddComplex(std::shared_ptr<PyComplexTunableAdapter> value) {
    m_complex.emplace_back(std::move(value));
  }
  void RemoveChildren() {
    for (auto&& path : m_paths) {
      wpi::Tunables::Remove(path);
    }
  }

 private:
  py::object m_value;
  std::string m_type;
  mutable std::vector<std::shared_ptr<PyTunable>> m_values;
  std::vector<std::string> m_paths;
  std::vector<std::shared_ptr<PyComplexTunableAdapter>> m_complex;
};

std::unordered_map<std::string, std::shared_ptr<PyTunable>>& GetValues() {
  static std::unordered_map<std::string, std::shared_ptr<PyTunable>> values;
  return values;
}

std::unordered_map<std::string, std::shared_ptr<PyComplexTunableAdapter>>&
GetComplexValues() {
  static std::unordered_map<std::string,
                            std::shared_ptr<PyComplexTunableAdapter>>
      values;
  return values;
}

void PyTunableTable::Publish(std::string_view name, py::object value) {
  if (py::isinstance<PyTunable>(value)) {
    auto tunable = value.cast<std::shared_ptr<PyTunable>>();
    m_table.Publish(name, tunable->GetBase());
    if (m_owner) {
      m_owner->AddValue(m_table.GetPath() + std::string{name},
                        std::move(tunable));
    } else {
      GetValues().insert_or_assign(m_table.GetPath() + std::string{name},
                                   std::move(tunable));
    }
  } else if (py::hasattr(value, "publish_tunable")) {
    auto tunable = std::make_shared<PyComplexTunableAdapter>(std::move(value));
    m_table.Publish(name, *tunable);
    if (m_owner) {
      m_owner->AddComplex(std::move(tunable));
    } else {
      GetComplexValues().insert_or_assign(m_table.GetPath() + std::string{name},
                                          std::move(tunable));
    }
  } else {
    throw py::type_error("value must be a Tunable or ComplexTunable");
  }
}

void PyTunableTable::PublishValue(std::string_view name, py::object getter,
                                  py::object setter, std::string valueType,
                                  bool robust) {
  auto value = std::make_shared<PyTunable>(getter(), getter, setter, py::none(),
                                           robust, true, std::move(valueType));
  m_table.Publish(name, value->GetBase());
  if (m_owner) {
    m_owner->AddValue(m_table.GetPath() + std::string{name}, std::move(value));
  } else {
    GetValues().insert_or_assign(m_table.GetPath() + std::string{name},
                                 std::move(value));
  }
}

}  // namespace

void wpi::InitTelemetryTunable(py::module_& m) {
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
      .def_static("reset", &TelemetryRegistry::Reset)
      .def_static("register_networktables_backend", [] {
        TelemetryRegistry::RegisterBackend(
            "", std::make_shared<backend::NetworkTablesTelemetryBackend>(
                    nt::NetworkTableInstance::GetDefault(), "/Telemetry"));
      });

  py::class_<PyTunable, std::shared_ptr<PyTunable>>(m, "Tunable")
      .def(py::init<py::object, py::object, py::object, py::object, bool, bool,
                    std::string>(),
           py::arg("value"), py::arg("getter") = py::none(),
           py::arg("setter") = py::none(), py::arg("on_tune") = py::none(),
           py::arg("robust") = false, py::arg("mutable") = true,
           py::arg("value_type") = "")
      .def("get", &PyTunable::Get)
      .def("set", &PyTunable::Set);

  py::class_<wpi::TunableTable>(m, "_NativeTunableTable")
      .def_property_readonly("path", &TunableTable::GetPath)
      .def("get_table", &TunableTable::GetTable)
      .def("remove", &TunableTable::Remove);

  py::class_<PyTunableTable>(m, "TunableTable")
      .def_property_readonly("path", &PyTunableTable::GetPath)
      .def("get_table", &PyTunableTable::GetTable)
      .def("publish", &PyTunableTable::Publish)
      .def("publish_value", &PyTunableTable::PublishValue, py::arg("name"),
           py::arg("getter"), py::arg("setter"), py::arg("value_type"),
           py::arg("robust") = false)
      .def("publish_boolean", &PyTunableTable::PublishValue, py::arg("name"),
           py::arg("getter"), py::arg("setter"),
           py::arg("value_type") = "boolean", py::arg("robust") = false)
      .def("publish_integer_array", &PyTunableTable::PublishValue,
           py::arg("name"), py::arg("getter"), py::arg("setter"),
           py::arg("value_type") = "integer[]", py::arg("robust") = false)
      .def("remove", &PyTunableTable::Remove);

  py::class_<wpi::Tunables>(m, "Tunables")
      .def_static(
          "get_table",
          [](std::string_view name) {
            return PyTunableTable{Tunables::GetTable(name), nullptr};
          },
          py::arg("name") = "")
      .def_static("publish",
                  [](std::string_view name, py::object value) {
                    PyTunableTable{Tunables::GetTable(), nullptr}.Publish(
                        name, value);
                  })
      .def_static("remove", [](std::string_view name) {
        Tunables::Remove(name);
        GetValues().erase("/" + std::string{name});
        auto it = GetComplexValues().find("/" + std::string{name});
        if (it != GetComplexValues().end()) {
          it->second->RemoveChildren();
          GetComplexValues().erase(it);
        }
      });

  py::class_<wpi::TunableRegistry>(m, "TunableRegistry")
      .def_static("update", &TunableRegistry::Update)
      .def_static("reset", &TunableRegistry::Reset)
      .def_static("register_networktables_backend", [] {
        TunableRegistry::RegisterBackend(
            "", std::make_shared<backend::NetworkTablesTunableBackend>(
                    nt::NetworkTableInstance::GetDefault(), "/Tunables"));
      });
}
