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

unsigned int NT_CreateRpc(const char *name, size_t name_len,
                          const NT_RpcDefinition *def, void *data,
                          NT_RpcCallback callback) {
  return nt::CreateRpc(
      StringRef(name, name_len),
      ConvertFromC(*def),
      [=](unsigned int uid, StringRef name,
          ArrayRef<std::shared_ptr<Value>> params)
          -> std::vector<std::shared_ptr<Value>> {
        // convert params to NT_Value* array
        std::vector<const NT_Value*> params_c(params.size());
        for (size_t i = 0; i < params.size(); ++i)
          params_c[i] = &params[i]->value();

        size_t results_len;
        NT_Value** results_c = callback(uid, data, name.data(), name.size(),
                                        params_c.data(), params.size(),
                                        &results_len);

        // convert results to Value array
        std::vector<std::shared_ptr<Value>> results;
        results.reserve(results_len);
        for (size_t i = 0; i < results_len; ++i)
          results.push_back(ConvertFromC(*results_c[i]));

        // dispose the C version
        for (size_t i = 0; i < results_len; ++i) {
          NT_DisposeValue(results_c[i]);
          std::free(results_c[i]);
        }
        std::free(results_c);

        return results;
      });
}

void NT_DeleteRpc(unsigned int rpc_uid) {
  nt::DeleteRpc(rpc_uid);
}

unsigned int NT_CallRpc(const char *name, size_t name_len,
                        const NT_Value **params, size_t params_len) {
  // create input vector
  std::vector<std::shared_ptr<Value>> params_v;
  params_v.reserve(params_len);
  for (size_t i = 0; i < params_len; ++i)
    params_v.push_back(ConvertFromC(*params[i]));

  // make the call
  return nt::CallRpc(StringRef(name, name_len), params_v);
}

NT_Value **NT_GetRpcResult(unsigned int result_uid, size_t *results_len) {
  auto results_v = nt::GetRpcResult(result_uid);
  *results_len = results_v.size();
  if (results_v.size() == 0) return nullptr;

  // create array and copy into it
  NT_Value** results = static_cast<NT_Value**>(
      std::malloc(results_v.size() * sizeof(NT_Value*)));
  for (size_t i = 0; i < results_v.size(); ++i) {
    results[i] = static_cast<NT_Value*>(std::malloc(sizeof(NT_Value)));
    NT_InitValue(results[i]);
    ConvertToC(*results_v[i], results[i]);
  }
  return results;
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
