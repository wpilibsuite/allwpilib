// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/datalog/DataLogReaderThread.hpp"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <memory>
#include <span>
#include <thread>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "wpi/datalog/DataLogReader.hpp"
#include "wpi/datalog/DataLogWriter.hpp"
#include "wpi/util/MemoryBuffer.hpp"
#include "wpi/util/raw_ostream.hpp"

namespace {
struct EntryCallbackState {
  std::atomic_bool callbackCalled{false};
  std::atomic_bool foundByName{false};
  std::atomic_bool foundById{false};
  std::atomic_bool foundByIteration{false};
};

std::vector<uint8_t> MakeSchemaLog(std::span<const uint8_t> schema) {
  std::vector<uint8_t> output;
  {
    wpi::log::DataLogWriter writer{
        std::make_unique<wpi::util::raw_uvector_ostream>(output)};
    writer.AddSchema("proto:x", "proto:FileDescriptorProto", schema, 1);
    writer.Flush();
  }
  return output;
}

std::vector<uint8_t> MakeDelayedEntryLog() {
  std::vector<uint8_t> output;
  {
    wpi::log::DataLogWriter writer{
        std::make_unique<wpi::util::raw_uvector_ostream>(output)};
    int padding = writer.Start("padding", "int64", {}, 1);
    for (int i = 0; i < 200000; ++i) {
      writer.AppendInteger(padding, i, i + 2);
      if ((i % 1000) == 999) {
        writer.Flush();
      }
    }
    writer.Start("late-entry", "double", {}, 300000);
    writer.Flush();
  }
  return output;
}

bool WaitForDone(const wpi::log::DataLogReaderThread& thread,
                 std::chrono::steady_clock::duration timeout) {
  auto deadline = std::chrono::steady_clock::now() + timeout;
  while (!thread.IsDone() && std::chrono::steady_clock::now() < deadline) {
    std::this_thread::sleep_for(std::chrono::milliseconds{1});
  }
  return thread.IsDone();
}
}  // namespace

TEST_CASE("DataLogReaderThreadTest MalformedProtobufDescriptor",
          "[datalog][reader-thread]") {
  const uint8_t invalidDescriptor[] = {0x80};
  auto output = MakeSchemaLog(invalidDescriptor);
  wpi::log::DataLogReaderThread thread{wpi::log::DataLogReader{
      wpi::util::MemoryBuffer::GetMemBufferCopy(output, "malformed-proto")}};
  REQUIRE(WaitForDone(thread, std::chrono::seconds{2}));
  CHECK(upb_DefPool_FindFileByName(thread.GetProtobufDatabase(), "x") ==
        nullptr);
}

TEST_CASE("DataLogReaderThreadTest EntryCallbackCanQueryEntries",
          "[datalog][reader-thread]") {
  auto output = MakeDelayedEntryLog();
  auto thread = std::make_unique<wpi::log::DataLogReaderThread>(
      wpi::log::DataLogReader{wpi::util::MemoryBuffer::GetMemBufferCopy(
          output, "callback-reentrancy")});
  auto* threadPtr = thread.get();
  auto state = std::make_shared<EntryCallbackState>();
  thread->sigEntryAdded.connect(
      [threadPtr, state](const wpi::log::DataLogReaderEntry& entry) {
        if (entry.name != "late-entry") {
          return;
        }
        state->foundByName = threadPtr->GetEntry(entry.name) != nullptr;
        state->foundById = threadPtr->GetEntry(entry.entry) != nullptr;
        threadPtr->ForEachEntryName(
            [state](const wpi::log::DataLogReaderEntry& item) {
              if (item.name == "late-entry") {
                state->foundByIteration = true;
              }
            });
        state->callbackCalled = true;
      });

  if (!WaitForDone(*thread, std::chrono::seconds{2})) {
    thread.release();
    FAIL("entry callback deadlocked while querying entries");
  }
  CHECK(state->callbackCalled);
  CHECK(state->foundByName);
  CHECK(state->foundById);
  CHECK(state->foundByIteration);
}
