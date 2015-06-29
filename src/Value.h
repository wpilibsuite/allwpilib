/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_VALUE_H_
#define NT_VALUE_H_

#include <cassert>

#include "ntcore.h"

#include "llvm/ArrayRef.h"
#include "llvm/StringRef.h"

namespace ntimpl {

class Storage;
class Value;

/*
 * C++ wrapper class around NT_String.
 */
class StringValue : private NT_String {
  friend class Value;
 public:
  StringValue() { NT_InitString(this); }
  ~StringValue() { NT_DisposeString(this); }

  operator llvm::StringRef() const { return llvm::StringRef(str, len); }

  StringValue(const StringValue&) = delete;
  StringValue& operator=(const StringValue&) = delete;
};

/*
 * C++ wrapper class around NT_Value.
 */
class Value : private NT_Value {
  friend class Storage;
 public:
  Value() { NT_InitValue(this); }
  ~Value() { NT_DisposeValue(this); }

  // A little ugly, as it hides the identically-named NT_Value::type.
  NT_Type type() const { return NT_Value::type; }

  /*
   * Type-Safe Getters
   */
  bool GetBoolean() const {
    assert(NT_Value::type == NT_BOOLEAN);
    return data.v_boolean;
  }
  double GetDouble() const {
    assert(NT_Value::type == NT_DOUBLE);
    return data.v_double;
  }
  llvm::StringRef GetString() const {
    assert(NT_Value::type == NT_STRING);
    return static_cast<const StringValue&>(data.v_string);
  }
  llvm::StringRef GetRaw() const {
    assert(NT_Value::type == NT_RAW);
    return static_cast<const StringValue&>(data.v_raw);
  }
  // Ideally this would return llvm::ArrayRef<bool> but the C headers must
  // use "int" and casting may be very unsafe.
  llvm::ArrayRef<int> GetBooleanArray() const {
    assert(NT_Value::type == NT_BOOLEAN_ARRAY);
    return llvm::ArrayRef<int>(data.arr_boolean.arr, data.arr_boolean.size);
  }
  llvm::ArrayRef<double> GetDoubleArray() const {
    assert(NT_Value::type == NT_DOUBLE_ARRAY);
    return llvm::ArrayRef<double>(data.arr_double.arr, data.arr_double.size);
  }
  llvm::ArrayRef<StringValue> GetStringArray() const {
    assert(NT_Value::type == NT_BOOLEAN_ARRAY);
    return llvm::ArrayRef<StringValue>(
        static_cast<StringValue*>(data.arr_string.arr), data.arr_string.size);
  }

  /*
   * Type-Safe Setters
   */
  void SetBoolean(bool value);
  void SetDouble(double value);
  void SetString(llvm::StringRef value);
  void SetRaw(llvm::StringRef value);

  template<typename It>
  void SetBooleanArray(It begin, It end) {
  }

  template<typename It>
  void SetDoubleArray(It begin, It end) {
  }

  template<typename It>
  void SetStringArray(It begin, It end) {
  }

  Value(const Value&) = delete;
  Value& operator=(const Value&) = delete;

  friend bool operator==(const Value& lhs, const Value& rhs);
};

bool operator==(const Value& lhs, const Value& rhs);

}  // namespace ntimpl

#endif  // NT_VALUE_H_
