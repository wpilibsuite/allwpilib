/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ntcore.h"

#include <cassert>
#include <cstdlib>

#include "support/timestamp.h"
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

static void DisposeConnectionInfo(NT_ConnectionInfo* info) {
  std::free(info->remote_id.str);
  std::free(info->remote_ip.str);
}

static void DisposeEntryInfo(NT_EntryInfo* info) { std::free(info->name.str); }

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
 * Table Functions
 */

void NT_GetEntryValue(const char* name, size_t name_len,
                      struct NT_Value* value) {
  NT_InitValue(value);
  auto v = nt::GetEntryValue(StringRef(name, name_len));
  if (!v) return;
  ConvertToC(*v, value);
}

int NT_SetDefaultEntryValue(const char* name, size_t name_len,
                            const struct NT_Value* set_value) {
  return nt::SetDefaultEntryValue(StringRef(name, name_len),
                                  ConvertFromC(*set_value));
}

int NT_SetEntryValue(const char* name, size_t name_len,
                     const struct NT_Value* value) {
  return nt::SetEntryValue(StringRef(name, name_len), ConvertFromC(*value));
}

void NT_SetEntryTypeValue(const char* name, size_t name_len,
                          const struct NT_Value* value) {
  nt::SetEntryTypeValue(StringRef(name, name_len), ConvertFromC(*value));
}

void NT_SetEntryFlags(const char* name, size_t name_len, unsigned int flags) {
  nt::SetEntryFlags(StringRef(name, name_len), flags);
}

unsigned int NT_GetEntryFlags(const char* name, size_t name_len) {
  return nt::GetEntryFlags(StringRef(name, name_len));
}

void NT_DeleteEntry(const char* name, size_t name_len) {
  nt::DeleteEntry(StringRef(name, name_len));
}

void NT_DeleteAllEntries(void) { nt::DeleteAllEntries(); }

struct NT_EntryInfo* NT_GetEntryInfo(const char* prefix, size_t prefix_len,
                                     unsigned int types, size_t* count) {
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

void NT_SetListenerOnStart(void (*on_start)(void* data), void* data) {
  nt::SetListenerOnStart([=]() { on_start(data); });
}

void NT_SetListenerOnExit(void (*on_exit)(void* data), void* data) {
  nt::SetListenerOnExit([=]() { on_exit(data); });
}

unsigned int NT_AddEntryListener(const char* prefix, size_t prefix_len,
                                 void* data, NT_EntryListenerCallback callback,
                                 unsigned int flags) {
  return nt::AddEntryListener(
      StringRef(prefix, prefix_len),
      [=](unsigned int uid, StringRef name, std::shared_ptr<Value> value,
          unsigned int flags_) {
        callback(uid, data, name.data(), name.size(), &value->value(), flags_);
      },
      flags);
}

void NT_RemoveEntryListener(unsigned int entry_listener_uid) {
  nt::RemoveEntryListener(entry_listener_uid);
}

unsigned int NT_AddConnectionListener(void* data,
                                      NT_ConnectionListenerCallback callback,
                                      int immediate_notify) {
  return nt::AddConnectionListener(
      [=](unsigned int uid, bool connected, const ConnectionInfo& conn) {
        NT_ConnectionInfo conn_c;
        ConvertToC(conn, &conn_c);
        callback(uid, data, connected ? 1 : 0, &conn_c);
        DisposeConnectionInfo(&conn_c);
      },
      immediate_notify != 0);
}

void NT_RemoveConnectionListener(unsigned int conn_listener_uid) {
  nt::RemoveConnectionListener(conn_listener_uid);
}

int NT_NotifierDestroyed() { return nt::NotifierDestroyed(); }

/*
 * Remote Procedure Call Functions
 */

void NT_SetRpcServerOnStart(void (*on_start)(void* data), void* data) {
  nt::SetRpcServerOnStart([=]() { on_start(data); });
}

void NT_SetRpcServerOnExit(void (*on_exit)(void* data), void* data) {
  nt::SetRpcServerOnExit([=]() { on_exit(data); });
}

void NT_CreateRpc(const char* name, size_t name_len, const char* def,
                  size_t def_len, void* data, NT_RpcCallback callback) {
  nt::CreateRpc(StringRef(name, name_len), StringRef(def, def_len),
                [=](StringRef name, StringRef params,
                    const ConnectionInfo& conn_info) -> std::string {
                  size_t results_len;
                  NT_ConnectionInfo conn_c;
                  ConvertToC(conn_info, &conn_c);
                  char* results_c =
                      callback(data, name.data(), name.size(), params.data(),
                               params.size(), &results_len, &conn_c);
                  std::string results(results_c, results_len);
                  std::free(results_c);
                  DisposeConnectionInfo(&conn_c);
                  return results;
                });
}

void NT_CreatePolledRpc(const char* name, size_t name_len, const char* def,
                        size_t def_len) {
  nt::CreatePolledRpc(StringRef(name, name_len), StringRef(def, def_len));
}

int NT_PollRpc(int blocking, NT_RpcCallInfo* call_info) {
  RpcCallInfo call_info_cpp;
  if (!nt::PollRpc(blocking != 0, &call_info_cpp)) return 0;
  ConvertToC(call_info_cpp, call_info);
  return 1;
}

int NT_PollRpcTimeout(int blocking, double time_out,
                      NT_RpcCallInfo* call_info) {
  RpcCallInfo call_info_cpp;
  if (!nt::PollRpc(blocking != 0, time_out, &call_info_cpp)) return 0;
  ConvertToC(call_info_cpp, call_info);
  return 1;
}

void NT_PostRpcResponse(unsigned int rpc_id, unsigned int call_uid,
                        const char* result, size_t result_len) {
  nt::PostRpcResponse(rpc_id, call_uid, StringRef(result, result_len));
}

unsigned int NT_CallRpc(const char* name, size_t name_len, const char* params,
                        size_t params_len) {
  return nt::CallRpc(StringRef(name, name_len), StringRef(params, params_len));
}

char* NT_GetRpcResult(int blocking, unsigned int call_uid, size_t* result_len) {
  std::string result;
  if (!nt::GetRpcResult(blocking != 0, call_uid, &result)) return nullptr;

  // convert result
  *result_len = result.size();
  char* result_cstr;
  ConvertToC(result, &result_cstr);
  return result_cstr;
}

char* NT_GetRpcResultTimeout(int blocking, unsigned int call_uid,
                             double time_out, size_t* result_len) {
  std::string result;
  if (!nt::GetRpcResult(blocking != 0, call_uid, time_out, &result))
    return nullptr;

  // convert result
  *result_len = result.size();
  char* result_cstr;
  ConvertToC(result, &result_cstr);
  return result_cstr;
}

void NT_CancelBlockingRpcResult(unsigned int call_uid) {
  nt::CancelBlockingRpcResult(call_uid);
}

char* NT_PackRpcDefinition(const NT_RpcDefinition* def, size_t* packed_len) {
  auto packed = nt::PackRpcDefinition(ConvertFromC(*def));

  // convert result
  *packed_len = packed.size();
  char* packed_cstr;
  ConvertToC(packed, &packed_cstr);
  return packed_cstr;
}

int NT_UnpackRpcDefinition(const char* packed, size_t packed_len,
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
  NT_Value** values =
      static_cast<NT_Value**>(std::malloc(values_v.size() * sizeof(NT_Value*)));
  for (size_t i = 0; i < values_v.size(); ++i) {
    values[i] = static_cast<NT_Value*>(std::malloc(sizeof(NT_Value)));
    ConvertToC(*values_v[i], values[i]);
  }
  return values;
}

/*
 * Client/Server Functions
 */

void NT_SetNetworkIdentity(const char* name, size_t name_len) {
  nt::SetNetworkIdentity(StringRef(name, name_len));
}

void NT_StartServer(const char* persist_filename, const char* listen_address,
                    unsigned int port) {
  nt::StartServer(persist_filename, listen_address, port);
}

void NT_StopServer(void) { nt::StopServer(); }

void NT_StartClientNone(void) { nt::StartClient(); }

void NT_StartClient(const char* server_name, unsigned int port) {
  nt::StartClient(server_name, port);
}

void NT_StartClientMulti(size_t count, const char** server_names,
                         const unsigned int* ports) {
  std::vector<std::pair<StringRef, unsigned int>> servers;
  servers.reserve(count);
  for (size_t i = 0; i < count; ++i)
    servers.emplace_back(std::make_pair(server_names[i], ports[i]));
  nt::StartClient(servers);
}

void NT_StopClient(void) { nt::StopClient(); }

void NT_SetServer(const char* server_name, unsigned int port) {
  nt::SetServer(server_name, port);
}

void NT_SetServerMulti(size_t count, const char** server_names,
                         const unsigned int* ports) {
  std::vector<std::pair<StringRef, unsigned int>> servers;
  servers.reserve(count);
  for (size_t i = 0; i < count; ++i)
    servers.emplace_back(std::make_pair(server_names[i], ports[i]));
  nt::SetServer(servers);
}

void NT_StartDSClient(unsigned int port) { nt::StartDSClient(port); }

void NT_StopDSClient(void) { nt::StopDSClient(); }

void NT_StopRpcServer(void) { nt::StopRpcServer(); }

void NT_StopNotifier(void) { nt::StopNotifier(); }

void NT_SetUpdateRate(double interval) { nt::SetUpdateRate(interval); }

struct NT_ConnectionInfo* NT_GetConnections(size_t* count) {
  auto conn_v = nt::GetConnections();
  *count = conn_v.size();
  if (conn_v.size() == 0) return nullptr;

  // create array and copy into it
  NT_ConnectionInfo* conn = static_cast<NT_ConnectionInfo*>(
      std::malloc(conn_v.size() * sizeof(NT_ConnectionInfo)));
  for (size_t i = 0; i < conn_v.size(); ++i) ConvertToC(conn_v[i], &conn[i]);
  return conn;
}

/*
 * Persistent Functions
 */

const char* NT_SavePersistent(const char* filename) {
  return nt::SavePersistent(filename);
}

const char* NT_LoadPersistent(const char* filename,
                              void (*warn)(size_t line, const char* msg)) {
  return nt::LoadPersistent(filename, warn);
}

/*
 * Utility Functions
 */

unsigned long long NT_Now() { return wpi::Now(); }

void NT_SetLogger(NT_LogFunc func, unsigned int min_level) {
  nt::SetLogger(func, min_level);
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

enum NT_Type NT_GetType(const char* name, size_t name_len) {
  auto v = nt::GetEntryValue(StringRef(name, name_len));
  if (!v) return NT_Type::NT_UNASSIGNED;
  return v->type();
}

void NT_DisposeConnectionInfoArray(NT_ConnectionInfo* arr, size_t count) {
  for (size_t i = 0; i < count; i++) DisposeConnectionInfo(&arr[i]);
  std::free(arr);
}

void NT_DisposeEntryInfoArray(NT_EntryInfo* arr, size_t count) {
  for (size_t i = 0; i < count; i++) DisposeEntryInfo(&arr[i]);
  std::free(arr);
}

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

void NT_DisposeRpcCallInfo(NT_RpcCallInfo* call_info) {
  NT_DisposeString(&call_info->name);
  NT_DisposeString(&call_info->params);
}

/* Interop Utility Functions */

/* Array and Struct Allocations */

/* Allocates a char array of the specified size.*/
char* NT_AllocateCharArray(size_t size) {
  char* retVal = static_cast<char*>(std::malloc(size * sizeof(char)));
  return retVal;
}

/* Allocates an integer or boolean array of the specified size. */
int* NT_AllocateBooleanArray(size_t size) {
  int* retVal = static_cast<int*>(std::malloc(size * sizeof(int)));
  return retVal;
}

/* Allocates a double array of the specified size. */
double* NT_AllocateDoubleArray(size_t size) {
  double* retVal = static_cast<double*>(std::malloc(size * sizeof(double)));
  return retVal;
}

/* Allocates an NT_String array of the specified size. */
struct NT_String* NT_AllocateStringArray(size_t size) {
  NT_String* retVal =
      static_cast<NT_String*>(std::malloc(size * sizeof(NT_String)));
  return retVal;
}

void NT_FreeCharArray(char* v_char) { std::free(v_char); }
void NT_FreeDoubleArray(double* v_double) { std::free(v_double); }
void NT_FreeBooleanArray(int* v_boolean) { std::free(v_boolean); }
void NT_FreeStringArray(struct NT_String* v_string, size_t arr_size) {
  for (size_t i = 0; i < arr_size; i++) std::free(v_string[i].str);
  std::free(v_string);
}

int NT_SetEntryDouble(const char* name, size_t name_len, double v_double,
                      int force) {
  if (force != 0) {
    nt::SetEntryTypeValue(StringRef(name, name_len),
                          Value::MakeDouble(v_double));
    return 1;
  } else {
    return nt::SetEntryValue(StringRef(name, name_len),
                             Value::MakeDouble(v_double));
  }
}

int NT_SetEntryBoolean(const char* name, size_t name_len, int v_boolean,
                       int force) {
  if (force != 0) {
    nt::SetEntryTypeValue(StringRef(name, name_len),
                          Value::MakeBoolean(v_boolean != 0));
    return 1;
  } else {
    return nt::SetEntryValue(StringRef(name, name_len),
                             Value::MakeBoolean(v_boolean != 0));
  }
}

int NT_SetEntryString(const char* name, size_t name_len, const char* str,
                      size_t str_len, int force) {
  if (force != 0) {
    nt::SetEntryTypeValue(StringRef(name, name_len),
                          Value::MakeString(StringRef(str, str_len)));
    return 1;
  } else {
    return nt::SetEntryValue(StringRef(name, name_len),
                             Value::MakeString(StringRef(str, str_len)));
  }
}

int NT_SetEntryRaw(const char* name, size_t name_len, const char* raw,
                   size_t raw_len, int force) {
  if (force != 0) {
    nt::SetEntryTypeValue(StringRef(name, name_len),
                          Value::MakeRaw(StringRef(raw, raw_len)));
    return 1;
  } else {
    return nt::SetEntryValue(StringRef(name, name_len),
                             Value::MakeRaw(StringRef(raw, raw_len)));
  }
}

int NT_SetEntryBooleanArray(const char* name, size_t name_len, const int* arr,
                            size_t size, int force) {
  if (force != 0) {
    nt::SetEntryTypeValue(
        StringRef(name, name_len),
        Value::MakeBooleanArray(llvm::makeArrayRef(arr, size)));
    return 1;
  } else {
    return nt::SetEntryValue(
        StringRef(name, name_len),
        Value::MakeBooleanArray(llvm::makeArrayRef(arr, size)));
  }
}

int NT_SetEntryDoubleArray(const char* name, size_t name_len, const double* arr,
                           size_t size, int force) {
  if (force != 0) {
    nt::SetEntryTypeValue(
        StringRef(name, name_len),
        Value::MakeDoubleArray(llvm::makeArrayRef(arr, size)));
    return 1;
  } else {
    return nt::SetEntryValue(
        StringRef(name, name_len),
        Value::MakeDoubleArray(llvm::makeArrayRef(arr, size)));
  }
}

int NT_SetEntryStringArray(const char* name, size_t name_len,
                           const struct NT_String* arr, size_t size,
                           int force) {
  std::vector<std::string> v;
  v.reserve(size);
  for (size_t i = 0; i < size; ++i) v.push_back(ConvertFromC(arr[i]));

  if (force != 0) {
    nt::SetEntryTypeValue(StringRef(name, name_len),
                          Value::MakeStringArray(std::move(v)));
    return 1;
  } else {
    return nt::SetEntryValue(StringRef(name, name_len),
                             Value::MakeStringArray(std::move(v)));
  }
}

enum NT_Type NT_GetValueType(const struct NT_Value* value) {
  if (!value) return NT_Type::NT_UNASSIGNED;
  return value->type;
}

int NT_GetValueBoolean(const struct NT_Value* value,
                       unsigned long long* last_change, int* v_boolean) {
  if (!value || value->type != NT_Type::NT_BOOLEAN) return 0;
  *v_boolean = value->data.v_boolean;
  *last_change = value->last_change;
  return 1;
}

int NT_GetValueDouble(const struct NT_Value* value,
                      unsigned long long* last_change, double* v_double) {
  if (!value || value->type != NT_Type::NT_DOUBLE) return 0;
  *last_change = value->last_change;
  *v_double = value->data.v_double;
  return 1;
}

char* NT_GetValueString(const struct NT_Value* value,
                        unsigned long long* last_change, size_t* str_len) {
  if (!value || value->type != NT_Type::NT_STRING) return nullptr;
  *last_change = value->last_change;
  *str_len = value->data.v_string.len;
  char* str = (char*)std::malloc(value->data.v_string.len + 1);
  std::memcpy(str, value->data.v_string.str, value->data.v_string.len + 1);
  return str;
}

char* NT_GetValueRaw(const struct NT_Value* value,
                     unsigned long long* last_change, size_t* raw_len) {
  if (!value || value->type != NT_Type::NT_RAW) return nullptr;
  *last_change = value->last_change;
  *raw_len = value->data.v_string.len;
  char* raw = (char*)std::malloc(value->data.v_string.len + 1);
  std::memcpy(raw, value->data.v_string.str, value->data.v_string.len + 1);
  return raw;
}

int* NT_GetValueBooleanArray(const struct NT_Value* value,
                             unsigned long long* last_change,
                             size_t* arr_size) {
  if (!value || value->type != NT_Type::NT_BOOLEAN_ARRAY) return nullptr;
  *last_change = value->last_change;
  *arr_size = value->data.arr_boolean.size;
  int* arr = (int*)std::malloc(value->data.arr_boolean.size * sizeof(int));
  std::memcpy(arr, value->data.arr_boolean.arr,
              value->data.arr_boolean.size * sizeof(int));
  return arr;
}

double* NT_GetValueDoubleArray(const struct NT_Value* value,
                               unsigned long long* last_change,
                               size_t* arr_size) {
  if (!value || value->type != NT_Type::NT_DOUBLE_ARRAY) return nullptr;
  *last_change = value->last_change;
  *arr_size = value->data.arr_double.size;
  double* arr =
      (double*)std::malloc(value->data.arr_double.size * sizeof(double));
  std::memcpy(arr, value->data.arr_double.arr,
              value->data.arr_double.size * sizeof(double));
  return arr;
}

NT_String* NT_GetValueStringArray(const struct NT_Value* value,
                                  unsigned long long* last_change,
                                  size_t* arr_size) {
  if (!value || value->type != NT_Type::NT_STRING_ARRAY) return nullptr;
  *last_change = value->last_change;
  *arr_size = value->data.arr_string.size;
  NT_String* arr = static_cast<NT_String*>(
      std::malloc(value->data.arr_string.size * sizeof(NT_String)));
  for (size_t i = 0; i < value->data.arr_string.size; ++i) {
    size_t len = value->data.arr_string.arr[i].len;
    arr[i].len = len;
    arr[i].str = (char*)std::malloc(len + 1);
    std::memcpy(arr[i].str, value->data.arr_string.arr[i].str, len + 1);
  }
  return arr;
}

int NT_SetDefaultEntryBoolean(const char* name, size_t name_len,
                              int default_boolean) {
  return nt::SetDefaultEntryValue(StringRef(name, name_len),
                                  Value::MakeBoolean(default_boolean != 0));
}

int NT_SetDefaultEntryDouble(const char* name, size_t name_len,
                             double default_double) {
  return nt::SetDefaultEntryValue(StringRef(name, name_len),
                                  Value::MakeDouble(default_double));
}

int NT_SetDefaultEntryString(const char* name, size_t name_len,
                             const char* default_value, size_t default_len) {
  return nt::SetDefaultEntryValue(
      StringRef(name, name_len),
      Value::MakeString(StringRef(default_value, default_len)));
}

int NT_SetDefaultEntryRaw(const char* name, size_t name_len,
                          const char* default_value, size_t default_len) {
  return nt::SetDefaultEntryValue(
      StringRef(name, name_len),
      Value::MakeString(StringRef(default_value, default_len)));
}

int NT_SetDefaultEntryBooleanArray(const char* name, size_t name_len,
                                   const int* default_value,
                                   size_t default_size) {
  return nt::SetDefaultEntryValue(
      StringRef(name, name_len),
      Value::MakeBooleanArray(llvm::makeArrayRef(default_value, default_size)));
}

int NT_SetDefaultEntryDoubleArray(const char* name, size_t name_len,
                                  const double* default_value,
                                  size_t default_size) {
  return nt::SetDefaultEntryValue(
      StringRef(name, name_len),
      Value::MakeDoubleArray(llvm::makeArrayRef(default_value, default_size)));
}

int NT_SetDefaultEntryStringArray(const char* name, size_t name_len,
                                  const struct NT_String* default_value,
                                  size_t default_size) {
  std::vector<std::string> vec;
  vec.reserve(default_size);
  for (size_t i = 0; i < default_size; ++i)
    vec.push_back(ConvertFromC(default_value[i]));

  return nt::SetDefaultEntryValue(StringRef(name, name_len),
                                  Value::MakeStringArray(std::move(vec)));
}

int NT_GetEntryBoolean(const char* name, size_t name_len,
                       unsigned long long* last_change, int* v_boolean) {
  auto v = nt::GetEntryValue(StringRef(name, name_len));
  if (!v || !v->IsBoolean()) return 0;
  *v_boolean = v->GetBoolean();
  *last_change = v->last_change();
  return 1;
}

int NT_GetEntryDouble(const char* name, size_t name_len,
                      unsigned long long* last_change, double* v_double) {
  auto v = nt::GetEntryValue(StringRef(name, name_len));
  if (!v || !v->IsDouble()) return 0;
  *last_change = v->last_change();
  *v_double = v->GetDouble();
  return 1;
}

char* NT_GetEntryString(const char* name, size_t name_len,
                        unsigned long long* last_change, size_t* str_len) {
  auto v = nt::GetEntryValue(StringRef(name, name_len));
  if (!v || !v->IsString()) return nullptr;
  *last_change = v->last_change();
  struct NT_String v_string;
  nt::ConvertToC(v->GetString(), &v_string);
  *str_len = v_string.len;
  return v_string.str;
}

char* NT_GetEntryRaw(const char* name, size_t name_len,
                     unsigned long long* last_change, size_t* raw_len) {
  auto v = nt::GetEntryValue(StringRef(name, name_len));
  if (!v || !v->IsRaw()) return nullptr;
  *last_change = v->last_change();
  struct NT_String v_raw;
  nt::ConvertToC(v->GetRaw(), &v_raw);
  *raw_len = v_raw.len;
  return v_raw.str;
}

int* NT_GetEntryBooleanArray(const char* name, size_t name_len,
                             unsigned long long* last_change,
                             size_t* arr_size) {
  auto v = nt::GetEntryValue(StringRef(name, name_len));
  if (!v || !v->IsBooleanArray()) return nullptr;
  *last_change = v->last_change();
  auto vArr = v->GetBooleanArray();
  int* arr = static_cast<int*>(std::malloc(vArr.size() * sizeof(int)));
  *arr_size = vArr.size();
  std::copy(vArr.begin(), vArr.end(), arr);
  return arr;
}

double* NT_GetEntryDoubleArray(const char* name, size_t name_len,
                               unsigned long long* last_change,
                               size_t* arr_size) {
  auto v = nt::GetEntryValue(StringRef(name, name_len));
  if (!v || !v->IsDoubleArray()) return nullptr;
  *last_change = v->last_change();
  auto vArr = v->GetDoubleArray();
  double* arr = static_cast<double*>(std::malloc(vArr.size() * sizeof(double)));
  *arr_size = vArr.size();
  std::copy(vArr.begin(), vArr.end(), arr);
  return arr;
}

NT_String* NT_GetEntryStringArray(const char* name, size_t name_len,
                                  unsigned long long* last_change,
                                  size_t* arr_size) {
  auto v = nt::GetEntryValue(StringRef(name, name_len));
  if (!v || !v->IsStringArray()) return nullptr;
  *last_change = v->last_change();
  auto vArr = v->GetStringArray();
  NT_String* arr =
      static_cast<NT_String*>(std::malloc(vArr.size() * sizeof(NT_String)));
  for (size_t i = 0; i < vArr.size(); ++i) {
    ConvertToC(vArr[i], &arr[i]);
  }
  *arr_size = vArr.size();
  return arr;
}

}  // extern "C"
