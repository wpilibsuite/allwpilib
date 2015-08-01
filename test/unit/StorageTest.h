/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_TEST_STORAGETEST_H_
#define NT_TEST_STORAGETEST_H_

#include <functional>
#include <memory>
#include <vector>

#include "Storage.h"

namespace nt {

class StorageTest {
 public:
  StorageTest() : tmp_entry("foobar") {}

  Storage::EntriesMap& entries() { return storage.m_entries; }
  Storage::IdMap& idmap() { return storage.m_idmap; }

  Storage::Entry* GetEntry(StringRef name) {
    auto i = storage.m_entries.find(name);
    return i == storage.m_entries.end() ? &tmp_entry : i->getValue().get();
  }

  void HookOutgoing(bool server) {
    using namespace std::placeholders;
    storage.SetOutgoing(
        std::bind(&StorageTest::QueueOutgoing, this, _1, _2, _3), server);
  }

  struct OutgoingData {
    std::shared_ptr<Message> msg;
    NetworkConnection* only;
    NetworkConnection* except;
  };

  void QueueOutgoing(std::shared_ptr<Message> msg, NetworkConnection* only,
                     NetworkConnection* except) {
    outgoing.emplace_back(OutgoingData{msg, only, except});
  }

  Storage storage;
  Storage::Entry tmp_entry;
  std::vector<OutgoingData> outgoing;
};

}  // namespace nt

#endif  // NT_TEST_STORAGETEST_H_
