/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NTCORE_STORAGE_H_
#define NTCORE_STORAGE_H_

#include <stdint.h>

#include <atomic>
#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <wpi/DenseMap.h>
#include <wpi/SmallSet.h>
#include <wpi/StringMap.h>
#include <wpi/condition_variable.h>
#include <wpi/mutex.h>

#include "IStorage.h"
#include "Message.h"
#include "SequenceNumber.h"
#include "ntcore_cpp.h"

namespace wpi {
class Logger;
class raw_istream;
class raw_ostream;
}  // namespace wpi

namespace nt {

class IEntryNotifier;
class INetworkConnection;
class IRpcServer;
class IStorageTest;

class Storage : public IStorage {
  friend class StorageTest;

 public:
  Storage(IEntryNotifier& notifier, IRpcServer& rpcserver, wpi::Logger& logger);
  Storage(const Storage&) = delete;
  Storage& operator=(const Storage&) = delete;

  ~Storage();

  // Accessors required by Dispatcher.  An interface is used for
  // generation of outgoing messages to break a dependency loop between
  // Storage and Dispatcher.
  void SetDispatcher(IDispatcher* dispatcher, bool server) override;
  void ClearDispatcher() override;

  // Required for wire protocol 2.0 to get the entry type of an entry when
  // receiving entry updates (because the length/type is not provided in the
  // message itself).  Not used in wire protocol 3.0.
  NT_Type GetMessageEntryType(unsigned int id) const override;

  void ProcessIncoming(std::shared_ptr<Message> msg, INetworkConnection* conn,
                       std::weak_ptr<INetworkConnection> conn_weak) override;
  void GetInitialAssignments(
      INetworkConnection& conn,
      std::vector<std::shared_ptr<Message>>* msgs) override;
  void ApplyInitialAssignments(
      INetworkConnection& conn, wpi::ArrayRef<std::shared_ptr<Message>> msgs,
      bool new_server,
      std::vector<std::shared_ptr<Message>>* out_msgs) override;

  // User functions.  These are the actual implementations of the corresponding
  // user API functions in ntcore_cpp.
  std::shared_ptr<Value> GetEntryValue(StringRef name) const;
  std::shared_ptr<Value> GetEntryValue(unsigned int local_id) const;

  bool SetDefaultEntryValue(StringRef name, std::shared_ptr<Value> value);
  bool SetDefaultEntryValue(unsigned int local_id,
                            std::shared_ptr<Value> value);

  bool SetEntryValue(StringRef name, std::shared_ptr<Value> value);
  bool SetEntryValue(unsigned int local_id, std::shared_ptr<Value> value);

  void SetEntryTypeValue(StringRef name, std::shared_ptr<Value> value);
  void SetEntryTypeValue(unsigned int local_id, std::shared_ptr<Value> value);

  void SetEntryFlags(StringRef name, unsigned int flags);
  void SetEntryFlags(unsigned int local_id, unsigned int flags);

  unsigned int GetEntryFlags(StringRef name) const;
  unsigned int GetEntryFlags(unsigned int local_id) const;

  void DeleteEntry(StringRef name);
  void DeleteEntry(unsigned int local_id);

  void DeleteAllEntries();

  std::vector<EntryInfo> GetEntryInfo(int inst, const Twine& prefix,
                                      unsigned int types);

  unsigned int AddListener(
      const Twine& prefix,
      std::function<void(const EntryNotification& event)> callback,
      unsigned int flags) const;
  unsigned int AddListener(
      unsigned int local_id,
      std::function<void(const EntryNotification& event)> callback,
      unsigned int flags) const;

  unsigned int AddPolledListener(unsigned int poller_uid, const Twine& prefix,
                                 unsigned int flags) const;
  unsigned int AddPolledListener(unsigned int poller_uid, unsigned int local_id,
                                 unsigned int flags) const;

  // Index-only
  unsigned int GetEntry(const Twine& name);
  std::vector<unsigned int> GetEntries(const Twine& prefix, unsigned int types);
  EntryInfo GetEntryInfo(int inst, unsigned int local_id) const;
  std::string GetEntryName(unsigned int local_id) const;
  NT_Type GetEntryType(unsigned int local_id) const;
  uint64_t GetEntryLastChange(unsigned int local_id) const;

  // Filename-based save/load functions.  Used both by periodic saves and
  // accessible directly via the user API.
  const char* SavePersistent(const Twine& filename,
                             bool periodic) const override;
  const char* LoadPersistent(
      const Twine& filename,
      std::function<void(size_t line, const char* msg)> warn) override;

  const char* SaveEntries(const Twine& filename, const Twine& prefix) const;
  const char* LoadEntries(
      const Twine& filename, const Twine& prefix,
      std::function<void(size_t line, const char* msg)> warn);

  // Stream-based save/load functions (exposed for testing purposes).  These
  // implement the guts of the filename-based functions.
  void SavePersistent(wpi::raw_ostream& os, bool periodic) const;
  bool LoadEntries(wpi::raw_istream& is, const Twine& prefix, bool persistent,
                   std::function<void(size_t line, const char* msg)> warn);

  void SaveEntries(wpi::raw_ostream& os, const Twine& prefix) const;

  // RPC configuration needs to come through here as RPC definitions are
  // actually special Storage value types.
  void CreateRpc(unsigned int local_id, StringRef def, unsigned int rpc_uid);
  unsigned int CallRpc(unsigned int local_id, StringRef params);
  bool GetRpcResult(unsigned int local_id, unsigned int call_uid,
                    std::string* result);
  bool GetRpcResult(unsigned int local_id, unsigned int call_uid,
                    std::string* result, double timeout, bool* timed_out);
  void CancelRpcResult(unsigned int local_id, unsigned int call_uid);

 private:
  // Data for each table entry.
  struct Entry {
    explicit Entry(wpi::StringRef name_) : name(name_) {}
    bool IsPersistent() const { return (flags & NT_PERSISTENT) != 0; }

    // We redundantly store the name so that it's available when accessing the
    // raw Entry* via the ID map.
    std::string name;

    // The current value and flags.
    std::shared_ptr<Value> value;
    unsigned int flags{0};

    // Unique ID for this entry as used in network messages.  The value is
    // assigned by the server, so on the client this is 0xffff until an
    // entry assignment is received back from the server.
    unsigned int id{0xffff};

    // Local ID.
    unsigned int local_id{UINT_MAX};

    // Sequence number for update resolution.
    SequenceNumber seq_num;

    // If value has been written locally.  Used during initial handshake
    // on client to determine whether or not to accept remote changes.
    bool local_write{false};

    // RPC handle.
    unsigned int rpc_uid{UINT_MAX};

    // Last UID used when calling this RPC (primarily for client use).  This
    // is incremented for each call.
    unsigned int rpc_call_uid{0};
  };

  typedef wpi::StringMap<Entry*> EntriesMap;
  typedef std::vector<Entry*> IdMap;
  typedef std::vector<std::unique_ptr<Entry>> LocalMap;
  typedef std::pair<unsigned int, unsigned int> RpcIdPair;
  typedef wpi::DenseMap<RpcIdPair, std::string> RpcResultMap;
  typedef wpi::SmallSet<RpcIdPair, 12> RpcBlockingCallSet;

  mutable wpi::mutex m_mutex;
  EntriesMap m_entries;
  IdMap m_idmap;
  LocalMap m_localmap;
  RpcResultMap m_rpc_results;
  RpcBlockingCallSet m_rpc_blocking_calls;
  // If any persistent values have changed
  mutable bool m_persistent_dirty = false;

  // condition variable and termination flag for blocking on a RPC result
  std::atomic_bool m_terminating;
  wpi::condition_variable m_rpc_results_cond;

  // configured by dispatcher at startup
  IDispatcher* m_dispatcher = nullptr;
  bool m_server = true;

  IEntryNotifier& m_notifier;
  IRpcServer& m_rpc_server;
  wpi::Logger& m_logger;

  void ProcessIncomingEntryAssign(std::shared_ptr<Message> msg,
                                  INetworkConnection* conn);
  void ProcessIncomingEntryUpdate(std::shared_ptr<Message> msg,
                                  INetworkConnection* conn);
  void ProcessIncomingFlagsUpdate(std::shared_ptr<Message> msg,
                                  INetworkConnection* conn);
  void ProcessIncomingEntryDelete(std::shared_ptr<Message> msg,
                                  INetworkConnection* conn);
  void ProcessIncomingClearEntries(std::shared_ptr<Message> msg,
                                   INetworkConnection* conn);
  void ProcessIncomingExecuteRpc(std::shared_ptr<Message> msg,
                                 INetworkConnection* conn,
                                 std::weak_ptr<INetworkConnection> conn_weak);
  void ProcessIncomingRpcResponse(std::shared_ptr<Message> msg,
                                  INetworkConnection* conn);

  bool GetPersistentEntries(
      bool periodic,
      std::vector<std::pair<std::string, std::shared_ptr<Value>>>* entries)
      const;
  bool GetEntries(const Twine& prefix,
                  std::vector<std::pair<std::string, std::shared_ptr<Value>>>*
                      entries) const;
  void SetEntryValueImpl(Entry* entry, std::shared_ptr<Value> value,
                         std::unique_lock<wpi::mutex>& lock, bool local);
  void SetEntryFlagsImpl(Entry* entry, unsigned int flags,
                         std::unique_lock<wpi::mutex>& lock, bool local);
  void DeleteEntryImpl(Entry* entry, std::unique_lock<wpi::mutex>& lock,
                       bool local);

  // Must be called with m_mutex held
  template <typename F>
  void DeleteAllEntriesImpl(bool local, F should_delete);
  void DeleteAllEntriesImpl(bool local);
  Entry* GetOrNew(const Twine& name);
};

}  // namespace nt

#endif  // NTCORE_STORAGE_H_
