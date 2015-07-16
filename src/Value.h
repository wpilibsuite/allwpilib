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
#include <vector>

#include "ntcore.h"

#include "llvm/ArrayRef.h"
#include "llvm/StringRef.h"

namespace ntimpl {

class Value {
  struct private_init {};

 public:
  Value();
  Value(NT_Type type, const private_init&);

  NT_Type type() const { return m_type; }

  /*
   * Type-Safe Getters
   */
  bool GetBoolean() const {
    assert(m_type == NT_BOOLEAN);
    return m_boolean;
  }
  double GetDouble() const {
    assert(m_type == NT_DOUBLE);
    return m_double;
  }
  llvm::StringRef GetString() const {
    assert(m_type == NT_STRING);
    return m_string;
  }
  llvm::StringRef GetRaw() const {
    assert(m_type == NT_RAW);
    return m_string;
  }
  llvm::StringRef GetRpc() const {
    assert(m_type == NT_RPC);
    return m_string;
  }
  llvm::ArrayRef<int> GetBooleanArray() const {
    assert(m_type == NT_BOOLEAN_ARRAY);
    return m_boolean_array;
  }
  llvm::ArrayRef<double> GetDoubleArray() const {
    assert(m_type == NT_DOUBLE_ARRAY);
    return m_double_array;
  }
  llvm::ArrayRef<std::string> GetStringArray() const {
    assert(m_type == NT_STRING_ARRAY);
    return m_string_array;
  }

  static std::shared_ptr<Value> MakeBoolean(bool value) {
    auto val = std::make_shared<Value>(NT_BOOLEAN, private_init());
    val->m_boolean = value;
    return val;
  }
  static std::shared_ptr<Value> MakeDouble(double value) {
    auto val = std::make_shared<Value>(NT_DOUBLE, private_init());
    val->m_double = value;
    return val;
  }
  static std::shared_ptr<Value> MakeString(llvm::StringRef value) {
    auto val = std::make_shared<Value>(NT_STRING, private_init());
    val->m_string = value;
    return val;
  }
  static std::shared_ptr<Value> MakeString(std::string&& value) {
    auto val = std::make_shared<Value>(NT_STRING, private_init());
    val->m_string = std::move(value);
    return val;
  }
  static std::shared_ptr<Value> MakeRaw(llvm::StringRef value) {
    auto val = std::make_shared<Value>(NT_RAW, private_init());
    val->m_string = value;
    return val;
  }
  static std::shared_ptr<Value> MakeRaw(std::string&& value) {
    auto val = std::make_shared<Value>(NT_RAW, private_init());
    val->m_string = std::move(value);
    return val;
  }
  static std::shared_ptr<Value> MakeRpc(llvm::StringRef value) {
    auto val = std::make_shared<Value>(NT_RPC, private_init());
    val->m_string = value;
    return val;
  }
  static std::shared_ptr<Value> MakeRpc(std::string&& value) {
    auto val = std::make_shared<Value>(NT_RPC, private_init());
    val->m_string = std::move(value);
    return val;
  }

  static std::shared_ptr<Value> MakeBooleanArray(llvm::ArrayRef<int> value);
  static std::shared_ptr<Value> MakeDoubleArray(llvm::ArrayRef<double> value);
  static std::shared_ptr<Value> MakeStringArray(
      llvm::ArrayRef<std::string> value);

  // Note: These functions move the values out of the vector.
  static std::shared_ptr<Value> MakeBooleanArray(std::vector<int>&& value);
  static std::shared_ptr<Value> MakeDoubleArray(std::vector<double>&& value);
  static std::shared_ptr<Value> MakeStringArray(
      std::vector<std::string>&& value);

  Value(const Value&) = delete;
  Value& operator=(const Value&) = delete;
  friend bool operator==(const Value& lhs, const Value& rhs);

 private:
  NT_Type m_type;
  bool m_boolean;
  double m_double;
  std::string m_string;
  std::vector<int> m_boolean_array;
  std::vector<double> m_double_array;
  std::vector<std::string> m_string_array;
};

void ConvertToC(const Value& in, NT_Value* out);
void ConvertToC(llvm::StringRef in, NT_String* out);
std::shared_ptr<Value> ConvertFromC(const NT_Value& value);
inline llvm::StringRef ConvertFromC(const NT_String& str) {
  return llvm::StringRef(str.str, str.len);
}

bool operator==(const Value& lhs, const Value& rhs);
inline bool operator!=(const Value& lhs, const Value& rhs) {
  return !(lhs == rhs);
}

}  // namespace ntimpl

#endif  // NT_VALUE_H_
