/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <stdint.h>

#include <cassert>
#include <cstdlib>

#include <wpi/memory.h>
#include <wpi/timestamp.h>

#include "Value_internal.h"
#include "ntcore.h"

using namespace nt;

// Conversion helpers

static void ConvertToC(wpi::StringRef in, char** out) {
  *out = static_cast<char*>(wpi::CheckedMalloc(in.size() + 1));
  std::memmove(*out, in.data(), in.size());
  (*out)[in.size()] = '\0';
}

static void ConvertToC(const EntryInfo& in, NT_EntryInfo* out) {
  out->entry = in.entry;
  ConvertToC(in.name, &out->name);
  out->type = in.type;
  out->flags = in.flags;
  out->last_change = in.last_change;
}

static void ConvertToC(const ConnectionInfo& in, NT_ConnectionInfo* out) {
  ConvertToC(in.remote_id, &out->remote_id);
  ConvertToC(in.remote_ip, &out->remote_ip);
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
      wpi::CheckedMalloc(in.params.size() * sizeof(NT_RpcParamDef)));
  for (size_t i = 0; i < in.params.size(); ++i)
    ConvertToC(in.params[i], &out->params[i]);

  out->num_results = in.results.size();
  out->results = static_cast<NT_RpcResultDef*>(
      wpi::CheckedMalloc(in.results.size() * sizeof(NT_RpcResultDef)));
  for (size_t i = 0; i < in.results.size(); ++i)
    ConvertToC(in.results[i], &out->results[i]);
}

static void ConvertToC(const RpcAnswer& in, NT_RpcAnswer* out) {
  out->entry = in.entry;
  out->call = in.call;
  ConvertToC(in.name, &out->name);
  ConvertToC(in.params, &out->params);
  ConvertToC(in.conn, &out->conn);
}

static void ConvertToC(const EntryNotification& in, NT_EntryNotification* out) {
  out->listener = in.listener;
  out->entry = in.entry;
  ConvertToC(in.name, &out->name);
  ConvertToC(*in.value, &out->value);
  out->flags = in.flags;
}

static void ConvertToC(const ConnectionNotification& in,
                       NT_ConnectionNotification* out) {
  out->listener = in.listener;
  out->connected = in.connected;
  ConvertToC(in.conn, &out->conn);
}

static void ConvertToC(const LogMessage& in, NT_LogMessage* out) {
  out->logger = in.logger;
  out->level = in.level;
  out->filename = in.filename;
  out->line = in.line;
  ConvertToC(in.message, &out->message);
}

template <typename O, typename I>
static O* ConvertToC(const std::vector<I>& in, size_t* out_len) {
  if (!out_len) return nullptr;
  *out_len = in.size();
  if (in.empty()) return nullptr;
  O* out = static_cast<O*>(wpi::CheckedMalloc(sizeof(O) * in.size()));
  for (size_t i = 0; i < in.size(); ++i) ConvertToC(in[i], &out[i]);
  return out;
}

static void DisposeConnectionInfo(NT_ConnectionInfo* info) {
  std::free(info->remote_id.str);
  std::free(info->remote_ip.str);
}

static void DisposeEntryInfo(NT_EntryInfo* info) { std::free(info->name.str); }

static void DisposeEntryNotification(NT_EntryNotification* info) {
  std::free(info->name.str);
  NT_DisposeValue(&info->value);
}

static void DisposeConnectionNotification(NT_ConnectionNotification* info) {
  DisposeConnectionInfo(&info->conn);
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

extern "C" {

/*
 * Instance Functions
 */

NT_Inst NT_GetDefaultInstance(void) { return nt::GetDefaultInstance(); }

NT_Inst NT_CreateInstance(void) { return nt::CreateInstance(); }

void NT_DestroyInstance(NT_Inst inst) { return nt::DestroyInstance(inst); }

NT_Inst NT_GetInstanceFromHandle(NT_Handle handle) {
  return nt::GetInstanceFromHandle(handle);
}

/*
 * Table Functions
 */

NT_Entry NT_GetEntry(NT_Inst inst, const char* name, size_t name_len) {
  return nt::GetEntry(inst, StringRef(name, name_len));
}

NT_Entry* NT_GetEntries(NT_Inst inst, const char* prefix, size_t prefix_len,
                        unsigned int types, size_t* count) {
  auto info_v = nt::GetEntries(inst, StringRef(prefix, prefix_len), types);
  *count = info_v.size();
  if (info_v.size() == 0) return nullptr;

  // create array and copy into it
  NT_Entry* info = static_cast<NT_Entry*>(
      wpi::CheckedMalloc(info_v.size() * sizeof(NT_Entry)));
  std::memcpy(info, info_v.data(), info_v.size() * sizeof(NT_Entry));
  return info;
}

char* NT_GetEntryName(NT_Entry entry, size_t* name_len) {
  struct NT_String v_name;
  nt::ConvertToC(nt::GetEntryName(entry), &v_name);
  *name_len = v_name.len;
  return v_name.str;
}

enum NT_Type NT_GetEntryType(NT_Entry entry) { return nt::GetEntryType(entry); }

uint64_t NT_GetEntryLastChange(NT_Entry entry) {
  return nt::GetEntryLastChange(entry);
}

void NT_GetEntryValue(NT_Entry entry, struct NT_Value* value) {
  NT_InitValue(value);
  auto v = nt::GetEntryValue(entry);
  if (!v) return;
  ConvertToC(*v, value);
}

int NT_SetDefaultEntryValue(NT_Entry entry,
                            const struct NT_Value* default_value) {
  return nt::SetDefaultEntryValue(entry, ConvertFromC(*default_value));
}

int NT_SetEntryValue(NT_Entry entry, const struct NT_Value* value) {
  return nt::SetEntryValue(entry, ConvertFromC(*value));
}

void NT_SetEntryTypeValue(NT_Entry entry, const struct NT_Value* value) {
  nt::SetEntryTypeValue(entry, ConvertFromC(*value));
}

void NT_SetEntryFlags(NT_Entry entry, unsigned int flags) {
  nt::SetEntryFlags(entry, flags);
}

unsigned int NT_GetEntryFlags(NT_Entry entry) {
  return nt::GetEntryFlags(entry);
}

void NT_DeleteEntry(NT_Entry entry) { nt::DeleteEntry(entry); }

void NT_DeleteAllEntries(NT_Inst inst) { nt::DeleteAllEntries(inst); }

struct NT_EntryInfo* NT_GetEntryInfo(NT_Inst inst, const char* prefix,
                                     size_t prefix_len, unsigned int types,
                                     size_t* count) {
  auto info_v = nt::GetEntryInfo(inst, StringRef(prefix, prefix_len), types);
  return ConvertToC<NT_EntryInfo>(info_v, count);
}

NT_Bool NT_GetEntryInfoHandle(NT_Entry entry, struct NT_EntryInfo* info) {
  auto info_v = nt::GetEntryInfo(entry);
  if (info_v.name.empty()) return false;
  ConvertToC(info_v, info);
  return true;
}

/*
 * Callback Creation Functions
 */

NT_EntryListener NT_AddEntryListener(NT_Inst inst, const char* prefix,
                                     size_t prefix_len, void* data,
                                     NT_EntryListenerCallback callback,
                                     unsigned int flags) {
  return nt::AddEntryListener(inst, StringRef(prefix, prefix_len),
                              [=](const EntryNotification& event) {
                                NT_EntryNotification c_event;
                                ConvertToC(event, &c_event);
                                callback(data, &c_event);
                                DisposeEntryNotification(&c_event);
                              },
                              flags);
}

NT_EntryListener NT_AddEntryListenerSingle(NT_Entry entry, void* data,
                                           NT_EntryListenerCallback callback,
                                           unsigned int flags) {
  return nt::AddEntryListener(entry,
                              [=](const EntryNotification& event) {
                                NT_EntryNotification c_event;
                                ConvertToC(event, &c_event);
                                callback(data, &c_event);
                                DisposeEntryNotification(&c_event);
                              },
                              flags);
}

NT_EntryListenerPoller NT_CreateEntryListenerPoller(NT_Inst inst) {
  return nt::CreateEntryListenerPoller(inst);
}

void NT_DestroyEntryListenerPoller(NT_EntryListenerPoller poller) {
  nt::DestroyEntryListenerPoller(poller);
}

NT_EntryListener NT_AddPolledEntryListener(NT_EntryListenerPoller poller,
                                           const char* prefix,
                                           size_t prefix_len,
                                           unsigned int flags) {
  return nt::AddPolledEntryListener(poller, StringRef(prefix, prefix_len),
                                    flags);
}

NT_EntryListener NT_AddPolledEntryListenerSingle(NT_EntryListenerPoller poller,
                                                 NT_Entry entry,
                                                 unsigned int flags) {
  return nt::AddPolledEntryListener(poller, entry, flags);
}

struct NT_EntryNotification* NT_PollEntryListener(NT_EntryListenerPoller poller,
                                                  size_t* len) {
  auto arr_cpp = nt::PollEntryListener(poller);
  return ConvertToC<NT_EntryNotification>(arr_cpp, len);
}

struct NT_EntryNotification* NT_PollEntryListenerTimeout(
    NT_EntryListenerPoller poller, size_t* len, double timeout,
    NT_Bool* timed_out) {
  bool cpp_timed_out = false;
  auto arr_cpp = nt::PollEntryListener(poller, timeout, &cpp_timed_out);
  *timed_out = cpp_timed_out;
  return ConvertToC<NT_EntryNotification>(arr_cpp, len);
}

void NT_CancelPollEntryListener(NT_EntryListenerPoller poller) {
  nt::CancelPollEntryListener(poller);
}

void NT_RemoveEntryListener(NT_EntryListener entry_listener) {
  nt::RemoveEntryListener(entry_listener);
}

NT_Bool NT_WaitForEntryListenerQueue(NT_Inst inst, double timeout) {
  return nt::WaitForEntryListenerQueue(inst, timeout);
}

NT_ConnectionListener NT_AddConnectionListener(
    NT_Inst inst, void* data, NT_ConnectionListenerCallback callback,
    NT_Bool immediate_notify) {
  return nt::AddConnectionListener(inst,
                                   [=](const ConnectionNotification& event) {
                                     NT_ConnectionNotification event_c;
                                     ConvertToC(event, &event_c);
                                     callback(data, &event_c);
                                     DisposeConnectionNotification(&event_c);
                                   },
                                   immediate_notify != 0);
}

NT_ConnectionListenerPoller NT_CreateConnectionListenerPoller(NT_Inst inst) {
  return nt::CreateConnectionListenerPoller(inst);
}

void NT_DestroyConnectionListenerPoller(NT_ConnectionListenerPoller poller) {
  nt::DestroyConnectionListenerPoller(poller);
}

NT_ConnectionListener NT_AddPolledConnectionListener(
    NT_ConnectionListenerPoller poller, NT_Bool immediate_notify) {
  return nt::AddPolledConnectionListener(poller, immediate_notify);
}

struct NT_ConnectionNotification* NT_PollConnectionListener(
    NT_ConnectionListenerPoller poller, size_t* len) {
  auto arr_cpp = nt::PollConnectionListener(poller);
  return ConvertToC<NT_ConnectionNotification>(arr_cpp, len);
}

struct NT_ConnectionNotification* NT_PollConnectionListenerTimeout(
    NT_ConnectionListenerPoller poller, size_t* len, double timeout,
    NT_Bool* timed_out) {
  bool cpp_timed_out = false;
  auto arr_cpp = nt::PollConnectionListener(poller, timeout, &cpp_timed_out);
  *timed_out = cpp_timed_out;
  return ConvertToC<NT_ConnectionNotification>(arr_cpp, len);
}

void NT_CancelPollConnectionListener(NT_ConnectionListenerPoller poller) {
  nt::CancelPollConnectionListener(poller);
}

void NT_RemoveConnectionListener(NT_ConnectionListener conn_listener) {
  nt::RemoveConnectionListener(conn_listener);
}

NT_Bool NT_WaitForConnectionListenerQueue(NT_Inst inst, double timeout) {
  return nt::WaitForConnectionListenerQueue(inst, timeout);
}

/*
 * Remote Procedure Call Functions
 */

void NT_CreateRpc(NT_Entry entry, const char* def, size_t def_len, void* data,
                  NT_RpcCallback callback) {
  nt::CreateRpc(entry, StringRef(def, def_len), [=](const RpcAnswer& answer) {
    NT_RpcAnswer answer_c;
    ConvertToC(answer, &answer_c);
    callback(data, &answer_c);
    NT_DisposeRpcAnswer(&answer_c);
  });
}

NT_RpcCallPoller NT_CreateRpcCallPoller(NT_Inst inst) {
  return nt::CreateRpcCallPoller(inst);
}

void NT_DestroyRpcCallPoller(NT_RpcCallPoller poller) {
  nt::DestroyRpcCallPoller(poller);
}

void NT_CreatePolledRpc(NT_Entry entry, const char* def, size_t def_len,
                        NT_RpcCallPoller poller) {
  nt::CreatePolledRpc(entry, StringRef(def, def_len), poller);
}

NT_RpcAnswer* NT_PollRpc(NT_RpcCallPoller poller, size_t* len) {
  auto arr_cpp = nt::PollRpc(poller);
  return ConvertToC<NT_RpcAnswer>(arr_cpp, len);
}

NT_RpcAnswer* NT_PollRpcTimeout(NT_RpcCallPoller poller, size_t* len,
                                double timeout, NT_Bool* timed_out) {
  bool cpp_timed_out = false;
  auto arr_cpp = nt::PollRpc(poller, timeout, &cpp_timed_out);
  *timed_out = cpp_timed_out;
  return ConvertToC<NT_RpcAnswer>(arr_cpp, len);
}

void NT_CancelPollRpc(NT_RpcCallPoller poller) { nt::CancelPollRpc(poller); }

NT_Bool NT_WaitForRpcCallQueue(NT_Inst inst, double timeout) {
  return nt::WaitForRpcCallQueue(inst, timeout);
}

NT_Bool NT_PostRpcResponse(NT_Entry entry, NT_RpcCall call, const char* result,
                           size_t result_len) {
  return nt::PostRpcResponse(entry, call, StringRef(result, result_len));
}

NT_RpcCall NT_CallRpc(NT_Entry entry, const char* params, size_t params_len) {
  return nt::CallRpc(entry, StringRef(params, params_len));
}

char* NT_GetRpcResult(NT_Entry entry, NT_RpcCall call, size_t* result_len) {
  std::string result;
  if (!nt::GetRpcResult(entry, call, &result)) return nullptr;

  // convert result
  *result_len = result.size();
  char* result_cstr;
  ConvertToC(result, &result_cstr);
  return result_cstr;
}

char* NT_GetRpcResultTimeout(NT_Entry entry, NT_RpcCall call,
                             size_t* result_len, double timeout,
                             NT_Bool* timed_out) {
  std::string result;
  bool cpp_timed_out = false;
  if (!nt::GetRpcResult(entry, call, &result, timeout, &cpp_timed_out)) {
    *timed_out = cpp_timed_out;
    return nullptr;
  }

  *timed_out = cpp_timed_out;
  // convert result
  *result_len = result.size();
  char* result_cstr;
  ConvertToC(result, &result_cstr);
  return result_cstr;
}

void NT_CancelRpcResult(NT_Entry entry, NT_RpcCall call) {
  nt::CancelRpcResult(entry, call);
}

char* NT_PackRpcDefinition(const NT_RpcDefinition* def, size_t* packed_len) {
  auto packed = nt::PackRpcDefinition(ConvertFromC(*def));

  // convert result
  *packed_len = packed.size();
  char* packed_cstr;
  ConvertToC(packed, &packed_cstr);
  return packed_cstr;
}

NT_Bool NT_UnpackRpcDefinition(const char* packed, size_t packed_len,
                               NT_RpcDefinition* def) {
  nt::RpcDefinition def_v;
  if (!nt::UnpackRpcDefinition(StringRef(packed, packed_len), &def_v)) return 0;

  // convert result
  ConvertToC(def_v, def);
  return 1;
}

char* NT_PackRpcValues(const NT_Value** values, size_t values_len,
                       size_t* packed_len) {
  // create input vector
  std::vector<std::shared_ptr<Value>> values_v;
  values_v.reserve(values_len);
  for (size_t i = 0; i < values_len; ++i)
    values_v.push_back(ConvertFromC(*values[i]));

  // make the call
  auto packed = nt::PackRpcValues(values_v);

  // convert result
  *packed_len = packed.size();
  char* packed_cstr;
  ConvertToC(packed, &packed_cstr);
  return packed_cstr;
}

NT_Value** NT_UnpackRpcValues(const char* packed, size_t packed_len,
                              const NT_Type* types, size_t types_len) {
  auto values_v = nt::UnpackRpcValues(StringRef(packed, packed_len),
                                      ArrayRef<NT_Type>(types, types_len));
  if (values_v.size() == 0) return nullptr;

  // create array and copy into it
  NT_Value** values = static_cast<NT_Value**>(
      wpi::CheckedMalloc(values_v.size() * sizeof(NT_Value*)));
  for (size_t i = 0; i < values_v.size(); ++i) {
    values[i] = static_cast<NT_Value*>(wpi::CheckedMalloc(sizeof(NT_Value)));
    ConvertToC(*values_v[i], values[i]);
  }
  return values;
}

/*
 * Client/Server Functions
 */

void NT_SetNetworkIdentity(NT_Inst inst, const char* name, size_t name_len) {
  nt::SetNetworkIdentity(inst, StringRef(name, name_len));
}

unsigned int NT_GetNetworkMode(NT_Inst inst) {
  return nt::GetNetworkMode(inst);
}

void NT_StartServer(NT_Inst inst, const char* persist_filename,
                    const char* listen_address, unsigned int port) {
  nt::StartServer(inst, persist_filename, listen_address, port);
}

void NT_StopServer(NT_Inst inst) { nt::StopServer(inst); }

void NT_StartClientNone(NT_Inst inst) { nt::StartClient(inst); }

void NT_StartClient(NT_Inst inst, const char* server_name, unsigned int port) {
  nt::StartClient(inst, server_name, port);
}

void NT_StartClientMulti(NT_Inst inst, size_t count, const char** server_names,
                         const unsigned int* ports) {
  std::vector<std::pair<StringRef, unsigned int>> servers;
  servers.reserve(count);
  for (size_t i = 0; i < count; ++i)
    servers.emplace_back(std::make_pair(server_names[i], ports[i]));
  nt::StartClient(inst, servers);
}

void NT_StartClientTeam(NT_Inst inst, unsigned int team, unsigned int port) {
  nt::StartClientTeam(inst, team, port);
}

void NT_StopClient(NT_Inst inst) { nt::StopClient(inst); }

void NT_SetServer(NT_Inst inst, const char* server_name, unsigned int port) {
  nt::SetServer(inst, server_name, port);
}

void NT_SetServerMulti(NT_Inst inst, size_t count, const char** server_names,
                       const unsigned int* ports) {
  std::vector<std::pair<StringRef, unsigned int>> servers;
  servers.reserve(count);
  for (size_t i = 0; i < count; ++i)
    servers.emplace_back(std::make_pair(server_names[i], ports[i]));
  nt::SetServer(inst, servers);
}

void NT_SetServerTeam(NT_Inst inst, unsigned int team, unsigned int port) {
  nt::SetServerTeam(inst, team, port);
}

void NT_StartDSClient(NT_Inst inst, unsigned int port) {
  nt::StartDSClient(inst, port);
}

void NT_StopDSClient(NT_Inst inst) { nt::StopDSClient(inst); }

void NT_SetUpdateRate(NT_Inst inst, double interval) {
  nt::SetUpdateRate(inst, interval);
}

void NT_Flush(NT_Inst inst) { nt::Flush(inst); }

NT_Bool NT_IsConnected(NT_Inst inst) { return nt::IsConnected(inst); }

struct NT_ConnectionInfo* NT_GetConnections(NT_Inst inst, size_t* count) {
  auto conn_v = nt::GetConnections(inst);
  return ConvertToC<NT_ConnectionInfo>(conn_v, count);
}

/*
 * File Save/Load Functions
 */

const char* NT_SavePersistent(NT_Inst inst, const char* filename) {
  return nt::SavePersistent(inst, filename);
}

const char* NT_LoadPersistent(NT_Inst inst, const char* filename,
                              void (*warn)(size_t line, const char* msg)) {
  return nt::LoadPersistent(inst, filename, warn);
}

const char* NT_SaveEntries(NT_Inst inst, const char* filename,
                           const char* prefix, size_t prefix_len) {
  return nt::SaveEntries(inst, filename, StringRef(prefix, prefix_len));
}

const char* NT_LoadEntries(NT_Inst inst, const char* filename,
                           const char* prefix, size_t prefix_len,
                           void (*warn)(size_t line, const char* msg)) {
  return nt::LoadEntries(inst, filename, StringRef(prefix, prefix_len), warn);
}

/*
 * Utility Functions
 */

uint64_t NT_Now(void) { return wpi::Now(); }

NT_Logger NT_AddLogger(NT_Inst inst, void* data, NT_LogFunc func,
                       unsigned int min_level, unsigned int max_level) {
  return nt::AddLogger(inst,
                       [=](const LogMessage& msg) {
                         NT_LogMessage msg_c;
                         ConvertToC(msg, &msg_c);
                         func(data, &msg_c);
                         NT_DisposeLogMessage(&msg_c);
                       },
                       min_level, max_level);
}

NT_LoggerPoller NT_CreateLoggerPoller(NT_Inst inst) {
  return nt::CreateLoggerPoller(inst);
}

void NT_DestroyLoggerPoller(NT_LoggerPoller poller) {
  nt::DestroyLoggerPoller(poller);
}

NT_Logger NT_AddPolledLogger(NT_LoggerPoller poller, unsigned int min_level,
                             unsigned int max_level) {
  return nt::AddPolledLogger(poller, min_level, max_level);
}

struct NT_LogMessage* NT_PollLogger(NT_LoggerPoller poller, size_t* len) {
  auto arr_cpp = nt::PollLogger(poller);
  return ConvertToC<NT_LogMessage>(arr_cpp, len);
}

struct NT_LogMessage* NT_PollLoggerTimeout(NT_LoggerPoller poller, size_t* len,
                                           double timeout, NT_Bool* timed_out) {
  bool cpp_timed_out = false;
  auto arr_cpp = nt::PollLogger(poller, timeout, &cpp_timed_out);
  *timed_out = cpp_timed_out;
  return ConvertToC<NT_LogMessage>(arr_cpp, len);
}

void NT_CancelPollLogger(NT_LoggerPoller poller) {
  nt::CancelPollLogger(poller);
}

void NT_RemoveLogger(NT_Logger logger) { nt::RemoveLogger(logger); }

NT_Bool NT_WaitForLoggerQueue(NT_Inst inst, double timeout) {
  return nt::WaitForLoggerQueue(inst, timeout);
}

void NT_DisposeValue(NT_Value* value) {
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

void NT_InitValue(NT_Value* value) {
  value->type = NT_UNASSIGNED;
  value->last_change = 0;
}

void NT_DisposeString(NT_String* str) {
  std::free(str->str);
  str->str = nullptr;
  str->len = 0;
}

void NT_InitString(NT_String* str) {
  str->str = nullptr;
  str->len = 0;
}

void NT_DisposeEntryArray(NT_Entry* arr, size_t /*count*/) { std::free(arr); }

void NT_DisposeConnectionInfoArray(NT_ConnectionInfo* arr, size_t count) {
  for (size_t i = 0; i < count; i++) DisposeConnectionInfo(&arr[i]);
  std::free(arr);
}

void NT_DisposeEntryInfoArray(NT_EntryInfo* arr, size_t count) {
  for (size_t i = 0; i < count; i++) DisposeEntryInfo(&arr[i]);
  std::free(arr);
}

void NT_DisposeEntryInfo(NT_EntryInfo* info) { DisposeEntryInfo(info); }

void NT_DisposeEntryNotificationArray(NT_EntryNotification* arr, size_t count) {
  for (size_t i = 0; i < count; i++) DisposeEntryNotification(&arr[i]);
  std::free(arr);
}

void NT_DisposeEntryNotification(NT_EntryNotification* info) {
  DisposeEntryNotification(info);
}

void NT_DisposeConnectionNotificationArray(NT_ConnectionNotification* arr,
                                           size_t count) {
  for (size_t i = 0; i < count; i++) DisposeConnectionNotification(&arr[i]);
  std::free(arr);
}

void NT_DisposeConnectionNotification(NT_ConnectionNotification* info) {
  DisposeConnectionNotification(info);
}

void NT_DisposeLogMessageArray(NT_LogMessage* arr, size_t count) {
  for (size_t i = 0; i < count; i++) NT_DisposeLogMessage(&arr[i]);
  std::free(arr);
}

void NT_DisposeLogMessage(NT_LogMessage* info) { std::free(info->message); }

void NT_DisposeRpcDefinition(NT_RpcDefinition* def) {
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

void NT_DisposeRpcAnswerArray(NT_RpcAnswer* arr, size_t count) {
  for (size_t i = 0; i < count; i++) NT_DisposeRpcAnswer(&arr[i]);
  std::free(arr);
}

void NT_DisposeRpcAnswer(NT_RpcAnswer* call_info) {
  NT_DisposeString(&call_info->name);
  NT_DisposeString(&call_info->params);
  DisposeConnectionInfo(&call_info->conn);
}

/* Interop Utility Functions */

/* Array and Struct Allocations */

/* Allocates a char array of the specified size.*/
char* NT_AllocateCharArray(size_t size) {
  char* retVal = static_cast<char*>(wpi::CheckedMalloc(size * sizeof(char)));
  return retVal;
}

/* Allocates an integer or boolean array of the specified size. */
int* NT_AllocateBooleanArray(size_t size) {
  int* retVal = static_cast<int*>(wpi::CheckedMalloc(size * sizeof(int)));
  return retVal;
}

/* Allocates a double array of the specified size. */
double* NT_AllocateDoubleArray(size_t size) {
  double* retVal =
      static_cast<double*>(wpi::CheckedMalloc(size * sizeof(double)));
  return retVal;
}

/* Allocates an NT_String array of the specified size. */
struct NT_String* NT_AllocateStringArray(size_t size) {
  NT_String* retVal =
      static_cast<NT_String*>(wpi::CheckedMalloc(size * sizeof(NT_String)));
  return retVal;
}

void NT_FreeCharArray(char* v_char) { std::free(v_char); }
void NT_FreeDoubleArray(double* v_double) { std::free(v_double); }
void NT_FreeBooleanArray(int* v_boolean) { std::free(v_boolean); }
void NT_FreeStringArray(struct NT_String* v_string, size_t arr_size) {
  for (size_t i = 0; i < arr_size; i++) std::free(v_string[i].str);
  std::free(v_string);
}

NT_Bool NT_SetEntryDouble(NT_Entry entry, uint64_t time, double v_double,
                          NT_Bool force) {
  if (force != 0) {
    nt::SetEntryTypeValue(entry, Value::MakeDouble(v_double, time));
    return 1;
  } else {
    return nt::SetEntryValue(entry, Value::MakeDouble(v_double, time));
  }
}

NT_Bool NT_SetEntryBoolean(NT_Entry entry, uint64_t time, NT_Bool v_boolean,
                           NT_Bool force) {
  if (force != 0) {
    nt::SetEntryTypeValue(entry, Value::MakeBoolean(v_boolean != 0, time));
    return 1;
  } else {
    return nt::SetEntryValue(entry, Value::MakeBoolean(v_boolean != 0, time));
  }
}

NT_Bool NT_SetEntryString(NT_Entry entry, uint64_t time, const char* str,
                          size_t str_len, NT_Bool force) {
  if (force != 0) {
    nt::SetEntryTypeValue(entry,
                          Value::MakeString(StringRef(str, str_len), time));
    return 1;
  } else {
    return nt::SetEntryValue(entry,
                             Value::MakeString(StringRef(str, str_len), time));
  }
}

NT_Bool NT_SetEntryRaw(NT_Entry entry, uint64_t time, const char* raw,
                       size_t raw_len, NT_Bool force) {
  if (force != 0) {
    nt::SetEntryTypeValue(entry, Value::MakeRaw(StringRef(raw, raw_len), time));
    return 1;
  } else {
    return nt::SetEntryValue(entry,
                             Value::MakeRaw(StringRef(raw, raw_len), time));
  }
}

NT_Bool NT_SetEntryBooleanArray(NT_Entry entry, uint64_t time,
                                const NT_Bool* arr, size_t size,
                                NT_Bool force) {
  if (force != 0) {
    nt::SetEntryTypeValue(
        entry, Value::MakeBooleanArray(wpi::makeArrayRef(arr, size), time));
    return 1;
  } else {
    return nt::SetEntryValue(
        entry, Value::MakeBooleanArray(wpi::makeArrayRef(arr, size), time));
  }
}

NT_Bool NT_SetEntryDoubleArray(NT_Entry entry, uint64_t time, const double* arr,
                               size_t size, NT_Bool force) {
  if (force != 0) {
    nt::SetEntryTypeValue(
        entry, Value::MakeDoubleArray(wpi::makeArrayRef(arr, size), time));
    return 1;
  } else {
    return nt::SetEntryValue(
        entry, Value::MakeDoubleArray(wpi::makeArrayRef(arr, size), time));
  }
}

NT_Bool NT_SetEntryStringArray(NT_Entry entry, uint64_t time,
                               const struct NT_String* arr, size_t size,
                               NT_Bool force) {
  std::vector<std::string> v;
  v.reserve(size);
  for (size_t i = 0; i < size; ++i) v.push_back(ConvertFromC(arr[i]));

  if (force != 0) {
    nt::SetEntryTypeValue(entry, Value::MakeStringArray(std::move(v), time));
    return 1;
  } else {
    return nt::SetEntryValue(entry, Value::MakeStringArray(std::move(v), time));
  }
}

enum NT_Type NT_GetValueType(const struct NT_Value* value) {
  if (!value) return NT_Type::NT_UNASSIGNED;
  return value->type;
}

NT_Bool NT_GetValueBoolean(const struct NT_Value* value, uint64_t* last_change,
                           NT_Bool* v_boolean) {
  if (!value || value->type != NT_Type::NT_BOOLEAN) return 0;
  *v_boolean = value->data.v_boolean;
  *last_change = value->last_change;
  return 1;
}

NT_Bool NT_GetValueDouble(const struct NT_Value* value, uint64_t* last_change,
                          double* v_double) {
  if (!value || value->type != NT_Type::NT_DOUBLE) return 0;
  *last_change = value->last_change;
  *v_double = value->data.v_double;
  return 1;
}

char* NT_GetValueString(const struct NT_Value* value, uint64_t* last_change,
                        size_t* str_len) {
  if (!value || value->type != NT_Type::NT_STRING) return nullptr;
  *last_change = value->last_change;
  *str_len = value->data.v_string.len;
  char* str =
      static_cast<char*>(wpi::CheckedMalloc(value->data.v_string.len + 1));
  std::memcpy(str, value->data.v_string.str, value->data.v_string.len + 1);
  return str;
}

char* NT_GetValueRaw(const struct NT_Value* value, uint64_t* last_change,
                     size_t* raw_len) {
  if (!value || value->type != NT_Type::NT_RAW) return nullptr;
  *last_change = value->last_change;
  *raw_len = value->data.v_string.len;
  char* raw =
      static_cast<char*>(wpi::CheckedMalloc(value->data.v_string.len + 1));
  std::memcpy(raw, value->data.v_string.str, value->data.v_string.len + 1);
  return raw;
}

NT_Bool* NT_GetValueBooleanArray(const struct NT_Value* value,
                                 uint64_t* last_change, size_t* arr_size) {
  if (!value || value->type != NT_Type::NT_BOOLEAN_ARRAY) return nullptr;
  *last_change = value->last_change;
  *arr_size = value->data.arr_boolean.size;
  NT_Bool* arr = static_cast<int*>(
      wpi::CheckedMalloc(value->data.arr_boolean.size * sizeof(NT_Bool)));
  std::memcpy(arr, value->data.arr_boolean.arr,
              value->data.arr_boolean.size * sizeof(NT_Bool));
  return arr;
}

double* NT_GetValueDoubleArray(const struct NT_Value* value,
                               uint64_t* last_change, size_t* arr_size) {
  if (!value || value->type != NT_Type::NT_DOUBLE_ARRAY) return nullptr;
  *last_change = value->last_change;
  *arr_size = value->data.arr_double.size;
  double* arr = static_cast<double*>(
      wpi::CheckedMalloc(value->data.arr_double.size * sizeof(double)));
  std::memcpy(arr, value->data.arr_double.arr,
              value->data.arr_double.size * sizeof(double));
  return arr;
}

NT_String* NT_GetValueStringArray(const struct NT_Value* value,
                                  uint64_t* last_change, size_t* arr_size) {
  if (!value || value->type != NT_Type::NT_STRING_ARRAY) return nullptr;
  *last_change = value->last_change;
  *arr_size = value->data.arr_string.size;
  NT_String* arr = static_cast<NT_String*>(
      wpi::CheckedMalloc(value->data.arr_string.size * sizeof(NT_String)));
  for (size_t i = 0; i < value->data.arr_string.size; ++i) {
    size_t len = value->data.arr_string.arr[i].len;
    arr[i].len = len;
    arr[i].str = static_cast<char*>(wpi::CheckedMalloc(len + 1));
    std::memcpy(arr[i].str, value->data.arr_string.arr[i].str, len + 1);
  }
  return arr;
}

NT_Bool NT_SetDefaultEntryBoolean(NT_Entry entry, uint64_t time,
                                  NT_Bool default_boolean) {
  return nt::SetDefaultEntryValue(
      entry, Value::MakeBoolean(default_boolean != 0, time));
}

NT_Bool NT_SetDefaultEntryDouble(NT_Entry entry, uint64_t time,
                                 double default_double) {
  return nt::SetDefaultEntryValue(entry,
                                  Value::MakeDouble(default_double, time));
}

NT_Bool NT_SetDefaultEntryString(NT_Entry entry, uint64_t time,
                                 const char* default_value,
                                 size_t default_len) {
  return nt::SetDefaultEntryValue(
      entry, Value::MakeString(StringRef(default_value, default_len), time));
}

NT_Bool NT_SetDefaultEntryRaw(NT_Entry entry, uint64_t time,
                              const char* default_value, size_t default_len) {
  return nt::SetDefaultEntryValue(
      entry, Value::MakeRaw(StringRef(default_value, default_len), time));
}

NT_Bool NT_SetDefaultEntryBooleanArray(NT_Entry entry, uint64_t time,
                                       const NT_Bool* default_value,
                                       size_t default_size) {
  return nt::SetDefaultEntryValue(
      entry, Value::MakeBooleanArray(
                 wpi::makeArrayRef(default_value, default_size), time));
}

NT_Bool NT_SetDefaultEntryDoubleArray(NT_Entry entry, uint64_t time,
                                      const double* default_value,
                                      size_t default_size) {
  return nt::SetDefaultEntryValue(
      entry, Value::MakeDoubleArray(
                 wpi::makeArrayRef(default_value, default_size), time));
}

NT_Bool NT_SetDefaultEntryStringArray(NT_Entry entry, uint64_t time,
                                      const struct NT_String* default_value,
                                      size_t default_size) {
  std::vector<std::string> vec;
  vec.reserve(default_size);
  for (size_t i = 0; i < default_size; ++i)
    vec.push_back(ConvertFromC(default_value[i]));

  return nt::SetDefaultEntryValue(entry,
                                  Value::MakeStringArray(std::move(vec), time));
}

NT_Bool NT_GetEntryBoolean(NT_Entry entry, uint64_t* last_change,
                           NT_Bool* v_boolean) {
  auto v = nt::GetEntryValue(entry);
  if (!v || !v->IsBoolean()) return 0;
  *v_boolean = v->GetBoolean();
  *last_change = v->last_change();
  return 1;
}

NT_Bool NT_GetEntryDouble(NT_Entry entry, uint64_t* last_change,
                          double* v_double) {
  auto v = nt::GetEntryValue(entry);
  if (!v || !v->IsDouble()) return 0;
  *last_change = v->last_change();
  *v_double = v->GetDouble();
  return 1;
}

char* NT_GetEntryString(NT_Entry entry, uint64_t* last_change,
                        size_t* str_len) {
  auto v = nt::GetEntryValue(entry);
  if (!v || !v->IsString()) return nullptr;
  *last_change = v->last_change();
  struct NT_String v_string;
  nt::ConvertToC(v->GetString(), &v_string);
  *str_len = v_string.len;
  return v_string.str;
}

char* NT_GetEntryRaw(NT_Entry entry, uint64_t* last_change, size_t* raw_len) {
  auto v = nt::GetEntryValue(entry);
  if (!v || !v->IsRaw()) return nullptr;
  *last_change = v->last_change();
  struct NT_String v_raw;
  nt::ConvertToC(v->GetRaw(), &v_raw);
  *raw_len = v_raw.len;
  return v_raw.str;
}

NT_Bool* NT_GetEntryBooleanArray(NT_Entry entry, uint64_t* last_change,
                                 size_t* arr_size) {
  auto v = nt::GetEntryValue(entry);
  if (!v || !v->IsBooleanArray()) return nullptr;
  *last_change = v->last_change();
  auto vArr = v->GetBooleanArray();
  NT_Bool* arr =
      static_cast<int*>(wpi::CheckedMalloc(vArr.size() * sizeof(NT_Bool)));
  *arr_size = vArr.size();
  std::copy(vArr.begin(), vArr.end(), arr);
  return arr;
}

double* NT_GetEntryDoubleArray(NT_Entry entry, uint64_t* last_change,
                               size_t* arr_size) {
  auto v = nt::GetEntryValue(entry);
  if (!v || !v->IsDoubleArray()) return nullptr;
  *last_change = v->last_change();
  auto vArr = v->GetDoubleArray();
  double* arr =
      static_cast<double*>(wpi::CheckedMalloc(vArr.size() * sizeof(double)));
  *arr_size = vArr.size();
  std::copy(vArr.begin(), vArr.end(), arr);
  return arr;
}

NT_String* NT_GetEntryStringArray(NT_Entry entry, uint64_t* last_change,
                                  size_t* arr_size) {
  auto v = nt::GetEntryValue(entry);
  if (!v || !v->IsStringArray()) return nullptr;
  *last_change = v->last_change();
  auto vArr = v->GetStringArray();
  NT_String* arr = static_cast<NT_String*>(
      wpi::CheckedMalloc(vArr.size() * sizeof(NT_String)));
  for (size_t i = 0; i < vArr.size(); ++i) {
    ConvertToC(vArr[i], &arr[i]);
  }
  *arr_size = vArr.size();
  return arr;
}

}  // extern "C"
