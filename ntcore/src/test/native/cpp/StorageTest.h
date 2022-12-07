// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "Log.h"
#include "MockDispatcher.h"
#include "Storage.h"

namespace nt {

class StorageTest {
 public:
  StorageTest() : storage(logger), tmp_entry("foobar") {}

  Storage::EntriesMap& entries() { return storage.m_entries; }
  Storage::IdMap& idmap() { return storage.m_idmap; }

  Storage::Entry* GetEntry(std::string_view name) {
    auto i = storage.m_entries.find(name);
    return i == storage.m_entries.end() ? &tmp_entry : i->getValue();
  }

  void HookOutgoing(bool server) { storage.SetDispatcher(&dispatcher, server); }

  wpi::Logger logger;
  ::testing::StrictMock<MockDispatcher> dispatcher;
  Storage storage;
  Storage::Entry tmp_entry;
};

}  // namespace nt
