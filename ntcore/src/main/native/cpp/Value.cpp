// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <stdint.h>

#include <algorithm>
#include <cstring>
#include <memory>
#include <numeric>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include <wpi/MemAlloc.h>
#include <wpi/timestamp.h>

#include "Value_internal.h"
#include "networktables/NetworkTableValue.h"

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
  size_t EstimateSize() const {
    return sizeof(StringArrayStorage) +
           strings.capacity() * sizeof(std::string) +
           ntStrings.capacity() * sizeof(WPI_String) +
           std::accumulate(strings.begin(), strings.end(), 0,
                           [](const auto& sum, const auto& val) {
                             return sum + val.capacity();
                           });
  }

  std::vector<std::string> strings;
  std::vector<WPI_String> ntStrings;
};

template <typename T>
inline std::shared_ptr<T[]> AllocateArray(size_t nelem) {
#if __cpp_lib_shared_ptr_arrays >= 201707L
#if __cpp_lib_smart_ptr_for_overwrite >= 202002L
  return std::make_shared_for_overwrite<T[]>(nelem);
#else
  return std::make_shared<T[]>(nelem);
#endif
#else
  return std::shared_ptr<T[]>{new T[nelem]};
#endif
}
}  // namespace

void StringArrayStorage::InitNtStrings() {
  // point WPI_String's to the contents in the vector.
  ntStrings.reserve(strings.size());
  for (const auto& str : strings) {
    ntStrings.emplace_back(
        WPI_String{const_cast<char*>(str.c_str()), str.size()});
  }
}

Value Value::MakeBooleanArray(std::span<const bool> value, int64_t time) {
  Value val{NT_BOOLEAN_ARRAY, value.size() * sizeof(int), time, private_init{}};
  auto data = AllocateArray<int>(value.size());
  std::copy(value.begin(), value.end(), data.get());
  val.m_val.data.arr_boolean.arr = data.get();
  val.m_val.data.arr_boolean.size = value.size();
  val.m_storage = std::move(data);
  return val;
}

Value Value::MakeBooleanArray(std::span<const int> value, int64_t time) {
  Value val{NT_BOOLEAN_ARRAY, value.size() * sizeof(int), time, private_init{}};
  auto data = AllocateArray<int>(value.size());
  std::copy(value.begin(), value.end(), data.get());
  val.m_val.data.arr_boolean.arr = data.get();
  val.m_val.data.arr_boolean.size = value.size();
  val.m_storage = std::move(data);
  return val;
}

Value Value::MakeBooleanArray(std::vector<int>&& value, int64_t time) {
  Value val{NT_BOOLEAN_ARRAY, value.size() * sizeof(int), time, private_init{}};
  auto data = std::make_shared<std::vector<int>>(std::move(value));
  val.m_val.data.arr_boolean.arr = data->data();
  val.m_val.data.arr_boolean.size = data->size();
  val.m_storage = std::move(data);
  return val;
}

Value Value::MakeIntegerArray(std::span<const int64_t> value, int64_t time) {
  Value val{NT_INTEGER_ARRAY, value.size() * sizeof(int64_t), time,
            private_init{}};
  auto data = AllocateArray<int64_t>(value.size());
  std::copy(value.begin(), value.end(), data.get());
  val.m_val.data.arr_int.arr = data.get();
  val.m_val.data.arr_int.size = value.size();
  val.m_storage = std::move(data);
  return val;
}

Value Value::MakeIntegerArray(std::vector<int64_t>&& value, int64_t time) {
  Value val{NT_INTEGER_ARRAY, value.size() * sizeof(int64_t), time,
            private_init{}};
  auto data = std::make_shared<std::vector<int64_t>>(std::move(value));
  val.m_val.data.arr_int.arr = data->data();
  val.m_val.data.arr_int.size = data->size();
  val.m_storage = std::move(data);
  return val;
}

Value Value::MakeFloatArray(std::span<const float> value, int64_t time) {
  Value val{NT_FLOAT_ARRAY, value.size() * sizeof(float), time, private_init{}};
  auto data = AllocateArray<float>(value.size());
  std::copy(value.begin(), value.end(), data.get());
  val.m_val.data.arr_float.arr = data.get();
  val.m_val.data.arr_float.size = value.size();
  val.m_storage = std::move(data);
  return val;
}

Value Value::MakeFloatArray(std::vector<float>&& value, int64_t time) {
  Value val{NT_FLOAT_ARRAY, value.size() * sizeof(float), time, private_init{}};
  auto data = std::make_shared<std::vector<float>>(std::move(value));
  val.m_val.data.arr_float.arr = data->data();
  val.m_val.data.arr_float.size = data->size();
  val.m_storage = std::move(data);
  return val;
}

Value Value::MakeDoubleArray(std::span<const double> value, int64_t time) {
  Value val{NT_DOUBLE_ARRAY, value.size() * sizeof(double), time,
            private_init{}};
  auto data = AllocateArray<double>(value.size());
  std::copy(value.begin(), value.end(), data.get());
  val.m_val.data.arr_double.arr = data.get();
  val.m_val.data.arr_double.size = value.size();
  val.m_storage = std::move(data);
  return val;
}

Value Value::MakeDoubleArray(std::vector<double>&& value, int64_t time) {
  Value val{NT_DOUBLE_ARRAY, value.size() * sizeof(double), time,
            private_init{}};
  auto data = std::make_shared<std::vector<double>>(std::move(value));
  val.m_val.data.arr_double.arr = data->data();
  val.m_val.data.arr_double.size = data->size();
  val.m_storage = std::move(data);
  return val;
}

Value Value::MakeStringArray(std::span<const std::string> value, int64_t time) {
  auto data = std::make_shared<StringArrayStorage>(value);
  Value val{NT_STRING_ARRAY, data->EstimateSize(), time, private_init{}};
  val.m_val.data.arr_string.arr = data->ntStrings.data();
  val.m_val.data.arr_string.size = data->ntStrings.size();
  val.m_storage = std::move(data);
  return val;
}

Value Value::MakeStringArray(std::vector<std::string>&& value, int64_t time) {
  auto data = std::make_shared<StringArrayStorage>(std::move(value));
  Value val{NT_STRING_ARRAY, data->EstimateSize(), time, private_init{}};
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
      out->data.arr_string.arr = WPI_AllocateStringArray(v.size());
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

void nt::ConvertToC(std::string_view in, WPI_String* out) {
  if (in.empty()) {
    out->len = 0;
    out->str = nullptr;
    return;
  }
  auto write = WPI_AllocateString(out, in.size());
  std::memcpy(write, in.data(), in.size());
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
      if (lhs.m_val.data.arr_boolean.size == 0) {
        return true;
      }
      return std::memcmp(lhs.m_val.data.arr_boolean.arr,
                         rhs.m_val.data.arr_boolean.arr,
                         lhs.m_val.data.arr_boolean.size *
                             sizeof(lhs.m_val.data.arr_boolean.arr[0])) == 0;
    case NT_INTEGER_ARRAY:
      if (lhs.m_val.data.arr_int.size != rhs.m_val.data.arr_int.size) {
        return false;
      }
      if (lhs.m_val.data.arr_int.size == 0) {
        return true;
      }
      return std::memcmp(lhs.m_val.data.arr_int.arr, rhs.m_val.data.arr_int.arr,
                         lhs.m_val.data.arr_int.size *
                             sizeof(lhs.m_val.data.arr_int.arr[0])) == 0;
    case NT_FLOAT_ARRAY:
      if (lhs.m_val.data.arr_float.size != rhs.m_val.data.arr_float.size) {
        return false;
      }
      if (lhs.m_val.data.arr_float.size == 0) {
        return true;
      }
      return std::memcmp(lhs.m_val.data.arr_float.arr,
                         rhs.m_val.data.arr_float.arr,
                         lhs.m_val.data.arr_float.size *
                             sizeof(lhs.m_val.data.arr_float.arr[0])) == 0;
    case NT_DOUBLE_ARRAY:
      if (lhs.m_val.data.arr_double.size != rhs.m_val.data.arr_double.size) {
        return false;
      }
      if (lhs.m_val.data.arr_double.size == 0) {
        return true;
      }
      return std::memcmp(lhs.m_val.data.arr_double.arr,
                         rhs.m_val.data.arr_double.arr,
                         lhs.m_val.data.arr_double.size *
                             sizeof(lhs.m_val.data.arr_double.arr[0])) == 0;
    case NT_STRING_ARRAY:
      if (lhs.m_val.data.arr_string.size != rhs.m_val.data.arr_string.size) {
        return false;
      }
      if (lhs.m_val.data.arr_string.size == 0) {
        return true;
      }
      return static_cast<StringArrayStorage*>(lhs.m_storage.get())->strings ==
             static_cast<StringArrayStorage*>(rhs.m_storage.get())->strings;
    default:
      // assert(false && "unknown value type");
      return false;
  }
}
