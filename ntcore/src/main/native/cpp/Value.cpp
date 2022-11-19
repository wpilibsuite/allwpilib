// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <stdint.h>

#include <cstring>
#include <span>

#include <wpi/MemAlloc.h>
#include <wpi/timestamp.h>

#include "Value_internal.h"
#include "networktables/NetworkTableValue.h"
#include "ntcore_cpp.h"

using namespace nt;

namespace {
struct StringArrayStorage {
  explicit StringArrayStorage(std::span<const std::string> value)
      : strings{value.begin(), value.end()} {
    InitNtStrings();
  }
  explicit StringArrayStorage(std::vector<std::string>&& value)
      : strings{std::move(value)} {
    InitNtStrings();
  }
  void InitNtStrings();

  std::vector<std::string> strings;
  std::vector<NT_String> ntStrings;
};
}  // namespace

void StringArrayStorage::InitNtStrings() {
  // point NT_String's to the contents in the vector.
  ntStrings.reserve(strings.size());
  for (const auto& str : strings) {
    ntStrings.emplace_back(
        NT_String{const_cast<char*>(str.c_str()), str.size()});
  }
}

Value::Value() {
  m_val.type = NT_UNASSIGNED;
  m_val.last_change = 0;
  m_val.server_time = 0;
}

Value::Value(NT_Type type, int64_t time, const private_init&)
    : Value{type, time == 0 ? nt::Now() : time, 1, private_init{}} {}

Value::Value(NT_Type type, int64_t time, int64_t serverTime,
             const private_init&) {
  m_val.type = type;
  m_val.last_change = time;
  m_val.server_time = serverTime;
  if (m_val.type == NT_BOOLEAN_ARRAY) {
    m_val.data.arr_boolean.arr = nullptr;
  } else if (m_val.type == NT_INTEGER_ARRAY) {
    m_val.data.arr_int.arr = nullptr;
  } else if (m_val.type == NT_FLOAT_ARRAY) {
    m_val.data.arr_float.arr = nullptr;
  } else if (m_val.type == NT_DOUBLE_ARRAY) {
    m_val.data.arr_double.arr = nullptr;
  } else if (m_val.type == NT_STRING_ARRAY) {
    m_val.data.arr_string.arr = nullptr;
  }
}

Value Value::MakeBooleanArray(std::span<const bool> value, int64_t time) {
  Value val{NT_BOOLEAN_ARRAY, time, private_init{}};
  auto data = std::make_shared<std::vector<int>>(value.begin(), value.end());
  // data->reserve(value.size());
  // std::copy(value.begin(), value.end(), *data);
  val.m_val.data.arr_boolean.arr = data->data();
  val.m_val.data.arr_boolean.size = data->size();
  val.m_storage = std::move(data);
  return val;
}

Value Value::MakeBooleanArray(std::span<const int> value, int64_t time) {
  Value val{NT_BOOLEAN_ARRAY, time, private_init{}};
  auto data = std::make_shared<std::vector<int>>(value.begin(), value.end());
  val.m_val.data.arr_boolean.arr = data->data();
  val.m_val.data.arr_boolean.size = data->size();
  val.m_storage = std::move(data);
  return val;
}

Value Value::MakeBooleanArray(std::vector<int>&& value, int64_t time) {
  Value val{NT_BOOLEAN_ARRAY, time, private_init{}};
  auto data = std::make_shared<std::vector<int>>(std::move(value));
  val.m_val.data.arr_boolean.arr = data->data();
  val.m_val.data.arr_boolean.size = data->size();
  val.m_storage = std::move(data);
  return val;
}

Value Value::MakeIntegerArray(std::span<const int64_t> value, int64_t time) {
  Value val{NT_INTEGER_ARRAY, time, private_init{}};
  auto data =
      std::make_shared<std::vector<int64_t>>(value.begin(), value.end());
  val.m_val.data.arr_int.arr = data->data();
  val.m_val.data.arr_int.size = data->size();
  val.m_storage = std::move(data);
  return val;
}

Value Value::MakeIntegerArray(std::vector<int64_t>&& value, int64_t time) {
  Value val{NT_INTEGER_ARRAY, time, private_init{}};
  auto data = std::make_shared<std::vector<int64_t>>(std::move(value));
  val.m_val.data.arr_int.arr = data->data();
  val.m_val.data.arr_int.size = data->size();
  val.m_storage = std::move(data);
  return val;
}

Value Value::MakeFloatArray(std::span<const float> value, int64_t time) {
  Value val{NT_FLOAT_ARRAY, time, private_init{}};
  auto data = std::make_shared<std::vector<float>>(value.begin(), value.end());
  val.m_val.data.arr_float.arr = data->data();
  val.m_val.data.arr_float.size = data->size();
  val.m_storage = std::move(data);
  return val;
}

Value Value::MakeFloatArray(std::vector<float>&& value, int64_t time) {
  Value val{NT_FLOAT_ARRAY, time, private_init{}};
  auto data = std::make_shared<std::vector<float>>(std::move(value));
  val.m_val.data.arr_float.arr = data->data();
  val.m_val.data.arr_float.size = data->size();
  val.m_storage = std::move(data);
  return val;
}

Value Value::MakeDoubleArray(std::span<const double> value, int64_t time) {
  Value val{NT_DOUBLE_ARRAY, time, private_init{}};
  auto data = std::make_shared<std::vector<double>>(value.begin(), value.end());
  val.m_val.data.arr_double.arr = data->data();
  val.m_val.data.arr_double.size = data->size();
  val.m_storage = std::move(data);
  return val;
}

Value Value::MakeDoubleArray(std::vector<double>&& value, int64_t time) {
  Value val{NT_DOUBLE_ARRAY, time, private_init{}};
  auto data = std::make_shared<std::vector<double>>(std::move(value));
  val.m_val.data.arr_double.arr = data->data();
  val.m_val.data.arr_double.size = data->size();
  val.m_storage = std::move(data);
  return val;
}

Value Value::MakeStringArray(std::span<const std::string> value, int64_t time) {
  Value val{NT_STRING_ARRAY, time, private_init{}};
  auto data = std::make_shared<StringArrayStorage>(value);
  val.m_val.data.arr_string.arr = data->ntStrings.data();
  val.m_val.data.arr_string.size = data->ntStrings.size();
  val.m_storage = std::move(data);
  return val;
}

Value Value::MakeStringArray(std::vector<std::string>&& value, int64_t time) {
  Value val{NT_STRING_ARRAY, time, private_init{}};
  auto data = std::make_shared<StringArrayStorage>(std::move(value));
  val.m_val.data.arr_string.arr = data->ntStrings.data();
  val.m_val.data.arr_string.size = data->ntStrings.size();
  val.m_storage = std::move(data);
  return val;
}

void nt::ConvertToC(const Value& in, NT_Value* out) {
  *out = in.value();
  switch (in.type()) {
    case NT_STRING:
      ConvertToC(in.GetString(), &out->data.v_string);
      break;
    case NT_RAW: {
      auto v = in.GetRaw();
      out->data.v_raw.data = static_cast<uint8_t*>(wpi::safe_malloc(v.size()));
      out->data.v_raw.size = v.size();
      std::memcpy(out->data.v_raw.data, v.data(), v.size());
      break;
    }
    case NT_BOOLEAN_ARRAY: {
      auto v = in.GetBooleanArray();
      out->data.arr_boolean.arr =
          static_cast<int*>(wpi::safe_malloc(v.size() * sizeof(int)));
      out->data.arr_boolean.size = v.size();
      std::copy(v.begin(), v.end(), out->data.arr_boolean.arr);
      break;
    }
    case NT_INTEGER_ARRAY: {
      auto v = in.GetIntegerArray();
      out->data.arr_int.arr =
          static_cast<int64_t*>(wpi::safe_malloc(v.size() * sizeof(int64_t)));
      out->data.arr_int.size = v.size();
      std::copy(v.begin(), v.end(), out->data.arr_int.arr);
      break;
    }
    case NT_FLOAT_ARRAY: {
      auto v = in.GetFloatArray();
      out->data.arr_float.arr =
          static_cast<float*>(wpi::safe_malloc(v.size() * sizeof(float)));
      out->data.arr_float.size = v.size();
      std::copy(v.begin(), v.end(), out->data.arr_float.arr);
      break;
    }
    case NT_DOUBLE_ARRAY: {
      auto v = in.GetDoubleArray();
      out->data.arr_double.arr =
          static_cast<double*>(wpi::safe_malloc(v.size() * sizeof(double)));
      out->data.arr_double.size = v.size();
      std::copy(v.begin(), v.end(), out->data.arr_double.arr);
      break;
    }
    case NT_STRING_ARRAY: {
      auto v = in.GetStringArray();
      out->data.arr_string.arr = static_cast<NT_String*>(
          wpi::safe_malloc(v.size() * sizeof(NT_String)));
      for (size_t i = 0; i < v.size(); ++i) {
        ConvertToC(std::string_view{v[i]}, &out->data.arr_string.arr[i]);
      }
      out->data.arr_string.size = v.size();
      break;
    }
    default:
      break;
  }
}

size_t nt::ConvertToC(std::string_view in, char** out) {
  *out = static_cast<char*>(wpi::safe_malloc(in.size() + 1));
  std::memmove(*out, in.data(), in.size());  // NOLINT
  (*out)[in.size()] = '\0';
  return in.size();
}

void nt::ConvertToC(std::string_view in, NT_String* out) {
  out->len = in.size();
  out->str = static_cast<char*>(wpi::safe_malloc(in.size() + 1));
  std::memcpy(out->str, in.data(), in.size());
  out->str[in.size()] = '\0';
}

Value nt::ConvertFromC(const NT_Value& value) {
  switch (value.type) {
    case NT_BOOLEAN:
      return Value::MakeBoolean(value.data.v_boolean != 0, value.last_change);
    case NT_INTEGER:
      return Value::MakeInteger(value.data.v_int, value.last_change);
    case NT_FLOAT:
      return Value::MakeFloat(value.data.v_float, value.last_change);
    case NT_DOUBLE:
      return Value::MakeDouble(value.data.v_double, value.last_change);
    case NT_STRING:
      return Value::MakeString(ConvertFromC(value.data.v_string),
                               value.last_change);
    case NT_RAW:
      return Value::MakeRaw({value.data.v_raw.data, value.data.v_raw.size},
                            value.last_change);
    case NT_BOOLEAN_ARRAY:
      return Value::MakeBooleanArray(
          std::span(value.data.arr_boolean.arr, value.data.arr_boolean.size),
          value.last_change);
    case NT_INTEGER_ARRAY:
      return Value::MakeIntegerArray(
          std::span(value.data.arr_int.arr, value.data.arr_int.size),
          value.last_change);
    case NT_FLOAT_ARRAY:
      return Value::MakeFloatArray(
          std::span(value.data.arr_float.arr, value.data.arr_float.size),
          value.last_change);
    case NT_DOUBLE_ARRAY:
      return Value::MakeDoubleArray(
          std::span(value.data.arr_double.arr, value.data.arr_double.size),
          value.last_change);
    case NT_STRING_ARRAY: {
      std::vector<std::string> v;
      v.reserve(value.data.arr_string.size);
      for (size_t i = 0; i < value.data.arr_string.size; ++i) {
        v.emplace_back(ConvertFromC(value.data.arr_string.arr[i]));
      }
      return Value::MakeStringArray(std::move(v), value.last_change);
    }
    default:
      return {};
  }
}

bool nt::operator==(const Value& lhs, const Value& rhs) {
  if (lhs.type() != rhs.type()) {
    return false;
  }
  switch (lhs.type()) {
    case NT_UNASSIGNED:
      return true;  // XXX: is this better being false instead?
    case NT_BOOLEAN:
      return lhs.m_val.data.v_boolean == rhs.m_val.data.v_boolean;
    case NT_INTEGER:
      return lhs.m_val.data.v_int == rhs.m_val.data.v_int;
    case NT_FLOAT:
      return lhs.m_val.data.v_float == rhs.m_val.data.v_float;
    case NT_DOUBLE:
      return lhs.m_val.data.v_double == rhs.m_val.data.v_double;
    case NT_STRING:
      return lhs.GetString() == rhs.GetString();
    case NT_RAW:
      if (lhs.m_val.data.v_raw.size != rhs.m_val.data.v_raw.size) {
        return false;
      }
      return std::memcmp(lhs.m_val.data.v_raw.data, rhs.m_val.data.v_raw.data,
                         lhs.m_val.data.v_raw.size) == 0;
    case NT_BOOLEAN_ARRAY:
      if (lhs.m_val.data.arr_boolean.size != rhs.m_val.data.arr_boolean.size) {
        return false;
      }
      return std::memcmp(lhs.m_val.data.arr_boolean.arr,
                         rhs.m_val.data.arr_boolean.arr,
                         lhs.m_val.data.arr_boolean.size *
                             sizeof(lhs.m_val.data.arr_boolean.arr[0])) == 0;
    case NT_INTEGER_ARRAY:
      if (lhs.m_val.data.arr_int.size != rhs.m_val.data.arr_int.size) {
        return false;
      }
      return std::memcmp(lhs.m_val.data.arr_int.arr, rhs.m_val.data.arr_int.arr,
                         lhs.m_val.data.arr_int.size *
                             sizeof(lhs.m_val.data.arr_int.arr[0])) == 0;
    case NT_FLOAT_ARRAY:
      if (lhs.m_val.data.arr_float.size != rhs.m_val.data.arr_float.size) {
        return false;
      }
      return std::memcmp(lhs.m_val.data.arr_float.arr,
                         rhs.m_val.data.arr_float.arr,
                         lhs.m_val.data.arr_float.size *
                             sizeof(lhs.m_val.data.arr_float.arr[0])) == 0;
    case NT_DOUBLE_ARRAY:
      if (lhs.m_val.data.arr_double.size != rhs.m_val.data.arr_double.size) {
        return false;
      }
      return std::memcmp(lhs.m_val.data.arr_double.arr,
                         rhs.m_val.data.arr_double.arr,
                         lhs.m_val.data.arr_double.size *
                             sizeof(lhs.m_val.data.arr_double.arr[0])) == 0;
    case NT_STRING_ARRAY:
      return static_cast<StringArrayStorage*>(lhs.m_storage.get())->strings ==
             static_cast<StringArrayStorage*>(rhs.m_storage.get())->strings;
    default:
      // assert(false && "unknown value type");
      return false;
  }
}
