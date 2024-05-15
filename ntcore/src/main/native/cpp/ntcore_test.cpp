// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ntcore_test.h"

#include <cstring>

#include <wpi/MemAlloc.h>

#include "Value_internal.h"

extern "C" {
struct WPI_String* NT_GetStringForTesting(const char* str, int* struct_size) {
  struct WPI_String* strout =
      static_cast<WPI_String*>(wpi::safe_calloc(1, sizeof(WPI_String)));
  nt::ConvertToC(str, strout);
  *struct_size = sizeof(WPI_String);
  return strout;
}

struct NT_TopicInfo* NT_GetTopicInfoForTesting(const char* name,
                                               enum NT_Type type,
                                               const char* type_str,
                                               int* struct_size) {
  struct NT_TopicInfo* topic_info =
      static_cast<NT_TopicInfo*>(wpi::safe_calloc(1, sizeof(NT_TopicInfo)));
  nt::ConvertToC(name, &topic_info->name);
  topic_info->type = type;
  nt::ConvertToC(type_str, &topic_info->type_str);
  *struct_size = sizeof(NT_TopicInfo);
  return topic_info;
}

void NT_FreeTopicInfoForTesting(struct NT_TopicInfo* info) {
  WPI_FreeString(&info->name);
  WPI_FreeString(&info->type_str);
  WPI_FreeString(&info->properties);
  std::free(info);
}

struct NT_ConnectionInfo* NT_GetConnectionInfoForTesting(
    const char* remote_id, const char* remote_ip, unsigned int remote_port,
    uint64_t last_update, unsigned int protocol_version, int* struct_size) {
  struct NT_ConnectionInfo* conn_info = static_cast<NT_ConnectionInfo*>(
      wpi::safe_calloc(1, sizeof(NT_ConnectionInfo)));
  nt::ConvertToC(remote_id, &conn_info->remote_id);
  nt::ConvertToC(remote_ip, &conn_info->remote_ip);
  conn_info->remote_port = remote_port;
  conn_info->last_update = last_update;
  conn_info->protocol_version = protocol_version;
  *struct_size = sizeof(NT_ConnectionInfo);
  return conn_info;
}

void NT_FreeConnectionInfoForTesting(struct NT_ConnectionInfo* info) {
  WPI_FreeString(&info->remote_id);
  WPI_FreeString(&info->remote_ip);
  std::free(info);
}

struct NT_Value* NT_GetValueBooleanForTesting(uint64_t last_change, int val,
                                              int* struct_size) {
  struct NT_Value* value =
      static_cast<NT_Value*>(wpi::safe_calloc(1, sizeof(NT_Value)));
  value->type = NT_BOOLEAN;
  value->last_change = last_change;
  value->data.v_boolean = val;
  *struct_size = sizeof(NT_Value);
  return value;
}

struct NT_Value* NT_GetValueDoubleForTesting(uint64_t last_change, double val,
                                             int* struct_size) {
  struct NT_Value* value =
      static_cast<NT_Value*>(wpi::safe_calloc(1, sizeof(NT_Value)));
  value->type = NT_DOUBLE;
  value->last_change = last_change;
  value->data.v_double = val;
  *struct_size = sizeof(NT_Value);
  return value;
}

struct NT_Value* NT_GetValueStringForTesting(uint64_t last_change,
                                             const char* str,
                                             int* struct_size) {
  struct NT_Value* value =
      static_cast<NT_Value*>(wpi::safe_calloc(1, sizeof(NT_Value)));
  value->type = NT_STRING;
  value->last_change = last_change;
  nt::ConvertToC(str, &value->data.v_string);
  *struct_size = sizeof(NT_Value);
  return value;
}

struct NT_Value* NT_GetValueRawForTesting(uint64_t last_change, const char* raw,
                                          int raw_len, int* struct_size) {
  struct NT_Value* value =
      static_cast<NT_Value*>(wpi::safe_calloc(1, sizeof(NT_Value)));
  value->type = NT_RAW;
  value->last_change = last_change;
  nt::ConvertToC(std::string_view(raw, raw_len), &value->data.v_string);
  *struct_size = sizeof(NT_Value);
  return value;
}

struct NT_Value* NT_GetValueBooleanArrayForTesting(uint64_t last_change,
                                                   const int* arr,
                                                   size_t array_len,
                                                   int* struct_size) {
  struct NT_Value* value =
      static_cast<NT_Value*>(wpi::safe_calloc(1, sizeof(NT_Value)));
  value->type = NT_BOOLEAN_ARRAY;
  value->last_change = last_change;
  value->data.arr_boolean.arr = NT_AllocateBooleanArray(array_len);
  value->data.arr_boolean.size = array_len;
  std::memcpy(value->data.arr_boolean.arr, arr,
              value->data.arr_boolean.size * sizeof(int));
  *struct_size = sizeof(NT_Value);
  return value;
}

struct NT_Value* NT_GetValueDoubleArrayForTesting(uint64_t last_change,
                                                  const double* arr,
                                                  size_t array_len,
                                                  int* struct_size) {
  struct NT_Value* value =
      static_cast<NT_Value*>(wpi::safe_calloc(1, sizeof(NT_Value)));
  value->type = NT_BOOLEAN;
  value->last_change = last_change;
  value->data.arr_double.arr = NT_AllocateDoubleArray(array_len);
  value->data.arr_double.size = array_len;
  std::memcpy(value->data.arr_double.arr, arr,
              value->data.arr_double.size * sizeof(int));
  *struct_size = sizeof(NT_Value);
  return value;
}

struct NT_Value* NT_GetValueStringArrayForTesting(uint64_t last_change,
                                                  const struct WPI_String* arr,
                                                  size_t array_len,
                                                  int* struct_size) {
  struct NT_Value* value =
      static_cast<NT_Value*>(wpi::safe_calloc(1, sizeof(NT_Value)));
  value->type = NT_BOOLEAN;
  value->last_change = last_change;
  value->data.arr_string.arr = WPI_AllocateStringArray(array_len);
  value->data.arr_string.size = array_len;
  for (size_t i = 0; i < value->data.arr_string.size; ++i) {
    size_t len = arr[i].len;
    auto write = WPI_AllocateString(&value->data.arr_string.arr[i], len);
    std::memcpy(write, arr[i].str, len);
  }
  *struct_size = sizeof(NT_Value);
  return value;
}
// No need for free as one already exists in the main library

}  // extern "C"
