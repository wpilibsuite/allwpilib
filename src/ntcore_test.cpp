/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ntcore_test.h"

#include "Value_internal.h"

extern "C" {
struct NT_String* NT_GetStringForTesting(const char* string, int* struct_size) {
  struct NT_String* str =
      static_cast<NT_String*>(std::calloc(1, sizeof(NT_String)));
  nt::ConvertToC(llvm::StringRef(string), str);
  *struct_size = sizeof(NT_String);
  return str;
}

struct NT_EntryInfo* NT_GetEntryInfoForTesting(const char* name,
                                               enum NT_Type type,
                                               unsigned int flags,
                                               unsigned long long last_change,
                                               int* struct_size) {
  struct NT_EntryInfo* entry_info =
      static_cast<NT_EntryInfo*>(std::calloc(1, sizeof(NT_EntryInfo)));
  nt::ConvertToC(llvm::StringRef(name), &entry_info->name);
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
    unsigned long long last_update, unsigned int protocol_version,
    int* struct_size) {
  struct NT_ConnectionInfo* conn_info = static_cast<NT_ConnectionInfo*>(
      std::calloc(1, sizeof(NT_ConnectionInfo)));
  nt::ConvertToC(llvm::StringRef(remote_id), &conn_info->remote_id);
  nt::ConvertToC(llvm::StringRef(remote_ip), &conn_info->remote_ip);
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

struct NT_Value* NT_GetValueBooleanForTesting(unsigned long long last_change,
                                              int val, int* struct_size) {
  struct NT_Value* value =
      static_cast<NT_Value*>(std::calloc(1, sizeof(NT_Value)));
  value->type = NT_BOOLEAN;
  value->last_change = last_change;
  value->data.v_boolean = val;
  *struct_size = sizeof(NT_Value);
  return value;
}

struct NT_Value* NT_GetValueDoubleForTesting(unsigned long long last_change,
                                             double val, int* struct_size) {
  struct NT_Value* value =
      static_cast<NT_Value*>(std::calloc(1, sizeof(NT_Value)));
  value->type = NT_DOUBLE;
  value->last_change = last_change;
  value->data.v_double = val;
  *struct_size = sizeof(NT_Value);
  return value;
}

struct NT_Value* NT_GetValueStringForTesting(unsigned long long last_change,
                                             const char* str,
                                             int* struct_size) {
  struct NT_Value* value =
      static_cast<NT_Value*>(std::calloc(1, sizeof(NT_Value)));
  value->type = NT_STRING;
  value->last_change = last_change;
  nt::ConvertToC(llvm::StringRef(str), &value->data.v_string);
  *struct_size = sizeof(NT_Value);
  return value;
}

struct NT_Value* NT_GetValueRawForTesting(unsigned long long last_change,
                                          const char* raw, int raw_len,
                                          int* struct_size) {
  struct NT_Value* value =
      static_cast<NT_Value*>(std::calloc(1, sizeof(NT_Value)));
  value->type = NT_RAW;
  value->last_change = last_change;
  nt::ConvertToC(llvm::StringRef(raw, raw_len), &value->data.v_string);
  *struct_size = sizeof(NT_Value);
  return value;
}

struct NT_Value* NT_GetValueBooleanArrayForTesting(
    unsigned long long last_change, const int* arr, size_t array_len,
    int* struct_size) {
  struct NT_Value* value =
      static_cast<NT_Value*>(std::calloc(1, sizeof(NT_Value)));
  value->type = NT_BOOLEAN_ARRAY;
  value->last_change = last_change;
  value->data.arr_boolean.arr = NT_AllocateBooleanArray(array_len);
  value->data.arr_boolean.size = array_len;
  std::memcpy(value->data.arr_boolean.arr, arr,
              value->data.arr_boolean.size * sizeof(int));
  *struct_size = sizeof(NT_Value);
  return value;
}

struct NT_Value* NT_GetValueDoubleArrayForTesting(
    unsigned long long last_change, const double* arr, size_t array_len,
    int* struct_size) {
  struct NT_Value* value =
      static_cast<NT_Value*>(std::calloc(1, sizeof(NT_Value)));
  value->type = NT_BOOLEAN;
  value->last_change = last_change;
  value->data.arr_double.arr = NT_AllocateDoubleArray(array_len);
  value->data.arr_double.size = array_len;
  std::memcpy(value->data.arr_double.arr, arr,
              value->data.arr_double.size * sizeof(int));
  *struct_size = sizeof(NT_Value);
  return value;
}

struct NT_Value* NT_GetValueStringArrayForTesting(
    unsigned long long last_change, const struct NT_String* arr,
    size_t array_len, int* struct_size) {
  struct NT_Value* value =
      static_cast<NT_Value*>(std::calloc(1, sizeof(NT_Value)));
  value->type = NT_BOOLEAN;
  value->last_change = last_change;
  value->data.arr_string.arr = NT_AllocateStringArray(array_len);
  value->data.arr_string.size = array_len;
  for (size_t i = 0; i < value->data.arr_string.size; ++i) {
    size_t len = arr[i].len;
    value->data.arr_string.arr[i].len = len;
    value->data.arr_string.arr[i].str = (char*)std::malloc(len + 1);
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
  nt::ConvertToC(llvm::StringRef(copy_from->str, copy_from->len), copy_to);
}

struct NT_RpcParamDef* NT_GetRpcParamDefForTesting(const char* name,
                                                   const struct NT_Value* val,
                                                   int* struct_size) {
  struct NT_RpcParamDef* def =
      static_cast<NT_RpcParamDef*>(std::calloc(1, sizeof(NT_RpcParamDef)));
  nt::ConvertToC(llvm::StringRef(name), &def->name);
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
  struct NT_RpcResultDef* def =
      static_cast<NT_RpcResultDef*>(std::calloc(1, sizeof(NT_RpcResultDef)));
  nt::ConvertToC(llvm::StringRef(name), &def->name);
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
  struct NT_RpcDefinition* def =
      static_cast<NT_RpcDefinition*>(std::calloc(1, sizeof(NT_RpcDefinition)));
  def->version = version;
  nt::ConvertToC(llvm::StringRef(name), &def->name);
  def->num_params = num_params;
  def->params = static_cast<NT_RpcParamDef*>(
      std::malloc(num_params * sizeof(NT_RpcParamDef)));
  for (size_t i = 0; i < num_params; ++i) {
    CopyNtString(&params[i].name, &def->params[i].name);
    CopyNtValue(&params[i].def_value, &def->params[i].def_value);
  }
  def->num_results = num_results;
  def->results = static_cast<NT_RpcResultDef*>(
      std::malloc(num_results * sizeof(NT_RpcResultDef)));
  for (size_t i = 0; i < num_results; ++i) {
    CopyNtString(&results[i].name, &def->results[i].name);
    def->results[i].type = results[i].type;
  }
  *struct_size = sizeof(NT_RpcDefinition);
  return def;
}
// No need for free as one already exists in the main library

struct NT_RpcCallInfo* NT_GetRpcCallInfoForTesting(
    unsigned int rpc_id, unsigned int call_uid, const char* name,
    const char* params, size_t params_len, int* struct_size) {
  struct NT_RpcCallInfo* info =
      static_cast<NT_RpcCallInfo*>(std::calloc(1, sizeof(NT_RpcCallInfo)));
  info->rpc_id = rpc_id;
  info->call_uid = call_uid;
  nt::ConvertToC(llvm::StringRef(name), &info->name);
  nt::ConvertToC(llvm::StringRef(params, params_len), &info->params);
  *struct_size = sizeof(NT_RpcCallInfo);
  return info;
}
// No need for free as one already exists in the main library
}
