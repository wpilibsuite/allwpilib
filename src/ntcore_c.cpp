/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ntcore.h"

#include <cassert>
#include <cstdlib>
#include <fstream>

#include "Value_internal.h"

using namespace nt;

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
  return nullptr;
}

void NT_Flush(void) { nt::Flush(); }

/*
 * Callback Creation Functions
 */

unsigned int NT_AddEntryListener(const char *prefix, size_t prefix_len,
                                 void *data,
                                 NT_EntryListenerCallback callback) {
  return 0;
}
void NT_RemoveEntryListener(unsigned int entry_listener_uid) {
  nt::RemoveEntryListener(entry_listener_uid);
}
unsigned int NT_AddConnectionListener(void *data,
                                      NT_ConnectionListenerCallback callback) {
  return 0;
}
void NT_RemoveConnectionListener(unsigned int conn_listener_uid) {
  nt::RemoveConnectionListener(conn_listener_uid);
}

/*
 * Remote Procedure Call Functions
 */

unsigned int NT_CreateRpc(const char *name, size_t name_len,
                          const NT_RpcDefinition *def, void *data,
                          NT_RpcCallback callback);

void NT_DeleteRpc(unsigned int rpc_uid) {
  nt::DeleteRpc(rpc_uid);
}

unsigned int NT_CallRpc(const char *name, size_t name_len,
                        const NT_Value *params, size_t params_len) {
  std::vector<std::shared_ptr<Value>> params_v;
  params_v.reserve(params_len);
  for (size_t i=0; i<params_len; ++i)
    params_v.push_back(ConvertFromC(params[i]));
  return nt::CallRpc(StringRef(name, name_len), params_v);
}

NT_Value *NT_GetRpcResult(unsigned int result_uid, size_t *results_len) {
  auto results_v = nt::GetRpcResult(result_uid);
  *results_len = results_v.size();
  if (results_v.size() == 0) return nullptr;
  NT_Value *results =
      static_cast<NT_Value *>(std::malloc(results_v.size() * sizeof(NT_Value)));
  for (size_t i=0; i<results_v.size(); ++i)
    ConvertToC(*results_v[i], &results[i]);
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
  return nullptr;
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
      assert(0 && "unknown value type");
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
  unsigned int i;
  for (i = 0; i < count; i++)
    std::free(arr[i].remote_id.str);
  std::free(arr);
}
