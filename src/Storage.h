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

#include "llvm/StringMap.h"
#include "nt_Value.h"

namespace nt {

class StorageEntry {
 public:
  StorageEntry() : m_flags(0) {}

  std::shared_ptr<Value> value() const { return m_value; }
  void set_value(std::shared_ptr<Value> value) { m_value = value; }

  unsigned int flags() const { return m_flags; }
  void set_flags(unsigned int flags) { m_flags = flags; }

  bool IsPersistent() const { return (m_flags & NT_PERSISTENT) != 0; }

  StorageEntry(const StorageEntry&) = delete;
  StorageEntry& operator=(const StorageEntry&) = delete;

private:
  std::shared_ptr<Value> m_value;
  unsigned int m_flags;
};

class Storage {
 public:
  static Storage& GetInstance() {
    if (!m_instance) m_instance.reset(new Storage);
    return *m_instance;
  }
  ~Storage();

  typedef llvm::StringMap<StorageEntry> EntriesMap;

  EntriesMap& entries() { return m_entries; }
  const EntriesMap& entries() const { return m_entries; }

  void SavePersistent(std::ostream& os) const;
  bool LoadPersistent(
      std::istream& is,
      std::function<void(std::size_t line, const char* msg)> warn);

 private:
  Storage();
  Storage(const Storage&) = delete;
  Storage& operator=(const Storage&) = delete;

  EntriesMap m_entries;

  static std::unique_ptr<Storage> m_instance;
};

}  // namespace nt

#endif  // NT_STORAGE_H_
