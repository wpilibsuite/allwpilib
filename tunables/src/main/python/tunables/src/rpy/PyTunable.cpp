#include "PyTunable.h"

#include <span>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <pybind11/stl.h>
#include <wpi/tunables/ComplexTunable.hpp>

#include "PyMutationList.h"
#include "TunableValuePython.h"

namespace py = pybind11;

namespace wpi::tunables::python {
namespace {

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

bool IsNoType(py::handle type) {
  return type.is_none();
}

bool IsSequenceValue(py::handle value) {
  return PySequence_Check(value.ptr()) && !py::isinstance<py::str>(value) &&
         !IsBytesLike(value);
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

}  // namespace

PyTunable::PyTunable(py::object value, py::object getter, py::object setter,
                     py::object onTune, bool robust, bool isMutable,
                     py::object valueType, py::object elementType,
                     py::object properties, std::string typeString,
                     bool alwaysGet)
    : m_getter{std::move(getter)},
      m_setter{std::move(setter)},
      m_onTune{std::move(onTune)},
      m_value{MakeValue(value, robust, isMutable, std::move(valueType),
                        std::move(elementType), std::move(properties),
                        std::move(typeString), alwaysGet)} {
  py::gil_scoped_acquire gil;
  m_lastStructData = PackCachedStructData();
}

wpi::tunables::detail::TunableBase& PyTunable::GetBase() {
  return std::visit(
      [](auto& value) -> wpi::tunables::detail::TunableBase& { return value; },
      m_value);
}

py::object PyTunable::Get() const {
  if (!m_getter.is_none()) {
    return m_getter();
  }
  return GetCached();
}

void PyTunable::Set(py::handle value) {
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

py::object PyTunable::Mutate() {
  if (!m_getter.is_none()) {
    py::object value = m_getter();
    SetCached(value);
    return value;
  }
  return MutateCached();
}

void PyTunable::Refresh() {
  if (!m_getter.is_none()) {
    py::gil_scoped_acquire gil;
    SetCachedIfChanged(m_getter());
  }
}

bool PyTunable::NeedsRefresh() const {
  return !m_getter.is_none();
}

template <typename T>
bool PyTunable::CachedValuesEqual(const T& lhs, const T& rhs) {
  return lhs == rhs;
}

std::vector<uint8_t> PyTunable::PackStructValue(const WPyStruct& value,
                                                const WPyStructInfo& info) {
  std::vector<uint8_t> data(wpi::util::GetStructSize<WPyStruct>(info));
  wpi::util::PackStruct(data, value, info);
  return data;
}

template <typename T>
std::optional<std::vector<uint8_t>> PyTunable::PackStructData(const T&) {
  return std::nullopt;
}

std::optional<std::vector<uint8_t>> PyTunable::PackStructData(
    const WPyStruct& value) {
  return PackStructValue(value, WPyStructInfo{value});
}

std::optional<std::vector<uint8_t>> PyTunable::PackStructData(
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
T PyTunable::ToCachedValue(py::handle value) {
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

py::object PyTunable::GetCached() const {
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

py::object PyTunable::MutateCached() {
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

void PyTunable::SetCached(py::handle value) {
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

void PyTunable::SetCachedIfChanged(py::handle value) {
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

std::optional<std::vector<uint8_t>> PyTunable::PackCachedStructData() const {
  return std::visit(
      [](const auto& tunable) { return PackStructData(tunable.Get()); },
      m_value);
}

wpi::tunables::TunableConfig PyTunable::MakeConfig(bool robust, bool isMutable,
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

TunableVariant PyTunable::MakeValue(py::handle value, bool robust,
                                    bool isMutable, py::object valueType,
                                    py::object elementType,
                                    py::object properties,
                                    std::string typeString, bool alwaysGet) {
  auto kind = InferValueKind(value, valueType, elementType);
  auto config = MakeConfig(robust, isMutable, properties, std::move(typeString),
                           alwaysGet);
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

}  // namespace wpi::tunables::python
