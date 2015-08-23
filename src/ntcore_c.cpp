/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ntcore.h"

#include <cassert>
#include <cstdlib>

#include "Value_internal.h"

using namespace nt;

// Conversion helpers

static void ConvertToC(llvm::StringRef in, char** out) {
  *out = static_cast<char*>(std::malloc(in.size() + 1));
  std::memmove(*out, in.data(), in.size());
  (*out)[in.size()] = '\0';
}

static void ConvertToC(const EntryInfo& in, NT_EntryInfo* out) {
  ConvertToC(in.name, &out->name);
  out->type = in.type;
  out->flags = in.flags;
  out->last_change = in.last_change;
}

static void ConvertToC(const ConnectionInfo& in, NT_ConnectionInfo* out) {
  ConvertToC(in.remote_id, &out->remote_id);
  ConvertToC(in.remote_name, &out->remote_name);
  out->remote_port = in.remote_port;
  out->last_update = in.last_update;
  out->protocol_version = in.protocol_version;
}

static void ConvertToC(const RpcParamDef& in, NT_RpcParamDef* out) {
  ConvertToC(in.name, &out->name);
  ConvertToC(*in.def_value, &out->def_value);
}

static void ConvertToC(const RpcResultDef& in, NT_RpcResultDef* out) {
  ConvertToC(in.name, &out->name);
  out->type = in.type;
}

static void ConvertToC(const RpcDefinition& in, NT_RpcDefinition* out) {
  out->version = in.version;
  ConvertToC(in.name, &out->name);

  out->num_params = in.params.size();
  out->params = static_cast<NT_RpcParamDef*>(
      std::malloc(in.params.size() * sizeof(NT_RpcParamDef)));
  for (size_t i = 0; i < in.params.size(); ++i)
    ConvertToC(in.params[i], &out->params[i]);

  out->num_results = in.results.size();
  out->results = static_cast<NT_RpcResultDef*>(
      std::malloc(in.results.size() * sizeof(NT_RpcResultDef)));
  for (size_t i = 0; i < in.results.size(); ++i)
    ConvertToC(in.results[i], &out->results[i]);
}

static void ConvertToC(const RpcCallInfo& in, NT_RpcCallInfo* out) {
  out->rpc_id = in.rpc_id;
  out->call_uid = in.call_uid;
  ConvertToC(in.name, &out->name);
  ConvertToC(in.params, &out->params);
}

static void DisposeConnectionInfo(NT_ConnectionInfo *info) {
  std::free(info->remote_id.str);
  std::free(info->remote_name);
}

static RpcParamDef ConvertFromC(const NT_RpcParamDef& in) {
  RpcParamDef out;
  out.name = ConvertFromC(in.name);
  out.def_value = ConvertFromC(in.def_value);
  return out;
}

static RpcResultDef ConvertFromC(const NT_RpcResultDef& in) {
  RpcResultDef out;
  out.name = ConvertFromC(in.name);
  out.type = in.type;
  return out;
}

static RpcDefinition ConvertFromC(const NT_RpcDefinition& in) {
  RpcDefinition out;
  out.version = in.version;
  out.name = ConvertFromC(in.name);

  out.params.reserve(in.num_params);
  for (size_t i = 0; i < in.num_params; ++i)
    out.params.push_back(ConvertFromC(in.params[i]));

  out.results.reserve(in.num_results);
  for (size_t i = 0; i < in.num_results; ++i)
    out.results.push_back(ConvertFromC(in.results[i]));

  return out;
}

/*
 * Table Functions
 */

void NT_GetEntryValue(const char *name, unsigned int name_len,
                      struct NT_Value *value) {
  NT_InitValue(value);
  auto v = nt::GetEntryValue(StringRef(name, name_len));
  if (!v) return;
  ConvertToC(*v, value);
}

int NT_SetEntryValue(const char *name, unsigned int name_len,
                     const struct NT_Value *value) {
  return nt::SetEntryValue(StringRef(name, name_len), ConvertFromC(*value));
}

void NT_SetEntryTypeValue(const char *name, unsigned int name_len,
                          const struct NT_Value *value) {
  nt::SetEntryTypeValue(StringRef(name, name_len), ConvertFromC(*value));
}

void NT_SetEntryFlags(const char *name, size_t name_len, unsigned int flags) {
  nt::SetEntryFlags(StringRef(name, name_len), flags);
}

unsigned int NT_GetEntryFlags(const char *name, size_t name_len) {
  return nt::GetEntryFlags(StringRef(name, name_len));
}

void NT_DeleteEntry(const char *name, unsigned int name_len) {
  nt::DeleteEntry(StringRef(name, name_len));
}

void NT_DeleteAllEntries(void) { nt::DeleteAllEntries(); }

struct NT_EntryInfo *NT_GetEntryInfo(const char *prefix,
                                     unsigned int prefix_len, int types,
                                     unsigned int *count) {
  auto info_v = nt::GetEntryInfo(StringRef(prefix, prefix_len), types);
  *count = info_v.size();
  if (info_v.size() == 0) return nullptr;

  // create array and copy into it
  NT_EntryInfo* info = static_cast<NT_EntryInfo*>(
      std::malloc(info_v.size() * sizeof(NT_EntryInfo)));
  for (size_t i = 0; i < info_v.size(); ++i) ConvertToC(info_v[i], &info[i]);
  return info;
}

void NT_Flush(void) { nt::Flush(); }

/*
 * Callback Creation Functions
 */

unsigned int NT_AddEntryListener(const char *prefix, size_t prefix_len,
                                 void *data,
                                 NT_EntryListenerCallback callback,
                                 int immediate_notify) {
  return nt::AddEntryListener(
      StringRef(prefix, prefix_len),
      [=](unsigned int uid, StringRef name, std::shared_ptr<Value> value,
          bool is_new) {
        callback(uid, data, name.data(), name.size(), &value->value(), is_new);
      },
      immediate_notify != 0);
}

void NT_RemoveEntryListener(unsigned int entry_listener_uid) {
  nt::RemoveEntryListener(entry_listener_uid);
}

unsigned int NT_AddConnectionListener(void *data,
                                      NT_ConnectionListenerCallback callback) {
  return nt::AddConnectionListener(
      [=](unsigned int uid, int connected, const ConnectionInfo &conn) {
        NT_ConnectionInfo conn_c;
        ConvertToC(conn, &conn_c);
        callback(uid, data, connected, &conn_c);
        DisposeConnectionInfo(&conn_c);
      });
}

void NT_RemoveConnectionListener(unsigned int conn_listener_uid) {
  nt::RemoveConnectionListener(conn_listener_uid);
}

/*
 * Remote Procedure Call Functions
 */

void NT_CreateRpc(const char *name, size_t name_len, const char *def,
                  size_t def_len, void *data, NT_RpcCallback callback) {
  nt::CreateRpc(
      StringRef(name, name_len), StringRef(def, def_len),
      [=](StringRef name, StringRef params) -> std::string {
        size_t results_len;
        char* results_c = callback(data, name.data(), name.size(),
                                   params.data(), params.size(), &results_len);
        std::string results(results_c, results_len);
        std::free(results_c);
        return results;
      });
}

void NT_CreatePolledRpc(const char *name, size_t name_len, const char *def,
                        size_t def_len) {
  nt::CreatePolledRpc(StringRef(name, name_len), StringRef(def, def_len));
}

int NT_PollRpc(int blocking, NT_RpcCallInfo* call_info) {
  RpcCallInfo call_info_cpp;
  if (!nt::PollRpc(blocking, &call_info_cpp))
    return 0;
  ConvertToC(call_info_cpp, call_info);
  return 1;
}

void NT_PostRpcResponse(unsigned int rpc_id, unsigned int call_uid,
                        const char *result, size_t result_len) {
  nt::PostRpcResponse(rpc_id, call_uid, StringRef(result, result_len));
}

unsigned int NT_CallRpc(const char *name, size_t name_len,
                        const char *params, size_t params_len) {
  return nt::CallRpc(StringRef(name, name_len), StringRef(params, params_len));
}

char *NT_GetRpcResult(int blocking, unsigned int call_uid, size_t *result_len) {
  std::string result;
  if (!nt::GetRpcResult(blocking, call_uid, &result)) return nullptr;

  // convert result
  *result_len = result.size();
  char *result_cstr;
  ConvertToC(result, &result_cstr);
  return result_cstr;
}

char *NT_PackRpcDefinition(const NT_RpcDefinition *def, size_t *packed_len) {
  auto packed = nt::PackRpcDefinition(ConvertFromC(*def));

  // convert result
  *packed_len = packed.size();
  char *packed_cstr;
  ConvertToC(packed, &packed_cstr);
  return packed_cstr;
}

int NT_UnpackRpcDefinition(const char *packed, size_t packed_len,
                           NT_RpcDefinition *def) {
  nt::RpcDefinition def_v;
  if (!nt::UnpackRpcDefinition(StringRef(packed, packed_len), &def_v))
      return 0;

  // convert result
  ConvertToC(def_v, def);
  return 1;
}

char *NT_PackRpcValues(const NT_Value **values, size_t values_len,
                       size_t *packed_len) {
  // create input vector
  std::vector<std::shared_ptr<Value>> values_v;
  values_v.reserve(values_len);
  for (size_t i = 0; i < values_len; ++i)
    values_v.push_back(ConvertFromC(*values[i]));

  // make the call
  auto packed = nt::PackRpcValues(values_v);

  // convert result
  *packed_len = packed.size();
  char *packed_cstr;
  ConvertToC(packed, &packed_cstr);
  return packed_cstr;
}

NT_Value **NT_UnpackRpcValues(const char *packed, size_t packed_len,
                              const NT_Type *types, size_t types_len) {
  auto values_v = nt::UnpackRpcValues(StringRef(packed, packed_len),
                                      ArrayRef<NT_Type>(types, types_len));
  if (values_v.size() == 0) return nullptr;

  // create array and copy into it
  NT_Value** values = static_cast<NT_Value**>(
      std::malloc(values_v.size() * sizeof(NT_Value*)));
  for (size_t i = 0; i < values_v.size(); ++i) {
    values[i] = static_cast<NT_Value*>(std::malloc(sizeof(NT_Value)));
    ConvertToC(*values_v[i], values[i]);
  }
  return values;
}

/*
 * Client/Server Functions
 */

void NT_SetNetworkIdentity(const char *name, size_t name_len) {
  nt::SetNetworkIdentity(StringRef(name, name_len));
}

void NT_StartServer(const char *persist_filename, const char *listen_address,
                    unsigned int port) {
  nt::StartServer(persist_filename, listen_address, port);
}

void NT_StopServer(void) { nt::StopServer(); }

void NT_StartClient(const char *server_name, unsigned int port) {
  nt::StartClient(server_name, port);
}

void NT_StopClient(void) {
  nt::StopClient();
}

void NT_SetUpdateRate(double interval) {
  nt::SetUpdateRate(interval);
}

struct NT_ConnectionInfo *NT_GetConnections(size_t *count) {
  auto conn_v = nt::GetConnections();
  *count = conn_v.size();
  if (conn_v.size() == 0) return nullptr;

  // create array and copy into it
  NT_ConnectionInfo *conn = static_cast<NT_ConnectionInfo *>(
      std::malloc(conn_v.size() * sizeof(NT_ConnectionInfo)));
  for (size_t i = 0; i < conn_v.size(); ++i) ConvertToC(conn_v[i], &conn[i]);
  return conn;
}

/*
 * Persistent Functions
 */

const char *NT_SavePersistent(const char *filename) {
  return nt::SavePersistent(filename);
}

const char *NT_LoadPersistent(const char *filename,
                              void (*warn)(size_t line, const char *msg)) {
  return nt::LoadPersistent(filename, warn);
}

/*
 * Utility Functions
 */

void NT_SetLogger(NT_LogFunc func, unsigned int min_level) {
  nt::SetLogger(func, min_level);
}

void NT_DisposeValue(NT_Value *value) {
  switch (value->type) {
    case NT_UNASSIGNED:
    case NT_BOOLEAN:
    case NT_DOUBLE:
      break;
    case NT_STRING:
    case NT_RAW:
    case NT_RPC:
      std::free(value->data.v_string.str);
      break;
    case NT_BOOLEAN_ARRAY:
      std::free(value->data.arr_boolean.arr);
      break;
    case NT_DOUBLE_ARRAY:
      std::free(value->data.arr_double.arr);
      break;
    case NT_STRING_ARRAY: {
      for (size_t i = 0; i < value->data.arr_string.size; i++)
        std::free(value->data.arr_string.arr[i].str);
      std::free(value->data.arr_string.arr);
      break;
    }
    default:
      assert(false && "unknown value type");
  }
  value->type = NT_UNASSIGNED;
  value->last_change = 0;
}

void NT_InitValue(NT_Value *value) {
  value->type = NT_UNASSIGNED;
  value->last_change = 0;
}

void NT_DisposeString(NT_String *str) {
  std::free(str->str);
  str->str = nullptr;
  str->len = 0;
}

void NT_InitString(NT_String *str) {
  str->str = nullptr;
  str->len = 0;
}

void NT_DisposeConnectionInfoArray(NT_ConnectionInfo *arr, size_t count) {
  for (size_t i = 0; i < count; i++) DisposeConnectionInfo(&arr[i]);
  std::free(arr);
}

void NT_DisposeRpcDefinition(NT_RpcDefinition *def) {
  NT_DisposeString(&def->name);

  for (size_t i = 0; i < def->num_params; ++i) {
    NT_DisposeString(&def->params[i].name);
    NT_DisposeValue(&def->params[i].def_value);
  }
  std::free(def->params);
  def->params = nullptr;
  def->num_params = 0;

  for (size_t i = 0; i < def->num_results; ++i)
    NT_DisposeString(&def->results[i].name);
  std::free(def->results);
  def->results = nullptr;
  def->num_results = 0;
}

void NT_DisposeRpcCallInfo(NT_RpcCallInfo *call_info) {
  NT_DisposeString(&call_info->name);
  NT_DisposeString(&call_info->params);
}
