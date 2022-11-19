// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstring>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <wpi/MemAlloc.h>

#include "networktables/NetworkTableValue.h"
#include "ntcore_c.h"

namespace nt {

class Value;

template <typename T>
inline void ConvertToC(const T& in, T* out) {
  *out = in;
}

void ConvertToC(const Value& in, NT_Value* out);
Value ConvertFromC(const NT_Value& value);
size_t ConvertToC(std::string_view in, char** out);
void ConvertToC(std::string_view in, NT_String* out);
inline std::string_view ConvertFromC(const NT_String& str) {
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

template <typename T>
T GetNumericAs(const Value& value) {
  if (value.IsInteger()) {
    return static_cast<T>(value.GetInteger());
  } else if (value.IsFloat()) {
    return static_cast<T>(value.GetFloat());
  } else if (value.IsDouble()) {
    return static_cast<T>(value.GetDouble());
  } else {
    return {};
  }
}

template <typename T>
std::vector<T> GetNumericArrayAs(const Value& value) {
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

Value ConvertNumericValue(const Value& value, NT_Type type);

}  // namespace nt
