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

#include "llvm/StringMap.h"
#include "atomic_static.h"
#include "Message.h"
#include "ntcore_cpp.h"
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
                       unsigned int proto_rev);
  void GetInitialAssignments(std::vector<std::shared_ptr<Message>>* msgs);
  void ApplyInitialAssignments(llvm::ArrayRef<std::shared_ptr<Message>> msgs,
                               bool new_server, unsigned int proto_rev,
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

  void SavePersistent(std::ostream& os) const;
  bool LoadPersistent(
      std::istream& is,
      std::function<void(std::size_t line, const char* msg)> warn);

 private:
  Storage();
  Storage(const Storage&) = delete;
  Storage& operator=(const Storage&) = delete;

  struct Entry {
    Entry(llvm::StringRef name_) : name(name_), flags(0), id(0xffff) {}
    bool IsPersistent() const { return (flags & NT_PERSISTENT) != 0; }

    std::string name;
    std::shared_ptr<Value> value;
    unsigned int flags;
    unsigned int id;
    SequenceNumber seq_num;
  };

  typedef llvm::StringMap<std::shared_ptr<Entry>> EntriesMap;
  typedef std::vector<std::shared_ptr<Entry>> IdMap;

  mutable std::mutex m_mutex;
  EntriesMap m_entries;
  IdMap m_idmap;

  QueueOutgoingFunc m_queue_outgoing;
  bool m_server;

  ATOMIC_STATIC_DECL(Storage)
};

}  // namespace nt

#endif  // NT_STORAGE_H_
