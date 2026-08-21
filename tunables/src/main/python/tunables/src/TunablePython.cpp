#include "TunablePython.h"

#include <stdint.h>

#include <algorithm>
#include <memory>
#include <mutex>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "pybind11/functional.h"
#include "pybind11/stl.h"
#include "wpi/tunables/ComplexTunable.hpp"
#include "wpi/tunables/MockTunableBackend.hpp"
#include "wpi/tunables/Tunable.hpp"
#include "wpi/tunables/TunableBackend.hpp"
#include "wpi/tunables/TunableConfig.hpp"
#include "wpi/tunables/TunableRegistry.hpp"
#include "wpi/tunables/TunableTable.hpp"
#include "wpi/tunables/Tunables.hpp"
#include "wpi/tunables/detail/PathUtil.hpp"
#include "wpi/tunables/detail/TunableBase.hpp"
#include "wpi/tunables/detail/TunableDetail.hpp"
#include "wpi/util/json.hpp"
#include "wpystruct.h"

namespace py = pybind11;

namespace {

using TunableVariant =
    std::variant<wpi::tunables::TunableBool, wpi::tunables::TunableInt64,
                 wpi::tunables::TunableDouble, wpi::tunables::TunableString,
                 wpi::tunables::TunableRaw, wpi::tunables::TunableBoolVector,
                 wpi::tunables::TunableInt64Vector,
                 wpi::tunables::TunableDoubleVector,
                 wpi::tunables::TunableStringVector,
                 wpi::tunables::Tunable<WPyStruct, WPyStructInfo>,
                 wpi::tunables::Tunable<std::vector<WPyStruct>, WPyStructInfo>>;

class PyTunable;

py::object MakeMutationList(std::shared_ptr<PyTunable> owner, py::list data);

template <typename T>
struct IsStdVector : std::false_type {};

template <typename T, typename Allocator>
struct IsStdVector<std::vector<T, Allocator>> : std::true_type {};

template <typename T>
inline constexpr bool IsStdVectorV = IsStdVector<T>::value;

template <typename T>
py::list ToPythonList(const std::vector<T>& value) {
  py::list data;
  for (const auto& item : value) {
    data.append(item);
  }
  return data;
}

py::list ToPythonList(const std::vector<uint8_t>& value) {
  py::list data;
  for (uint8_t item : value) {
    data.append(static_cast<int>(item));
  }
  return data;
}

py::list ToPythonList(const std::vector<bool>& value) {
  py::list data;
  for (bool item : value) {
    data.append(item);
  }
  return data;
}

py::list ToPythonList(const std::vector<WPyStruct>& value) {
  py::list data;
  for (auto&& item : value) {
    data.append(item.py);
  }
  return data;
}

enum class ValueKind {
  BOOLEAN,
  INTEGER,
  DOUBLE,
  STRING,
  RAW,
  BOOLEAN_ARRAY,
  INTEGER_ARRAY,
  DOUBLE_ARRAY,
  STRING_ARRAY,
  STRUCT,
  STRUCT_ARRAY,
};

bool IsWpiStruct(py::handle value) {
  return py::hasattr(py::type::of(value), "WPIStruct");
}

bool IsWpiStructType(py::handle value) {
  return PyType_Check(value.ptr()) && py::hasattr(value, "WPIStruct");
}

bool IsBytesLike(py::handle value) {
  return PyBytes_Check(value.ptr()) || PyByteArray_Check(value.ptr()) ||
         PyMemoryView_Check(value.ptr());
}

bool IsBuiltinType(py::handle value, const char* name) {
  return value.is(py::module_::import("builtins").attr(name));
}

py::object BuiltinType(const char* name) {
  return py::module_::import("builtins").attr(name);
}

bool IsNoType(py::handle type) {
  return type.is_none();
}

bool IsSequenceValue(py::handle value) {
  return PySequence_Check(value.ptr()) && !py::isinstance<py::str>(value) &&
         !IsBytesLike(value);
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

std::string BytesLikeToString(py::handle value) {
  py::object bytes =
      py::reinterpret_steal<py::object>(PyBytes_FromObject(value.ptr()));
  if (!bytes) {
    throw py::error_already_set{};
  }
  return bytes.cast<std::string>();
}

ValueKind KindFromScalarType(py::handle valueType) {
  if (py::isinstance<py::str>(valueType)) {
    throw py::type_error("tunable value_type must be a Python type");
  }
  if (IsBuiltinType(valueType, "bool")) {
    return ValueKind::BOOLEAN;
  }
  if (IsBuiltinType(valueType, "int")) {
    return ValueKind::INTEGER;
  }
  if (IsBuiltinType(valueType, "float")) {
    return ValueKind::DOUBLE;
  }
  if (IsBuiltinType(valueType, "str")) {
    return ValueKind::STRING;
  }
  if (IsBuiltinType(valueType, "bytes") ||
      IsBuiltinType(valueType, "bytearray")) {
    return ValueKind::RAW;
  }
  if (IsWpiStructType(valueType)) {
    return ValueKind::STRUCT;
  }
  throw py::type_error("unsupported tunable value_type");
}

ValueKind KindFromElementType(py::handle elementType) {
  if (py::isinstance<py::str>(elementType)) {
    throw py::type_error("tunable element_type must be a Python type");
  }
  if (IsBuiltinType(elementType, "bool")) {
    return ValueKind::BOOLEAN_ARRAY;
  }
  if (IsBuiltinType(elementType, "int")) {
    return ValueKind::INTEGER_ARRAY;
  }
  if (IsBuiltinType(elementType, "float")) {
    return ValueKind::DOUBLE_ARRAY;
  }
  if (IsBuiltinType(elementType, "str")) {
    return ValueKind::STRING_ARRAY;
  }
  if (IsWpiStructType(elementType)) {
    return ValueKind::STRUCT_ARRAY;
  }
  throw py::type_error("unsupported tunable element_type");
}

ValueKind InferSequenceKind(const py::sequence& value) {
  bool allBool = true;
  bool allInt = true;
  bool allNumeric = true;
  bool allString = true;
  const size_t size = py::len(value);
  if (size == 0) {
    throw py::type_error("empty tunable sequences require element_type");
  }

  if (IsWpiStruct(value[0])) {
    return ValueKind::STRUCT_ARRAY;
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
    return ValueKind::BOOLEAN_ARRAY;
  }
  if (allInt) {
    return ValueKind::INTEGER_ARRAY;
  }
  if (allNumeric) {
    return ValueKind::DOUBLE_ARRAY;
  }
  if (allString) {
    return ValueKind::STRING_ARRAY;
  }
  return ValueKind::STRING_ARRAY;
}

ValueKind InferValueKind(py::handle value, py::handle valueType,
                         py::handle elementType) {
  bool hasValueType = !IsNoType(valueType);
  bool hasElementType = !IsNoType(elementType);
  if (hasValueType && hasElementType) {
    throw py::type_error("value_type and element_type are mutually exclusive");
  }
  if (hasElementType) {
    if (!IsSequenceValue(value)) {
      throw py::type_error(
          "element_type is only supported for tunable sequences");
    }
    return KindFromElementType(elementType);
  }
  if (hasValueType) {
    if (IsSequenceValue(value)) {
      throw py::type_error(
          "value_type is only supported for scalar tunables; use "
          "element_type for sequences");
    }
    return KindFromScalarType(valueType);
  }
  if (py::isinstance<py::bool_>(value)) {
    return ValueKind::BOOLEAN;
  }
  if (py::isinstance<py::int_>(value)) {
    return ValueKind::INTEGER;
  }
  if (py::isinstance<py::float_>(value)) {
    return ValueKind::DOUBLE;
  }
  if (py::isinstance<py::str>(value)) {
    return ValueKind::STRING;
  }
  if (IsBytesLike(value)) {
    return ValueKind::RAW;
  }
  if (IsWpiStruct(value)) {
    return ValueKind::STRUCT;
  }
  if (IsSequenceValue(value)) {
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

py::type GetStructSequenceType(const py::sequence& value) {
  if (py::len(value) == 0) {
    throw py::value_error("struct tunable arrays require at least one value");
  }

  py::handle first = value[0];
  if (!IsWpiStruct(first)) {
    throw py::type_error("struct tunable arrays require WPIStruct values");
  }

  py::type type = py::type::of(first);
  const size_t size = py::len(value);
  for (size_t i = 1; i < size; ++i) {
    py::handle item = value[static_cast<py::ssize_t>(i)];
    if (!py::type::of(item).is(type)) {
      throw py::type_error("struct tunable arrays require one WPIStruct type");
    }
  }
  return type;
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
          "struct tunable arrays require values of the specified WPIStruct "
          "type");
    }
  }
}

std::vector<WPyStruct> ToStructVector(const py::sequence& value,
                                      bool allowEmpty = false) {
  if (allowEmpty && py::len(value) == 0) {
    return {};
  }

  GetStructSequenceType(value);

  std::vector<WPyStruct> data;
  const size_t size = py::len(value);
  data.reserve(size);
  for (size_t i = 0; i < size; ++i) {
    data.emplace_back(
        py::reinterpret_borrow<py::object>(value[static_cast<py::ssize_t>(i)]));
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

class PyTunable : public std::enable_shared_from_this<PyTunable> {
 public:
  PyTunable(py::object value, py::object getter, py::object setter,
            py::object onTune, bool robust, bool isMutable,
            py::object valueType, py::object elementType, py::object properties,
            std::string typeString, bool alwaysGet)
      : m_getter{std::move(getter)},
        m_setter{std::move(setter)},
        m_onTune{std::move(onTune)},
        m_value{MakeValue(value, robust, isMutable, std::move(valueType),
                          std::move(elementType), std::move(properties),
                          std::move(typeString), alwaysGet)} {
    py::gil_scoped_acquire gil;
    m_lastStructData = PackCachedStructData();
  }

  wpi::tunables::detail::TunableBase& GetBase() {
    return std::visit(
        [](auto& value) -> wpi::tunables::detail::TunableBase& {
          return value;
        },
        m_value);
  }

  py::object Get() const {
    if (!m_getter.is_none()) {
      return m_getter();
    }
    return GetCached();
  }

  void Set(py::handle value) {
    py::object pyValue = py::reinterpret_borrow<py::object>(value);
    if (!m_setter.is_none()) {
      m_setter(pyValue);
    }
    if (!m_getter.is_none()) {
      SetCached(m_getter());
    } else {
      SetCached(pyValue);
    }
  }

  py::object Mutate() {
    if (!m_getter.is_none()) {
      py::object value = m_getter();
      SetCached(value);
      return value;
    }
    return MutateCached();
  }

  void Refresh() {
    if (!m_getter.is_none()) {
      py::gil_scoped_acquire gil;
      SetCachedIfChanged(m_getter());
    }
  }

  bool NeedsRefresh() const { return !m_getter.is_none(); }

 private:
  friend class PyMutationList;

  template <typename T>
  static bool CachedValuesEqual(const T& lhs, const T& rhs) {
    return lhs == rhs;
  }

  template <typename T>
  static constexpr bool IsStructCachedValue =
      std::same_as<T, WPyStruct> || std::same_as<T, std::vector<WPyStruct>>;

  static std::vector<uint8_t> PackStructValue(const WPyStruct& value,
                                              const WPyStructInfo& info) {
    std::vector<uint8_t> data(wpi::util::GetStructSize<WPyStruct>(info));
    wpi::util::PackStruct(data, value, info);
    return data;
  }

  template <typename T>
  static std::optional<std::vector<uint8_t>> PackStructData(const T&) {
    return std::nullopt;
  }

  static std::optional<std::vector<uint8_t>> PackStructData(
      const WPyStruct& value) {
    return PackStructValue(value, WPyStructInfo{value});
  }

  static std::optional<std::vector<uint8_t>> PackStructData(
      const std::vector<WPyStruct>& values) {
    if (values.empty()) {
      return std::vector<uint8_t>{};
    }
    WPyStructInfo info{values.front()};
    const size_t itemSize = wpi::util::GetStructSize<WPyStruct>(info);
    std::vector<uint8_t> data(itemSize * values.size());
    for (size_t i = 0; i < values.size(); ++i) {
      wpi::util::PackStruct(
          std::span<uint8_t>{data}.subspan(i * itemSize, itemSize), values[i],
          info);
    }
    return data;
  }

  template <typename T>
  static T ToCachedValue(py::handle value) {
    if constexpr (std::same_as<T, std::vector<uint8_t>>) {
      return ToRawVector(value);
    } else if constexpr (std::same_as<T, WPyStruct>) {
      return WPyStruct{py::reinterpret_borrow<py::object>(value)};
    } else if constexpr (std::same_as<T, std::vector<WPyStruct>>) {
      return ToStructVector(py::reinterpret_borrow<py::sequence>(value), true);
    } else {
      return py::cast<T>(value);
    }
  }

  py::object GetCached() const {
    return std::visit(
        [](const auto& value) -> py::object {
          using T = std::remove_cvref_t<decltype(value.Get())>;
          if constexpr (std::same_as<T, std::vector<uint8_t>>) {
            const auto& raw = value.Get();
            return py::bytes{reinterpret_cast<const char*>(raw.data()),
                             raw.size()};
          } else if constexpr (std::same_as<T, WPyStruct>) {
            return value.Get().py;
          } else if constexpr (std::same_as<T, std::vector<WPyStruct>>) {
            py::list data;
            for (auto&& item : value.Get()) {
              data.append(item.py);
            }
            return std::move(data);
          } else {
            return py::cast(value.Get());
          }
        },
        m_value);
  }

  py::object MutateCached() {
    auto owner = shared_from_this();
    return std::visit(
        [&owner](auto& tunable) -> py::object {
          auto& value = tunable.Mutate();
          using T = std::remove_cvref_t<decltype(value)>;
          if constexpr (std::same_as<T, WPyStruct>) {
            return value.py;
          } else if constexpr (IsStdVectorV<T>) {
            return MakeMutationList(owner, ToPythonList(value));
          } else {
            return py::cast(value);
          }
        },
        m_value);
  }

  void SetCached(py::handle value) {
    std::visit(
        [&](auto& tunable) {
          using T = std::remove_cvref_t<decltype(tunable.Get())>;
          auto newValue = ToCachedValue<T>(value);
          auto structData = PackStructData(newValue);
          tunable.Set(std::move(newValue));
          if (structData) {
            m_lastStructData = std::move(*structData);
          }
        },
        m_value);
  }

  void SetCachedIfChanged(py::handle value) {
    std::visit(
        [&](auto& tunable) {
          using T = std::remove_cvref_t<decltype(tunable.Get())>;
          auto newValue = ToCachedValue<T>(value);
          if constexpr (IsStructCachedValue<T>) {
            auto structData = PackStructData(newValue);
            if (!m_lastStructData || *m_lastStructData != *structData) {
              tunable.Set(std::move(newValue));
              m_lastStructData = std::move(*structData);
            }
          } else if (!CachedValuesEqual(newValue, tunable.Get())) {
            tunable.Set(std::move(newValue));
          }
        },
        m_value);
  }

  std::optional<std::vector<uint8_t>> PackCachedStructData() const {
    return std::visit(
        [](const auto& tunable) { return PackStructData(tunable.Get()); },
        m_value);
  }

  wpi::tunables::TunableConfig MakeConfig(bool robust, bool isMutable,
                                          py::handle properties,
                                          std::string typeString,
                                          bool alwaysGet) {
    wpi::tunables::TunableConfig config{
        .robust = robust,
        .isMutable = isMutable,
        .polling = alwaysGet ? wpi::tunables::TunableConfig::Polling::ALWAYS_GET
                             : wpi::tunables::TunableConfig::Polling::DEFAULT};
    if (!m_onTune.is_none()) {
      config.onTune = [this](wpi::tunables::detail::TunableBase&,
                             wpi::tunables::ComplexTunable*) {
        py::gil_scoped_acquire gil;
        m_onTune(GetCached());
      };
    }
    if (!m_getter.is_none() || !m_setter.is_none()) {
      config.onRemoteSet = [this](wpi::tunables::detail::TunableBase&,
                                  wpi::tunables::ComplexTunable*) {
        py::gil_scoped_acquire gil;
        if (!m_setter.is_none()) {
          m_setter(GetCached());
        }
        if (!m_getter.is_none()) {
          SetCached(m_getter());
        }
      };
    }
    if (!typeString.empty()) {
      config.typeString = std::move(typeString);
    }
    if (!properties.is_none()) {
      config.properties = ToJson(properties);
    }
    return config;
  }

  TunableVariant MakeValue(py::handle value, bool robust, bool isMutable,
                           py::object valueType, py::object elementType,
                           py::object properties, std::string typeString,
                           bool alwaysGet) {
    auto kind = InferValueKind(value, valueType, elementType);
    auto config = MakeConfig(robust, isMutable, properties,
                             std::move(typeString), alwaysGet);
    switch (kind) {
      case ValueKind::BOOLEAN:
        return wpi::tunables::TunableBool{value.cast<bool>(), config};
      case ValueKind::INTEGER:
        return wpi::tunables::TunableInt64{value.cast<int64_t>(), config};
      case ValueKind::DOUBLE:
        return wpi::tunables::TunableDouble{value.cast<double>(), config};
      case ValueKind::STRING:
        return wpi::tunables::TunableString{value.cast<std::string>(), config};
      case ValueKind::RAW:
        return wpi::tunables::TunableRaw{ToRawVector(value), config};
      case ValueKind::BOOLEAN_ARRAY:
        return wpi::tunables::TunableBoolVector{value.cast<std::vector<bool>>(),
                                                config};
      case ValueKind::INTEGER_ARRAY:
        return wpi::tunables::TunableInt64Vector{
            value.cast<std::vector<int64_t>>(), config};
      case ValueKind::DOUBLE_ARRAY:
        return wpi::tunables::TunableDoubleVector{
            value.cast<std::vector<double>>(), config};
      case ValueKind::STRING_ARRAY:
        return wpi::tunables::TunableStringVector{
            value.cast<std::vector<std::string>>(), config};
      case ValueKind::STRUCT: {
        py::type type = IsWpiStructType(valueType)
                            ? py::reinterpret_borrow<py::type>(valueType)
                            : py::type::of(value);
        int isInstance = PyObject_IsInstance(value.ptr(), type.ptr());
        if (isInstance < 0) {
          throw py::error_already_set{};
        }
        if (isInstance == 0) {
          throw py::type_error(
              "struct tunables require values of the specified WPIStruct "
              "type");
        }
        WPyStructInfo info{type};
        return wpi::tunables::Tunable<WPyStruct, WPyStructInfo>{
            config, std::move(info),
            WPyStruct{py::reinterpret_borrow<py::object>(value)}};
      }
      case ValueKind::STRUCT_ARRAY: {
        auto sequence = py::reinterpret_borrow<py::sequence>(value);
        py::type type = IsWpiStructType(elementType)
                            ? py::reinterpret_borrow<py::type>(elementType)
                            : GetStructSequenceType(sequence);
        ValidateStructSequenceType(sequence, type);
        WPyStructInfo info{type};
        return wpi::tunables::Tunable<std::vector<WPyStruct>, WPyStructInfo>{
            config, std::move(info), ToStructVector(sequence, true)};
      }
    }
    throw py::type_error("unsupported tunable value_type");
  }

  py::object m_getter;
  py::object m_setter;
  py::object m_onTune;
  TunableVariant m_value;
  std::optional<std::vector<uint8_t>> m_lastStructData;
};

class PyMutationList {
 public:
  PyMutationList(std::shared_ptr<PyTunable> owner, py::list data)
      : m_owner{std::move(owner)}, m_data{std::move(data)} {}

  size_t Size() const { return py::len(m_data); }

  py::iterator Iter() const { return py::iter(m_data); }

  py::object GetItem(py::object key) const {
    return m_data.attr("__getitem__")(key);
  }

  void SetItem(py::object key, py::object value) {
    m_data.attr("__setitem__")(key, value);
    Sync();
  }

  void DelItem(py::object key) {
    m_data.attr("__delitem__")(key);
    Sync();
  }

  bool Contains(py::object value) const {
    int result = PySequence_Contains(m_data.ptr(), value.ptr());
    if (result < 0) {
      throw py::error_already_set{};
    }
    return result == 1;
  }

  bool Equal(py::object value) const {
    int result = PyObject_RichCompareBool(m_data.ptr(), value.ptr(), Py_EQ);
    if (result < 0) {
      throw py::error_already_set{};
    }
    return result == 1;
  }

  std::string Repr() const { return py::repr(m_data).cast<std::string>(); }

  py::list Copy() const {
    return py::reinterpret_borrow<py::list>(m_data.attr("copy")());
  }

  void Append(py::object value) {
    m_data.attr("append")(value);
    Sync();
  }

  void Extend(py::object value) {
    m_data.attr("extend")(value);
    Sync();
  }

  void Insert(py::ssize_t index, py::object value) {
    m_data.attr("insert")(index, value);
    Sync();
  }

  py::object Pop(py::args args) {
    py::object value = m_data.attr("pop")(*args);
    Sync();
    return value;
  }

  void Remove(py::object value) {
    m_data.attr("remove")(value);
    Sync();
  }

  void Clear() {
    m_data.attr("clear")();
    Sync();
  }

  void Reverse() {
    m_data.attr("reverse")();
    Sync();
  }

  void Sort(py::args args, py::kwargs kwargs) {
    m_data.attr("sort")(*args, **kwargs);
    Sync();
  }

  PyMutationList& IAdd(py::object value) {
    Extend(value);
    return *this;
  }

 private:
  void Sync() { m_owner->SetCached(m_data); }

  std::shared_ptr<PyTunable> m_owner;
  py::list m_data;
};

py::object MakeMutationList(std::shared_ptr<PyTunable> owner, py::list data) {
  return py::cast(PyMutationList{std::move(owner), std::move(data)});
}

class PyComplexTunableAdapter;

template <typename T>
std::vector<std::unique_ptr<std::vector<std::shared_ptr<T>>>>&
GetSnapshotStorage() {
  static thread_local std::vector<
      std::unique_ptr<std::vector<std::shared_ptr<T>>>>
      storage;
  return storage;
}

template <typename T>
size_t& GetSnapshotDepth() {
  static thread_local size_t depth = 0;
  return depth;
}

template <typename T>
class RetainedValueSnapshot {
 public:
  template <typename Range>
  explicit RetainedValueSnapshot(const Range& values)
      : m_depth{GetSnapshotDepth<T>()},
        m_values{GetSnapshotValueStorage(m_depth)} {
    m_values.clear();
    m_values.reserve(values.size());
    for (const auto& entry : values) {
      m_values.emplace_back(entry.second);
    }
    ++GetSnapshotDepth<T>();
  }

  RetainedValueSnapshot(const RetainedValueSnapshot&) = delete;
  RetainedValueSnapshot& operator=(const RetainedValueSnapshot&) = delete;
  RetainedValueSnapshot(RetainedValueSnapshot&&) = delete;
  RetainedValueSnapshot& operator=(RetainedValueSnapshot&&) = delete;

  ~RetainedValueSnapshot() {
    --GetSnapshotDepth<T>();
    m_values.clear();
  }

  auto begin() const { return m_values.begin(); }
  auto end() const { return m_values.end(); }

 private:
  static std::vector<std::shared_ptr<T>>& GetSnapshotValueStorage(
      size_t depth) {
    auto& storage = GetSnapshotStorage<T>();
    while (storage.size() <= depth) {
      storage.emplace_back(std::make_unique<std::vector<std::shared_ptr<T>>>());
    }
    return *storage[depth];
  }

  size_t m_depth;
  std::vector<std::shared_ptr<T>>& m_values;
};

template <typename T, typename Range>
RetainedValueSnapshot<T> SnapshotRetainedValues(const Range& values) {
  return RetainedValueSnapshot<T>{values};
}

std::string MakeChildPrefix(std::string_view path) {
  std::string prefix{path};
  if (prefix.empty() || prefix.back() != '/') {
    prefix.push_back('/');
  }
  return prefix;
}

bool IsPathOrDescendant(std::string_view candidate, std::string_view path,
                        std::string_view childPrefix) {
  return candidate == path || candidate.starts_with(childPrefix);
}

bool IsPathOrDescendant(std::string_view candidate, std::string_view path) {
  return IsPathOrDescendant(candidate, path, MakeChildPrefix(path));
}

void RemoveRefreshPath(std::string_view path);

class PyTunableTable {
 public:
  PyTunableTable(wpi::tunables::TunableTable table,
                 PyComplexTunableAdapter* owner)
      : m_table{std::move(table)}, m_owner{owner} {}

  std::string GetPath() const { return m_table.GetPath(); }
  PyTunableTable GetTable(std::string_view name) {
    return PyTunableTable{m_table.GetTable(name), m_owner};
  }
  bool Publish(std::string_view name, py::object value);
  std::shared_ptr<PyTunable> Add(std::string_view name, py::object value,
                                 py::object valueType, py::object elementType,
                                 bool robust, bool isMutable, py::object onTune,
                                 py::object properties, std::string typeString);
  std::shared_ptr<PyTunable> PublishValue(std::string_view name,
                                          py::object getter, py::object setter,
                                          py::object valueType,
                                          py::object elementType, bool robust,
                                          bool isMutable, py::object properties,
                                          std::string typeString);
  void Remove(std::string_view name);

 private:
  void StoreValue(std::string_view name, std::shared_ptr<PyTunable> tunable);

  wpi::tunables::TunableTable m_table;
  PyComplexTunableAdapter* m_owner;
};

class PyComplexTunableAdapter : public wpi::tunables::ComplexTunable {
 public:
  PyComplexTunableAdapter(py::object value, py::object initialPublishTunable)
      : m_value{std::move(value)},
        m_initialPublishTunable{std::move(initialPublishTunable)} {
    if (auto getTunableType = GetOptionalAttr(m_value, "get_tunable_type")) {
      py::object typeObj = (*getTunableType)();
      if (!typeObj.is_none()) {
        m_type = typeObj.cast<std::string>();
      }
    }
  }

  ~PyComplexTunableAdapter() override = default;

  std::string_view GetTunableType() const override { return m_type; }

  bool IsValue(py::handle value) const { return m_value.is(value); }

  void PublishTunable(wpi::tunables::TunableTable& table) override {
    py::gil_scoped_acquire gil;
    py::object publishTunable;
    if (m_initialPublishTunable) {
      publishTunable = std::move(*m_initialPublishTunable);
      m_initialPublishTunable.reset();
    } else {
      publishTunable = m_value.attr("publish_tunables");
    }
    publishTunable(PyTunableTable{table, this});
  }

  void UpdateTunable() const override {
    py::gil_scoped_acquire gil;
    py::object updateTunable =
        py::getattr(m_value, "update_tunables", py::none());
    if (!updateTunable.is_none()) {
      updateTunable();
    }
  }

  void AddValue(std::string path, std::shared_ptr<PyTunable> value) {
    for (auto&& child : m_values) {
      if (child.first == path) {
        child.second = std::move(value);
        return;
      }
    }
    m_values.emplace_back(std::move(path), std::move(value));
  }
  void AddComplex(std::string path,
                  std::shared_ptr<PyComplexTunableAdapter> value) {
    for (auto&& child : m_complex) {
      if (child.first == path) {
        child.second = std::move(value);
        return;
      }
    }
    m_complex.emplace_back(std::move(path), std::move(value));
  }
  void AddNativeComplex(std::string path, py::object value) {
    for (auto&& child : m_nativeComplex) {
      if (child.first == path) {
        child.second = std::move(value);
        return;
      }
    }
    m_nativeComplex.emplace_back(std::move(path), std::move(value));
  }

  void RemovePath(std::string_view path) {
    {
      py::gil_scoped_release release;
      wpi::tunables::TunableRegistry::Remove(path);
    }
    RemoveRetainedPath(path);
  }

  void RemoveRetainedPath(std::string_view path) {
    RemoveRefreshPath(path);
    std::string childPrefix = MakeChildPrefix(path);
    std::erase_if(m_values, [&](auto&& child) {
      return IsPathOrDescendant(child.first, path, childPrefix);
    });
    for (auto it = m_complex.begin(); it != m_complex.end();) {
      if (IsPathOrDescendant(it->first, path, childPrefix)) {
        it = m_complex.erase(it);
      } else if (IsPathOrDescendant(path, it->first)) {
        it->second->RemoveRetainedPath(path);
        ++it;
      } else {
        ++it;
      }
    }
    std::erase_if(m_nativeComplex, [&](auto&& child) {
      return IsPathOrDescendant(child.first, path, childPrefix);
    });
  }

 private:
  py::object m_value;
  std::optional<py::object> m_initialPublishTunable;
  std::string m_type;
  mutable std::vector<std::pair<std::string, std::shared_ptr<PyTunable>>>
      m_values;
  std::vector<std::pair<std::string, std::shared_ptr<PyComplexTunableAdapter>>>
      m_complex;
  std::vector<std::pair<std::string, py::object>> m_nativeComplex;
};

std::unordered_map<std::string, std::shared_ptr<PyTunable>>& GetValues() {
  static std::unordered_map<std::string, std::shared_ptr<PyTunable>> values;
  return values;
}

std::unordered_map<std::string, std::shared_ptr<PyTunable>>&
GetRefreshValues() {
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

std::unordered_map<std::string, py::object>& GetNativeComplexValues() {
  static std::unordered_map<std::string, py::object> values;
  return values;
}

void StoreRefreshValue(std::string_view path,
                       const std::shared_ptr<PyTunable>& tunable) {
  auto& values = GetRefreshValues();
  if (tunable->NeedsRefresh()) {
    values.insert_or_assign(std::string{path}, tunable);
  } else {
    values.erase(std::string{path});
  }
}

void RemoveRefreshPath(std::string_view path) {
  std::string childPrefix = MakeChildPrefix(path);
  std::erase_if(GetRefreshValues(), [&](auto&& child) {
    return IsPathOrDescendant(child.first, path, childPrefix);
  });
}

void RefreshValues() {
  py::gil_scoped_acquire gil;
  auto values = SnapshotRetainedValues<PyTunable>(GetRefreshValues());
  for (auto&& value : values) {
    value->Refresh();
  }
}

void ClearValues() {
  GetValues().clear();
  GetRefreshValues().clear();
  GetComplexValues().clear();
  GetNativeComplexValues().clear();
}

void RegisterPreUpdateCallback() {
  wpi::tunables::detail::SetTunableRegistryPreUpdateCallback(
      [] { RefreshValues(); });
}

void CleanupPythonStorage() {
  wpi::tunables::detail::SetTunableRegistryPreUpdateCallback(nullptr);
  ClearValues();
}

std::string NormalizePath(std::string_view path) {
  std::string buf;
  return std::string{wpi::tunables::detail::NormalizeName(path, buf)};
}

std::string NormalizeTablePath(const wpi::tunables::TunableTable& table,
                               std::string_view name) {
  return NormalizePath(table.GetPath() + std::string{name});
}

void RemoveRetainedPath(std::string_view path) {
  RemoveRefreshPath(path);
  std::string childPrefix = MakeChildPrefix(path);
  std::erase_if(GetValues(), [&](auto&& child) {
    return IsPathOrDescendant(child.first, path, childPrefix);
  });
  auto& complexValues = GetComplexValues();
  for (auto it = complexValues.begin(); it != complexValues.end();) {
    if (IsPathOrDescendant(it->first, path, childPrefix)) {
      it = complexValues.erase(it);
    } else if (IsPathOrDescendant(path, it->first)) {
      it->second->RemoveRetainedPath(path);
      ++it;
    } else {
      ++it;
    }
  }
  std::erase_if(GetNativeComplexValues(), [&](auto&& child) {
    return IsPathOrDescendant(child.first, path, childPrefix);
  });
}

void RemovePath(std::string_view path) {
  std::string normalized = NormalizePath(path);
  {
    py::gil_scoped_release release;
    wpi::tunables::TunableRegistry::Remove(normalized);
  }
  RemoveRetainedPath(normalized);
}

void RemoveValue(py::handle value) {
  std::vector<std::string> paths;

  if (py::isinstance<PyTunable>(value)) {
    auto tunable = value.cast<std::shared_ptr<PyTunable>>();
    for (auto&& [path, retained] : GetValues()) {
      if (retained == tunable) {
        paths.emplace_back(path);
      }
    }
  }

  for (auto&& [path, retained] : GetComplexValues()) {
    if (retained->IsValue(value)) {
      paths.emplace_back(path);
    }
  }

  for (auto&& [path, retained] : GetNativeComplexValues()) {
    if (retained.is(value)) {
      paths.emplace_back(path);
    }
  }

  for (auto&& path : paths) {
    RemovePath(path);
  }
}

void PyTunableTable::StoreValue(std::string_view name,
                                std::shared_ptr<PyTunable> tunable) {
  std::string path = NormalizeTablePath(m_table, name);
  if (m_owner) {
    m_owner->AddValue(path, tunable);
  } else {
    GetValues().insert_or_assign(path, tunable);
  }
  StoreRefreshValue(path, tunable);
}

bool PyTunableTable::Publish(std::string_view name, py::object value) {
  bool published;
  if (py::isinstance<PyTunable>(value)) {
    auto tunable = value.cast<std::shared_ptr<PyTunable>>();
    {
      std::string nameString{name};
      py::gil_scoped_release release;
      published = m_table.Publish(nameString, tunable->GetBase());
    }
    if (published) {
      StoreValue(name, std::move(tunable));
    }
  } else if (py::isinstance<wpi::tunables::ComplexTunable>(value)) {
    auto& tunable = value.cast<wpi::tunables::ComplexTunable&>();
    {
      std::string nameString{name};
      py::gil_scoped_release release;
      published = m_table.Publish(nameString, tunable);
    }
    if (published) {
      std::string path = NormalizeTablePath(m_table, name);
      if (m_owner) {
        m_owner->AddNativeComplex(std::move(path), std::move(value));
      } else {
        GetNativeComplexValues().insert_or_assign(std::move(path),
                                                  std::move(value));
      }
    }
  } else if (auto publishTunable = GetOptionalAttr(value, "publish_tunables")) {
    auto tunable = std::make_shared<PyComplexTunableAdapter>(
        std::move(value), std::move(*publishTunable));
    {
      std::string nameString{name};
      py::gil_scoped_release release;
      published = m_table.Publish(nameString, *tunable);
    }
    if (published) {
      std::string path = NormalizeTablePath(m_table, name);
      if (m_owner) {
        m_owner->AddComplex(std::move(path), std::move(tunable));
      } else {
        GetComplexValues().insert_or_assign(std::move(path),
                                            std::move(tunable));
      }
    }
  } else {
    throw py::type_error(
        "value must be a wpi::tunables::Tunable or "
        "wpi::tunables::ComplexTunable");
  }
  return published;
}

std::shared_ptr<PyTunable> PyTunableTable::Add(
    std::string_view name, py::object value, py::object valueType,
    py::object elementType, bool robust, bool isMutable, py::object onTune,
    py::object properties, std::string typeString) {
  auto tunable = std::make_shared<PyTunable>(
      value, py::none(), py::none(), std::move(onTune), robust, isMutable,
      std::move(valueType), std::move(elementType), std::move(properties),
      std::move(typeString), false);
  bool published;
  {
    std::string nameString{name};
    py::gil_scoped_release release;
    published = m_table.Publish(nameString, tunable->GetBase());
  }
  if (published) {
    StoreValue(name, tunable);
  }
  return tunable;
}

std::shared_ptr<PyTunable> PyTunableTable::PublishValue(
    std::string_view name, py::object getter, py::object setter,
    py::object valueType, py::object elementType, bool robust, bool isMutable,
    py::object properties, std::string typeString) {
  auto value = std::make_shared<PyTunable>(
      getter(), getter, setter, py::none(), robust, isMutable,
      std::move(valueType), std::move(elementType), std::move(properties),
      std::move(typeString), false);
  bool published;
  {
    std::string nameString{name};
    py::gil_scoped_release release;
    published = m_table.Publish(nameString, value->GetBase());
  }
  if (published) {
    StoreValue(name, value);
  }
  return value;
}

void PyTunableTable::Remove(std::string_view name) {
  std::string path = NormalizeTablePath(m_table, name);
  if (m_owner) {
    m_owner->RemovePath(path);
  } else {
    RemovePath(path);
  }
}

void RemoveRootValue(std::string_view name) {
  RemovePath("/" + std::string{name});
}

void SetRaw(wpi::tunables::MockTunableBackend& self, std::string_view path,
            py::handle value) {
  auto raw = ToRawVector(value);
  self.SetRaw(path, std::span<const uint8_t>{raw.data(), raw.size()});
}

template <typename T, typename F>
void SetVector(wpi::tunables::MockTunableBackend& self, std::string_view path,
               const py::sequence& value, F setter) {
  std::vector<T> data;
  const size_t size = py::len(value);
  data.reserve(size);
  for (size_t i = 0; i < size; ++i) {
    data.emplace_back(value[static_cast<py::ssize_t>(i)].cast<T>());
  }
  (self.*setter)(path, std::span<const T>{data.data(), data.size()});
}

void SetBoolVector(wpi::tunables::MockTunableBackend& self,
                   std::string_view path, const py::sequence& value) {
  const size_t size = py::len(value);
  auto data = std::make_unique<bool[]>(size);
  for (size_t i = 0; i < size; ++i) {
    data[i] = value[static_cast<py::ssize_t>(i)].cast<bool>();
  }
  self.SetBoolVector(path, std::span<const bool>{data.get(), size});
}

void SetStruct(wpi::tunables::MockTunableBackend& self, std::string_view path,
               py::handle value) {
  WPyStructInfo info{py::type::of(value)};
  self.SetStruct<WPyStruct, WPyStructInfo>(
      path, WPyStruct{py::reinterpret_borrow<py::object>(value)},
      std::move(info));
}

void SetStructVector(wpi::tunables::MockTunableBackend& self,
                     std::string_view path, const py::sequence& value) {
  WPyStructInfo info{GetStructSequenceType(value)};
  auto data = ToStructVector(value);
  self.SetStructVector<WPyStruct, WPyStructInfo>(
      path, std::span<const WPyStruct>{data.data(), data.size()},
      std::move(info));
}

py::object GetUid(const wpi::tunables::MockTunableBackend& self,
                  std::string_view path) {
  if (auto uid = self.GetUid(path)) {
    return py::int_{*uid};
  }
  return py::none{};
}

template <typename T>
py::object ValueToPython(const T& value) {
  if constexpr (std::same_as<T, std::vector<uint8_t>>) {
    return py::bytes{reinterpret_cast<const char*>(value.data()), value.size()};
  } else {
    return py::cast(value);
  }
}

template <typename T>
py::object ReadTunableValue(
    const wpi::tunables::TunableRegistry::TunableInfo& info) {
  if (auto v = wpi::tunables::detail::CastTunable<T, false>(info.tunable,
                                                            info.type)) {
    return ValueToPython(v->Get());
  }
  if (auto v = wpi::tunables::detail::CastTunable<T, true>(info.tunable,
                                                           info.type)) {
    return ValueToPython(v->Get(info.config->parent));
  }
  throw py::type_error("tunable has unexpected type");
}

py::object ReadStructValue(
    const wpi::tunables::TunableRegistry::TunableInfo& info) {
  if (auto v = wpi::tunables::detail::CastTunable<
          wpi::tunables::detail::TunableStructTag, false>(info.tunable,
                                                          info.type)) {
    std::vector<uint8_t> data(v->GetStructSize());
    v->PackStruct(data);
    return py::bytes{reinterpret_cast<const char*>(data.data()), data.size()};
  }
  if (auto v = wpi::tunables::detail::CastTunable<
          wpi::tunables::detail::TunableStructTag, true>(info.tunable,
                                                         info.type)) {
    std::vector<uint8_t> data(v->GetStructSize(info.config->parent));
    v->PackStruct(info.config->parent, data);
    return py::bytes{reinterpret_cast<const char*>(data.data()), data.size()};
  }
  throw py::type_error("tunable has unexpected type");
}

py::object GetTunableValue(const wpi::tunables::MockTunableBackend& self,
                           std::string_view path) {
  auto uid = self.GetUid(path);
  if (!uid) {
    throw py::value_error("no tunable at path");
  }

  auto info = wpi::tunables::TunableRegistry::GetTunable(*uid);
  if (!info) {
    throw py::value_error("no registered tunable for path");
  }

  using Type = wpi::tunables::detail::TunableTypeValue;
  switch (info.type) {
    case Type::BOOLEAN:
    case Type::MEMBER_BOOLEAN:
      return ReadTunableValue<bool>(info);
    case Type::INT32:
    case Type::MEMBER_INT32:
      return ReadTunableValue<int32_t>(info);
    case Type::INT64:
    case Type::MEMBER_INT64:
      return ReadTunableValue<int64_t>(info);
    case Type::FLOAT:
    case Type::MEMBER_FLOAT:
      return ReadTunableValue<float>(info);
    case Type::DOUBLE:
    case Type::MEMBER_DOUBLE:
      return ReadTunableValue<double>(info);
    case Type::STRING:
    case Type::MEMBER_STRING:
      return ReadTunableValue<std::string>(info);
    case Type::RAW:
    case Type::MEMBER_RAW:
      return ReadTunableValue<std::vector<uint8_t>>(info);
    case Type::BOOLEAN_ARRAY:
    case Type::MEMBER_BOOLEAN_ARRAY:
      return ReadTunableValue<std::vector<bool>>(info);
    case Type::INT32_ARRAY:
    case Type::MEMBER_INT32_ARRAY:
      return ReadTunableValue<std::vector<int32_t>>(info);
    case Type::INT64_ARRAY:
    case Type::MEMBER_INT64_ARRAY:
      return ReadTunableValue<std::vector<int64_t>>(info);
    case Type::FLOAT_ARRAY:
    case Type::MEMBER_FLOAT_ARRAY:
      return ReadTunableValue<std::vector<float>>(info);
    case Type::DOUBLE_ARRAY:
    case Type::MEMBER_DOUBLE_ARRAY:
      return ReadTunableValue<std::vector<double>>(info);
    case Type::STRING_ARRAY:
    case Type::MEMBER_STRING_ARRAY:
      return ReadTunableValue<std::vector<std::string>>(info);
    case Type::STRUCT:
    case Type::MEMBER_STRUCT:
      return ReadStructValue(info);
    default:
      return py::none{};
  }
}

}  // namespace

void wpi::InitTunablePython(py::module_& m) {
  RegisterPreUpdateCallback();

  static int unused;
  py::capsule cleanup(&unused, [](void*) {
    py::gil_scoped_acquire gil;
    CleanupPythonStorage();
  });
  m.add_object("_tunable_cleanup", cleanup);

  py::class_<PyMutationList>(m, "_MutationList")
      .def("__len__", &PyMutationList::Size)
      .def("__iter__", &PyMutationList::Iter, py::keep_alive<0, 1>())
      .def("__getitem__", &PyMutationList::GetItem)
      .def("__setitem__", &PyMutationList::SetItem)
      .def("__delitem__", &PyMutationList::DelItem)
      .def("__contains__", &PyMutationList::Contains)
      .def("__eq__", &PyMutationList::Equal)
      .def("__repr__", &PyMutationList::Repr)
      .def("__iadd__", &PyMutationList::IAdd,
           py::return_value_policy::reference_internal)
      .def("copy", &PyMutationList::Copy)
      .def("append", &PyMutationList::Append)
      .def("extend", &PyMutationList::Extend)
      .def("insert", &PyMutationList::Insert)
      .def("pop", &PyMutationList::Pop)
      .def("remove", &PyMutationList::Remove)
      .def("clear", &PyMutationList::Clear)
      .def("reverse", &PyMutationList::Reverse)
      .def("sort", &PyMutationList::Sort);

  py::class_<PyTunable, std::shared_ptr<PyTunable>>(m, "Tunable")
      .def(py::init<py::object, py::object, py::object, py::object, bool, bool,
                    py::object, py::object, py::object, std::string, bool>(),
           py::arg("value"), py::kw_only(), py::arg("getter") = py::none(),
           py::arg("setter") = py::none(), py::arg("on_tune") = py::none(),
           py::arg("robust") = false, py::arg("mutable") = true,
           py::arg("value_type") = py::none(),
           py::arg("element_type") = py::none(),
           py::arg("properties") = py::none(), py::arg("type_string") = "",
           py::arg("always_get") = false)
      .def("get", &PyTunable::Get)
      .def("set", &PyTunable::Set)
      .def("mutate", &PyTunable::Mutate);

  py::class_<wpi::tunables::TunableTable>(m, "_NativeTunableTable")
      .def_property_readonly("path", &wpi::tunables::TunableTable::GetPath)
      .def("get_table", &wpi::tunables::TunableTable::GetTable)
      .def("remove", &wpi::tunables::TunableTable::Remove);

  py::class_<PyTunableTable>(m, "TunableTable")
      .def_property_readonly("path", &PyTunableTable::GetPath)
      .def("get_table", &PyTunableTable::GetTable)
      .def("publish", &PyTunableTable::Publish)
      .def("add", &PyTunableTable::Add, py::arg("name"), py::arg("value"),
           py::kw_only(), py::arg("value_type") = py::none(),
           py::arg("element_type") = py::none(), py::arg("robust") = false,
           py::arg("mutable") = true, py::arg("on_tune") = py::none(),
           py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def(
          "add_boolean",
          [](PyTunableTable& self, std::string_view name, py::object value,
             bool robust, bool isMutable, py::object onTune,
             py::object properties, std::string typeString) {
            return self.Add(name, std::move(value), BuiltinType("bool"),
                            py::none(), robust, isMutable, std::move(onTune),
                            std::move(properties), std::move(typeString));
          },
          py::arg("name"), py::arg("value"), py::kw_only(),
          py::arg("robust") = false, py::arg("mutable") = true,
          py::arg("on_tune") = py::none(), py::arg("properties") = py::none(),
          py::arg("type_string") = "")
      .def(
          "add_int",
          [](PyTunableTable& self, std::string_view name, py::object value,
             bool robust, bool isMutable, py::object onTune,
             py::object properties, std::string typeString) {
            return self.Add(name, std::move(value), BuiltinType("int"),
                            py::none(), robust, isMutable, std::move(onTune),
                            std::move(properties), std::move(typeString));
          },
          py::arg("name"), py::arg("value"), py::kw_only(),
          py::arg("robust") = false, py::arg("mutable") = true,
          py::arg("on_tune") = py::none(), py::arg("properties") = py::none(),
          py::arg("type_string") = "")
      .def(
          "add_long",
          [](PyTunableTable& self, std::string_view name, py::object value,
             bool robust, bool isMutable, py::object onTune,
             py::object properties, std::string typeString) {
            return self.Add(name, std::move(value), BuiltinType("int"),
                            py::none(), robust, isMutable, std::move(onTune),
                            std::move(properties), std::move(typeString));
          },
          py::arg("name"), py::arg("value"), py::kw_only(),
          py::arg("robust") = false, py::arg("mutable") = true,
          py::arg("on_tune") = py::none(), py::arg("properties") = py::none(),
          py::arg("type_string") = "")
      .def(
          "add_float",
          [](PyTunableTable& self, std::string_view name, py::object value,
             bool robust, bool isMutable, py::object onTune,
             py::object properties, std::string typeString) {
            return self.Add(name, std::move(value), BuiltinType("float"),
                            py::none(), robust, isMutable, std::move(onTune),
                            std::move(properties), std::move(typeString));
          },
          py::arg("name"), py::arg("value"), py::kw_only(),
          py::arg("robust") = false, py::arg("mutable") = true,
          py::arg("on_tune") = py::none(), py::arg("properties") = py::none(),
          py::arg("type_string") = "")
      .def(
          "add_double",
          [](PyTunableTable& self, std::string_view name, py::object value,
             bool robust, bool isMutable, py::object onTune,
             py::object properties, std::string typeString) {
            return self.Add(name, std::move(value), BuiltinType("float"),
                            py::none(), robust, isMutable, std::move(onTune),
                            std::move(properties), std::move(typeString));
          },
          py::arg("name"), py::arg("value"), py::kw_only(),
          py::arg("robust") = false, py::arg("mutable") = true,
          py::arg("on_tune") = py::none(), py::arg("properties") = py::none(),
          py::arg("type_string") = "")
      .def("publish_value", &PyTunableTable::PublishValue, py::arg("name"),
           py::arg("getter"), py::arg("setter"), py::kw_only(),
           py::arg("value_type") = py::none(),
           py::arg("element_type") = py::none(), py::arg("robust") = false,
           py::arg("mutable") = true, py::arg("properties") = py::none(),
           py::arg("type_string") = "")
      .def(
          "publish_boolean",
          [](PyTunableTable& self, std::string_view name, py::object getter,
             py::object setter, bool robust, bool isMutable,
             py::object properties, std::string typeString) {
            return self.PublishValue(name, std::move(getter), std::move(setter),
                                     BuiltinType("bool"), py::none(), robust,
                                     isMutable, std::move(properties),
                                     std::move(typeString));
          },
          py::arg("name"), py::arg("getter"), py::arg("setter"), py::kw_only(),
          py::arg("robust") = false, py::arg("mutable") = true,
          py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def(
          "publish_int",
          [](PyTunableTable& self, std::string_view name, py::object getter,
             py::object setter, bool robust, bool isMutable,
             py::object properties, std::string typeString) {
            return self.PublishValue(name, std::move(getter), std::move(setter),
                                     BuiltinType("int"), py::none(), robust,
                                     isMutable, std::move(properties),
                                     std::move(typeString));
          },
          py::arg("name"), py::arg("getter"), py::arg("setter"), py::kw_only(),
          py::arg("robust") = false, py::arg("mutable") = true,
          py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def(
          "publish_long",
          [](PyTunableTable& self, std::string_view name, py::object getter,
             py::object setter, bool robust, bool isMutable,
             py::object properties, std::string typeString) {
            return self.PublishValue(name, std::move(getter), std::move(setter),
                                     BuiltinType("int"), py::none(), robust,
                                     isMutable, std::move(properties),
                                     std::move(typeString));
          },
          py::arg("name"), py::arg("getter"), py::arg("setter"), py::kw_only(),
          py::arg("robust") = false, py::arg("mutable") = true,
          py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def(
          "publish_float",
          [](PyTunableTable& self, std::string_view name, py::object getter,
             py::object setter, bool robust, bool isMutable,
             py::object properties, std::string typeString) {
            return self.PublishValue(name, std::move(getter), std::move(setter),
                                     BuiltinType("float"), py::none(), robust,
                                     isMutable, std::move(properties),
                                     std::move(typeString));
          },
          py::arg("name"), py::arg("getter"), py::arg("setter"), py::kw_only(),
          py::arg("robust") = false, py::arg("mutable") = true,
          py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def(
          "publish_double",
          [](PyTunableTable& self, std::string_view name, py::object getter,
             py::object setter, bool robust, bool isMutable,
             py::object properties, std::string typeString) {
            return self.PublishValue(name, std::move(getter), std::move(setter),
                                     BuiltinType("float"), py::none(), robust,
                                     isMutable, std::move(properties),
                                     std::move(typeString));
          },
          py::arg("name"), py::arg("getter"), py::arg("setter"), py::kw_only(),
          py::arg("robust") = false, py::arg("mutable") = true,
          py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def(
          "publish_string",
          [](PyTunableTable& self, std::string_view name, py::object getter,
             py::object setter, bool robust, bool isMutable,
             py::object properties, std::string typeString) {
            return self.PublishValue(name, std::move(getter), std::move(setter),
                                     BuiltinType("str"), py::none(), robust,
                                     isMutable, std::move(properties),
                                     std::move(typeString));
          },
          py::arg("name"), py::arg("getter"), py::arg("setter"), py::kw_only(),
          py::arg("robust") = false, py::arg("mutable") = true,
          py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def(
          "publish_boolean_array",
          [](PyTunableTable& self, std::string_view name, py::object getter,
             py::object setter, bool robust, bool isMutable,
             py::object properties, std::string typeString) {
            return self.PublishValue(name, std::move(getter), std::move(setter),
                                     py::none(), BuiltinType("bool"), robust,
                                     isMutable, std::move(properties),
                                     std::move(typeString));
          },
          py::arg("name"), py::arg("getter"), py::arg("setter"), py::kw_only(),
          py::arg("robust") = false, py::arg("mutable") = true,
          py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def(
          "publish_integer_array",
          [](PyTunableTable& self, std::string_view name, py::object getter,
             py::object setter, bool robust, bool isMutable,
             py::object properties, std::string typeString) {
            return self.PublishValue(name, std::move(getter), std::move(setter),
                                     py::none(), BuiltinType("int"), robust,
                                     isMutable, std::move(properties),
                                     std::move(typeString));
          },
          py::arg("name"), py::arg("getter"), py::arg("setter"), py::kw_only(),
          py::arg("robust") = false, py::arg("mutable") = true,
          py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def(
          "publish_double_array",
          [](PyTunableTable& self, std::string_view name, py::object getter,
             py::object setter, bool robust, bool isMutable,
             py::object properties, std::string typeString) {
            return self.PublishValue(name, std::move(getter), std::move(setter),
                                     py::none(), BuiltinType("float"), robust,
                                     isMutable, std::move(properties),
                                     std::move(typeString));
          },
          py::arg("name"), py::arg("getter"), py::arg("setter"), py::kw_only(),
          py::arg("robust") = false, py::arg("mutable") = true,
          py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def(
          "publish_string_array",
          [](PyTunableTable& self, std::string_view name, py::object getter,
             py::object setter, bool robust, bool isMutable,
             py::object properties, std::string typeString) {
            return self.PublishValue(name, std::move(getter), std::move(setter),
                                     py::none(), BuiltinType("str"), robust,
                                     isMutable, std::move(properties),
                                     std::move(typeString));
          },
          py::arg("name"), py::arg("getter"), py::arg("setter"), py::kw_only(),
          py::arg("robust") = false, py::arg("mutable") = true,
          py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def(
          "publish_raw",
          [](PyTunableTable& self, std::string_view name, py::object getter,
             py::object setter, bool robust, bool isMutable,
             py::object properties, std::string typeString) {
            return self.PublishValue(name, std::move(getter), std::move(setter),
                                     BuiltinType("bytes"), py::none(), robust,
                                     isMutable, std::move(properties),
                                     std::move(typeString));
          },
          py::arg("name"), py::arg("getter"), py::arg("setter"), py::kw_only(),
          py::arg("robust") = false, py::arg("mutable") = true,
          py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def("remove", &PyTunableTable::Remove);

  m.def(
      "get_table",
      [](std::string_view name) {
        return PyTunableTable{name.empty() ? wpi::tunables::GetTable()
                                           : wpi::tunables::GetTable(name),
                              nullptr};
      },
      py::arg("name") = "");
  m.def("publish", [](std::string_view name, py::object value) {
    return PyTunableTable{wpi::tunables::GetTable(), nullptr}.Publish(name,
                                                                      value);
  });
  m.def(
      "add",
      [](std::string_view name, py::object value, py::object valueType,
         py::object elementType, bool robust, bool isMutable, py::object onTune,
         py::object properties, std::string typeString) {
        return PyTunableTable{wpi::tunables::GetTable(), nullptr}.Add(
            name, std::move(value), std::move(valueType),
            std::move(elementType), robust, isMutable, std::move(onTune),
            std::move(properties), std::move(typeString));
      },
      py::arg("name"), py::arg("value"), py::kw_only(),
      py::arg("value_type") = py::none(), py::arg("element_type") = py::none(),
      py::arg("robust") = false, py::arg("mutable") = true,
      py::arg("on_tune") = py::none(), py::arg("properties") = py::none(),
      py::arg("type_string") = "");
  m.def("add_boolean", [](std::string_view name, py::object value) {
    return PyTunableTable{wpi::tunables::GetTable(), nullptr}.Add(
        name, std::move(value), BuiltinType("bool"), py::none(), false, true,
        py::none(), py::none(), "");
  });
  m.def("add_int", [](std::string_view name, py::object value) {
    return PyTunableTable{wpi::tunables::GetTable(), nullptr}.Add(
        name, std::move(value), BuiltinType("int"), py::none(), false, true,
        py::none(), py::none(), "");
  });
  m.def("add_long", [](std::string_view name, py::object value) {
    return PyTunableTable{wpi::tunables::GetTable(), nullptr}.Add(
        name, std::move(value), BuiltinType("int"), py::none(), false, true,
        py::none(), py::none(), "");
  });
  m.def("add_float", [](std::string_view name, py::object value) {
    return PyTunableTable{wpi::tunables::GetTable(), nullptr}.Add(
        name, std::move(value), BuiltinType("float"), py::none(), false, true,
        py::none(), py::none(), "");
  });
  m.def("add_double", [](std::string_view name, py::object value) {
    return PyTunableTable{wpi::tunables::GetTable(), nullptr}.Add(
        name, std::move(value), BuiltinType("float"), py::none(), false, true,
        py::none(), py::none(), "");
  });
  m.def("remove", &RemoveRootValue);

  py::class_<wpi::tunables::TunableRegistry>(m, "TunableRegistry")
      .def_static("set_report_warning",
                  [](py::object func) {
                    if (func.is_none()) {
                      wpi::tunables::TunableRegistry::SetReportWarning(nullptr);
                    } else {
                      auto callback = std::shared_ptr<py::object>{
                          new py::object{std::move(func)}, [](auto object) {
                            py::gil_scoped_acquire gil;
                            delete object;
                          }};
                      wpi::tunables::TunableRegistry::SetReportWarning(
                          [callback](std::string_view msg) {
                            py::gil_scoped_acquire gil;
                            (*callback)(std::string{msg});
                          });
                    }
                  })
      .def_static("report_warning",
                  &wpi::tunables::TunableRegistry::ReportWarning)
      .def_static("register_backend",
                  [](std::string_view prefix,
                     std::shared_ptr<wpi::tunables::TunableBackend> backend) {
                    std::string prefixString{prefix};
                    py::gil_scoped_release release;
                    wpi::tunables::TunableRegistry::RegisterBackend(
                        prefixString, std::move(backend));
                  })
      .def_static("get_backend", &wpi::tunables::TunableRegistry::GetBackend)
      .def_static(
          "get_table",
          [](std::string_view path) {
            return PyTunableTable{wpi::tunables::GetTable(path), nullptr};
          },
          py::arg("path"))
      .def_static(
          "normalize_name",
          [](std::string_view path) {
            std::string buf;
            return std::string{wpi::tunables::detail::NormalizeName(path, buf)};
          })
      .def_static(
          "remove",
          [](py::object value) {
            if (py::isinstance<py::str>(value)) {
              RemovePath(value.cast<std::string>());
            } else {
              RemoveValue(value);
            }
          },
          py::arg("value"))
      .def_static("update",
                  [] {
                    py::gil_scoped_release release;
                    wpi::tunables::TunableRegistry::Update();
                  })
      .def_static(
          "with_update_mutex",
          [](py::function func) {
            std::unique_lock lock{
                wpi::tunables::TunableRegistry::GetUpdateMutex(),
                std::defer_lock};
            {
              py::gil_scoped_release release;
              lock.lock();
            }
            func();
          },
          py::arg("func"),
          "Run a short function while holding the registry update "
          "mutex; every competing tunable access must participate.")
      .def_static("reset", [] {
        ClearValues();
        {
          py::gil_scoped_release release;
          wpi::tunables::TunableRegistry::Reset();
        }
        RegisterPreUpdateCallback();
      });

  py::class_<wpi::tunables::MockTunableBackend, py::smart_holder,
             wpi::tunables::TunableBackend>(m, "MockTunableBackend")
      .def(py::init<>())
      .def("set_bool", &wpi::tunables::MockTunableBackend::SetBool)
      .def("set_int32", &wpi::tunables::MockTunableBackend::SetInt32)
      .def("set_int64", &wpi::tunables::MockTunableBackend::SetInt64)
      .def("set_float", &wpi::tunables::MockTunableBackend::SetFloat)
      .def("set_double", &wpi::tunables::MockTunableBackend::SetDouble)
      .def("set_string", &wpi::tunables::MockTunableBackend::SetString)
      .def("set_raw", &SetRaw)
      .def("set_struct", &SetStruct)
      .def("set_struct_vector", &SetStructVector)
      .def("set_struct_array", &SetStructVector)
      .def("set_bool_vector", &SetBoolVector)
      .def("set_int32_vector",
           [](wpi::tunables::MockTunableBackend& self, std::string_view path,
              const py::sequence& value) {
             SetVector<int32_t>(
                 self, path, value,
                 &wpi::tunables::MockTunableBackend::SetInt32Vector);
           })
      .def("set_int64_vector",
           [](wpi::tunables::MockTunableBackend& self, std::string_view path,
              const py::sequence& value) {
             SetVector<int64_t>(
                 self, path, value,
                 &wpi::tunables::MockTunableBackend::SetInt64Vector);
           })
      .def("set_float_vector",
           [](wpi::tunables::MockTunableBackend& self, std::string_view path,
              const py::sequence& value) {
             SetVector<float>(
                 self, path, value,
                 &wpi::tunables::MockTunableBackend::SetFloatVector);
           })
      .def("set_double_vector",
           [](wpi::tunables::MockTunableBackend& self, std::string_view path,
              const py::sequence& value) {
             SetVector<double>(
                 self, path, value,
                 &wpi::tunables::MockTunableBackend::SetDoubleVector);
           })
      .def("set_string_vector",
           [](wpi::tunables::MockTunableBackend& self, std::string_view path,
              const py::sequence& value) {
             SetVector<std::string>(
                 self, path, value,
                 &wpi::tunables::MockTunableBackend::SetStringVector);
           })
      .def("get_uid", &GetUid)
      .def("get_value", &GetTunableValue);
}
