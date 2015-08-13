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
#include <functional>
#include <iosfwd>
#include <memory>
#include <mutex>
#include <vector>

#include "llvm/DenseMap.h"
#include "llvm/StringMap.h"
#include "atomic_static.h"
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

  // Accessors required by Dispatcher.
  typedef std::function<void(std::shared_ptr<Message> msg,
                             NetworkConnection* only,
                             NetworkConnection* except)> QueueOutgoingFunc;
  void SetOutgoing(QueueOutgoingFunc queue_outgoing, bool server);
  void ClearOutgoing();

  NT_Type GetEntryType(unsigned int id) const;

  void ProcessIncoming(std::shared_ptr<Message> msg, NetworkConnection* conn,
                       std::weak_ptr<NetworkConnection> conn_weak);
  void GetInitialAssignments(NetworkConnection& conn,
                             std::vector<std::shared_ptr<Message>>* msgs);
  void ApplyInitialAssignments(NetworkConnection& conn,
                               llvm::ArrayRef<std::shared_ptr<Message>> msgs,
                               bool new_server,
                               std::vector<std::shared_ptr<Message>>* out_msgs);

  std::mutex& mutex() { return m_mutex; }

  // User functions
  std::shared_ptr<Value> GetEntryValue(StringRef name) const;
  bool SetEntryValue(StringRef name, std::shared_ptr<Value> value);
  void SetEntryTypeValue(StringRef name, std::shared_ptr<Value> value);
  void SetEntryFlags(StringRef name, unsigned int flags);
  unsigned int GetEntryFlags(StringRef name) const;
  void DeleteEntry(StringRef name);
  void DeleteAllEntries();
  std::vector<EntryInfo> GetEntryInfo(StringRef prefix, unsigned int types);
  void NotifyEntries(StringRef prefix);

  void SavePersistent(std::ostream& os) const;
  bool LoadPersistent(
      std::istream& is,
      std::function<void(std::size_t line, const char* msg)> warn);

  // RPC configuration needs to come through here as RPC definitions are
  // actually special Storage value types.
  void CreateRpc(StringRef name, StringRef def, RpcCallback callback);
  void CreatePolledRpc(StringRef name, StringRef def);

  unsigned int CallRpc(StringRef name, StringRef params);
  bool GetRpcResult(bool blocking, unsigned int call_uid, std::string* result);

 private:
  Storage() : Storage(Notifier::GetInstance(), RpcServer::GetInstance()) {}
  Storage(Notifier& notifier, RpcServer& rpcserver);
  Storage(const Storage&) = delete;
  Storage& operator=(const Storage&) = delete;

  struct Entry {
    Entry(llvm::StringRef name_)
        : name(name_), flags(0), id(0xffff), rpc_call_uid(0) {}
    bool IsPersistent() const { return (flags & NT_PERSISTENT) != 0; }

    std::string name;
    std::shared_ptr<Value> value;
    unsigned int flags;
    unsigned int id;
    SequenceNumber seq_num;
    RpcCallback rpc_callback;
    unsigned int rpc_call_uid;
  };

  typedef llvm::StringMap<std::unique_ptr<Entry>> EntriesMap;
  typedef std::vector<Entry*> IdMap;
  typedef llvm::DenseMap<std::pair<unsigned int, unsigned int>, std::string>
      RpcResultMap;

  mutable std::mutex m_mutex;
  EntriesMap m_entries;
  IdMap m_idmap;
  RpcResultMap m_rpc_results;
  std::atomic_bool m_terminating;
  std::condition_variable m_rpc_results_cond;

  QueueOutgoingFunc m_queue_outgoing;
  bool m_server = true;
  Notifier& m_notifier;
  RpcServer& m_rpc_server;

  ATOMIC_STATIC_DECL(Storage)
};

}  // namespace nt

#endif  // NT_STORAGE_H_
