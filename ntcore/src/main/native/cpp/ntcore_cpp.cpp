/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <stdint.h>

#include <cassert>
#include <cstdio>
#include <cstdlib>

#include <wpi/timestamp.h>

#include "Handle.h"
#include "InstanceImpl.h"
#include "Log.h"
#include "WireDecoder.h"
#include "WireEncoder.h"
#include "ntcore.h"

namespace nt {

/*
 * Instance Functions
 */

NT_Inst GetDefaultInstance() {
  return Handle{InstanceImpl::GetDefaultIndex(), 0, Handle::kInstance};
}

NT_Inst CreateInstance() {
  return Handle{InstanceImpl::Alloc(), 0, Handle::kInstance};
}

void DestroyInstance(NT_Inst inst) {
  int i = Handle{inst}.GetTypedInst(Handle::kInstance);
  if (i < 0) return;
  InstanceImpl::Destroy(i);
}

NT_Inst GetInstanceFromHandle(NT_Handle handle) {
  Handle h{handle};
  auto type = h.GetType();
  if (type >= Handle::kConnectionListener && type <= Handle::kRpcCallPoller)
    return Handle(h.GetInst(), 0, Handle::kInstance);

  return 0;
}

/*
 * Table Functions
 */

NT_Entry GetEntry(NT_Inst inst, const Twine& name) {
  int i = Handle{inst}.GetTypedInst(Handle::kInstance);
  auto ii = InstanceImpl::Get(i);
  if (!ii) return 0;

  unsigned int id = ii->storage.GetEntry(name);
  if (id == UINT_MAX) return 0;
  return Handle(i, id, Handle::kEntry);
}

std::vector<NT_Entry> GetEntries(NT_Inst inst, const Twine& prefix,
                                 unsigned int types) {
  int i = Handle{inst}.GetTypedInst(Handle::kInstance);
  auto ii = InstanceImpl::Get(i);
  if (!ii) return std::vector<NT_Entry>{};

  auto arr = ii->storage.GetEntries(prefix, types);
  // convert indices to handles
  for (auto& val : arr) val = Handle(i, val, Handle::kEntry);
  return arr;
}

std::string GetEntryName(NT_Entry entry) {
  Handle handle{entry};
  int id = handle.GetTypedIndex(Handle::kEntry);
  auto ii = InstanceImpl::Get(handle.GetInst());
  if (id < 0 || !ii) return std::string{};

  return ii->storage.GetEntryName(id);
}

NT_Type GetEntryType(NT_Entry entry) {
  Handle handle{entry};
  int id = handle.GetTypedIndex(Handle::kEntry);
  auto ii = InstanceImpl::Get(handle.GetInst());
  if (id < 0 || !ii) return NT_UNASSIGNED;

  return ii->storage.GetEntryType(id);
}

uint64_t GetEntryLastChange(NT_Entry entry) {
  Handle handle{entry};
  int id = handle.GetTypedIndex(Handle::kEntry);
  auto ii = InstanceImpl::Get(handle.GetInst());
  if (id < 0 || !ii) return 0;

  return ii->storage.GetEntryLastChange(id);
}

std::shared_ptr<Value> GetEntryValue(StringRef name) {
  return InstanceImpl::GetDefault()->storage.GetEntryValue(name);
}

std::shared_ptr<Value> GetEntryValue(NT_Entry entry) {
  Handle handle{entry};
  int id = handle.GetTypedIndex(Handle::kEntry);
  auto ii = InstanceImpl::Get(handle.GetInst());
  if (id < 0 || !ii) return nullptr;

  return ii->storage.GetEntryValue(id);
}

bool SetDefaultEntryValue(StringRef name, std::shared_ptr<Value> value) {
  return InstanceImpl::GetDefault()->storage.SetDefaultEntryValue(name, value);
}

bool SetDefaultEntryValue(NT_Entry entry, std::shared_ptr<Value> value) {
  Handle handle{entry};
  int id = handle.GetTypedIndex(Handle::kEntry);
  auto ii = InstanceImpl::Get(handle.GetInst());
  if (id < 0 || !ii) return false;

  return ii->storage.SetDefaultEntryValue(id, value);
}

bool SetEntryValue(StringRef name, std::shared_ptr<Value> value) {
  return InstanceImpl::GetDefault()->storage.SetEntryValue(name, value);
}

bool SetEntryValue(NT_Entry entry, std::shared_ptr<Value> value) {
  Handle handle{entry};
  int id = handle.GetTypedIndex(Handle::kEntry);
  auto ii = InstanceImpl::Get(handle.GetInst());
  if (id < 0 || !ii) return false;

  return ii->storage.SetEntryValue(id, value);
}

void SetEntryTypeValue(StringRef name, std::shared_ptr<Value> value) {
  InstanceImpl::GetDefault()->storage.SetEntryTypeValue(name, value);
}

void SetEntryTypeValue(NT_Entry entry, std::shared_ptr<Value> value) {
  Handle handle{entry};
  int id = handle.GetTypedIndex(Handle::kEntry);
  auto ii = InstanceImpl::Get(handle.GetInst());
  if (id < 0 || !ii) return;

  ii->storage.SetEntryTypeValue(id, value);
}

void SetEntryFlags(StringRef name, unsigned int flags) {
  InstanceImpl::GetDefault()->storage.SetEntryFlags(name, flags);
}

void SetEntryFlags(NT_Entry entry, unsigned int flags) {
  Handle handle{entry};
  int id = handle.GetTypedIndex(Handle::kEntry);
  auto ii = InstanceImpl::Get(handle.GetInst());
  if (id < 0 || !ii) return;

  ii->storage.SetEntryFlags(id, flags);
}

unsigned int GetEntryFlags(StringRef name) {
  return InstanceImpl::GetDefault()->storage.GetEntryFlags(name);
}

unsigned int GetEntryFlags(NT_Entry entry) {
  Handle handle{entry};
  int id = handle.GetTypedIndex(Handle::kEntry);
  auto ii = InstanceImpl::Get(handle.GetInst());
  if (id < 0 || !ii) return 0;

  return ii->storage.GetEntryFlags(id);
}

void DeleteEntry(StringRef name) {
  InstanceImpl::GetDefault()->storage.DeleteEntry(name);
}

void DeleteEntry(NT_Entry entry) {
  Handle handle{entry};
  int id = handle.GetTypedIndex(Handle::kEntry);
  auto ii = InstanceImpl::Get(handle.GetInst());
  if (id < 0 || !ii) return;

  ii->storage.DeleteEntry(id);
}

void DeleteAllEntries() {
  InstanceImpl::GetDefault()->storage.DeleteAllEntries();
}

void DeleteAllEntries(NT_Inst inst) {
  int i = Handle{inst}.GetTypedInst(Handle::kInstance);
  auto ii = InstanceImpl::Get(i);
  if (i < 0 || !ii) return;

  ii->storage.DeleteAllEntries();
}

std::vector<EntryInfo> GetEntryInfo(StringRef prefix, unsigned int types) {
  return InstanceImpl::GetDefault()->storage.GetEntryInfo(0, prefix, types);
}

std::vector<EntryInfo> GetEntryInfo(NT_Inst inst, const Twine& prefix,
                                    unsigned int types) {
  int i = Handle{inst}.GetTypedInst(Handle::kInstance);
  auto ii = InstanceImpl::Get(i);
  if (!ii) return std::vector<EntryInfo>{};

  return ii->storage.GetEntryInfo(i, prefix, types);
}

EntryInfo GetEntryInfo(NT_Entry entry) {
  Handle handle{entry};
  int id = handle.GetTypedIndex(Handle::kEntry);
  int i = handle.GetInst();
  auto ii = InstanceImpl::Get(i);
  if (id < 0 || !ii) {
    EntryInfo info;
    info.entry = 0;
    info.type = NT_UNASSIGNED;
    info.flags = 0;
    info.last_change = 0;
    return info;
  }

  return ii->storage.GetEntryInfo(i, id);
}

/*
 * Callback Creation Functions
 */

NT_EntryListener AddEntryListener(StringRef prefix,
                                  EntryListenerCallback callback,
                                  unsigned int flags) {
  return AddEntryListener(
      Handle(InstanceImpl::GetDefaultIndex(), 0, Handle::kInstance), prefix,
      [=](const EntryNotification& event) {
        callback(event.listener, event.name, event.value, event.flags);
      },
      flags);
}

NT_EntryListener AddEntryListener(
    NT_Inst inst, const Twine& prefix,
    std::function<void(const EntryNotification& event)> callback,
    unsigned int flags) {
  int i = Handle{inst}.GetTypedInst(Handle::kInstance);
  auto ii = InstanceImpl::Get(i);
  if (i < 0 || !ii) return 0;

  unsigned int uid = ii->storage.AddListener(prefix, callback, flags);
  return Handle(i, uid, Handle::kEntryListener);
}

NT_EntryListener AddEntryListener(
    NT_Entry entry,
    std::function<void(const EntryNotification& event)> callback,
    unsigned int flags) {
  Handle handle{entry};
  int id = handle.GetTypedIndex(Handle::kEntry);
  int i = handle.GetInst();
  auto ii = InstanceImpl::Get(i);
  if (id < 0 || !ii) return 0;

  unsigned int uid = ii->storage.AddListener(id, callback, flags);
  return Handle(i, uid, Handle::kEntryListener);
}

NT_EntryListenerPoller CreateEntryListenerPoller(NT_Inst inst) {
  int i = Handle{inst}.GetTypedInst(Handle::kInstance);
  auto ii = InstanceImpl::Get(i);
  if (!ii) return 0;

  return Handle(i, ii->entry_notifier.CreatePoller(),
                Handle::kEntryListenerPoller);
}

void DestroyEntryListenerPoller(NT_EntryListenerPoller poller) {
  Handle handle{poller};
  int id = handle.GetTypedIndex(Handle::kEntryListenerPoller);
  auto ii = InstanceImpl::Get(handle.GetInst());
  if (id < 0 || !ii) return;

  ii->entry_notifier.RemovePoller(id);
}

NT_EntryListener AddPolledEntryListener(NT_EntryListenerPoller poller,
                                        const Twine& prefix,
                                        unsigned int flags) {
  Handle handle{poller};
  int id = handle.GetTypedIndex(Handle::kEntryListenerPoller);
  int i = handle.GetInst();
  auto ii = InstanceImpl::Get(i);
  if (id < 0 || !ii) return 0;

  unsigned int uid = ii->storage.AddPolledListener(id, prefix, flags);
  return Handle(i, uid, Handle::kEntryListener);
}

NT_EntryListener AddPolledEntryListener(NT_EntryListenerPoller poller,
                                        NT_Entry entry, unsigned int flags) {
  Handle handle{entry};
  int id = handle.GetTypedIndex(Handle::kEntry);
  int i = handle.GetInst();
  auto ii = InstanceImpl::Get(i);
  if (id < 0 || !ii) return 0;

  Handle phandle{poller};
  int p_id = phandle.GetTypedIndex(Handle::kEntryListenerPoller);
  if (p_id < 0) return 0;
  if (handle.GetInst() != phandle.GetInst()) return 0;

  unsigned int uid = ii->storage.AddPolledListener(p_id, id, flags);
  return Handle(i, uid, Handle::kEntryListener);
}

std::vector<EntryNotification> PollEntryListener(
    NT_EntryListenerPoller poller) {
  Handle handle{poller};
  int id = handle.GetTypedIndex(Handle::kEntryListenerPoller);
  auto ii = InstanceImpl::Get(handle.GetInst());
  if (id < 0 || !ii) return std::vector<EntryNotification>{};

  return ii->entry_notifier.Poll(static_cast<unsigned int>(id));
}

std::vector<EntryNotification> PollEntryListener(NT_EntryListenerPoller poller,
                                                 double timeout,
                                                 bool* timed_out) {
  *timed_out = false;
  Handle handle{poller};
  int id = handle.GetTypedIndex(Handle::kEntryListenerPoller);
  auto ii = InstanceImpl::Get(handle.GetInst());
  if (id < 0 || !ii) return std::vector<EntryNotification>{};

  return ii->entry_notifier.Poll(static_cast<unsigned int>(id), timeout,
                                 timed_out);
}

void CancelPollEntryListener(NT_EntryListenerPoller poller) {
  Handle handle{poller};
  int id = handle.GetTypedIndex(Handle::kEntryListenerPoller);
  auto ii = InstanceImpl::Get(handle.GetInst());
  if (id < 0 || !ii) return;

  ii->entry_notifier.CancelPoll(id);
}

void RemoveEntryListener(NT_EntryListener entry_listener) {
  Handle handle{entry_listener};
  int uid = handle.GetTypedIndex(Handle::kEntryListener);
  auto ii = InstanceImpl::Get(handle.GetInst());
  if (uid < 0 || !ii) return;

  ii->entry_notifier.Remove(uid);
}

bool WaitForEntryListenerQueue(NT_Inst inst, double timeout) {
  int i = Handle{inst}.GetTypedInst(Handle::kInstance);
  auto ii = InstanceImpl::Get(i);
  if (!ii) return true;
  return ii->entry_notifier.WaitForQueue(timeout);
}

NT_ConnectionListener AddConnectionListener(ConnectionListenerCallback callback,
                                            bool immediate_notify) {
  return AddConnectionListener(
      Handle(InstanceImpl::GetDefaultIndex(), 0, Handle::kInstance),
      [=](const ConnectionNotification& event) {
        callback(event.listener, event.connected, event.conn);
      },
      immediate_notify);
}

NT_ConnectionListener AddConnectionListener(
    NT_Inst inst,
    std::function<void(const ConnectionNotification& event)> callback,
    bool immediate_notify) {
  int i = Handle{inst}.GetTypedInst(Handle::kInstance);
  auto ii = InstanceImpl::Get(i);
  if (!ii) return 0;

  unsigned int uid = ii->dispatcher.AddListener(callback, immediate_notify);
  return Handle(i, uid, Handle::kConnectionListener);
}

NT_ConnectionListenerPoller CreateConnectionListenerPoller(NT_Inst inst) {
  int i = Handle{inst}.GetTypedInst(Handle::kInstance);
  auto ii = InstanceImpl::Get(i);
  if (!ii) return 0;

  return Handle(i, ii->connection_notifier.CreatePoller(),
                Handle::kConnectionListenerPoller);
}

void DestroyConnectionListenerPoller(NT_ConnectionListenerPoller poller) {
  Handle handle{poller};
  int id = handle.GetTypedIndex(Handle::kConnectionListenerPoller);
  auto ii = InstanceImpl::Get(handle.GetInst());
  if (id < 0 || !ii) return;

  ii->connection_notifier.RemovePoller(id);
}

NT_ConnectionListener AddPolledConnectionListener(
    NT_ConnectionListenerPoller poller, bool immediate_notify) {
  Handle handle{poller};
  int id = handle.GetTypedIndex(Handle::kConnectionListenerPoller);
  int i = handle.GetInst();
  auto ii = InstanceImpl::Get(i);
  if (id < 0 || !ii) return 0;

  unsigned int uid = ii->dispatcher.AddPolledListener(id, immediate_notify);
  return Handle(i, uid, Handle::kConnectionListener);
}

std::vector<ConnectionNotification> PollConnectionListener(
    NT_ConnectionListenerPoller poller) {
  Handle handle{poller};
  int id = handle.GetTypedIndex(Handle::kConnectionListenerPoller);
  auto ii = InstanceImpl::Get(handle.GetInst());
  if (id < 0 || !ii) return std::vector<ConnectionNotification>{};

  return ii->connection_notifier.Poll(static_cast<unsigned int>(id));
}

std::vector<ConnectionNotification> PollConnectionListener(
    NT_ConnectionListenerPoller poller, double timeout, bool* timed_out) {
  *timed_out = false;
  Handle handle{poller};
  int id = handle.GetTypedIndex(Handle::kConnectionListenerPoller);
  auto ii = InstanceImpl::Get(handle.GetInst());
  if (id < 0 || !ii) return std::vector<ConnectionNotification>{};

  return ii->connection_notifier.Poll(static_cast<unsigned int>(id), timeout,
                                      timed_out);
}

void CancelPollConnectionListener(NT_ConnectionListenerPoller poller) {
  Handle handle{poller};
  int id = handle.GetTypedIndex(Handle::kConnectionListenerPoller);
  auto ii = InstanceImpl::Get(handle.GetInst());
  if (id < 0 || !ii) return;

  ii->connection_notifier.CancelPoll(id);
}

void RemoveConnectionListener(NT_ConnectionListener conn_listener) {
  Handle handle{conn_listener};
  int uid = handle.GetTypedIndex(Handle::kConnectionListener);
  auto ii = InstanceImpl::Get(handle.GetInst());
  if (uid < 0 || !ii) return;

  ii->connection_notifier.Remove(uid);
}

bool WaitForConnectionListenerQueue(NT_Inst inst, double timeout) {
  int i = Handle{inst}.GetTypedInst(Handle::kInstance);
  auto ii = InstanceImpl::Get(i);
  if (!ii) return true;
  return ii->connection_notifier.WaitForQueue(timeout);
}

/*
 * Remote Procedure Call Functions
 */

void CreateRpc(NT_Entry entry, StringRef def,
               std::function<void(const RpcAnswer& answer)> callback) {
  Handle handle{entry};
  int id = handle.GetTypedIndex(Handle::kEntry);
  auto ii = InstanceImpl::Get(handle.GetInst());
  if (id < 0 || !ii) return;

  // only server can create RPCs
  if ((ii->dispatcher.GetNetworkMode() & NT_NET_MODE_SERVER) == 0) return;
  if (def.empty() || !callback) return;

  ii->storage.CreateRpc(id, def, ii->rpc_server.Add(callback));
}

NT_RpcCallPoller CreateRpcCallPoller(NT_Inst inst) {
  int i = Handle{inst}.GetTypedInst(Handle::kInstance);
  auto ii = InstanceImpl::Get(i);
  if (!ii) return 0;

  return Handle(i, ii->rpc_server.CreatePoller(), Handle::kRpcCallPoller);
}

void DestroyRpcCallPoller(NT_RpcCallPoller poller) {
  Handle handle{poller};
  int id = handle.GetTypedIndex(Handle::kRpcCallPoller);
  auto ii = InstanceImpl::Get(handle.GetInst());
  if (id < 0 || !ii) return;

  ii->rpc_server.RemovePoller(id);
}

void CreatePolledRpc(NT_Entry entry, StringRef def, NT_RpcCallPoller poller) {
  Handle handle{entry};
  int id = handle.GetTypedIndex(Handle::kEntry);
  auto ii = InstanceImpl::Get(handle.GetInst());
  if (id < 0 || !ii) return;

  Handle phandle{poller};
  int p_id = phandle.GetTypedIndex(Handle::kRpcCallPoller);
  if (p_id < 0) return;
  if (handle.GetInst() != phandle.GetInst()) return;

  // only server can create RPCs
  if ((ii->dispatcher.GetNetworkMode() & NT_NET_MODE_SERVER) == 0) return;
  if (def.empty()) return;

  ii->storage.CreateRpc(id, def, ii->rpc_server.AddPolled(p_id));
}

std::vector<RpcAnswer> PollRpc(NT_RpcCallPoller poller) {
  Handle handle{poller};
  int id = handle.GetTypedIndex(Handle::kRpcCallPoller);
  auto ii = InstanceImpl::Get(handle.GetInst());
  if (id < 0 || !ii) return std::vector<RpcAnswer>{};

  return ii->rpc_server.Poll(static_cast<unsigned int>(id));
}

std::vector<RpcAnswer> PollRpc(NT_RpcCallPoller poller, double timeout,
                               bool* timed_out) {
  *timed_out = false;
  Handle handle{poller};
  int id = handle.GetTypedIndex(Handle::kRpcCallPoller);
  auto ii = InstanceImpl::Get(handle.GetInst());
  if (id < 0 || !ii) return std::vector<RpcAnswer>{};

  return ii->rpc_server.Poll(static_cast<unsigned int>(id), timeout, timed_out);
}

void CancelPollRpc(NT_RpcCallPoller poller) {
  Handle handle{poller};
  int id = handle.GetTypedIndex(Handle::kRpcCallPoller);
  auto ii = InstanceImpl::Get(handle.GetInst());
  if (id < 0 || !ii) return;

  ii->rpc_server.CancelPoll(id);
}

bool WaitForRpcCallQueue(NT_Inst inst, double timeout) {
  int i = Handle{inst}.GetTypedInst(Handle::kInstance);
  auto ii = InstanceImpl::Get(i);
  if (!ii) return true;
  return ii->rpc_server.WaitForQueue(timeout);
}

bool PostRpcResponse(NT_Entry entry, NT_RpcCall call, StringRef result) {
  Handle handle{entry};
  int id = handle.GetTypedIndex(Handle::kEntry);
  auto ii = InstanceImpl::Get(handle.GetInst());
  if (id < 0 || !ii) return false;

  Handle chandle{call};
  int call_uid = chandle.GetTypedIndex(Handle::kRpcCall);
  if (call_uid < 0) return false;
  if (handle.GetInst() != chandle.GetInst()) return false;

  return ii->rpc_server.PostRpcResponse(id, call_uid, result);
}

NT_RpcCall CallRpc(NT_Entry entry, StringRef params) {
  Handle handle{entry};
  int id = handle.GetTypedIndex(Handle::kEntry);
  int i = handle.GetInst();
  auto ii = InstanceImpl::Get(i);
  if (id < 0 || !ii) return 0;

  unsigned int call_uid = ii->storage.CallRpc(id, params);
  if (call_uid == 0) return 0;
  return Handle(i, call_uid, Handle::kRpcCall);
}

bool GetRpcResult(NT_Entry entry, NT_RpcCall call, std::string* result) {
  Handle handle{entry};
  int id = handle.GetTypedIndex(Handle::kEntry);
  auto ii = InstanceImpl::Get(handle.GetInst());
  if (id < 0 || !ii) return false;

  Handle chandle{call};
  int call_uid = chandle.GetTypedIndex(Handle::kRpcCall);
  if (call_uid < 0) return false;
  if (handle.GetInst() != chandle.GetInst()) return false;

  return ii->storage.GetRpcResult(id, call_uid, result);
}

bool GetRpcResult(NT_Entry entry, NT_RpcCall call, std::string* result,
                  double timeout, bool* timed_out) {
  *timed_out = false;
  Handle handle{entry};
  int id = handle.GetTypedIndex(Handle::kEntry);
  auto ii = InstanceImpl::Get(handle.GetInst());
  if (id < 0 || !ii) return false;

  Handle chandle{call};
  int call_uid = chandle.GetTypedIndex(Handle::kRpcCall);
  if (call_uid < 0) return false;
  if (handle.GetInst() != chandle.GetInst()) return false;

  return ii->storage.GetRpcResult(id, call_uid, result, timeout, timed_out);
}

void CancelRpcResult(NT_Entry entry, NT_RpcCall call) {
  Handle handle{entry};
  int id = handle.GetTypedIndex(Handle::kEntry);
  auto ii = InstanceImpl::Get(handle.GetInst());
  if (id < 0 || !ii) return;

  Handle chandle{call};
  int call_uid = chandle.GetTypedIndex(Handle::kRpcCall);
  if (call_uid < 0) return;
  if (handle.GetInst() != chandle.GetInst()) return;

  ii->storage.CancelRpcResult(id, call_uid);
}

std::string PackRpcDefinition(const RpcDefinition& def) {
  WireEncoder enc(0x0300);
  enc.Write8(def.version);
  enc.WriteString(def.name);

  // parameters
  unsigned int params_size = def.params.size();
  if (params_size > 0xff) params_size = 0xff;
  enc.Write8(params_size);
  for (size_t i = 0; i < params_size; ++i) {
    enc.WriteType(def.params[i].def_value->type());
    enc.WriteString(def.params[i].name);
    enc.WriteValue(*def.params[i].def_value);
  }

  // results
  unsigned int results_size = def.results.size();
  if (results_size > 0xff) results_size = 0xff;
  enc.Write8(results_size);
  for (size_t i = 0; i < results_size; ++i) {
    enc.WriteType(def.results[i].type);
    enc.WriteString(def.results[i].name);
  }

  return enc.ToStringRef();
}

bool UnpackRpcDefinition(StringRef packed, RpcDefinition* def) {
  wpi::raw_mem_istream is(packed.data(), packed.size());
  wpi::Logger logger;
  WireDecoder dec(is, 0x0300, logger);
  if (!dec.Read8(&def->version)) return false;
  if (!dec.ReadString(&def->name)) return false;

  // parameters
  unsigned int params_size;
  if (!dec.Read8(&params_size)) return false;
  def->params.resize(0);
  def->params.reserve(params_size);
  for (size_t i = 0; i < params_size; ++i) {
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
  for (size_t i = 0; i < results_size; ++i) {
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
  wpi::Logger logger;
  WireDecoder dec(is, 0x0300, logger);
  std::vector<std::shared_ptr<Value>> vec;
  for (auto type : types) {
    auto item = dec.ReadValue(type);
    if (!item) return std::vector<std::shared_ptr<Value>>();
    vec.emplace_back(std::move(item));
  }
  return vec;
}

uint64_t Now() { return wpi::Now(); }

/*
 * Client/Server Functions
 */

void SetNetworkIdentity(StringRef name) {
  InstanceImpl::GetDefault()->dispatcher.SetIdentity(name);
}

void SetNetworkIdentity(NT_Inst inst, const Twine& name) {
  auto ii = InstanceImpl::Get(Handle{inst}.GetTypedInst(Handle::kInstance));
  if (!ii) return;

  ii->dispatcher.SetIdentity(name);
}

unsigned int GetNetworkMode() {
  return InstanceImpl::GetDefault()->dispatcher.GetNetworkMode();
}

unsigned int GetNetworkMode(NT_Inst inst) {
  auto ii = InstanceImpl::Get(Handle{inst}.GetTypedInst(Handle::kInstance));
  if (!ii) return 0;

  return ii->dispatcher.GetNetworkMode();
}

void StartServer(StringRef persist_filename, const char* listen_address,
                 unsigned int port) {
  auto ii = InstanceImpl::GetDefault();
  ii->dispatcher.StartServer(persist_filename, listen_address, port);
}

void StartServer(NT_Inst inst, const Twine& persist_filename,
                 const char* listen_address, unsigned int port) {
  auto ii = InstanceImpl::Get(Handle{inst}.GetTypedInst(Handle::kInstance));
  if (!ii) return;

  ii->dispatcher.StartServer(persist_filename, listen_address, port);
}

void StopServer() { InstanceImpl::GetDefault()->dispatcher.Stop(); }

void StopServer(NT_Inst inst) {
  auto ii = InstanceImpl::Get(Handle{inst}.GetTypedInst(Handle::kInstance));
  if (!ii) return;

  ii->dispatcher.Stop();
}

void StartClient() { InstanceImpl::GetDefault()->dispatcher.StartClient(); }

void StartClient(NT_Inst inst) {
  auto ii = InstanceImpl::Get(Handle{inst}.GetTypedInst(Handle::kInstance));
  if (!ii) return;

  ii->dispatcher.StartClient();
}

void StartClient(const char* server_name, unsigned int port) {
  auto ii = InstanceImpl::GetDefault();
  ii->dispatcher.SetServer(server_name, port);
  ii->dispatcher.StartClient();
}

void StartClient(NT_Inst inst, const char* server_name, unsigned int port) {
  auto ii = InstanceImpl::Get(Handle{inst}.GetTypedInst(Handle::kInstance));
  if (!ii) return;

  ii->dispatcher.SetServer(server_name, port);
  ii->dispatcher.StartClient();
}

void StartClient(ArrayRef<std::pair<StringRef, unsigned int>> servers) {
  auto ii = InstanceImpl::GetDefault();
  ii->dispatcher.SetServer(servers);
  ii->dispatcher.StartClient();
}

void StartClient(NT_Inst inst,
                 ArrayRef<std::pair<StringRef, unsigned int>> servers) {
  auto ii = InstanceImpl::Get(Handle{inst}.GetTypedInst(Handle::kInstance));
  if (!ii) return;

  ii->dispatcher.SetServer(servers);
  ii->dispatcher.StartClient();
}

void StartClientTeam(NT_Inst inst, unsigned int team, unsigned int port) {
  auto ii = InstanceImpl::Get(Handle{inst}.GetTypedInst(Handle::kInstance));
  if (!ii) return;

  ii->dispatcher.SetServerTeam(team, port);
  ii->dispatcher.StartClient();
}

void StopClient() { InstanceImpl::GetDefault()->dispatcher.Stop(); }

void StopClient(NT_Inst inst) {
  auto ii = InstanceImpl::Get(Handle{inst}.GetTypedInst(Handle::kInstance));
  if (!ii) return;

  ii->dispatcher.Stop();
}

void SetServer(const char* server_name, unsigned int port) {
  InstanceImpl::GetDefault()->dispatcher.SetServer(server_name, port);
}

void SetServer(NT_Inst inst, const char* server_name, unsigned int port) {
  auto ii = InstanceImpl::Get(Handle{inst}.GetTypedInst(Handle::kInstance));
  if (!ii) return;

  ii->dispatcher.SetServer(server_name, port);
}

void SetServer(ArrayRef<std::pair<StringRef, unsigned int>> servers) {
  InstanceImpl::GetDefault()->dispatcher.SetServer(servers);
}

void SetServer(NT_Inst inst,
               ArrayRef<std::pair<StringRef, unsigned int>> servers) {
  auto ii = InstanceImpl::Get(Handle{inst}.GetTypedInst(Handle::kInstance));
  if (!ii) return;

  ii->dispatcher.SetServer(servers);
}

void SetServerTeam(NT_Inst inst, unsigned int team, unsigned int port) {
  auto ii = InstanceImpl::Get(Handle{inst}.GetTypedInst(Handle::kInstance));
  if (!ii) return;

  ii->dispatcher.SetServerTeam(team, port);
}

void StartDSClient(unsigned int port) {
  InstanceImpl::GetDefault()->ds_client.Start(port);
}

void StartDSClient(NT_Inst inst, unsigned int port) {
  auto ii = InstanceImpl::Get(Handle{inst}.GetTypedInst(Handle::kInstance));
  if (!ii) return;

  ii->ds_client.Start(port);
}

void StopDSClient() { InstanceImpl::GetDefault()->ds_client.Stop(); }

void StopDSClient(NT_Inst inst) {
  auto ii = InstanceImpl::Get(Handle{inst}.GetTypedInst(Handle::kInstance));
  if (!ii) return;

  ii->ds_client.Stop();
}

void SetUpdateRate(double interval) {
  InstanceImpl::GetDefault()->dispatcher.SetUpdateRate(interval);
}

void SetUpdateRate(NT_Inst inst, double interval) {
  auto ii = InstanceImpl::Get(Handle{inst}.GetTypedInst(Handle::kInstance));
  if (!ii) return;

  ii->dispatcher.SetUpdateRate(interval);
}

void Flush() { InstanceImpl::GetDefault()->dispatcher.Flush(); }

void Flush(NT_Inst inst) {
  auto ii = InstanceImpl::Get(Handle{inst}.GetTypedInst(Handle::kInstance));
  if (!ii) return;

  ii->dispatcher.Flush();
}

std::vector<ConnectionInfo> GetConnections() {
  return InstanceImpl::GetDefault()->dispatcher.GetConnections();
}

std::vector<ConnectionInfo> GetConnections(NT_Inst inst) {
  auto ii = InstanceImpl::Get(Handle{inst}.GetTypedInst(Handle::kInstance));
  if (!ii) return std::vector<ConnectionInfo>{};

  return ii->dispatcher.GetConnections();
}

bool IsConnected(NT_Inst inst) {
  auto ii = InstanceImpl::Get(Handle{inst}.GetTypedInst(Handle::kInstance));
  if (!ii) return false;

  return ii->dispatcher.IsConnected();
}

/*
 * Persistent Functions
 */

const char* SavePersistent(StringRef filename) {
  return InstanceImpl::GetDefault()->storage.SavePersistent(filename, false);
}

const char* SavePersistent(NT_Inst inst, const Twine& filename) {
  auto ii = InstanceImpl::Get(Handle{inst}.GetTypedInst(Handle::kInstance));
  if (!ii) return "invalid instance handle";

  return ii->storage.SavePersistent(filename, false);
}

const char* LoadPersistent(
    StringRef filename,
    std::function<void(size_t line, const char* msg)> warn) {
  return InstanceImpl::GetDefault()->storage.LoadPersistent(filename, warn);
}

const char* LoadPersistent(
    NT_Inst inst, const Twine& filename,
    std::function<void(size_t line, const char* msg)> warn) {
  auto ii = InstanceImpl::Get(Handle{inst}.GetTypedInst(Handle::kInstance));
  if (!ii) return "invalid instance handle";

  return ii->storage.LoadPersistent(filename, warn);
}

const char* SaveEntries(NT_Inst inst, const Twine& filename,
                        const Twine& prefix) {
  auto ii = InstanceImpl::Get(Handle{inst}.GetTypedInst(Handle::kInstance));
  if (!ii) return "invalid instance handle";

  return ii->storage.SaveEntries(filename, prefix);
}

const char* LoadEntries(
    NT_Inst inst, const Twine& filename, const Twine& prefix,
    std::function<void(size_t line, const char* msg)> warn) {
  auto ii = InstanceImpl::Get(Handle{inst}.GetTypedInst(Handle::kInstance));
  if (!ii) return "invalid instance handle";

  return ii->storage.LoadEntries(filename, prefix, warn);
}

void SetLogger(LogFunc func, unsigned int min_level) {
  auto ii = InstanceImpl::GetDefault();
  static wpi::mutex mutex;
  static unsigned int logger = 0;
  std::lock_guard<wpi::mutex> lock(mutex);
  if (logger != 0) ii->logger_impl.Remove(logger);
  logger = ii->logger_impl.Add(
      [=](const LogMessage& msg) {
        func(msg.level, msg.filename, msg.line, msg.message.c_str());
      },
      min_level, UINT_MAX);
}

NT_Logger AddLogger(NT_Inst inst,
                    std::function<void(const LogMessage& msg)> func,
                    unsigned int min_level, unsigned int max_level) {
  int i = Handle{inst}.GetTypedInst(Handle::kInstance);
  auto ii = InstanceImpl::Get(i);
  if (!ii) return 0;

  if (min_level < ii->logger.min_level()) ii->logger.set_min_level(min_level);

  return Handle(i, ii->logger_impl.Add(func, min_level, max_level),
                Handle::kLogger);
}

NT_LoggerPoller CreateLoggerPoller(NT_Inst inst) {
  int i = Handle{inst}.GetTypedInst(Handle::kInstance);
  auto ii = InstanceImpl::Get(i);
  if (!ii) return 0;

  return Handle(i, ii->logger_impl.CreatePoller(), Handle::kLoggerPoller);
}

void DestroyLoggerPoller(NT_LoggerPoller poller) {
  Handle handle{poller};
  int id = handle.GetTypedIndex(Handle::kLoggerPoller);
  auto ii = InstanceImpl::Get(handle.GetInst());
  if (id < 0 || !ii) return;

  ii->logger_impl.RemovePoller(id);
}

NT_Logger AddPolledLogger(NT_LoggerPoller poller, unsigned int min_level,
                          unsigned int max_level) {
  Handle handle{poller};
  int id = handle.GetTypedIndex(Handle::kLoggerPoller);
  int i = handle.GetInst();
  auto ii = InstanceImpl::Get(i);
  if (id < 0 || !ii) return 0;

  if (min_level < ii->logger.min_level()) ii->logger.set_min_level(min_level);

  return Handle(i, ii->logger_impl.AddPolled(id, min_level, max_level),
                Handle::kLogger);
}

std::vector<LogMessage> PollLogger(NT_LoggerPoller poller) {
  Handle handle{poller};
  int id = handle.GetTypedIndex(Handle::kLoggerPoller);
  auto ii = InstanceImpl::Get(handle.GetInst());
  if (id < 0 || !ii) return std::vector<LogMessage>{};

  return ii->logger_impl.Poll(static_cast<unsigned int>(id));
}

std::vector<LogMessage> PollLogger(NT_LoggerPoller poller, double timeout,
                                   bool* timed_out) {
  *timed_out = false;
  Handle handle{poller};
  int id = handle.GetTypedIndex(Handle::kLoggerPoller);
  auto ii = InstanceImpl::Get(handle.GetInst());
  if (id < 0 || !ii) return std::vector<LogMessage>{};

  return ii->logger_impl.Poll(static_cast<unsigned int>(id), timeout,
                              timed_out);
}

void CancelPollLogger(NT_LoggerPoller poller) {
  Handle handle{poller};
  int id = handle.GetTypedIndex(Handle::kLoggerPoller);
  auto ii = InstanceImpl::Get(handle.GetInst());
  if (id < 0 || !ii) return;

  ii->logger_impl.CancelPoll(id);
}

void RemoveLogger(NT_Logger logger) {
  Handle handle{logger};
  int uid = handle.GetTypedIndex(Handle::kLogger);
  auto ii = InstanceImpl::Get(handle.GetInst());
  if (uid < 0 || !ii) return;

  ii->logger_impl.Remove(uid);
  ii->logger.set_min_level(ii->logger_impl.GetMinLevel());
}

bool WaitForLoggerQueue(NT_Inst inst, double timeout) {
  int i = Handle{inst}.GetTypedInst(Handle::kInstance);
  auto ii = InstanceImpl::Get(i);
  if (!ii) return true;
  return ii->logger_impl.WaitForQueue(timeout);
}

}  // namespace nt
