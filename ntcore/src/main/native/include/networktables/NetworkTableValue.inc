// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "NetworkTableValue.h"

#include <algorithm>
#include <memory>
#include <utility>

namespace nt {

inline Value Value::MakeString(std::string_view value, int64_t time) {
  auto data = AllocateArray<char>(value.size());
  std::memcpy(data.get(), value.data(), value.size());

  Value val{NT_STRING, time, private_init{}};
  val.m_val.data.v_string.str = data.get();
  val.m_val.data.v_string.len = value.size();
  val.m_storage = std::move(data);
  return val;
}

template <typename Alloc>
inline Value Value::MakeStringAlloc(const Alloc& alloc, std::string_view value,
                                    int64_t time) {
  auto data = AllocateArray<char>(alloc, value.size());
  std::memcpy(data.get(), value.data(), value.size());

  Value val{NT_STRING, time, private_init{}};
  val.m_val.data.v_string.str = data.get();
  val.m_val.data.v_string.len = value.size();
  val.m_storage = std::move(data);
  return val;
}

template <typename T, typename>
inline Value Value::MakeString(T&& value, int64_t time) {
  auto data = std::make_shared<T>(std::forward<T&&>(value));
  Value val{NT_STRING, time, private_init{}};
  val.m_val.data.v_string.str = const_cast<char*>(data->data());
  val.m_val.data.v_string.len = data->size();
  val.m_storage = std::move(data);
  return val;
}

template <typename Alloc, typename T, typename>
inline Value Value::MakeStringAlloc(const Alloc& alloc, T&& value,
                                    int64_t time) {
  auto data = std::allocate_shared<T>(alloc, std::forward<T&&>(value));
  Value val{NT_STRING, time, private_init{}};
  val.m_val.data.v_string.str = const_cast<char*>(data->data());
  val.m_val.data.v_string.len = data->size();
  val.m_storage = std::move(data);
  return val;
}

inline Value Value::MakeRaw(std::span<const uint8_t> value, int64_t time) {
  auto data = AllocateArray<uint8_t>(value.size());
  std::memcpy(data.get(), value.data(), value.size());

  Value val{NT_RAW, time, private_init{}};
  val.m_val.data.v_raw.data = data.get();
  val.m_val.data.v_raw.size = value.size();
  val.m_storage = std::move(data);
  return val;
}

template <typename Alloc>
inline Value Value::MakeRawAlloc(const Alloc& alloc,
                                 std::span<const uint8_t> value, int64_t time) {
  auto data = AllocateArray<uint8_t>(alloc, value.size());
  std::memcpy(data.get(), value.data(), value.size());

  Value val{NT_RAW, time, private_init{}};
  val.m_val.data.v_raw.data = data.get();
  val.m_val.data.v_raw.size = value.size();
  val.m_storage = std::move(data);
  return val;
}

template <typename T, typename>
inline Value Value::MakeRaw(T&& value, int64_t time) {
  Value val{NT_RAW, time, private_init{}};
  auto data = std::make_shared<T>(std::forward<T&&>(value));
  val.m_val.data.v_raw.data = const_cast<uint8_t*>(data->data());
  val.m_val.data.v_raw.size = data->size();
  val.m_storage = std::move(data);
  return val;
}

template <typename Alloc, typename T, typename>
inline Value Value::MakeRawAlloc(const Alloc& alloc, T&& value, int64_t time) {
  Value val{NT_RAW, time, private_init{}};
  auto data = std::allocate_shared<T>(alloc, std::forward<T&&>(value));
  val.m_val.data.v_raw.data = const_cast<uint8_t*>(data->data());
  val.m_val.data.v_raw.size = data->size();
  val.m_storage = std::move(data);
  return val;
}

inline Value Value::MakeBooleanArray(std::span<const bool> value,
                                     int64_t time) {
  auto data = AllocateArray<int>(value.size());
  std::copy(value.begin(), value.end(), data.get());

  Value val{NT_BOOLEAN_ARRAY, time, private_init{}};
  val.m_val.data.arr_boolean.arr = data.get();
  val.m_val.data.arr_boolean.size = value.size();
  val.m_storage = std::move(data);
  return val;
}

template <typename Alloc>
inline Value Value::MakeBooleanArrayAlloc(const Alloc& alloc,
                                          std::span<const bool> value,
                                          int64_t time) {
  auto data = AllocateArray<int>(alloc, value.size());
  std::copy(value.begin(), value.end(), data.get());

  Value val{NT_BOOLEAN_ARRAY, time, private_init{}};
  val.m_val.data.arr_boolean.arr = data.get();
  val.m_val.data.arr_boolean.size = value.size();
  val.m_storage = std::move(data);
  return val;
}

inline Value Value::MakeBooleanArray(std::initializer_list<bool> value,
                                     int64_t time) {
  return MakeBooleanArray(std::span(value.begin(), value.end()), time);
}

template <typename Alloc>
inline Value Value::MakeBooleanArrayAlloc(const Alloc& alloc,
                                          std::initializer_list<bool> value,
                                          int64_t time) {
  return MakeBooleanArrayAlloc(alloc, std::span(value.begin(), value.end()),
                               time);
}

#define NT_VALUE_ARRAY_IMPL(TYPE, Type, arr_type, ctype)                      \
  inline Value Value::Make##Type##Array(std::span<const ctype> value,         \
                                        int64_t time) {                       \
    auto data = AllocateArray<ctype>(value.size());                           \
    std::memcpy(data.get(), value.data(), value.size());                      \
                                                                              \
    Value val{NT_##TYPE##_ARRAY, time, private_init{}};                       \
    val.m_val.data.arr_##arr_type.arr = data.get();                           \
    val.m_val.data.arr_##arr_type.size = value.size();                        \
    val.m_storage = std::move(data);                                          \
    return val;                                                               \
  }                                                                           \
                                                                              \
  template <typename Alloc>                                                   \
  inline Value Value::Make##Type##ArrayAlloc(                                 \
      const Alloc& alloc, std::span<const ctype> value, int64_t time) {       \
    auto data = AllocateArray<ctype>(alloc, value.size());                    \
    std::memcpy(data.get(), value.data(), value.size());                      \
                                                                              \
    Value val{NT_##TYPE##_ARRAY, time, private_init{}};                       \
    val.m_val.data.arr_##arr_type.arr = data.get();                           \
    val.m_val.data.arr_##arr_type.size = value.size();                        \
    val.m_storage = std::move(data);                                          \
    return val;                                                               \
  }                                                                           \
                                                                              \
  inline Value Value::Make##Type##Array(std::initializer_list<ctype> value,   \
                                        int64_t time) {                       \
    return Make##Type##Array(std::span(value.begin(), value.end()), time);    \
  }                                                                           \
                                                                              \
  template <typename Alloc>                                                   \
  inline Value Value::Make##Type##ArrayAlloc(                                 \
      const Alloc& alloc, std::initializer_list<ctype> value, int64_t time) { \
    return Make##Type##ArrayAlloc(                                            \
        alloc, std::span(value.begin(), value.end()), time);                  \
  }                                                                           \
  template <typename T, typename>                                             \
  inline Value Value::Make##Type##Array(T&& value, int64_t time) {            \
    Value val{NT_##TYPE##_ARRAY, time, private_init{}};                       \
    auto data = std::make_shared<T>(std::forward<T&&>(value));                \
    val.m_val.data.arr_##arr_type.arr = data->data();                         \
    val.m_val.data.arr_##arr_type.size = data->size();                        \
    val.m_storage = std::move(data);                                          \
    return val;                                                               \
  }                                                                           \
                                                                              \
  template <typename Alloc, typename T, typename>                             \
  inline Value Value::Make##Type##ArrayAlloc(const Alloc& alloc, T&& value,   \
                                             int64_t time) {                  \
    Value val{NT_##TYPE##_ARRAY, time, private_init{}};                       \
    auto data = std::allocate_shared<T>(alloc, std::forward<T&&>(value));     \
    val.m_val.data.arr_##arr_type.arr = data->data();                         \
    val.m_val.data.arr_##arr_type.size = data->size();                        \
    val.m_storage = std::move(data);                                          \
    return val;                                                               \
  }

NT_VALUE_ARRAY_IMPL(BOOLEAN, Boolean, boolean, int)
NT_VALUE_ARRAY_IMPL(INTEGER, Integer, int, int64_t)
NT_VALUE_ARRAY_IMPL(FLOAT, Float, float, float)
NT_VALUE_ARRAY_IMPL(DOUBLE, Double, double, double)

#undef NT_VALUE_ARRAY_IMPL

template <typename T>
inline std::shared_ptr<T[]> Value::AllocateArray(size_t nelem) {
#if __cpp_lib_shared_ptr_arrays >= 201707L
#if __cpp_lib_smart_ptr_for_overwrite >= 202002L
  return std::make_shared_for_overwrite<T[]>(nelem);
#else
  auto data = std::make_shared<T[]>(nelem);
#endif
#else
  return std::shared_ptr<T[]>{new T[nelem]};
#endif
}

template <typename T, typename Alloc>
inline std::shared_ptr<T[]> Value::AllocateArray(const Alloc& alloc,
                                                 size_t nelem) {
#if __cpp_lib_shared_ptr_arrays >= 201707L
#if __cpp_lib_smart_ptr_for_overwrite >= 202002L
  return std::allocate_shared_for_overwrite<T[]>(alloc, nelem);
#else
  return std::allocate_shared<T[]>(alloc, nelem);
#endif
#else
  using RebindAlloc =
      typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
  RebindAlloc alloc2 = alloc;
  return std::shared_ptr<T[]>{
      std::allocator_traits<RebindAlloc>::allocate(alloc2, nelem)};
#endif
}

}  // namespace nt
