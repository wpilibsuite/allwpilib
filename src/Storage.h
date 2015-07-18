/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_STORAGE_H_
#define NT_STORAGE_H_

#include <cstddef>
#include <functional>
#include <iosfwd>
#include <memory>
#include <mutex>

#include "llvm/StringMap.h"
#include "support/ConcurrentQueue.h"
#include "ntcore_cpp.h"
#include "SequenceNumber.h"

namespace nt {

class StorageEntry {
 public:
  StorageEntry() : id(0xffff), flags(0) {}

  bool IsPersistent() const { return (flags & NT_PERSISTENT) != 0; }

  std::shared_ptr<Value> value;
  unsigned int id;
  unsigned int flags;
  SequenceNumber seq_num;
};

class Storage {
 public:
  static Storage& GetInstance() {
    if (!m_instance) m_instance.reset(new Storage);
    return *m_instance;
  }
  ~Storage();

  typedef llvm::StringMap<StorageEntry> EntriesMap;

  struct Update {
    std::string name;
    enum Kind { kAssign, kValueUpdate, kFlagsUpdate, kDelete, kDeleteAll };
    Kind kind;
  };
  typedef ConcurrentQueue<Update> UpdateQueue;

  std::mutex& mutex() { return m_mutex; }
  EntriesMap& entries() { return m_entries; }
  const EntriesMap& entries() const { return m_entries; }

  UpdateQueue& updates() { return m_updates; }

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

  mutable std::mutex m_mutex;
  EntriesMap m_entries;
  UpdateQueue m_updates;

  static std::unique_ptr<Storage> m_instance;
};

}  // namespace nt

#endif  // NT_STORAGE_H_
