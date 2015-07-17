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

#include "Dispatcher.h"
#include "Storage.h"

namespace nt {

/*
 * Table Functions
 */

std::shared_ptr<Value> GetEntryValue(StringRef name) {
  return nullptr;
}

bool SetEntryValue(StringRef name, std::shared_ptr<Value> value) {
  return false;
}

void SetEntryTypeValue(StringRef name, std::shared_ptr<Value> value) {}

void SetEntryFlags(StringRef name, unsigned int flags) {}

unsigned int GetEntryFlags(StringRef name) {
  return 0;
}

void DeleteEntry(StringRef name) {}

void DeleteAllEntries() {}

std::vector<EntryInfo> GetEntryInfo(StringRef prefix, unsigned int types) {
  return std::vector<EntryInfo>();
}

void Flush() {}

/*
 * Callback Creation Functions
 */

unsigned int AddEntryListener(StringRef prefix,
                              EntryListenerCallback callback) {
  return 0;
}

void RemoveEntryListener(unsigned int entry_listener_uid) {}

unsigned int AddConnectionListener(ConnectionListenerCallback callback) {
  return 0;
}

void RemoveConnectionListener(unsigned int conn_listener_uid) {}

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
  Dispatcher& dispatcher = Dispatcher::GetInstance();
  dispatcher.SetIdentity(name);
}

void StartServer(const char *persist_filename, const char *listen_address,
                 unsigned int port) {
  Dispatcher& dispatcher = Dispatcher::GetInstance();
  dispatcher.StartServer(listen_address, port);
}

void StopServer() {
  Dispatcher& dispatcher = Dispatcher::GetInstance();
  dispatcher.Stop();
}

void StartClient(const char *server_name, unsigned int port) {
  Dispatcher& dispatcher = Dispatcher::GetInstance();
  dispatcher.StartClient(server_name, port);
}

void StopClient() {
  Dispatcher& dispatcher = Dispatcher::GetInstance();
  dispatcher.Stop();
}

void SetUpdateRate(double interval) {
  Dispatcher& dispatcher = Dispatcher::GetInstance();
  dispatcher.SetUpdateRate(interval);
}

std::vector<ConnectionInfo> GetConnections() {
  return std::vector<ConnectionInfo>();
}

/*
 * Persistent Functions
 */

const char* SavePersistent(const char* filename) {
  const Storage& storage = Storage::GetInstance();
  std::ofstream os(filename);
  if (!os) return "could not open file";
  storage.SavePersistent(os);
  return nullptr;
}

const char* LoadPersistent(
    const char* filename,
    std::function<void(size_t line, const char* msg)> warn) {
  Storage& storage = Storage::GetInstance();
  std::ifstream is(filename);
  if (!is) return "could not open file";
  if (!storage.LoadPersistent(is, warn)) return "error reading file";
  return nullptr;
}

}  // namespace nt
