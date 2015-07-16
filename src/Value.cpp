/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Value.h"

using namespace ntimpl;

Value::Value() : m_type(NT_UNASSIGNED) {}

Value::Value(NT_Type type, const private_init&) : m_type(type) {}

std::shared_ptr<Value> Value::MakeBooleanArray(llvm::ArrayRef<int> value) {
  auto val = std::make_shared<Value>(NT_BOOLEAN_ARRAY, private_init());
  val->m_boolean_array = value;
  return val;
}

std::shared_ptr<Value> Value::MakeDoubleArray(llvm::ArrayRef<double> value) {
  auto val = std::make_shared<Value>(NT_DOUBLE_ARRAY, private_init());
  val->m_double_array = value;
  return val;
}

std::shared_ptr<Value> Value::MakeStringArray(
    llvm::ArrayRef<std::string> value) {
  auto val = std::make_shared<Value>(NT_STRING_ARRAY, private_init());
  val->m_string_array = value;
  return val;
}

std::shared_ptr<Value> Value::MakeBooleanArray(std::vector<int>&& value) {
  auto val = std::make_shared<Value>(NT_BOOLEAN_ARRAY, private_init());
  val->m_boolean_array = std::move(value);
  value.clear();
  return val;
}

std::shared_ptr<Value> Value::MakeDoubleArray(std::vector<double>&& value) {
  auto val = std::make_shared<Value>(NT_DOUBLE_ARRAY, private_init());
  val->m_double_array = std::move(value);
  value.clear();
  return val;
}

std::shared_ptr<Value> Value::MakeStringArray(
    std::vector<std::string>&& value) {
  auto val = std::make_shared<Value>(NT_STRING_ARRAY, private_init());
  val->m_string_array = std::move(value);
  value.clear();
  return val;
}

void ntimpl::ConvertToC(const Value& in, NT_Value* out) {
  NT_DisposeValue(out);
  switch (in.type()) {
    case NT_UNASSIGNED:
      return;
    case NT_BOOLEAN:
      out->data.v_boolean = in.GetBoolean() ? 1 : 0;
      break;
    case NT_DOUBLE:
      out->data.v_double = in.GetDouble();
      break;
    case NT_STRING:
      NT_InitString(&out->data.v_string);
      ConvertToC(in.GetString(), &out->data.v_string);
      break;
    case NT_RAW:
      NT_InitString(&out->data.v_raw);
      ConvertToC(in.GetRaw(), &out->data.v_raw);
      break;
    case NT_RPC:
      NT_InitString(&out->data.v_raw);
      ConvertToC(in.GetRpc(), &out->data.v_raw);
      break;
    case NT_BOOLEAN_ARRAY: {
      auto v = in.GetBooleanArray();
      out->data.arr_boolean.arr =
          static_cast<int*>(std::malloc(v.size() * sizeof(int)));
      out->data.arr_boolean.size = v.size();
      std::copy(v.begin(), v.end(), out->data.arr_boolean.arr);
      break;
    }
    case NT_DOUBLE_ARRAY: {
      auto v = in.GetDoubleArray();
      out->data.arr_double.arr =
          static_cast<double*>(std::malloc(v.size() * sizeof(double)));
      out->data.arr_double.size = v.size();
      std::copy(v.begin(), v.end(), out->data.arr_double.arr);
      break;
    }
    case NT_STRING_ARRAY: {
      auto v = in.GetStringArray();
      out->data.arr_string.arr =
          static_cast<NT_String*>(std::malloc(v.size()*sizeof(NT_String)));
      for (size_t i=0; i<v.size(); ++i) {
        NT_InitString(&out->data.arr_string.arr[i]);
        ConvertToC(v[i], &out->data.arr_string.arr[i]);
      }
      out->data.arr_string.size = v.size();
      break;
    }
    default:
      // assert(false && "unknown value type");
      return;
  }
  out->type = in.type();
}

void ntimpl::ConvertToC(llvm::StringRef in, NT_String* out) {
  NT_DisposeString(out);
  out->len = in.size();
  out->str = static_cast<char*>(std::malloc(in.size()+1));
  std::memcpy(out->str, in.data(), in.size());
  out->str[in.size()] = '\0';
}

std::shared_ptr<Value> ntimpl::ConvertFromC(const NT_Value& value) {
  switch (value.type) {
    case NT_UNASSIGNED:
      return nullptr;
    case NT_BOOLEAN:
      return Value::MakeBoolean(value.data.v_boolean != 0);
    case NT_DOUBLE:
      return Value::MakeDouble(value.data.v_double);
    case NT_STRING:
      return Value::MakeString(ConvertFromC(value.data.v_string));
    case NT_RAW:
      return Value::MakeRaw(ConvertFromC(value.data.v_raw));
    case NT_RPC:
      return Value::MakeRpc(ConvertFromC(value.data.v_raw));
    case NT_BOOLEAN_ARRAY:
      return Value::MakeBooleanArray(llvm::ArrayRef<int>(
          value.data.arr_boolean.arr, value.data.arr_boolean.size));
    case NT_DOUBLE_ARRAY:
      return Value::MakeDoubleArray(llvm::ArrayRef<double>(
          value.data.arr_double.arr, value.data.arr_double.size));
    case NT_STRING_ARRAY: {
      std::vector<std::string> v;
      v.reserve(value.data.arr_string.size);
      for (size_t i=0; i<value.data.arr_string.size; ++i)
        v.push_back(ConvertFromC(value.data.arr_string.arr[i]));
      return Value::MakeStringArray(std::move(v));
    }
    default:
      // assert(false && "unknown value type");
      return nullptr;
  }
}

bool ntimpl::operator==(const Value& lhs, const Value& rhs) {
  if (lhs.type() != rhs.type()) return false;
  switch (lhs.type()) {
    case NT_UNASSIGNED:
      return true;  // XXX: is this better being false instead?
    case NT_BOOLEAN:
      return lhs.m_boolean == rhs.m_boolean;
    case NT_DOUBLE:
      return lhs.m_double == rhs.m_double;
    case NT_STRING:
    case NT_RAW:
    case NT_RPC:
      return lhs.m_string == rhs.m_string;
    case NT_BOOLEAN_ARRAY:
      return lhs.m_boolean_array == rhs.m_boolean_array;
    case NT_DOUBLE_ARRAY:
      return lhs.m_double_array == rhs.m_double_array;
    case NT_STRING_ARRAY:
      return lhs.m_string_array == rhs.m_string_array;
    default:
      // assert(false && "unknown value type");
      return false;
  }
}
