/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ntcore_test.h"

#include <wpi/memory.h>

#include "Value_internal.h"

extern "C" {
struct NT_String* NT_GetStringForTesting(const char* string, int* struct_size) {
  struct NT_String* str =
      static_cast<NT_String*>(wpi::CheckedCalloc(1, sizeof(NT_String)));
  nt::ConvertToC(wpi::StringRef(string), str);
  *struct_size = sizeof(NT_String);
  return str;
}

struct NT_EntryInfo* NT_GetEntryInfoForTesting(const char* name,
                                               enum NT_Type type,
                                               unsigned int flags,
                                               uint64_t last_change,
                                               int* struct_size) {
  struct NT_EntryInfo* entry_info =
      static_cast<NT_EntryInfo*>(wpi::CheckedCalloc(1, sizeof(NT_EntryInfo)));
  nt::ConvertToC(wpi::StringRef(name), &entry_info->name);
  entry_info->type = type;
  entry_info->flags = flags;
  entry_info->last_change = last_change;
  *struct_size = sizeof(NT_EntryInfo);
  return entry_info;
}

void NT_FreeEntryInfoForTesting(struct NT_EntryInfo* info) {
  std::free(info->name.str);
  std::free(info);
}

struct NT_ConnectionInfo* NT_GetConnectionInfoForTesting(
    const char* remote_id, const char* remote_ip, unsigned int remote_port,
    uint64_t last_update, unsigned int protocol_version, int* struct_size) {
  struct NT_ConnectionInfo* conn_info = static_cast<NT_ConnectionInfo*>(
      wpi::CheckedCalloc(1, sizeof(NT_ConnectionInfo)));
  nt::ConvertToC(wpi::StringRef(remote_id), &conn_info->remote_id);
  nt::ConvertToC(wpi::StringRef(remote_ip), &conn_info->remote_ip);
  conn_info->remote_port = remote_port;
  conn_info->last_update = last_update;
  conn_info->protocol_version = protocol_version;
  *struct_size = sizeof(NT_ConnectionInfo);
  return conn_info;
}

void NT_FreeConnectionInfoForTesting(struct NT_ConnectionInfo* info) {
  std::free(info->remote_id.str);
  std::free(info->remote_ip.str);
  std::free(info);
}

struct NT_Value* NT_GetValueBooleanForTesting(uint64_t last_change, int val,
                                              int* struct_size) {
  struct NT_Value* value =
      static_cast<NT_Value*>(wpi::CheckedCalloc(1, sizeof(NT_Value)));
  value->type = NT_BOOLEAN;
  value->last_change = last_change;
  value->data.v_boolean = val;
  *struct_size = sizeof(NT_Value);
  return value;
}

struct NT_Value* NT_GetValueDoubleForTesting(uint64_t last_change, double val,
                                             int* struct_size) {
  struct NT_Value* value =
      static_cast<NT_Value*>(wpi::CheckedCalloc(1, sizeof(NT_Value)));
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
      static_cast<NT_Value*>(wpi::CheckedCalloc(1, sizeof(NT_Value)));
  value->type = NT_STRING;
  value->last_change = last_change;
  nt::ConvertToC(wpi::StringRef(str), &value->data.v_string);
  *struct_size = sizeof(NT_Value);
  return value;
}

struct NT_Value* NT_GetValueRawForTesting(uint64_t last_change, const char* raw,
                                          int raw_len, int* struct_size) {
  struct NT_Value* value =
      static_cast<NT_Value*>(wpi::CheckedCalloc(1, sizeof(NT_Value)));
  value->type = NT_RAW;
  value->last_change = last_change;
  nt::ConvertToC(wpi::StringRef(raw, raw_len), &value->data.v_string);
  *struct_size = sizeof(NT_Value);
  return value;
}

struct NT_Value* NT_GetValueBooleanArrayForTesting(uint64_t last_change,
                                                   const int* arr,
                                                   size_t array_len,
                                                   int* struct_size) {
  struct NT_Value* value =
      static_cast<NT_Value*>(wpi::CheckedCalloc(1, sizeof(NT_Value)));
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
      static_cast<NT_Value*>(wpi::CheckedCalloc(1, sizeof(NT_Value)));
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
                                                  const struct NT_String* arr,
                                                  size_t array_len,
                                                  int* struct_size) {
  struct NT_Value* value =
      static_cast<NT_Value*>(wpi::CheckedCalloc(1, sizeof(NT_Value)));
  value->type = NT_BOOLEAN;
  value->last_change = last_change;
  value->data.arr_string.arr = NT_AllocateStringArray(array_len);
  value->data.arr_string.size = array_len;
  for (size_t i = 0; i < value->data.arr_string.size; ++i) {
    size_t len = arr[i].len;
    value->data.arr_string.arr[i].len = len;
    value->data.arr_string.arr[i].str =
        static_cast<char*>(wpi::CheckedMalloc(len + 1));
    std::memcpy(value->data.arr_string.arr[i].str, arr[i].str, len + 1);
  }
  *struct_size = sizeof(NT_Value);
  return value;
}
// No need for free as one already exists in the main library

static void CopyNtValue(const struct NT_Value* copy_from,
                        struct NT_Value* copy_to) {
  auto cpp_value = nt::ConvertFromC(*copy_from);
  nt::ConvertToC(*cpp_value, copy_to);
}

static void CopyNtString(const struct NT_String* copy_from,
                         struct NT_String* copy_to) {
  nt::ConvertToC(wpi::StringRef(copy_from->str, copy_from->len), copy_to);
}

struct NT_RpcParamDef* NT_GetRpcParamDefForTesting(const char* name,
                                                   const struct NT_Value* val,
                                                   int* struct_size) {
  struct NT_RpcParamDef* def = static_cast<NT_RpcParamDef*>(
      wpi::CheckedCalloc(1, sizeof(NT_RpcParamDef)));
  nt::ConvertToC(wpi::StringRef(name), &def->name);
  CopyNtValue(val, &def->def_value);
  *struct_size = sizeof(NT_RpcParamDef);
  return def;
}

void NT_FreeRpcParamDefForTesting(struct NT_RpcParamDef* def) {
  NT_DisposeValue(&def->def_value);
  NT_DisposeString(&def->name);
  std::free(def);
}

struct NT_RpcResultDef* NT_GetRpcResultsDefForTesting(const char* name,
                                                      enum NT_Type type,
                                                      int* struct_size) {
  struct NT_RpcResultDef* def = static_cast<NT_RpcResultDef*>(
      wpi::CheckedCalloc(1, sizeof(NT_RpcResultDef)));
  nt::ConvertToC(wpi::StringRef(name), &def->name);
  def->type = type;
  *struct_size = sizeof(NT_RpcResultDef);
  return def;
}

void NT_FreeRpcResultsDefForTesting(struct NT_RpcResultDef* def) {
  NT_DisposeString(&def->name);
  std::free(def);
}

struct NT_RpcDefinition* NT_GetRpcDefinitionForTesting(
    unsigned int version, const char* name, size_t num_params,
    const struct NT_RpcParamDef* params, size_t num_results,
    const struct NT_RpcResultDef* results, int* struct_size) {
  struct NT_RpcDefinition* def = static_cast<NT_RpcDefinition*>(
      wpi::CheckedCalloc(1, sizeof(NT_RpcDefinition)));
  def->version = version;
  nt::ConvertToC(wpi::StringRef(name), &def->name);
  def->num_params = num_params;
  def->params = static_cast<NT_RpcParamDef*>(
      wpi::CheckedMalloc(num_params * sizeof(NT_RpcParamDef)));
  for (size_t i = 0; i < num_params; ++i) {
    CopyNtString(&params[i].name, &def->params[i].name);
    CopyNtValue(&params[i].def_value, &def->params[i].def_value);
  }
  def->num_results = num_results;
  def->results = static_cast<NT_RpcResultDef*>(
      wpi::CheckedMalloc(num_results * sizeof(NT_RpcResultDef)));
  for (size_t i = 0; i < num_results; ++i) {
    CopyNtString(&results[i].name, &def->results[i].name);
    def->results[i].type = results[i].type;
  }
  *struct_size = sizeof(NT_RpcDefinition);
  return def;
}
// No need for free as one already exists in the main library

struct NT_RpcAnswer* NT_GetRpcAnswerForTesting(
    unsigned int rpc_id, unsigned int call_uid, const char* name,
    const char* params, size_t params_len, int* struct_size) {
  struct NT_RpcAnswer* info =
      static_cast<NT_RpcAnswer*>(wpi::CheckedCalloc(1, sizeof(NT_RpcAnswer)));
  info->entry = rpc_id;
  info->call = call_uid;
  nt::ConvertToC(wpi::StringRef(name), &info->name);
  nt::ConvertToC(wpi::StringRef(params, params_len), &info->params);
  *struct_size = sizeof(NT_RpcAnswer);
  return info;
}
// No need for free as one already exists in the main library
}  // extern "C"
