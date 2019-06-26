/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NTCORE_NETWORKTABLES_NETWORKTABLEVALUE_H_
#define NTCORE_NETWORKTABLES_NETWORKTABLEVALUE_H_

#include <stdint.h>

#include <cassert>
#include <initializer_list>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <wpi/ArrayRef.h>
#include <wpi/StringRef.h>
#include <wpi/Twine.h>

#include "ntcore_c.h"

namespace nt {

using wpi::ArrayRef;
using wpi::StringRef;
using wpi::Twine;

/**
 * A network table entry value.
 * @ingroup ntcore_cpp_api
 */
class Value final {
  struct private_init {};

 public:
  Value();
  Value(NT_Type type, uint64_t time, const private_init&);
  ~Value();

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
   * Get the creation time of the value.
   *
   * @return The time, in the units returned by nt::Now().
   */
  uint64_t last_change() const { return m_val.last_change; }

  /**
   * Get the creation time of the value.
   *
   * @return The time, in the units returned by nt::Now().
   */
  uint64_t time() const { return m_val.last_change; }

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
   * Determine if entry value contains a rpc definition.
   *
   * @return True if the entry value is of rpc definition type.
   */
  bool IsRpc() const { return m_val.type == NT_RPC; }

  /**
   * Determine if entry value contains a boolean array.
   *
   * @return True if the entry value is of boolean array type.
   */
  bool IsBooleanArray() const { return m_val.type == NT_BOOLEAN_ARRAY; }

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
  StringRef GetString() const {
    assert(m_val.type == NT_STRING);
    return m_string;
  }

  /**
   * Get the entry's raw value.
   *
   * @return The raw value.
   */
  StringRef GetRaw() const {
    assert(m_val.type == NT_RAW);
    return m_string;
  }

  /**
   * Get the entry's rpc definition value.
   *
   * @return The rpc definition value.
   */
  StringRef GetRpc() const {
    assert(m_val.type == NT_RPC);
    return m_string;
  }

  /**
   * Get the entry's boolean array value.
   *
   * @return The boolean array value.
   */
  ArrayRef<int> GetBooleanArray() const {
    assert(m_val.type == NT_BOOLEAN_ARRAY);
    return ArrayRef<int>(m_val.data.arr_boolean.arr,
                         m_val.data.arr_boolean.size);
  }

  /**
   * Get the entry's double array value.
   *
   * @return The double array value.
   */
  ArrayRef<double> GetDoubleArray() const {
    assert(m_val.type == NT_DOUBLE_ARRAY);
    return ArrayRef<double>(m_val.data.arr_double.arr,
                            m_val.data.arr_double.size);
  }

  /**
   * Get the entry's string array value.
   *
   * @return The string array value.
   */
  ArrayRef<std::string> GetStringArray() const {
    assert(m_val.type == NT_STRING_ARRAY);
    return m_string_array;
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
  static std::shared_ptr<Value> MakeBoolean(bool value, uint64_t time = 0) {
    auto val = std::make_shared<Value>(NT_BOOLEAN, time, private_init());
    val->m_val.data.v_boolean = value;
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
  static std::shared_ptr<Value> MakeDouble(double value, uint64_t time = 0) {
    auto val = std::make_shared<Value>(NT_DOUBLE, time, private_init());
    val->m_val.data.v_double = value;
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
  static std::shared_ptr<Value> MakeString(const Twine& value,
                                           uint64_t time = 0) {
    auto val = std::make_shared<Value>(NT_STRING, time, private_init());
    val->m_string = value.str();
    val->m_val.data.v_string.str = const_cast<char*>(val->m_string.c_str());
    val->m_val.data.v_string.len = val->m_string.size();
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
  template <typename T,
            typename std::enable_if<std::is_same<T, std::string>::value>::type>
  static std::shared_ptr<Value> MakeString(T&& value, uint64_t time = 0) {
    auto val = std::make_shared<Value>(NT_STRING, time, private_init());
    val->m_string = std::move(value);
    val->m_val.data.v_string.str = const_cast<char*>(val->m_string.c_str());
    val->m_val.data.v_string.len = val->m_string.size();
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
  static std::shared_ptr<Value> MakeRaw(StringRef value, uint64_t time = 0) {
    auto val = std::make_shared<Value>(NT_RAW, time, private_init());
    val->m_string = value;
    val->m_val.data.v_raw.str = const_cast<char*>(val->m_string.c_str());
    val->m_val.data.v_raw.len = val->m_string.size();
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
  template <typename T,
            typename std::enable_if<std::is_same<T, std::string>::value>::type>
  static std::shared_ptr<Value> MakeRaw(T&& value, uint64_t time = 0) {
    auto val = std::make_shared<Value>(NT_RAW, time, private_init());
    val->m_string = std::move(value);
    val->m_val.data.v_raw.str = const_cast<char*>(val->m_string.c_str());
    val->m_val.data.v_raw.len = val->m_string.size();
    return val;
  }

  /**
   * Creates a rpc entry value.
   *
   * @param value the value
   * @param time if nonzero, the creation time to use (instead of the current
   *             time)
   * @return The entry value
   */
  static std::shared_ptr<Value> MakeRpc(StringRef value, uint64_t time = 0) {
    auto val = std::make_shared<Value>(NT_RPC, time, private_init());
    val->m_string = value;
    val->m_val.data.v_raw.str = const_cast<char*>(val->m_string.c_str());
    val->m_val.data.v_raw.len = val->m_string.size();
    return val;
  }

  /**
   * Creates a rpc entry value.
   *
   * @param value the value
   * @param time if nonzero, the creation time to use (instead of the current
   *             time)
   * @return The entry value
   */
  template <typename T>
  static std::shared_ptr<Value> MakeRpc(T&& value, uint64_t time = 0) {
    auto val = std::make_shared<Value>(NT_RPC, time, private_init());
    val->m_string = std::move(value);
    val->m_val.data.v_raw.str = const_cast<char*>(val->m_string.c_str());
    val->m_val.data.v_raw.len = val->m_string.size();
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
  static std::shared_ptr<Value> MakeBooleanArray(ArrayRef<bool> value,
                                                 uint64_t time = 0);

  /**
   * Creates a boolean array entry value.
   *
   * @param value the value
   * @param time if nonzero, the creation time to use (instead of the current
   *             time)
   * @return The entry value
   */
  static std::shared_ptr<Value> MakeBooleanArray(
      std::initializer_list<bool> value, uint64_t time = 0) {
    return MakeBooleanArray(wpi::makeArrayRef(value.begin(), value.end()),
                            time);
  }

  /**
   * Creates a boolean array entry value.
   *
   * @param value the value
   * @param time if nonzero, the creation time to use (instead of the current
   *             time)
   * @return The entry value
   */
  static std::shared_ptr<Value> MakeBooleanArray(ArrayRef<int> value,
                                                 uint64_t time = 0);

  /**
   * Creates a boolean array entry value.
   *
   * @param value the value
   * @param time if nonzero, the creation time to use (instead of the current
   *             time)
   * @return The entry value
   */
  static std::shared_ptr<Value> MakeBooleanArray(
      std::initializer_list<int> value, uint64_t time = 0) {
    return MakeBooleanArray(wpi::makeArrayRef(value.begin(), value.end()),
                            time);
  }

  /**
   * Creates a double array entry value.
   *
   * @param value the value
   * @param time if nonzero, the creation time to use (instead of the current
   *             time)
   * @return The entry value
   */
  static std::shared_ptr<Value> MakeDoubleArray(ArrayRef<double> value,
                                                uint64_t time = 0);

  /**
   * Creates a double array entry value.
   *
   * @param value the value
   * @param time if nonzero, the creation time to use (instead of the current
   *             time)
   * @return The entry value
   */
  static std::shared_ptr<Value> MakeDoubleArray(
      std::initializer_list<double> value, uint64_t time = 0) {
    return MakeDoubleArray(wpi::makeArrayRef(value.begin(), value.end()), time);
  }

  /**
   * Creates a string array entry value.
   *
   * @param value the value
   * @param time if nonzero, the creation time to use (instead of the current
   *             time)
   * @return The entry value
   */
  static std::shared_ptr<Value> MakeStringArray(ArrayRef<std::string> value,
                                                uint64_t time = 0);

  /**
   * Creates a string array entry value.
   *
   * @param value the value
   * @param time if nonzero, the creation time to use (instead of the current
   *             time)
   * @return The entry value
   */
  static std::shared_ptr<Value> MakeStringArray(
      std::initializer_list<std::string> value, uint64_t time = 0) {
    return MakeStringArray(wpi::makeArrayRef(value.begin(), value.end()), time);
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
  static std::shared_ptr<Value> MakeStringArray(
      std::vector<std::string>&& value, uint64_t time = 0);

  /** @} */

  Value(const Value&) = delete;
  Value& operator=(const Value&) = delete;
  friend bool operator==(const Value& lhs, const Value& rhs);

 private:
  NT_Value m_val;
  std::string m_string;
  std::vector<std::string> m_string_array;
};

bool operator==(const Value& lhs, const Value& rhs);
inline bool operator!=(const Value& lhs, const Value& rhs) {
  return !(lhs == rhs);
}

/**
 * NetworkTable Value alias for similarity with Java.
 * @ingroup ntcore_cpp_api
 */
typedef Value NetworkTableValue;

}  // namespace nt

#endif  // NTCORE_NETWORKTABLES_NETWORKTABLEVALUE_H_
