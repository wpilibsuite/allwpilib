/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_VALUE_H_
#define NT_VALUE_H_

#include <cassert>
#include <vector>

#include "ntcore.h"

#include "llvm/ArrayRef.h"
#include "llvm/StringRef.h"

namespace ntimpl {

class Message;
class StringValueTest;
class Storage;
class Value;
class ValueTest;
class WireDecoder;

/*
 * C++ wrapper class around NT_String.
 */
class StringValue : private NT_String {
  friend class Message;
  friend class StringValueTest;
  friend class Value;
  friend class WireDecoder;
 public:
  StringValue() { NT_InitString(this); }
  StringValue(llvm::StringRef val);
  ~StringValue() { NT_DisposeString(this); }

  operator llvm::StringRef() const { return llvm::StringRef(str, len); }

  StringValue(const StringValue&) = delete;
  StringValue& operator=(const StringValue&) = delete;

  StringValue(StringValue&& other) {
    str = other.str;
    len = other.len;
    other.str = nullptr;
    other.len = 0;
  }

  StringValue& operator=(StringValue&& other) {
    if (this != &other) {
      NT_DisposeString(this);
      str = other.str;
      len = other.len;
      other.str = nullptr;
      other.len = 0;
    }
    return *this;
  }

  StringValue& operator=(llvm::StringRef val);
};

inline bool operator==(const StringValue& lhs, const StringValue& rhs) {
  return llvm::StringRef(lhs) == llvm::StringRef(rhs);
}
inline bool operator!=(const StringValue& lhs, const StringValue& rhs) {
  return llvm::StringRef(lhs) != llvm::StringRef(rhs);
}
inline bool operator==(llvm::StringRef lhs, const StringValue& rhs) {
  return lhs == llvm::StringRef(rhs);
}
inline bool operator!=(llvm::StringRef lhs, const StringValue& rhs) {
  return lhs != llvm::StringRef(rhs);
}
inline bool operator==(const StringValue& lhs, llvm::StringRef rhs) {
  return llvm::StringRef(lhs) == rhs;
}
inline bool operator!=(const StringValue& lhs, llvm::StringRef rhs) {
  return llvm::StringRef(lhs) != rhs;
}

/*
 * C++ wrapper class around NT_Value.
 */
class Value : private NT_Value {
  friend class Message;
  friend class Storage;
  friend class ValueTest;
  friend class WireDecoder;
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
  llvm::ArrayRef<int> GetBooleanArray() const {
    assert(NT_Value::type == NT_BOOLEAN_ARRAY);
    return llvm::ArrayRef<int>(data.arr_boolean.arr, data.arr_boolean.size);
  }
  llvm::ArrayRef<double> GetDoubleArray() const {
    assert(NT_Value::type == NT_DOUBLE_ARRAY);
    return llvm::ArrayRef<double>(data.arr_double.arr, data.arr_double.size);
  }
  llvm::ArrayRef<StringValue> GetStringArray() const {
    assert(NT_Value::type == NT_STRING_ARRAY);
    return llvm::ArrayRef<StringValue>(
        static_cast<StringValue*>(data.arr_string.arr), data.arr_string.size);
  }

  /*
   * Type-Safe Setters
   */
  void SetBoolean(bool value) {
    if (NT_Value::type != NT_BOOLEAN) {
      NT_DisposeValue(this);
      NT_Value::type = NT_BOOLEAN;
    }
    data.v_boolean = value ? 1 : 0;
  }
  void SetDouble(double value) {
    if (NT_Value::type != NT_DOUBLE) {
      NT_DisposeValue(this);
      NT_Value::type = NT_DOUBLE;
    }
    data.v_double = value;
  }
  void SetString(llvm::StringRef value) { SetString(StringValue(value)); }
  void SetString(StringValue&& value) {
    if (NT_Value::type != NT_STRING) {
      NT_DisposeValue(this);
      data.v_string.str = nullptr;
      data.v_string.len = 0;
      NT_Value::type = NT_STRING;
    }
    static_cast<StringValue&>(data.v_string) = std::move(value);
  }
  void SetRaw(llvm::StringRef value) { SetRaw(StringValue(value)); }
  void SetRaw(StringValue&& value) {
    if (NT_Value::type != NT_RAW) {
      NT_DisposeValue(this);
      data.v_raw.str = nullptr;
      data.v_raw.len = 0;
      NT_Value::type = NT_RAW;
    }
    static_cast<StringValue&>(data.v_raw) = std::move(value);
  }

  void SetBooleanArray(llvm::ArrayRef<int> value);
  void SetDoubleArray(llvm::ArrayRef<double> value);

  // Note: This function moves the values out of the vector.
  void SetStringArray(std::vector<StringValue>& value);

  Value(const Value&) = delete;
  Value& operator=(const Value&) = delete;

  Value(Value&& other) {
    NT_Value::type = static_cast<NT_Value&>(other).type;
    last_change = other.last_change;
    data = other.data;
    static_cast<NT_Value&>(other).type = NT_UNASSIGNED;
    other.last_change = 0;
  }

  Value& operator=(Value&& other) {
    if (this != &other) {
      NT_DisposeValue(this);
      NT_Value::type = static_cast<NT_Value&>(other).type;
      last_change = other.last_change;
      data = other.data;
      static_cast<NT_Value&>(other).type = NT_UNASSIGNED;
      other.last_change = 0;
    }
    return *this;
  }

  friend bool operator==(const Value& lhs, const Value& rhs);
};

bool operator==(const Value& lhs, const Value& rhs);
inline bool operator!=(const Value& lhs, const Value& rhs) {
  return !(lhs == rhs);
}

}  // namespace ntimpl

#endif  // NT_VALUE_H_
