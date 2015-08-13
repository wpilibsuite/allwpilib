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
#include <fstream>

#include "Dispatcher.h"
#include "Log.h"
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

void DeleteEntry(StringRef name) {
  Storage::GetInstance().DeleteEntry(name);
}

void DeleteAllEntries() {
  Storage::GetInstance().DeleteAllEntries();
}

std::vector<EntryInfo> GetEntryInfo(StringRef prefix, unsigned int types) {
  return Storage::GetInstance().GetEntryInfo(prefix, types);
}

void Flush() {
  Dispatcher::GetInstance().Flush();
}

/*
 * Callback Creation Functions
 */

unsigned int AddEntryListener(StringRef prefix, EntryListenerCallback callback,
                              bool immediate_notify) {
  Notifier& notifier = Notifier::GetInstance();
  unsigned int uid = notifier.AddEntryListener(prefix, callback);
  notifier.Start();
  if (immediate_notify) Storage::GetInstance().NotifyEntries(prefix);
  return uid;
}

void RemoveEntryListener(unsigned int entry_listener_uid) {
  Notifier::GetInstance().RemoveEntryListener(entry_listener_uid);
}

unsigned int AddConnectionListener(ConnectionListenerCallback callback) {
  Notifier& notifier = Notifier::GetInstance();
  unsigned int uid = notifier.AddConnectionListener(callback);
  Notifier::GetInstance().Start();
  return uid;
}

void RemoveConnectionListener(unsigned int conn_listener_uid) {
  Notifier::GetInstance().RemoveConnectionListener(conn_listener_uid);
}

/*
 * Remote Procedure Call Functions
 */

void CreateRpc(StringRef name, StringRef def, RpcCallback callback) {
  Storage::GetInstance().CreateRpc(name, def, callback);
}

void CreatePolledRpc(StringRef name, StringRef def) {
  Storage::GetInstance().CreatePolledRpc(name, def);
}

bool PollRpc(bool blocking, RpcCallInfo* call_info) {
  return RpcServer::GetInstance().PollRpc(blocking, call_info);
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
  raw_mem_istream is(packed.data(), packed.size());
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
  raw_mem_istream is(packed.data(), packed.size());
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

void StartServer(StringRef persist_filename, const char *listen_address,
                 unsigned int port) {
  Dispatcher& dispatcher = Dispatcher::GetInstance();
  dispatcher.StartServer(listen_address, port);
}

void StopServer() {
  Dispatcher::GetInstance().Stop();
}

void StartClient(const char *server_name, unsigned int port) {
  Dispatcher::GetInstance().StartClient(server_name, port);
}

void StopClient() {
  Dispatcher::GetInstance().Stop();
}

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
  const Storage& storage = Storage::GetInstance();

  std::string fn = filename;
  std::string tmp = filename;
  tmp += ".tmp";
  std::string bak = filename;
  bak += ".bak";

  // start by writing to temporary file
  std::ofstream os(tmp);
  if (!os) return "could not open file";
  storage.SavePersistent(os);
  os.flush();
  if (!os) {
    os.close();
    std::remove(tmp.c_str());
    return "error saving file";
  }

  // safely move to real file
  std::remove(bak.c_str());
  if (std::rename(fn.c_str(), bak.c_str()) != 0)
    return "could not rename real file to backup";
  if (std::rename(tmp.c_str(), fn.c_str()) != 0) {
    std::rename(bak.c_str(), fn.c_str());  // attempt to restore backup
    return "could not rename temp file to real file";
  }
  return nullptr;
}

const char* LoadPersistent(
    StringRef filename,
    std::function<void(size_t line, const char* msg)> warn) {
  Storage& storage = Storage::GetInstance();
  std::ifstream is(filename);
  if (!is) return "could not open file";
  if (!storage.LoadPersistent(is, warn)) return "error reading file";
  return nullptr;
}

void SetLogger(LogFunc func, unsigned int min_level) {
  Logger& logger = Logger::GetInstance();
  logger.SetLogger(func);
  logger.set_min_level(min_level);
}

}  // namespace nt
