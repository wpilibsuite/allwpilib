/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_VALUE_H_
#define NT_VALUE_H_

#include <cassert>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include "llvm/ArrayRef.h"
#include "llvm/StringRef.h"

#include "ntcore_c.h"

namespace nt {

using llvm::ArrayRef;
using llvm::StringRef;

/** NetworkTables Entry Value */
class Value {
  struct private_init {};

 public:
  Value();
  Value(NT_Type type, const private_init&);
  ~Value();

  NT_Type type() const { return m_val.type; }
  const NT_Value& value() const { return m_val; }
  unsigned long long last_change() const { return m_val.last_change; }

  /*
   * Type Checkers
   */
  bool IsBoolean() const { return m_val.type == NT_BOOLEAN; }
  bool IsDouble() const { return m_val.type == NT_DOUBLE; }
  bool IsString() const { return m_val.type == NT_STRING; }
  bool IsRaw() const { return m_val.type == NT_RAW; }
  bool IsRpc() const { return m_val.type == NT_RPC; }
  bool IsBooleanArray() const { return m_val.type == NT_BOOLEAN_ARRAY; }
  bool IsDoubleArray() const { return m_val.type == NT_DOUBLE_ARRAY; }
  bool IsStringArray() const { return m_val.type == NT_STRING_ARRAY; }

  /*
   * Type-Safe Getters
   */
  bool GetBoolean() const {
    assert(m_val.type == NT_BOOLEAN);
    return m_val.data.v_boolean != 0;
  }
  double GetDouble() const {
    assert(m_val.type == NT_DOUBLE);
    return m_val.data.v_double;
  }
  StringRef GetString() const {
    assert(m_val.type == NT_STRING);
    return m_string;
  }
  StringRef GetRaw() const {
    assert(m_val.type == NT_RAW);
    return m_string;
  }
  StringRef GetRpc() const {
    assert(m_val.type == NT_RPC);
    return m_string;
  }
  ArrayRef<int> GetBooleanArray() const {
    assert(m_val.type == NT_BOOLEAN_ARRAY);
    return ArrayRef<int>(m_val.data.arr_boolean.arr,
                         m_val.data.arr_boolean.size);
  }
  ArrayRef<double> GetDoubleArray() const {
    assert(m_val.type == NT_DOUBLE_ARRAY);
    return ArrayRef<double>(m_val.data.arr_double.arr,
                            m_val.data.arr_double.size);
  }
  ArrayRef<std::string> GetStringArray() const {
    assert(m_val.type == NT_STRING_ARRAY);
    return m_string_array;
  }

  static std::shared_ptr<Value> MakeBoolean(bool value) {
    auto val = std::make_shared<Value>(NT_BOOLEAN, private_init());
    val->m_val.data.v_boolean = value;
    return val;
  }
  static std::shared_ptr<Value> MakeDouble(double value) {
    auto val = std::make_shared<Value>(NT_DOUBLE, private_init());
    val->m_val.data.v_double = value;
    return val;
  }
  static std::shared_ptr<Value> MakeString(StringRef value) {
    auto val = std::make_shared<Value>(NT_STRING, private_init());
    val->m_string = value;
    val->m_val.data.v_string.str = const_cast<char*>(val->m_string.c_str());
    val->m_val.data.v_string.len = val->m_string.size();
    return val;
  }
#ifdef _MSC_VER
  template <typename T,
            typename = std::enable_if_t<std::is_same<T, std::string>>>
#else
  template <typename T,
            typename std::enable_if<std::is_same<T, std::string>::value>::type>
#endif
  static std::shared_ptr<Value> MakeString(T&& value) {
    auto val = std::make_shared<Value>(NT_STRING, private_init());
    val->m_string = std::move(value);
    val->m_val.data.v_string.str = const_cast<char*>(val->m_string.c_str());
    val->m_val.data.v_string.len = val->m_string.size();
    return val;
  }
  static std::shared_ptr<Value> MakeRaw(StringRef value) {
    auto val = std::make_shared<Value>(NT_RAW, private_init());
    val->m_string = value;
    val->m_val.data.v_raw.str = const_cast<char*>(val->m_string.c_str());
    val->m_val.data.v_raw.len = val->m_string.size();
    return val;
  }
#ifdef _MSC_VER
  template <typename T,
            typename = std::enable_if_t<std::is_same<T, std::string>>>
#else
  template <typename T,
            typename std::enable_if<std::is_same<T, std::string>::value>::type>
#endif
  static std::shared_ptr<Value> MakeRaw(T&& value) {
    auto val = std::make_shared<Value>(NT_RAW, private_init());
    val->m_string = std::move(value);
    val->m_val.data.v_raw.str = const_cast<char*>(val->m_string.c_str());
    val->m_val.data.v_raw.len = val->m_string.size();
    return val;
  }
  static std::shared_ptr<Value> MakeRpc(StringRef value) {
    auto val = std::make_shared<Value>(NT_RPC, private_init());
    val->m_string = value;
    val->m_val.data.v_raw.str = const_cast<char*>(val->m_string.c_str());
    val->m_val.data.v_raw.len = val->m_string.size();
    return val;
  }
  template <typename T>
  static std::shared_ptr<Value> MakeRpc(T&& value) {
    auto val = std::make_shared<Value>(NT_RPC, private_init());
    val->m_string = std::move(value);
    val->m_val.data.v_raw.str = const_cast<char*>(val->m_string.c_str());
    val->m_val.data.v_raw.len = val->m_string.size();
    return val;
  }

  static std::shared_ptr<Value> MakeBooleanArray(ArrayRef<int> value);
  static std::shared_ptr<Value> MakeDoubleArray(ArrayRef<double> value);
  static std::shared_ptr<Value> MakeStringArray(ArrayRef<std::string> value);

  // Note: This function moves the values out of the vector.
  static std::shared_ptr<Value> MakeStringArray(
      std::vector<std::string>&& value);

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

}  // namespace nt

#endif  // NT_VALUE_H_
