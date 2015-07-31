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

class StorageEntry {
 public:
  StorageEntry(llvm::StringRef name) : m_name(name), m_flags(0), m_id(0xffff) {}

  bool IsPersistent() const { return (m_flags & NT_PERSISTENT) != 0; }

  std::shared_ptr<Value> value() const { return m_value; }
  void set_value(std::shared_ptr<Value> value) { m_value = value; }

  unsigned int flags() const { return m_flags; }
  void set_flags(unsigned int flags) { m_flags = flags; }

  StringRef name() const { return m_name; }

  unsigned int id() const { return m_id; }
  void set_id(unsigned int id) { m_id = id; }

  SequenceNumber seq_num() const { return m_seq_num; }
  void set_seq_num(SequenceNumber seq_num) { m_seq_num = seq_num; }
  SequenceNumber seq_num_inc() { return ++m_seq_num; }

 private:
  std::string m_name;
  std::shared_ptr<Value> m_value;
  unsigned int m_flags;
  unsigned int m_id;
  SequenceNumber m_seq_num;
};

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
  void SendAssignments(std::function<void(std::shared_ptr<Message>)> send_msg,
                       bool reset_ids);

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

  typedef llvm::StringMap<std::shared_ptr<StorageEntry>> EntriesMap;
  typedef std::vector<std::shared_ptr<StorageEntry>> IdMap;

  mutable std::mutex m_mutex;
  EntriesMap m_entries;
  IdMap m_idmap;

  QueueOutgoingFunc m_queue_outgoing;
  bool m_server;

  ATOMIC_STATIC_DECL(Storage)
};

}  // namespace nt

#endif  // NT_STORAGE_H_
