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
#include "Storage.h"

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

unsigned int CreateRpc(StringRef name, const RpcDefinition& def,
                       RpcCallback callback) {
  return 0;
}

void DeleteRpc(unsigned int rpc_uid) {}

unsigned int CallRpc(StringRef name,
                     ArrayRef<std::shared_ptr<Value>> params) {
  return 0;
}

std::vector<std::shared_ptr<Value>> GetRpcResult(unsigned int result_uid) {
  return std::vector<std::shared_ptr<Value>>();
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
