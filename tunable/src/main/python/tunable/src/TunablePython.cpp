#include "TunablePython.h"

#include <stdint.h>

#include <algorithm>
#include <memory>
#include <mutex>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "pybind11/functional.h"
#include "pybind11/stl.h"
#include "wpi/tunable/ComplexTunable.hpp"
#include "wpi/tunable/MockTunableBackend.hpp"
#include "wpi/tunable/Tunable.hpp"
#include "wpi/tunable/TunableBackend.hpp"
#include "wpi/tunable/TunableConfig.hpp"
#include "wpi/tunable/TunableRegistry.hpp"
#include "wpi/tunable/TunableTable.hpp"
#include "wpi/tunable/Tunables.hpp"
#include "wpi/tunable/detail/TunableBase.hpp"
#include "wpi/util/json.hpp"

namespace py = pybind11;

namespace {

using TunableVariant =
    std::variant<wpi::TunableBool, wpi::TunableInt64, wpi::TunableDouble,
                 wpi::TunableString, wpi::TunableRaw, wpi::TunableBoolVector,
                 wpi::TunableInt64Vector, wpi::TunableDoubleVector,
                 wpi::TunableStringVector>;

enum class ValueKind {
  kBoolean,
  kInteger,
  kDouble,
  kString,
  kRaw,
  kBooleanArray,
  kIntegerArray,
  kDoubleArray,
  kStringArray,
};

bool IsBytesLike(py::handle value) {
  return PyBytes_Check(value.ptr()) || PyByteArray_Check(value.ptr()) ||
         PyMemoryView_Check(value.ptr());
}

std::string BytesLikeToString(py::handle value) {
  py::object bytes =
      py::reinterpret_steal<py::object>(PyBytes_FromObject(value.ptr()));
  if (!bytes) {
    throw py::error_already_set{};
  }
  return bytes.cast<std::string>();
}

ValueKind KindFromType(std::string_view type) {
  if (type == "boolean" || type == "bool") {
    return ValueKind::kBoolean;
  }
  if (type == "integer" || type == "int" || type == "long") {
    return ValueKind::kInteger;
  }
  if (type == "double" || type == "float") {
    return ValueKind::kDouble;
  }
  if (type == "string") {
    return ValueKind::kString;
  }
  if (type == "raw" || type == "bytes" || type == "byte[]") {
    return ValueKind::kRaw;
  }
  if (type == "boolean[]" || type == "bool[]") {
    return ValueKind::kBooleanArray;
  }
  if (type == "integer[]" || type == "int[]" || type == "long[]") {
    return ValueKind::kIntegerArray;
  }
  if (type == "double[]" || type == "float[]") {
    return ValueKind::kDoubleArray;
  }
  if (type == "string[]") {
    return ValueKind::kStringArray;
  }
  throw py::type_error("unsupported tunable value_type");
}

ValueKind InferSequenceKind(const py::sequence& value) {
  bool allBool = true;
  bool allInt = true;
  bool allNumeric = true;
  bool allString = true;
  const size_t size = py::len(value);
  if (size == 0) {
    return ValueKind::kStringArray;
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
    return ValueKind::kBooleanArray;
  }
  if (allInt) {
    return ValueKind::kIntegerArray;
  }
  if (allNumeric) {
    return ValueKind::kDoubleArray;
  }
  if (allString) {
    return ValueKind::kStringArray;
  }
  return ValueKind::kStringArray;
}

ValueKind InferValueKind(py::handle value, std::string_view valueType) {
  if (!valueType.empty()) {
    return KindFromType(valueType);
  }
  if (py::isinstance<py::bool_>(value)) {
    return ValueKind::kBoolean;
  }
  if (py::isinstance<py::int_>(value)) {
    return ValueKind::kInteger;
  }
  if (py::isinstance<py::float_>(value)) {
    return ValueKind::kDouble;
  }
  if (py::isinstance<py::str>(value)) {
    return ValueKind::kString;
  }
  if (IsBytesLike(value)) {
    return ValueKind::kRaw;
  }
  if (PySequence_Check(value.ptr())) {
    return InferSequenceKind(py::reinterpret_borrow<py::sequence>(value));
  }
  throw py::type_error("cannot infer tunable type; pass value_type explicitly");
}

std::vector<uint8_t> ToRawVector(py::handle value) {
  if (IsBytesLike(value)) {
    auto raw = BytesLikeToString(value);
    return {raw.begin(), raw.end()};
  }
  auto sequence = py::reinterpret_borrow<py::sequence>(value);
  std::vector<uint8_t> data;
  const size_t size = py::len(sequence);
  data.reserve(size);
  for (size_t i = 0; i < size; ++i) {
    int item = sequence[static_cast<py::ssize_t>(i)].cast<int>();
    if (item < 0 || item > 255) {
      throw py::value_error("raw tunable values must be in range 0-255");
    }
    data.emplace_back(static_cast<uint8_t>(item));
  }
  return data;
}

wpi::util::json ToJson(py::handle value) {
  if (value.is_none()) {
    return nullptr;
  }
  if (py::isinstance<py::bool_>(value)) {
    return value.cast<bool>();
  }
  if (py::isinstance<py::int_>(value)) {
    return value.cast<int64_t>();
  }
  if (py::isinstance<py::float_>(value)) {
    return value.cast<double>();
  }
  if (py::isinstance<py::str>(value)) {
    return value.cast<std::string>();
  }
  if (py::isinstance<py::dict>(value)) {
    wpi::util::json obj = wpi::util::json::object();
    auto dict = py::reinterpret_borrow<py::dict>(value);
    for (auto&& item : dict) {
      obj[item.first.cast<std::string>()] = ToJson(item.second);
    }
    return obj;
  }
  if (PySequence_Check(value.ptr()) && !IsBytesLike(value)) {
    wpi::util::json arr = wpi::util::json::array();
    auto sequence = py::reinterpret_borrow<py::sequence>(value);
    const size_t size = py::len(sequence);
    for (size_t i = 0; i < size; ++i) {
      arr.emplace_back(ToJson(sequence[static_cast<py::ssize_t>(i)]));
    }
    return arr;
  }
  return py::str(value).cast<std::string>();
}

class PyTunable {
 public:
  PyTunable(py::object value, py::object getter, py::object setter,
            py::object onTune, bool robust, bool isMutable,
            std::string valueType, py::object properties,
            std::string typeString, bool alwaysGet)
      : m_getter{std::move(getter)},
        m_setter{std::move(setter)},
        m_onTune{std::move(onTune)},
        m_value{MakeValue(value, robust, isMutable, std::move(valueType),
                          std::move(properties), std::move(typeString),
                          alwaysGet)} {}

  wpi::detail::TunableBase& GetBase() {
    return std::visit(
        [](auto& value) -> wpi::detail::TunableBase& { return value; },
        m_value);
  }

  py::object Get() const {
    return std::visit(
        [](const auto& value) -> py::object {
          using T = std::remove_cvref_t<decltype(value.Get())>;
          if constexpr (std::same_as<T, std::vector<uint8_t>>) {
            const auto& raw = value.Get();
            return py::bytes{reinterpret_cast<const char*>(raw.data()),
                             raw.size()};
          } else {
            return py::cast(value.Get());
          }
        },
        m_value);
  }

  void Set(py::handle value) {
    std::visit(
        [&](auto& tunable) {
          using T = std::remove_cvref_t<decltype(tunable.Get())>;
          if constexpr (std::same_as<T, std::vector<uint8_t>>) {
            tunable.Set(ToRawVector(value));
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
  wpi::TunableConfig MakeConfig(bool robust, bool isMutable,
                                py::handle properties, std::string typeString,
                                bool alwaysGet) {
    wpi::TunableConfig config{
        .robust = robust,
        .isMutable = isMutable,
        .onTune =
            [this](wpi::detail::TunableBase&, wpi::ComplexTunable*) {
              py::gil_scoped_acquire gil;
              py::object value = Get();
              if (!m_setter.is_none()) {
                m_setter(value);
              }
              if (!m_onTune.is_none()) {
                m_onTune(value);
              }
            },
        .alwaysGet = alwaysGet || !m_getter.is_none()};
    if (!typeString.empty()) {
      config.typeString = std::move(typeString);
    }
    if (!properties.is_none()) {
      config.properties = ToJson(properties);
    }
    return config;
  }

  TunableVariant MakeValue(py::handle value, bool robust, bool isMutable,
                           std::string valueType, py::object properties,
                           std::string typeString, bool alwaysGet) {
    auto kind = InferValueKind(value, valueType);
    auto config = MakeConfig(robust, isMutable, properties,
                             std::move(typeString), alwaysGet);
    switch (kind) {
      case ValueKind::kBoolean:
        return wpi::TunableBool{value.cast<bool>(), config};
      case ValueKind::kInteger:
        return wpi::TunableInt64{value.cast<int64_t>(), config};
      case ValueKind::kDouble:
        return wpi::TunableDouble{value.cast<double>(), config};
      case ValueKind::kString:
        return wpi::TunableString{value.cast<std::string>(), config};
      case ValueKind::kRaw:
        return wpi::TunableRaw{ToRawVector(value), config};
      case ValueKind::kBooleanArray:
        return wpi::TunableBoolVector{value.cast<std::vector<bool>>(), config};
      case ValueKind::kIntegerArray:
        return wpi::TunableInt64Vector{value.cast<std::vector<int64_t>>(),
                                       config};
      case ValueKind::kDoubleArray:
        return wpi::TunableDoubleVector{value.cast<std::vector<double>>(),
                                        config};
      case ValueKind::kStringArray:
        return wpi::TunableStringVector{value.cast<std::vector<std::string>>(),
                                        config};
    }
    throw py::type_error("unsupported tunable value_type");
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
  std::shared_ptr<PyTunable> Add(std::string_view name, py::object value,
                                 std::string valueType, bool robust,
                                 bool isMutable, py::object onTune,
                                 py::object properties, std::string typeString);
  std::shared_ptr<PyTunable> PublishValue(std::string_view name,
                                          py::object getter, py::object setter,
                                          std::string valueType, bool robust,
                                          py::object properties,
                                          std::string typeString);
  void Remove(std::string_view name) { m_table.Remove(name); }

 private:
  void StoreValue(std::string_view name, std::shared_ptr<PyTunable> tunable);

  wpi::TunableTable m_table;
  PyComplexTunableAdapter* m_owner;
};

class PyComplexTunableAdapter : public wpi::ComplexTunable {
 public:
  explicit PyComplexTunableAdapter(py::object value)
      : m_value{std::move(value)} {
    if (py::hasattr(m_value, "get_tunable_type")) {
      py::object typeObj = m_value.attr("get_tunable_type")();
      if (!typeObj.is_none()) {
        m_type = typeObj.cast<std::string>();
      }
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

void RefreshValues() {
  for (auto&& entry : GetValues()) {
    entry.second->Refresh();
  }
}

void ClearValues() {
  GetValues().clear();
  GetComplexValues().clear();
}

void PyTunableTable::StoreValue(std::string_view name,
                                std::shared_ptr<PyTunable> tunable) {
  if (m_owner) {
    m_owner->AddValue(m_table.GetPath() + std::string{name},
                      std::move(tunable));
  } else {
    GetValues().insert_or_assign(m_table.GetPath() + std::string{name},
                                 std::move(tunable));
  }
}

void PyTunableTable::Publish(std::string_view name, py::object value) {
  if (py::isinstance<PyTunable>(value)) {
    auto tunable = value.cast<std::shared_ptr<PyTunable>>();
    m_table.Publish(name, tunable->GetBase());
    StoreValue(name, std::move(tunable));
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

std::shared_ptr<PyTunable> PyTunableTable::Add(
    std::string_view name, py::object value, std::string valueType, bool robust,
    bool isMutable, py::object onTune, py::object properties,
    std::string typeString) {
  auto tunable = std::make_shared<PyTunable>(
      value, py::none(), py::none(), std::move(onTune), robust, isMutable,
      std::move(valueType), std::move(properties), std::move(typeString),
      false);
  m_table.Publish(name, tunable->GetBase());
  StoreValue(name, tunable);
  return tunable;
}

std::shared_ptr<PyTunable> PyTunableTable::PublishValue(
    std::string_view name, py::object getter, py::object setter,
    std::string valueType, bool robust, py::object properties,
    std::string typeString) {
  auto value = std::make_shared<PyTunable>(
      getter(), getter, setter, py::none(), robust, true, std::move(valueType),
      std::move(properties), std::move(typeString), true);
  m_table.Publish(name, value->GetBase());
  StoreValue(name, value);
  return value;
}

void RemoveRootValue(std::string_view name) {
  wpi::Tunables::Remove(name);
  std::string path = "/" + std::string{name};
  GetValues().erase(path);
  auto it = GetComplexValues().find(path);
  if (it != GetComplexValues().end()) {
    it->second->RemoveChildren();
    GetComplexValues().erase(it);
  }
}

void SetRaw(wpi::MockTunableBackend& self, std::string_view path,
            py::handle value) {
  auto raw = ToRawVector(value);
  self.SetRaw(path, std::span<const uint8_t>{raw.data(), raw.size()});
}

template <typename T, typename F>
void SetVector(wpi::MockTunableBackend& self, std::string_view path,
               const py::sequence& value, F setter) {
  std::vector<T> data;
  const size_t size = py::len(value);
  data.reserve(size);
  for (size_t i = 0; i < size; ++i) {
    data.emplace_back(value[static_cast<py::ssize_t>(i)].cast<T>());
  }
  (self.*setter)(path, std::span<const T>{data.data(), data.size()});
}

void SetBoolVector(wpi::MockTunableBackend& self, std::string_view path,
                   const py::sequence& value) {
  const size_t size = py::len(value);
  auto data = std::make_unique<bool[]>(size);
  for (size_t i = 0; i < size; ++i) {
    data[i] = value[static_cast<py::ssize_t>(i)].cast<bool>();
  }
  self.SetBoolVector(path, std::span<const bool>{data.get(), size});
}

}  // namespace

void wpi::InitTunablePython(py::module_& m) {
  py::class_<PyTunable, std::shared_ptr<PyTunable>>(m, "Tunable")
      .def(py::init<py::object, py::object, py::object, py::object, bool, bool,
                    std::string, py::object, std::string, bool>(),
           py::arg("value"), py::arg("getter") = py::none(),
           py::arg("setter") = py::none(), py::arg("on_tune") = py::none(),
           py::arg("robust") = false, py::arg("mutable") = true,
           py::arg("value_type") = "", py::arg("properties") = py::none(),
           py::arg("type_string") = "", py::arg("always_get") = false)
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
      .def("add", &PyTunableTable::Add, py::arg("name"), py::arg("value"),
           py::arg("value_type") = "", py::arg("robust") = false,
           py::arg("mutable") = true, py::arg("on_tune") = py::none(),
           py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def("add_boolean", &PyTunableTable::Add, py::arg("name"),
           py::arg("value"), py::arg("value_type") = "boolean",
           py::arg("robust") = false, py::arg("mutable") = true,
           py::arg("on_tune") = py::none(), py::arg("properties") = py::none(),
           py::arg("type_string") = "")
      .def("add_int", &PyTunableTable::Add, py::arg("name"), py::arg("value"),
           py::arg("value_type") = "integer", py::arg("robust") = false,
           py::arg("mutable") = true, py::arg("on_tune") = py::none(),
           py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def("add_long", &PyTunableTable::Add, py::arg("name"), py::arg("value"),
           py::arg("value_type") = "integer", py::arg("robust") = false,
           py::arg("mutable") = true, py::arg("on_tune") = py::none(),
           py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def("add_float", &PyTunableTable::Add, py::arg("name"), py::arg("value"),
           py::arg("value_type") = "double", py::arg("robust") = false,
           py::arg("mutable") = true, py::arg("on_tune") = py::none(),
           py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def("add_double", &PyTunableTable::Add, py::arg("name"),
           py::arg("value"), py::arg("value_type") = "double",
           py::arg("robust") = false, py::arg("mutable") = true,
           py::arg("on_tune") = py::none(), py::arg("properties") = py::none(),
           py::arg("type_string") = "")
      .def("publish_value", &PyTunableTable::PublishValue, py::arg("name"),
           py::arg("getter"), py::arg("setter"), py::arg("value_type") = "",
           py::arg("robust") = false, py::arg("properties") = py::none(),
           py::arg("type_string") = "")
      .def("publish_boolean", &PyTunableTable::PublishValue, py::arg("name"),
           py::arg("getter"), py::arg("setter"),
           py::arg("value_type") = "boolean", py::arg("robust") = false,
           py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def("publish_int", &PyTunableTable::PublishValue, py::arg("name"),
           py::arg("getter"), py::arg("setter"),
           py::arg("value_type") = "integer", py::arg("robust") = false,
           py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def("publish_long", &PyTunableTable::PublishValue, py::arg("name"),
           py::arg("getter"), py::arg("setter"),
           py::arg("value_type") = "integer", py::arg("robust") = false,
           py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def("publish_float", &PyTunableTable::PublishValue, py::arg("name"),
           py::arg("getter"), py::arg("setter"),
           py::arg("value_type") = "double", py::arg("robust") = false,
           py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def("publish_double", &PyTunableTable::PublishValue, py::arg("name"),
           py::arg("getter"), py::arg("setter"),
           py::arg("value_type") = "double", py::arg("robust") = false,
           py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def("publish_string", &PyTunableTable::PublishValue, py::arg("name"),
           py::arg("getter"), py::arg("setter"),
           py::arg("value_type") = "string", py::arg("robust") = false,
           py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def("publish_boolean_array", &PyTunableTable::PublishValue,
           py::arg("name"), py::arg("getter"), py::arg("setter"),
           py::arg("value_type") = "boolean[]", py::arg("robust") = false,
           py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def("publish_integer_array", &PyTunableTable::PublishValue,
           py::arg("name"), py::arg("getter"), py::arg("setter"),
           py::arg("value_type") = "integer[]", py::arg("robust") = false,
           py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def("publish_double_array", &PyTunableTable::PublishValue,
           py::arg("name"), py::arg("getter"), py::arg("setter"),
           py::arg("value_type") = "double[]", py::arg("robust") = false,
           py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def("publish_string_array", &PyTunableTable::PublishValue,
           py::arg("name"), py::arg("getter"), py::arg("setter"),
           py::arg("value_type") = "string[]", py::arg("robust") = false,
           py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def("publish_raw", &PyTunableTable::PublishValue, py::arg("name"),
           py::arg("getter"), py::arg("setter"), py::arg("value_type") = "raw",
           py::arg("robust") = false, py::arg("properties") = py::none(),
           py::arg("type_string") = "")
      .def("remove", &PyTunableTable::Remove);

  py::class_<wpi::Tunables>(m, "Tunables")
      .def_static(
          "get_table",
          [](std::string_view name) {
            return PyTunableTable{
                name.empty() ? Tunables::GetTable() : Tunables::GetTable(name),
                nullptr};
          },
          py::arg("name") = "")
      .def_static("publish",
                  [](std::string_view name, py::object value) {
                    PyTunableTable{Tunables::GetTable(), nullptr}.Publish(
                        name, value);
                  })
      .def_static(
          "add",
          [](std::string_view name, py::object value, std::string valueType,
             bool robust, bool isMutable, py::object onTune,
             py::object properties, std::string typeString) {
            return PyTunableTable{Tunables::GetTable(), nullptr}.Add(
                name, std::move(value), std::move(valueType), robust, isMutable,
                std::move(onTune), std::move(properties),
                std::move(typeString));
          },
          py::arg("name"), py::arg("value"), py::arg("value_type") = "",
          py::arg("robust") = false, py::arg("mutable") = true,
          py::arg("on_tune") = py::none(), py::arg("properties") = py::none(),
          py::arg("type_string") = "")
      .def_static("add_boolean",
                  [](std::string_view name, py::object value) {
                    return PyTunableTable{Tunables::GetTable(), nullptr}.Add(
                        name, std::move(value), "boolean", false, true,
                        py::none(), py::none(), "");
                  })
      .def_static("add_int",
                  [](std::string_view name, py::object value) {
                    return PyTunableTable{Tunables::GetTable(), nullptr}.Add(
                        name, std::move(value), "integer", false, true,
                        py::none(), py::none(), "");
                  })
      .def_static("add_long",
                  [](std::string_view name, py::object value) {
                    return PyTunableTable{Tunables::GetTable(), nullptr}.Add(
                        name, std::move(value), "integer", false, true,
                        py::none(), py::none(), "");
                  })
      .def_static("add_float",
                  [](std::string_view name, py::object value) {
                    return PyTunableTable{Tunables::GetTable(), nullptr}.Add(
                        name, std::move(value), "double", false, true,
                        py::none(), py::none(), "");
                  })
      .def_static("add_double",
                  [](std::string_view name, py::object value) {
                    return PyTunableTable{Tunables::GetTable(), nullptr}.Add(
                        name, std::move(value), "double", false, true,
                        py::none(), py::none(), "");
                  })
      .def_static("remove", &RemoveRootValue);

  py::class_<wpi::TunableRegistry>(m, "TunableRegistry")
      .def_static("set_report_warning",
                  [](py::object func) {
                    if (func.is_none()) {
                      TunableRegistry::SetReportWarning(nullptr);
                    } else {
                      TunableRegistry::SetReportWarning(
                          [func = std::move(func)](std::string_view msg) {
                            py::gil_scoped_acquire gil;
                            func(std::string{msg});
                          });
                    }
                  })
      .def_static("report_warning", &TunableRegistry::ReportWarning)
      .def_static(
          "register_backend",
          [](std::string_view prefix, std::shared_ptr<TunableBackend> backend) {
            TunableRegistry::RegisterBackend(prefix, std::move(backend));
          })
      .def_static("get_backend", &TunableRegistry::GetBackend)
      .def_static(
          "get_table",
          [](std::string_view path) {
            return PyTunableTable{wpi::Tunables::GetTable(path), nullptr};
          },
          py::arg("path"))
      .def_static(
          "normalize_name",
          [](std::string_view path) {
            std::string buf;
            return std::string{TunableRegistry::NormalizeName(path, buf)};
          })
      .def_static("update",
                  [] {
                    RefreshValues();
                    TunableRegistry::Update();
                  })
      .def_static("with_update_mutex",
                  [](py::function func) {
                    std::scoped_lock lock{TunableRegistry::GetUpdateMutex()};
                    func();
                  })
      .def_static("reset", [] {
        ClearValues();
        TunableRegistry::Reset();
      });

  py::class_<wpi::MockTunableBackend, py::smart_holder, wpi::TunableBackend>(
      m, "MockTunableBackend")
      .def(py::init<>())
      .def("set_bool", &MockTunableBackend::SetBool)
      .def("set_int32", &MockTunableBackend::SetInt32)
      .def("set_int64", &MockTunableBackend::SetInt64)
      .def("set_float", &MockTunableBackend::SetFloat)
      .def("set_double", &MockTunableBackend::SetDouble)
      .def("set_string", &MockTunableBackend::SetString)
      .def("set_raw", &SetRaw)
      .def("set_bool_vector", &SetBoolVector)
      .def("set_int32_vector",
           [](MockTunableBackend& self, std::string_view path,
              const py::sequence& value) {
             SetVector<int32_t>(self, path, value,
                                &MockTunableBackend::SetInt32Vector);
           })
      .def("set_int64_vector",
           [](MockTunableBackend& self, std::string_view path,
              const py::sequence& value) {
             SetVector<int64_t>(self, path, value,
                                &MockTunableBackend::SetInt64Vector);
           })
      .def("set_float_vector",
           [](MockTunableBackend& self, std::string_view path,
              const py::sequence& value) {
             SetVector<float>(self, path, value,
                              &MockTunableBackend::SetFloatVector);
           })
      .def("set_double_vector",
           [](MockTunableBackend& self, std::string_view path,
              const py::sequence& value) {
             SetVector<double>(self, path, value,
                               &MockTunableBackend::SetDoubleVector);
           })
      .def("set_string_vector",
           [](MockTunableBackend& self, std::string_view path,
              const py::sequence& value) {
             SetVector<std::string>(self, path, value,
                                    &MockTunableBackend::SetStringVector);
           });
}
