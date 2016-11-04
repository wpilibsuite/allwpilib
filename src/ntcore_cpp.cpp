/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ntcore.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>

#include "support/timestamp.h"
#include "Log.h"
#include "Dispatcher.h"
#include "DsClient.h"
#include "Notifier.h"
#include "RpcServer.h"
#include "Storage.h"
#include "WireDecoder.h"
#include "WireEncoder.h"

namespace nt {

/*
 * Table Functions
 */

std::shared_ptr<Value> GetEntryValue(StringRef name) {
  return Storage::GetInstance().GetEntryValue(name);
}

bool SetDefaultEntryValue(StringRef name, std::shared_ptr<Value> value) {
  return Storage::GetInstance().SetDefaultEntryValue(name, value);
}

bool SetEntryValue(StringRef name, std::shared_ptr<Value> value) {
  return Storage::GetInstance().SetEntryValue(name, value);
}

void SetEntryTypeValue(StringRef name, std::shared_ptr<Value> value) {
  Storage::GetInstance().SetEntryTypeValue(name, value);
}

void SetEntryFlags(StringRef name, unsigned int flags) {
  Storage::GetInstance().SetEntryFlags(name, flags);
}

unsigned int GetEntryFlags(StringRef name) {
  return Storage::GetInstance().GetEntryFlags(name);
}

void DeleteEntry(StringRef name) { Storage::GetInstance().DeleteEntry(name); }

void DeleteAllEntries() { Storage::GetInstance().DeleteAllEntries(); }

std::vector<EntryInfo> GetEntryInfo(StringRef prefix, unsigned int types) {
  return Storage::GetInstance().GetEntryInfo(prefix, types);
}

void Flush() { Dispatcher::GetInstance().Flush(); }

/*
 * Callback Creation Functions
 */

void SetListenerOnStart(std::function<void()> on_start) {
  Notifier::GetInstance().SetOnStart(on_start);
}

void SetListenerOnExit(std::function<void()> on_exit) {
  Notifier::GetInstance().SetOnExit(on_exit);
}

unsigned int AddEntryListener(StringRef prefix, EntryListenerCallback callback,
                              unsigned int flags) {
  unsigned int uid =
      Notifier::GetInstance().AddEntryListener(prefix, callback, flags);
  if ((flags & NT_NOTIFY_IMMEDIATE) != 0)
    Storage::GetInstance().NotifyEntries(prefix, callback);
  return uid;
}

void RemoveEntryListener(unsigned int entry_listener_uid) {
  Notifier::GetInstance().RemoveEntryListener(entry_listener_uid);
}

unsigned int AddConnectionListener(ConnectionListenerCallback callback,
                                   bool immediate_notify) {
  unsigned int uid = Notifier::GetInstance().AddConnectionListener(callback);
  if (immediate_notify) Dispatcher::GetInstance().NotifyConnections(callback);
  return uid;
}

void RemoveConnectionListener(unsigned int conn_listener_uid) {
  Notifier::GetInstance().RemoveConnectionListener(conn_listener_uid);
}

bool NotifierDestroyed() { return Notifier::destroyed(); }

/*
 * Remote Procedure Call Functions
 */

void SetRpcServerOnStart(std::function<void()> on_start) {
  RpcServer::GetInstance().SetOnStart(on_start);
}

void SetRpcServerOnExit(std::function<void()> on_exit) {
  RpcServer::GetInstance().SetOnExit(on_exit);
}

void CreateRpc(StringRef name, StringRef def, RpcCallback callback) {
  Storage::GetInstance().CreateRpc(name, def, callback);
}

void CreatePolledRpc(StringRef name, StringRef def) {
  Storage::GetInstance().CreatePolledRpc(name, def);
}

bool PollRpc(bool blocking, RpcCallInfo* call_info) {
  return RpcServer::GetInstance().PollRpc(blocking, call_info);
}

bool PollRpc(bool blocking, double time_out, RpcCallInfo* call_info) {
  return RpcServer::GetInstance().PollRpc(blocking, time_out, call_info);
}

void PostRpcResponse(unsigned int rpc_id, unsigned int call_uid,
                     StringRef result) {
  RpcServer::GetInstance().PostRpcResponse(rpc_id, call_uid, result);
}

unsigned int CallRpc(StringRef name, StringRef params) {
  return Storage::GetInstance().CallRpc(name, params);
}

bool GetRpcResult(bool blocking, unsigned int call_uid, std::string* result) {
  return Storage::GetInstance().GetRpcResult(blocking, call_uid, result);
}

bool GetRpcResult(bool blocking, unsigned int call_uid, double time_out,
                  std::string* result) {
  return Storage::GetInstance().GetRpcResult(blocking, call_uid, time_out,
                                             result);
}

void CancelBlockingRpcResult(unsigned int call_uid) {
  Storage::GetInstance().CancelBlockingRpcResult(call_uid);
}

std::string PackRpcDefinition(const RpcDefinition& def) {
  WireEncoder enc(0x0300);
  enc.Write8(def.version);
  enc.WriteString(def.name);

  // parameters
  unsigned int params_size = def.params.size();
  if (params_size > 0xff) params_size = 0xff;
  enc.Write8(params_size);
  for (std::size_t i = 0; i < params_size; ++i) {
    enc.WriteType(def.params[i].def_value->type());
    enc.WriteString(def.params[i].name);
    enc.WriteValue(*def.params[i].def_value);
  }

  // results
  unsigned int results_size = def.results.size();
  if (results_size > 0xff) results_size = 0xff;
  enc.Write8(results_size);
  for (std::size_t i = 0; i < results_size; ++i) {
    enc.WriteType(def.results[i].type);
    enc.WriteString(def.results[i].name);
  }

  return enc.ToStringRef();
}

bool UnpackRpcDefinition(StringRef packed, RpcDefinition* def) {
  wpi::raw_mem_istream is(packed.data(), packed.size());
  WireDecoder dec(is, 0x0300);
  if (!dec.Read8(&def->version)) return false;
  if (!dec.ReadString(&def->name)) return false;

  // parameters
  unsigned int params_size;
  if (!dec.Read8(&params_size)) return false;
  def->params.resize(0);
  def->params.reserve(params_size);
  for (std::size_t i = 0; i < params_size; ++i) {
    RpcParamDef pdef;
    NT_Type type;
    if (!dec.ReadType(&type)) return false;
    if (!dec.ReadString(&pdef.name)) return false;
    pdef.def_value = dec.ReadValue(type);
    if (!pdef.def_value) return false;
    def->params.emplace_back(std::move(pdef));
  }

  // results
  unsigned int results_size;
  if (!dec.Read8(&results_size)) return false;
  def->results.resize(0);
  def->results.reserve(results_size);
  for (std::size_t i = 0; i < results_size; ++i) {
    RpcResultDef rdef;
    if (!dec.ReadType(&rdef.type)) return false;
    if (!dec.ReadString(&rdef.name)) return false;
    def->results.emplace_back(std::move(rdef));
  }

  return true;
}

std::string PackRpcValues(ArrayRef<std::shared_ptr<Value>> values) {
  WireEncoder enc(0x0300);
  for (auto& value : values) enc.WriteValue(*value);
  return enc.ToStringRef();
}

std::vector<std::shared_ptr<Value>> UnpackRpcValues(StringRef packed,
                                                    ArrayRef<NT_Type> types) {
  wpi::raw_mem_istream is(packed.data(), packed.size());
  WireDecoder dec(is, 0x0300);
  std::vector<std::shared_ptr<Value>> vec;
  for (auto type : types) {
    auto item = dec.ReadValue(type);
    if (!item) return std::vector<std::shared_ptr<Value>>();
    vec.emplace_back(std::move(item));
  }
  return vec;
}

/*
 * Client/Server Functions
 */

void SetNetworkIdentity(StringRef name) {
  Dispatcher::GetInstance().SetIdentity(name);
}

void StartServer(StringRef persist_filename, const char* listen_address,
                 unsigned int port) {
  Dispatcher::GetInstance().StartServer(persist_filename, listen_address, port);
}

void StopServer() { Dispatcher::GetInstance().Stop(); }

void StartClient() { Dispatcher::GetInstance().StartClient(); }

void StartClient(const char* server_name, unsigned int port) {
  auto& d = Dispatcher::GetInstance();
  d.SetServer(server_name, port);
  d.StartClient();
}

void StartClient(ArrayRef<std::pair<StringRef, unsigned int>> servers) {
  auto& d = Dispatcher::GetInstance();
  d.SetServer(servers);
  d.StartClient();
}

void StopClient() { Dispatcher::GetInstance().Stop(); }

void SetServer(const char* server_name, unsigned int port) {
  Dispatcher::GetInstance().SetServer(server_name, port);
}

void SetServer(ArrayRef<std::pair<StringRef, unsigned int>> servers) {
  Dispatcher::GetInstance().SetServer(servers);
}

void StartDSClient(unsigned int port) { DsClient::GetInstance().Start(port); }

void StopDSClient() { DsClient::GetInstance().Stop(); }

void StopRpcServer() { RpcServer::GetInstance().Stop(); }

void StopNotifier() { Notifier::GetInstance().Stop(); }

void SetUpdateRate(double interval) {
  Dispatcher::GetInstance().SetUpdateRate(interval);
}

std::vector<ConnectionInfo> GetConnections() {
  return Dispatcher::GetInstance().GetConnections();
}

/*
 * Persistent Functions
 */

const char* SavePersistent(StringRef filename) {
  return Storage::GetInstance().SavePersistent(filename, false);
}

const char* LoadPersistent(
    StringRef filename,
    std::function<void(size_t line, const char* msg)> warn) {
  return Storage::GetInstance().LoadPersistent(filename, warn);
}

unsigned long long Now() { return wpi::Now(); }

void SetLogger(LogFunc func, unsigned int min_level) {
  Logger& logger = Logger::GetInstance();
  logger.SetLogger(func);
  logger.set_min_level(min_level);
}

}  // namespace nt
