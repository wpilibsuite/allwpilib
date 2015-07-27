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
#include "support/ConcurrentQueue.h"
#include "atomic_static.h"
#include "ntcore_cpp.h"
#include "SequenceNumber.h"

namespace nt {

class StorageTest;

class StorageEntry {
 public:
  StorageEntry(llvm::StringRef name) : m_name(name), m_id(0xffff) {
    m_flags = 0;
  }

  bool IsPersistent() const { return (m_flags & NT_PERSISTENT) != 0; }

  std::shared_ptr<Value> value() {
#ifdef HAVE_SHARED_PTR_ATOMIC_LOAD
    return std::atomic_load(&m_value);
#else
    std::lock_guard<std::mutex> lock(m_value_mutex);
    return m_value;
#endif
  }
  void set_value(std::shared_ptr<Value> value) {
#ifdef HAVE_SHARED_PTR_ATOMIC_LOAD
    std::atomic_store(&m_value, value);
#else
    std::lock_guard<std::mutex> lock(m_value_mutex);
    m_value = value;
#endif
  }

  unsigned int flags() const { return m_flags; }
  void set_flags(unsigned int flags) { m_flags = flags; }

  StringRef name() const { return m_name; }

  unsigned int id() const { return m_id; }
  void set_id(unsigned int id) { m_id = id; }

  SequenceNumber seq_num() const { return m_seq_num; }
  void set_seq_num(SequenceNumber seq_num) { m_seq_num = seq_num; }

 private:
  // These variables are accessed by both Dispatcher and user, so must use
  // atomic accesses. Unfortunately, atomic shared_ptr is not yet available
  // on most compilers, so we need an explicit mutex instead.
#ifndef HAVE_SHARED_PTR_ATOMIC_LOAD
  std::mutex m_value_mutex;
#endif
  std::shared_ptr<Value> m_value;
  std::atomic_uint m_flags;

  // Only accessed from the Dispatcher, so these are NOT mutex-protected.
  std::string m_name;
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

  struct Update {
    enum Kind {
      kNone,
      kAssign,
      kValueUpdate,
      kFlagsUpdate,
      kValueFlagsUpdate,
      kDelete,
      kDeleteAll
    };
    Update() : flags(0), kind(kNone) {}

    std::shared_ptr<StorageEntry> entry;
    std::shared_ptr<Value> value;
    unsigned int flags;
    Kind kind;
  };
  typedef llvm::StringMap<Update> UpdateMap;

  // Finds, but does not create entry.  Returns nullptr if not found.
  std::shared_ptr<StorageEntry> FindEntry(StringRef name) const;

  // Accessors required by Dispatcher.
  void GetUpdates(UpdateMap* updates, bool* delete_all);
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

  void AddUpdate(std::shared_ptr<StorageEntry> entry, Update::Kind kind);

  typedef llvm::StringMap<std::shared_ptr<StorageEntry>> EntriesMap;

  mutable std::mutex m_mutex;
  EntriesMap m_entries;
  UpdateMap m_updates;
  bool m_updates_delete_all;

  ATOMIC_STATIC_DECL(Storage)
};

}  // namespace nt

#endif  // NT_STORAGE_H_
