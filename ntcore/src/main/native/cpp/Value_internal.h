// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <concepts>
#include <cstring>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include <wpi/MemAlloc.h>

#include "networktables/NetworkTableValue.h"
#include "ntcore_c.h"
#include "ntcore_cpp_types.h"

namespace nt {

template <typename T>
struct TypeInfo {};

template <>
struct TypeInfo<bool> {
  static constexpr NT_Type kType = NT_BOOLEAN;

  using Value = bool;
  using View = bool;
};

template <>
struct TypeInfo<int64_t> {
  static constexpr NT_Type kType = NT_INTEGER;

  using Value = int64_t;
  using View = int64_t;
};

template <>
struct TypeInfo<float> {
  static constexpr NT_Type kType = NT_FLOAT;

  using Value = float;
  using View = float;
};

template <>
struct TypeInfo<double> {
  static constexpr NT_Type kType = NT_DOUBLE;

  using Value = double;
  using View = double;
};

template <>
struct TypeInfo<std::string> {
  static constexpr NT_Type kType = NT_STRING;

  using Value = std::string;
  using View = std::string_view;

  using SmallRet = std::string_view;
  using SmallElem = char;
};

template <>
struct TypeInfo<std::string_view> : public TypeInfo<std::string> {};

template <>
struct TypeInfo<uint8_t[]> {
  static constexpr NT_Type kType = NT_RAW;

  using Value = std::vector<uint8_t>;
  using View = std::span<const uint8_t>;

  using SmallRet = std::span<uint8_t>;
  using SmallElem = uint8_t;
};

template <>
struct TypeInfo<std::vector<uint8_t>> : public TypeInfo<uint8_t[]> {};
template <>
struct TypeInfo<std::span<const uint8_t>> : public TypeInfo<uint8_t[]> {};

template <>
struct TypeInfo<bool[]> {
  static constexpr NT_Type kType = NT_BOOLEAN_ARRAY;
  using ElementType = bool;

  using Value = std::vector<int>;
  using View = std::span<const int>;

  using SmallRet = std::span<int>;
  using SmallElem = int;
};

template <>
struct TypeInfo<int64_t[]> {
  static constexpr NT_Type kType = NT_INTEGER_ARRAY;
  using ElementType = int64_t;

  using Value = std::vector<int64_t>;
  using View = std::span<const int64_t>;

  using SmallRet = std::span<int64_t>;
  using SmallElem = int64_t;
};

template <>
struct TypeInfo<std::vector<int64_t>> : public TypeInfo<int64_t[]> {};
template <>
struct TypeInfo<std::span<const int64_t>> : public TypeInfo<int64_t[]> {};

template <>
struct TypeInfo<float[]> {
  static constexpr NT_Type kType = NT_FLOAT_ARRAY;
  using ElementType = float;

  using Value = std::vector<float>;
  using View = std::span<const float>;

  using SmallRet = std::span<float>;
  using SmallElem = float;
};

template <>
struct TypeInfo<std::vector<float>> : public TypeInfo<float[]> {};
template <>
struct TypeInfo<std::span<const float>> : public TypeInfo<float[]> {};

template <>
struct TypeInfo<double[]> {
  static constexpr NT_Type kType = NT_DOUBLE_ARRAY;
  using ElementType = double;

  using Value = std::vector<double>;
  using View = std::span<const double>;

  using SmallRet = std::span<double>;
  using SmallElem = double;
};

template <>
struct TypeInfo<std::vector<double>> : public TypeInfo<double[]> {};
template <>
struct TypeInfo<std::span<const double>> : public TypeInfo<double[]> {};

template <>
struct TypeInfo<std::string[]> {
  static constexpr NT_Type kType = NT_STRING_ARRAY;
  using ElementType = std::string;

  using Value = std::vector<std::string>;
  using View = std::span<const std::string>;
};

template <>
struct TypeInfo<std::vector<std::string>> : public TypeInfo<std::string[]> {};
template <>
struct TypeInfo<std::span<const std::string>> : public TypeInfo<std::string[]> {
};

template <typename T>
concept ValidType = requires {
  { TypeInfo<std::remove_cvref_t<T>>::kType } -> std::convertible_to<NT_Type>;
  typename TypeInfo<std::remove_cvref_t<T>>::Value;
  typename TypeInfo<std::remove_cvref_t<T>>::View;
};

static_assert(ValidType<bool>);
static_assert(!ValidType<uint8_t>);
static_assert(ValidType<uint8_t[]>);
static_assert(ValidType<std::vector<uint8_t>>);

template <ValidType T, NT_Type type>
constexpr bool IsNTType = TypeInfo<std::remove_cvref_t<T>>::kType == type;

static_assert(IsNTType<bool, NT_BOOLEAN>);
static_assert(!IsNTType<bool, NT_DOUBLE>);

template <typename T>
concept ArrayType =
    requires { typename TypeInfo<std::remove_cvref_t<T>>::ElementType; };

static_assert(ArrayType<std::string[]>);
static_assert(!ArrayType<uint8_t[]>);

template <typename T>
concept SmallArrayType = requires {
  typename TypeInfo<std::remove_cvref_t<T>>::SmallRet;
  typename TypeInfo<std::remove_cvref_t<T>>::SmallElem;
};

static_assert(SmallArrayType<float[]>);
static_assert(!SmallArrayType<std::string[]>);

template <typename T>
concept NumericType =
    IsNTType<T, NT_INTEGER> || IsNTType<T, NT_FLOAT> || IsNTType<T, NT_DOUBLE>;

static_assert(NumericType<int64_t>);
static_assert(NumericType<float>);
static_assert(NumericType<double>);
static_assert(!NumericType<bool>);
static_assert(!NumericType<std::string>);
static_assert(!NumericType<int64_t[]>);
static_assert(!NumericType<double[]>);
static_assert(!NumericType<bool[]>);
static_assert(!NumericType<uint8_t[]>);

template <typename T>
concept NumericArrayType =
    ArrayType<T> &&
    NumericType<typename TypeInfo<std::remove_cvref_t<T>>::ElementType>;

static_assert(NumericArrayType<int64_t[]>);
static_assert(NumericArrayType<float[]>);
static_assert(NumericArrayType<double[]>);
static_assert(!NumericArrayType<bool[]>);

template <NumericType T>
inline typename TypeInfo<T>::Value GetNumericAs(const Value& value) {
  if (value.IsInteger()) {
    return static_cast<typename TypeInfo<T>::Value>(value.GetInteger());
  } else if (value.IsFloat()) {
    return static_cast<typename TypeInfo<T>::Value>(value.GetFloat());
  } else if (value.IsDouble()) {
    return static_cast<typename TypeInfo<T>::Value>(value.GetDouble());
  } else {
    return {};
  }
}

template <NumericArrayType T>
typename TypeInfo<T>::Value GetNumericArrayAs(const Value& value) {
  if (value.IsIntegerArray()) {
    auto arr = value.GetIntegerArray();
    return {arr.begin(), arr.end()};
  } else if (value.IsFloatArray()) {
    auto arr = value.GetFloatArray();
    return {arr.begin(), arr.end()};
  } else if (value.IsDoubleArray()) {
    auto arr = value.GetDoubleArray();
    return {arr.begin(), arr.end()};
  } else {
    return {};
  }
}

template <ValidType T>
inline bool IsType(const Value& value) {
  return value.type() == TypeInfo<T>::kType;
}

template <ValidType T>
inline bool IsNumericConvertibleTo(const Value& value) {
  if constexpr (NumericType<T>) {
    return value.IsInteger() || value.IsFloat() || value.IsDouble();
  } else if constexpr (NumericArrayType<T>) {
    return value.IsIntegerArray() || value.IsFloatArray() ||
           value.IsDoubleArray();
  } else {
    return false;
  }
}

template <ValidType T>
inline typename TypeInfo<T>::View GetValueView(const Value& value) {
  if constexpr (IsNTType<T, NT_BOOLEAN>) {
    return value.GetBoolean();
  } else if constexpr (IsNTType<T, NT_INTEGER>) {
    return value.GetInteger();
  } else if constexpr (IsNTType<T, NT_FLOAT>) {
    return value.GetFloat();
  } else if constexpr (IsNTType<T, NT_DOUBLE>) {
    return value.GetDouble();
  } else if constexpr (IsNTType<T, NT_STRING>) {
    return value.GetString();
  } else if constexpr (IsNTType<T, NT_RAW>) {
    return value.GetRaw();
  } else if constexpr (IsNTType<T, NT_BOOLEAN_ARRAY>) {
    return value.GetBooleanArray();
  } else if constexpr (IsNTType<T, NT_INTEGER_ARRAY>) {
    return value.GetIntegerArray();
  } else if constexpr (IsNTType<T, NT_FLOAT_ARRAY>) {
    return value.GetFloatArray();
  } else if constexpr (IsNTType<T, NT_DOUBLE_ARRAY>) {
    return value.GetDoubleArray();
  } else if constexpr (IsNTType<T, NT_STRING_ARRAY>) {
    return value.GetStringArray();
  }
}

template <ValidType T>
inline Value MakeValue(typename TypeInfo<T>::View value, int64_t time) {
  if constexpr (IsNTType<T, NT_BOOLEAN>) {
    return Value::MakeBoolean(value, time);
  } else if constexpr (IsNTType<T, NT_INTEGER>) {
    return Value::MakeInteger(value, time);
  } else if constexpr (IsNTType<T, NT_FLOAT>) {
    return Value::MakeFloat(value, time);
  } else if constexpr (IsNTType<T, NT_DOUBLE>) {
    return Value::MakeDouble(value, time);
  } else if constexpr (IsNTType<T, NT_STRING>) {
    return Value::MakeString(value, time);
  } else if constexpr (IsNTType<T, NT_RAW>) {
    return Value::MakeRaw(value, time);
  } else if constexpr (IsNTType<T, NT_BOOLEAN_ARRAY>) {
    return Value::MakeBooleanArray(value, time);
  } else if constexpr (IsNTType<T, NT_INTEGER_ARRAY>) {
    return Value::MakeIntegerArray(value, time);
  } else if constexpr (IsNTType<T, NT_FLOAT_ARRAY>) {
    return Value::MakeFloatArray(value, time);
  } else if constexpr (IsNTType<T, NT_DOUBLE_ARRAY>) {
    return Value::MakeDoubleArray(value, time);
  } else if constexpr (IsNTType<T, NT_STRING_ARRAY>) {
    return Value::MakeStringArray(value, time);
  }
}

template <ValidType T>
  requires ArrayType<T> || IsNTType<T, NT_STRING> || IsNTType<T, NT_RAW>
inline Value MakeValue(typename TypeInfo<T>::Value&& value, int64_t time) {
  if constexpr (IsNTType<T, NT_STRING>) {
    return Value::MakeString(value, time);
  } else if constexpr (IsNTType<T, NT_RAW>) {
    return Value::MakeRaw(value, time);
  } else if constexpr (IsNTType<T, NT_BOOLEAN_ARRAY>) {
    return Value::MakeBooleanArray(value, time);
  } else if constexpr (IsNTType<T, NT_INTEGER_ARRAY>) {
    return Value::MakeIntegerArray(value, time);
  } else if constexpr (IsNTType<T, NT_FLOAT_ARRAY>) {
    return Value::MakeFloatArray(value, time);
  } else if constexpr (IsNTType<T, NT_DOUBLE_ARRAY>) {
    return Value::MakeDoubleArray(value, time);
  } else if constexpr (IsNTType<T, NT_STRING_ARRAY>) {
    return Value::MakeStringArray(value, time);
  }
}

template <ValidType T>
inline typename TypeInfo<T>::Value CopyValue(typename TypeInfo<T>::View value) {
  if constexpr (ArrayType<T> || IsNTType<T, NT_RAW>) {
    return {value.begin(), value.end()};
  } else if constexpr (IsNTType<T, NT_STRING>) {
    return std::string{value};
  } else {
    return value;
  }
}

template <SmallArrayType T>
inline typename TypeInfo<T>::SmallRet CopyValue(
    typename TypeInfo<T>::View arr,
    wpi::SmallVectorImpl<typename TypeInfo<T>::SmallElem>& buf) {
  buf.assign(arr.begin(), arr.end());
  return {buf.data(), buf.size()};
}

template <ValidType T, bool ConvertNumeric>
inline typename TypeInfo<T>::Value GetValueCopy(const Value& value) {
  if constexpr (ConvertNumeric && NumericType<T>) {
    return GetNumericAs<T>(value);
  } else if constexpr (ConvertNumeric && NumericArrayType<T>) {
    return GetNumericArrayAs<T>(value);
  } else {
    return CopyValue<T>(GetValueView<T>(value));
  }
}

template <SmallArrayType T, bool ConvertNumeric>
inline typename TypeInfo<T>::SmallRet GetValueCopy(
    const Value& value,
    wpi::SmallVectorImpl<typename TypeInfo<T>::SmallElem>& buf) {
  if constexpr (ConvertNumeric && NumericArrayType<T>) {
    if (value.IsIntegerArray()) {
      auto arr = value.GetIntegerArray();
      buf.assign(arr.begin(), arr.end());
      return {buf.data(), buf.size()};
    } else if (value.IsFloatArray()) {
      auto arr = value.GetFloatArray();
      buf.assign(arr.begin(), arr.end());
      return {buf.data(), buf.size()};
    } else if (value.IsDoubleArray()) {
      auto arr = value.GetDoubleArray();
      buf.assign(arr.begin(), arr.end());
      return {buf.data(), buf.size()};
    } else {
      return {};
    }
  } else {
    return CopyValue<T>(GetValueView<T>(value), buf);
  }
}

template <ValidType T, bool ConvertNumeric>
inline Timestamped<typename TypeInfo<T>::Value> GetTimestamped(
    const Value& value) {
  return {value.time(), value.server_time(),
          GetValueCopy<T, ConvertNumeric>(value)};
}

template <SmallArrayType T, bool ConvertNumeric>
inline Timestamped<typename TypeInfo<T>::SmallRet> GetTimestamped(
    const Value& value,
    wpi::SmallVectorImpl<typename TypeInfo<T>::SmallElem>& buf) {
  return {value.time(), value.server_time(),
          GetValueCopy<T, ConvertNumeric>(value, buf)};
}

template <typename T>
inline void ConvertToC(const T& in, T* out) {
  *out = in;
}

void ConvertToC(const Value& in, NT_Value* out);
Value ConvertFromC(const NT_Value& value);
size_t ConvertToC(std::string_view in, char** out);
void ConvertToC(std::string_view in, WPI_String* out);
inline std::string_view ConvertFromC(const WPI_String& str) {
  return {str.str, str.len};
}

template <typename O, typename I>
O* ConvertToC(const std::vector<I>& in, size_t* out_len) {
  if (!out_len) {
    return nullptr;
  }
  *out_len = in.size();
  if (in.empty()) {
    return nullptr;
  }
  O* out = static_cast<O*>(wpi::safe_malloc(sizeof(O) * in.size()));
  for (size_t i = 0; i < in.size(); ++i) {
    ConvertToC(in[i], &out[i]);
  }
  return out;
}

template <typename O, typename I>
O* ConvertToC(const std::basic_string<I>& in, size_t* out_len) {
  char* out = static_cast<char*>(wpi::safe_malloc(in.size() + 1));
  std::memmove(out, in.data(), in.size());  // NOLINT
  out[in.size()] = '\0';
  *out_len = in.size();
  return out;
}

Value ConvertNumericValue(const Value& value, NT_Type type);

}  // namespace nt
