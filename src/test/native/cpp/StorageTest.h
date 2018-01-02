/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015-2018. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NTCORE_STORAGETEST_H_
#define NTCORE_STORAGETEST_H_

#include <functional>
#include <memory>
#include <vector>

#include "Log.h"
#include "MockDispatcher.h"
#include "MockEntryNotifier.h"
#include "MockRpcServer.h"
#include "Storage.h"

namespace nt {

class StorageTest {
 public:
  StorageTest() : storage(notifier, rpc_server, logger), tmp_entry("foobar") {}

  Storage::EntriesMap& entries() { return storage.m_entries; }
  Storage::IdMap& idmap() { return storage.m_idmap; }

  Storage::Entry* GetEntry(StringRef name) {
    auto i = storage.m_entries.find(name);
    return i == storage.m_entries.end() ? &tmp_entry : i->getValue();
  }

  void HookOutgoing(bool server) { storage.SetDispatcher(&dispatcher, server); }

  wpi::Logger logger;
  ::testing::StrictMock<MockEntryNotifier> notifier;
  ::testing::StrictMock<MockRpcServer> rpc_server;
  ::testing::StrictMock<MockDispatcher> dispatcher;
  Storage storage;
  Storage::Entry tmp_entry;
};

}  // namespace nt

#endif  // NTCORE_STORAGETEST_H_
