/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_STORAGE_H_
#define NT_STORAGE_H_

#include <atomic>
#include <cstddef>
#include <fstream>
#include <functional>
#include <iosfwd>
#include <memory>
#include <mutex>
#include <vector>

#include "llvm/DenseMap.h"
#include "llvm/SmallSet.h"
#include "llvm/StringMap.h"
#include "support/atomic_static.h"
#include "Message.h"
#include "Notifier.h"
#include "ntcore_cpp.h"
#include "RpcServer.h"
#include "SequenceNumber.h"

namespace nt {

class NetworkConnection;
class StorageTest;

class Storage {
  friend class StorageTest;

 public:
  static Storage& GetInstance() {
    ATOMIC_STATIC(Storage, instance);
    return instance;
  }
  ~Storage();

  // Accessors required by Dispatcher.  A function pointer is used for
  // generation of outgoing messages to break a dependency loop between
  // Storage and Dispatcher; in operation this is always set to
  // Dispatcher::QueueOutgoing.
  typedef std::function<void(std::shared_ptr<Message> msg,
                             NetworkConnection* only,
                             NetworkConnection* except)>
      QueueOutgoingFunc;
  void SetOutgoing(QueueOutgoingFunc queue_outgoing, bool server);
  void ClearOutgoing();

  // Required for wire protocol 2.0 to get the entry type of an entry when
  // receiving entry updates (because the length/type is not provided in the
  // message itself).  Not used in wire protocol 3.0.
  NT_Type GetEntryType(unsigned int id) const;

  void ProcessIncoming(std::shared_ptr<Message> msg, NetworkConnection* conn,
                       std::weak_ptr<NetworkConnection> conn_weak);
  void GetInitialAssignments(NetworkConnection& conn,
                             std::vector<std::shared_ptr<Message>>* msgs);
  void ApplyInitialAssignments(NetworkConnection& conn,
                               llvm::ArrayRef<std::shared_ptr<Message>> msgs,
                               bool new_server,
                               std::vector<std::shared_ptr<Message>>* out_msgs);

  // User functions.  These are the actual implementations of the corresponding
  // user API functions in ntcore_cpp.
  std::shared_ptr<Value> GetEntryValue(StringRef name) const;
  bool SetDefaultEntryValue(StringRef name, std::shared_ptr<Value> value);
  bool SetEntryValue(StringRef name, std::shared_ptr<Value> value);
  void SetEntryTypeValue(StringRef name, std::shared_ptr<Value> value);
  void SetEntryFlags(StringRef name, unsigned int flags);
  unsigned int GetEntryFlags(StringRef name) const;
  void DeleteEntry(StringRef name);
  void DeleteAllEntries();
  std::vector<EntryInfo> GetEntryInfo(StringRef prefix, unsigned int types);
  void NotifyEntries(StringRef prefix,
                     EntryListenerCallback only = nullptr) const;

  // Filename-based save/load functions.  Used both by periodic saves and
  // accessible directly via the user API.
  const char* SavePersistent(StringRef filename, bool periodic) const;
  const char* LoadPersistent(
      StringRef filename,
      std::function<void(std::size_t line, const char* msg)> warn);

  // Stream-based save/load functions (exposed for testing purposes).  These
  // implement the guts of the filename-based functions.
  void SavePersistent(std::ostream& os, bool periodic) const;
  bool LoadPersistent(
      std::istream& is,
      std::function<void(std::size_t line, const char* msg)> warn);

  // RPC configuration needs to come through here as RPC definitions are
  // actually special Storage value types.
  void CreateRpc(StringRef name, StringRef def, RpcCallback callback);
  void CreatePolledRpc(StringRef name, StringRef def);

  unsigned int CallRpc(StringRef name, StringRef params);
  bool GetRpcResult(bool blocking, unsigned int call_uid, std::string* result);
  bool GetRpcResult(bool blocking, unsigned int call_uid, double time_out,
                    std::string* result);
  void CancelBlockingRpcResult(unsigned int call_uid);

 private:
  Storage();
  Storage(Notifier& notifier, RpcServer& rpcserver);
  Storage(const Storage&) = delete;
  Storage& operator=(const Storage&) = delete;

  // Data for each table entry.
  struct Entry {
    Entry(llvm::StringRef name_)
        : name(name_), flags(0), id(0xffff), rpc_call_uid(0) {}
    bool IsPersistent() const { return (flags & NT_PERSISTENT) != 0; }

    // We redundantly store the name so that it's available when accessing the
    // raw Entry* via the ID map.
    std::string name;

    // The current value and flags.
    std::shared_ptr<Value> value;
    unsigned int flags;

    // Unique ID for this entry as used in network messages.  The value is
    // assigned by the server, so on the client this is 0xffff until an
    // entry assignment is received back from the server.
    unsigned int id;

    // Sequence number for update resolution.
    SequenceNumber seq_num;

    // RPC callback function.  Null if either not an RPC or if the RPC is
    // polled.
    RpcCallback rpc_callback;

    // Last UID used when calling this RPC (primarily for client use).  This
    // is incremented for each call.
    unsigned int rpc_call_uid;
  };

  typedef llvm::StringMap<std::unique_ptr<Entry>> EntriesMap;
  typedef std::vector<Entry*> IdMap;
  typedef llvm::DenseMap<std::pair<unsigned int, unsigned int>, std::string>
      RpcResultMap;
  typedef llvm::SmallSet<unsigned int, 12> RpcBlockingCallSet;

  mutable std::mutex m_mutex;
  EntriesMap m_entries;
  IdMap m_idmap;
  RpcResultMap m_rpc_results;
  RpcBlockingCallSet m_rpc_blocking_calls;
  // If any persistent values have changed
  mutable bool m_persistent_dirty = false;

  // condition variable and termination flag for blocking on a RPC result
  std::atomic_bool m_terminating;
  std::condition_variable m_rpc_results_cond;

  // configured by dispatcher at startup
  QueueOutgoingFunc m_queue_outgoing;
  bool m_server = true;

  // references to singletons (we don't grab them directly for testing purposes)
  Notifier& m_notifier;
  RpcServer& m_rpc_server;

  bool GetPersistentEntries(
      bool periodic,
      std::vector<std::pair<std::string, std::shared_ptr<Value>>>* entries)
      const;
  void DeleteAllEntriesImpl();

  ATOMIC_STATIC_DECL(Storage)
};

}  // namespace nt

#endif  // NT_STORAGE_H_
