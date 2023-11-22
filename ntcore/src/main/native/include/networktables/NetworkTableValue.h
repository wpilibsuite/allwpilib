// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <cassert>
#include <concepts>
#include <initializer_list>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "ntcore_c.h"

namespace nt {

#if __GNUC__ >= 13
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

/**
 * A network table entry value.
 * @ingroup ntcore_cpp_api
 */
class Value final {
  struct private_init {};

 public:
  Value();
  Value(NT_Type type, size_t size, int64_t time, const private_init&);
  Value(NT_Type type, size_t size, int64_t time, int64_t serverTime,
        const private_init&);

  explicit operator bool() const { return m_val.type != NT_UNASSIGNED; }

  /**
   * Get the data type.
   *
   * @return The type.
   */
  NT_Type type() const { return m_val.type; }

  /**
   * Get the data value stored.
   *
   * @return The type.
   */
  const NT_Value& value() const { return m_val; }

  /**
   * Get the creation time of the value, in local time.
   *
   * @return The time, in the units returned by nt::Now().
   */
  int64_t last_change() const { return m_val.last_change; }

  /**
   * Get the creation time of the value, in local time.
   *
   * @return The time, in the units returned by nt::Now().
   */
  int64_t time() const { return m_val.last_change; }

  /**
   * Get the approximate in-memory size of the value in bytes. This is zero for
   * values that do not require additional memory beyond the memory of the Value
   * itself.
   *
   * @return The size in bytes.
   */
  size_t size() const { return m_size; }

  /**
   * Set the local creation time of the value.
   *
   * @param time The time.
   */
  void SetTime(int64_t time) { m_val.last_change = time; }

  /**
   * Get the creation time of the value, in server time.
   *
   * @return The server time.
   */
  int64_t server_time() const { return m_val.server_time; }

  /**
   * Set the creation time of the value, in server time.
   *
   * @param time The server time.
   */
  void SetServerTime(int64_t time) { m_val.server_time = time; }

  /**
   * @{
   * @name Type Checkers
   */

  /**
   * Determine if entry value contains a value or is unassigned.
   *
   * @return True if the entry value contains a value.
   */
  bool IsValid() const { return m_val.type != NT_UNASSIGNED; }

  /**
   * Determine if entry value contains a boolean.
   *
   * @return True if the entry value is of boolean type.
   */
  bool IsBoolean() const { return m_val.type == NT_BOOLEAN; }

  /**
   * Determine if entry value contains an integer.
   *
   * @return True if the entry value is of integer type.
   */
  bool IsInteger() const { return m_val.type == NT_INTEGER; }

  /**
   * Determine if entry value contains a float.
   *
   * @return True if the entry value is of float type.
   */
  bool IsFloat() const { return m_val.type == NT_FLOAT; }

  /**
   * Determine if entry value contains a double.
   *
   * @return True if the entry value is of double type.
   */
  bool IsDouble() const { return m_val.type == NT_DOUBLE; }

  /**
   * Determine if entry value contains a string.
   *
   * @return True if the entry value is of string type.
   */
  bool IsString() const { return m_val.type == NT_STRING; }

  /**
   * Determine if entry value contains a raw.
   *
   * @return True if the entry value is of raw type.
   */
  bool IsRaw() const { return m_val.type == NT_RAW; }

  /**
   * Determine if entry value contains a boolean array.
   *
   * @return True if the entry value is of boolean array type.
   */
  bool IsBooleanArray() const { return m_val.type == NT_BOOLEAN_ARRAY; }

  /**
   * Determine if entry value contains an integer array.
   *
   * @return True if the entry value is of integer array type.
   */
  bool IsIntegerArray() const { return m_val.type == NT_INTEGER_ARRAY; }

  /**
   * Determine if entry value contains a float array.
   *
   * @return True if the entry value is of float array type.
   */
  bool IsFloatArray() const { return m_val.type == NT_FLOAT_ARRAY; }

  /**
   * Determine if entry value contains a double array.
   *
   * @return True if the entry value is of double array type.
   */
  bool IsDoubleArray() const { return m_val.type == NT_DOUBLE_ARRAY; }

  /**
   * Determine if entry value contains a string array.
   *
   * @return True if the entry value is of string array type.
   */
  bool IsStringArray() const { return m_val.type == NT_STRING_ARRAY; }

  /** @} */

  /**
   * @{
   * @name Type-Safe Getters
   */

  /**
   * Get the entry's boolean value.
   *
   * @return The boolean value.
   */
  bool GetBoolean() const {
    assert(m_val.type == NT_BOOLEAN);
    return m_val.data.v_boolean != 0;
  }

  /**
   * Get the entry's integer value.
   *
   * @return The integer value.
   */
  int64_t GetInteger() const {
    assert(m_val.type == NT_INTEGER);
    return m_val.data.v_int;
  }

  /**
   * Get the entry's float value.
   *
   * @return The float value.
   */
  float GetFloat() const {
    assert(m_val.type == NT_FLOAT);
    return m_val.data.v_float;
  }

  /**
   * Get the entry's double value.
   *
   * @return The double value.
   */
  double GetDouble() const {
    assert(m_val.type == NT_DOUBLE);
    return m_val.data.v_double;
  }

  /**
   * Get the entry's string value.
   *
   * @return The string value.
   */
  std::string_view GetString() const {
    assert(m_val.type == NT_STRING);
    return {m_val.data.v_string.str, m_val.data.v_string.len};
  }

  /**
   * Get the entry's raw value.
   *
   * @return The raw value.
   */
  std::span<const uint8_t> GetRaw() const {
    assert(m_val.type == NT_RAW);
    return {m_val.data.v_raw.data, m_val.data.v_raw.size};
  }

  /**
   * Get the entry's boolean array value.
   *
   * @return The boolean array value.
   */
  std::span<const int> GetBooleanArray() const {
    assert(m_val.type == NT_BOOLEAN_ARRAY);
    return {m_val.data.arr_boolean.arr, m_val.data.arr_boolean.size};
  }

  /**
   * Get the entry's integer array value.
   *
   * @return The integer array value.
   */
  std::span<const int64_t> GetIntegerArray() const {
    assert(m_val.type == NT_INTEGER_ARRAY);
    return {m_val.data.arr_int.arr, m_val.data.arr_int.size};
  }

  /**
   * Get the entry's float array value.
   *
   * @return The float array value.
   */
  std::span<const float> GetFloatArray() const {
    assert(m_val.type == NT_FLOAT_ARRAY);
    return {m_val.data.arr_float.arr, m_val.data.arr_float.size};
  }

  /**
   * Get the entry's double array value.
   *
   * @return The double array value.
   */
  std::span<const double> GetDoubleArray() const {
    assert(m_val.type == NT_DOUBLE_ARRAY);
    return {m_val.data.arr_double.arr, m_val.data.arr_double.size};
  }

  /**
   * Get the entry's string array value.
   *
   * @return The string array value.
   */
  std::span<const std::string> GetStringArray() const {
    assert(m_val.type == NT_STRING_ARRAY);
    return *static_cast<std::vector<std::string>*>(m_storage.get());
  }

  /** @} */

  /**
   * @{
   * @name Factory functions
   */

  /**
   * Creates a boolean entry value.
   *
   * @param value the value
   * @param time if nonzero, the creation time to use (instead of the current
   *             time)
   * @return The entry value
   */
  static Value MakeBoolean(bool value, int64_t time = 0) {
    Value val{NT_BOOLEAN, 0, time, private_init{}};
    val.m_val.data.v_boolean = value;
    return val;
  }

  /**
   * Creates an integer entry value.
   *
   * @param value the value
   * @param time if nonzero, the creation time to use (instead of the current
   *             time)
   * @return The entry value
   */
  static Value MakeInteger(int64_t value, int64_t time = 0) {
    Value val{NT_INTEGER, 0, time, private_init{}};
    val.m_val.data.v_int = value;
    return val;
  }

  /**
   * Creates a float entry value.
   *
   * @param value the value
   * @param time if nonzero, the creation time to use (instead of the current
   *             time)
   * @return The entry value
   */
  static Value MakeFloat(float value, int64_t time = 0) {
    Value val{NT_FLOAT, 0, time, private_init{}};
    val.m_val.data.v_float = value;
    return val;
  }

  /**
   * Creates a double entry value.
   *
   * @param value the value
   * @param time if nonzero, the creation time to use (instead of the current
   *             time)
   * @return The entry value
   */
  static Value MakeDouble(double value, int64_t time = 0) {
    Value val{NT_DOUBLE, 0, time, private_init{}};
    val.m_val.data.v_double = value;
    return val;
  }

  /**
   * Creates a string entry value.
   *
   * @param value the value
   * @param time if nonzero, the creation time to use (instead of the current
   *             time)
   * @return The entry value
   */
  static Value MakeString(std::string_view value, int64_t time = 0) {
    auto data = std::make_shared<std::string>(value);
    Value val{NT_STRING, data->capacity(), time, private_init{}};
    val.m_val.data.v_string.str = const_cast<char*>(data->c_str());
    val.m_val.data.v_string.len = data->size();
    val.m_storage = std::move(data);
    return val;
  }

  /**
   * Creates a string entry value.
   *
   * @param value the value
   * @param time if nonzero, the creation time to use (instead of the current
   *             time)
   * @return The entry value
   */
  template <std::same_as<std::string> T>
  static Value MakeString(T&& value, int64_t time = 0) {
    auto data = std::make_shared<std::string>(std::forward<T>(value));
    Value val{NT_STRING, data->capacity(), time, private_init{}};
    val.m_val.data.v_string.str = const_cast<char*>(data->c_str());
    val.m_val.data.v_string.len = data->size();
    val.m_storage = std::move(data);
    return val;
  }

  /**
   * Creates a raw entry value.
   *
   * @param value the value
   * @param time if nonzero, the creation time to use (instead of the current
   *             time)
   * @return The entry value
   */
  static Value MakeRaw(std::span<const uint8_t> value, int64_t time = 0) {
    auto data =
        std::make_shared<std::vector<uint8_t>>(value.begin(), value.end());
    Value val{NT_RAW, data->capacity(), time, private_init{}};
    val.m_val.data.v_raw.data = const_cast<uint8_t*>(data->data());
    val.m_val.data.v_raw.size = data->size();
    val.m_storage = std::move(data);
    return val;
  }

  /**
   * Creates a raw entry value.
   *
   * @param value the value
   * @param time if nonzero, the creation time to use (instead of the current
   *             time)
   * @return The entry value
   */
  template <std::same_as<std::vector<uint8_t>> T>
  static Value MakeRaw(T&& value, int64_t time = 0) {
    auto data = std::make_shared<std::vector<uint8_t>>(std::forward<T>(value));
    Value val{NT_RAW, data->capacity(), time, private_init{}};
    val.m_val.data.v_raw.data = const_cast<uint8_t*>(data->data());
    val.m_val.data.v_raw.size = data->size();
    val.m_storage = std::move(data);
    return val;
  }

  /**
   * Creates a boolean array entry value.
   *
   * @param value the value
   * @param time if nonzero, the creation time to use (instead of the current
   *             time)
   * @return The entry value
   */
  static Value MakeBooleanArray(std::span<const bool> value, int64_t time = 0);

  /**
   * Creates a boolean array entry value.
   *
   * @param value the value
   * @param time if nonzero, the creation time to use (instead of the current
   *             time)
   * @return The entry value
   */
  static Value MakeBooleanArray(std::initializer_list<bool> value,
                                int64_t time = 0) {
    return MakeBooleanArray(std::span(value.begin(), value.end()), time);
  }

  /**
   * Creates a boolean array entry value.
   *
   * @param value the value
   * @param time if nonzero, the creation time to use (instead of the current
   *             time)
   * @return The entry value
   */
  static Value MakeBooleanArray(std::span<const int> value, int64_t time = 0);

  /**
   * Creates a boolean array entry value.
   *
   * @param value the value
   * @param time if nonzero, the creation time to use (instead of the current
   *             time)
   * @return The entry value
   */
  static Value MakeBooleanArray(std::initializer_list<int> value,
                                int64_t time = 0) {
    return MakeBooleanArray(std::span(value.begin(), value.end()), time);
  }

  /**
   * Creates a boolean array entry value.
   *
   * @param value the value
   * @param time if nonzero, the creation time to use (instead of the current
   *             time)
   * @return The entry value
   *
   * @note This function moves the values out of the vector.
   */
  static Value MakeBooleanArray(std::vector<int>&& value, int64_t time = 0);

  /**
   * Creates an integer array entry value.
   *
   * @param value the value
   * @param time if nonzero, the creation time to use (instead of the current
   *             time)
   * @return The entry value
   */
  static Value MakeIntegerArray(std::span<const int64_t> value,
                                int64_t time = 0);

  /**
   * Creates an integer array entry value.
   *
   * @param value the value
   * @param time if nonzero, the creation time to use (instead of the current
   *             time)
   * @return The entry value
   */
  static Value MakeIntegerArray(std::initializer_list<int64_t> value,
                                int64_t time = 0) {
    return MakeIntegerArray(std::span(value.begin(), value.end()), time);
  }

  /**
   * Creates an integer array entry value.
   *
   * @param value the value
   * @param time if nonzero, the creation time to use (instead of the current
   *             time)
   * @return The entry value
   *
   * @note This function moves the values out of the vector.
   */
  static Value MakeIntegerArray(std::vector<int64_t>&& value, int64_t time = 0);

  /**
   * Creates a float array entry value.
   *
   * @param value the value
   * @param time if nonzero, the creation time to use (instead of the current
   *             time)
   * @return The entry value
   */
  static Value MakeFloatArray(std::span<const float> value, int64_t time = 0);

  /**
   * Creates a float array entry value.
   *
   * @param value the value
   * @param time if nonzero, the creation time to use (instead of the current
   *             time)
   * @return The entry value
   */
  static Value MakeFloatArray(std::initializer_list<float> value,
                              int64_t time = 0) {
    return MakeFloatArray(std::span(value.begin(), value.end()), time);
  }

  /**
   * Creates a float array entry value.
   *
   * @param value the value
   * @param time if nonzero, the creation time to use (instead of the current
   *             time)
   * @return The entry value
   *
   * @note This function moves the values out of the vector.
   */
  static Value MakeFloatArray(std::vector<float>&& value, int64_t time = 0);

  /**
   * Creates a double array entry value.
   *
   * @param value the value
   * @param time if nonzero, the creation time to use (instead of the current
   *             time)
   * @return The entry value
   */
  static Value MakeDoubleArray(std::span<const double> value, int64_t time = 0);

  /**
   * Creates a double array entry value.
   *
   * @param value the value
   * @param time if nonzero, the creation time to use (instead of the current
   *             time)
   * @return The entry value
   */
  static Value MakeDoubleArray(std::initializer_list<double> value,
                               int64_t time = 0) {
    return MakeDoubleArray(std::span(value.begin(), value.end()), time);
  }

  /**
   * Creates a double array entry value.
   *
   * @param value the value
   * @param time if nonzero, the creation time to use (instead of the current
   *             time)
   * @return The entry value
   *
   * @note This function moves the values out of the vector.
   */
  static Value MakeDoubleArray(std::vector<double>&& value, int64_t time = 0);

  /**
   * Creates a string array entry value.
   *
   * @param value the value
   * @param time if nonzero, the creation time to use (instead of the current
   *             time)
   * @return The entry value
   */
  static Value MakeStringArray(std::span<const std::string> value,
                               int64_t time = 0);

  /**
   * Creates a string array entry value.
   *
   * @param value the value
   * @param time if nonzero, the creation time to use (instead of the current
   *             time)
   * @return The entry value
   */
  static Value MakeStringArray(std::initializer_list<std::string> value,
                               int64_t time = 0) {
    return MakeStringArray(std::span(value.begin(), value.end()), time);
  }

  /**
   * Creates a string array entry value.
   *
   * @param value the value
   * @param time if nonzero, the creation time to use (instead of the current
   *             time)
   * @return The entry value
   *
   * @note This function moves the values out of the vector.
   */
  static Value MakeStringArray(std::vector<std::string>&& value,
                               int64_t time = 0);

  /** @} */

  friend bool operator==(const Value& lhs, const Value& rhs);

 private:
  NT_Value m_val = {};
  std::shared_ptr<void> m_storage;
  size_t m_size = 0;
};

#if __GNUC__ >= 13
#pragma GCC diagnostic pop
#endif

bool operator==(const Value& lhs, const Value& rhs);

/**
 * NetworkTable Value alias for similarity with Java.
 * @ingroup ntcore_cpp_api
 */
using NetworkTableValue = Value;

}  // namespace nt
