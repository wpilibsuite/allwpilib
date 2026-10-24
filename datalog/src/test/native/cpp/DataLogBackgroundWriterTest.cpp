// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/datalog/DataLogBackgroundWriter.hpp"

#include <algorithm>
#include <chrono>
#include <future>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "wpi/datalog/DataLogReader.hpp"
#include "wpi/util/Logger.hpp"
#include "wpi/util/MemoryBuffer.hpp"
#include "wpi/util/fs.hpp"

namespace {

struct MessageLog {
  std::vector<std::string> messages;
  wpi::util::Logger logger{
      [this](unsigned int, const char*, unsigned int, const char* msg) {
        messages.emplace_back(msg);
      }};
};

struct DrainCallbackState {
  DrainCallbackState()
      : releaseFirstDrainFuture{releaseFirstDrain.get_future().share()} {}

  std::promise<void> startupDrainCompleted;
  std::promise<void> firstDrainStarted;
  std::promise<void> releaseFirstDrain;
  std::shared_future<void> releaseFirstDrainFuture;
  std::promise<void> secondDrainStarted;
  int drainCount = 0;
};

struct AutomaticOutputState {
  AutomaticOutputState()
      : outputStarted{outputStartedPromise.get_future().share()} {}

  std::promise<void> outputStartedPromise;
  std::shared_future<void> outputStarted;
  bool outputReported = false;
};

struct TransferDirectories {
  fs::path root =
      fs::temp_directory_path() /
      ("DataLogTransferTest_" +
       std::to_string(
           std::chrono::steady_clock::now().time_since_epoch().count()));
  fs::path source = root / "source";
  fs::path dest = root / "dest";

  TransferDirectories() {
    fs::create_directories(source);
    fs::create_directory(dest);
  }

  ~TransferDirectories() {
    std::error_code ec;
    fs::remove_all(root, ec);
  }
};

template <typename Predicate>
bool WaitFor(Predicate predicate) {
  auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds{2};
  while (!predicate()) {
    if (std::chrono::steady_clock::now() >= deadline) {
      return false;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds{10});
  }
  return true;
}

}  // namespace

TEST_CASE("DataLogBackgroundWriterTest TransferPreservesRecords",
          "[datalog][background-writer]") {
  TransferDirectories dirs;
  std::vector<uint8_t> payload(128 * 1024, 0xa5);
  MessageLog msglog;
  int entry;
  {
    wpi::log::DataLogBackgroundWriter writer{
        msglog.logger, dirs.source.string(), "test.wpilog", 30.0, "extra"};
    entry = writer.Start("raw", "raw", "metadata", 1);
    writer.AppendRaw(entry, payload, 2000);
    SECTION("unchanged filename") {}
    SECTION("failed rename retains original path") {
      writer.SetFilename("missing/test.wpilog");
    }
    writer.Pause();
    writer.SetDir(dirs.dest.string(), 200000);
    REQUIRE(WaitFor([&] { return writer.GetDir() == dirs.dest.string(); }));
    CHECK_FALSE(fs::exists(dirs.source / "test.wpilog"));
    writer.Resume();
    writer.AppendRaw(entry, payload, 3000);
    writer.SetFilename("renamed.wpilog");
    REQUIRE(WaitFor([&] { return fs::exists(dirs.dest / "renamed.wpilog"); }));
    writer.AppendRaw(entry, payload, 4000);
  }
  auto buffer =
      wpi::util::MemoryBuffer::GetFile((dirs.dest / "renamed.wpilog").string());
  REQUIRE(buffer.has_value());
  wpi::log::DataLogReader reader{std::move(*buffer)};
  REQUIRE(reader.IsValid());
  CHECK(reader.GetExtraHeader() == "extra");
  std::vector<int64_t> timestamps;
  int starts = 0;
  for (const auto& record : reader) {
    if (record.IsStart()) {
      ++starts;
    }
    if (record.GetEntry() == entry) {
      CHECK(std::ranges::equal(record.GetRaw(), payload));
      timestamps.push_back(record.GetTimestamp());
    }
  }
  CHECK(starts == 1);
  CHECK(timestamps == std::vector<int64_t>{2000, 3000, 4000});
}

TEST_CASE("DataLogBackgroundWriterTest FailedTransferKeepsOriginal",
          "[datalog][background-writer]") {
  TransferDirectories dirs;
  uint64_t maxSize = 100000;
  bool collision = false;
  SECTION("size limit includes buffered records") {
    maxSize = 12;
  }
  SECTION("missing destination") {
    fs::remove(dirs.dest);
  }
  SECTION("existing destination") {
    collision = true;
    fs::ofstream{dirs.dest / "test.wpilog"} << "preserve me";
  }
  MessageLog msglog;
  int entry;
  {
    wpi::log::DataLogBackgroundWriter writer{
        msglog.logger, dirs.source.string(), "test.wpilog", 30.0};
    entry = writer.Start("integer", "int64", {}, 1);
    writer.AppendInteger(entry, 42, 2);
    writer.SetDir(dirs.dest.string(), maxSize);
    writer.AppendInteger(entry, 43, 3);
    // Destruction drains both the records and the pending transfer request.
  }
  auto buffer =
      wpi::util::MemoryBuffer::GetFile((dirs.source / "test.wpilog").string());
  REQUIRE(buffer.has_value());
  wpi::log::DataLogReader reader{std::move(*buffer)};
  REQUIRE(reader.IsValid());
  std::vector<int64_t> values;
  for (const auto& record : reader) {
    int64_t value;
    if (record.GetEntry() == entry && record.GetInteger(&value)) {
      values.push_back(value);
    }
  }
  CHECK(values == std::vector<int64_t>{42, 43});
  if (collision) {
    CHECK(fs::file_size(dirs.dest / "test.wpilog") == 11);
  } else {
    CHECK_FALSE(fs::exists(dirs.dest / "test.wpilog"));
  }
}

TEST_CASE("DataLogBackgroundWriterTest TransferWithoutBufferedRecords",
          "[datalog][background-writer]") {
  TransferDirectories dirs;
  MessageLog msglog;
  wpi::log::DataLogBackgroundWriter writer{msglog.logger, dirs.source.string(),
                                           "test.wpilog", 0.01};
  REQUIRE(WaitFor([&] {
    std::error_code ec;
    return fs::file_size(dirs.source / "test.wpilog", ec) == 12 && !ec;
  }));
  writer.SetDir(dirs.dest.string(), 12);
  REQUIRE(WaitFor([&] { return writer.GetDir() == dirs.dest.string(); }));
  CHECK(fs::file_size(dirs.dest / "test.wpilog") == 12);
}

TEST_CASE("DataLogBackgroundWriterTest AppendDuringTransfer",
          "[datalog][background-writer]") {
  TransferDirectories dirs;
  std::promise<void> copiedPromise;
  auto copied = copiedPromise.get_future();
  std::promise<void> releasePromise;
  auto release = releasePromise.get_future();
  wpi::util::Logger logger{
      [&](unsigned int, const char*, unsigned int, const char* message) {
        if (std::string_view{message}.starts_with("Transferred log to")) {
          copiedPromise.set_value();
          release.wait();
        }
      }};
  auto writer = std::make_unique<wpi::log::DataLogBackgroundWriter>(
      logger, dirs.source.string(), "test.wpilog", 30.0);
  int entry = writer->Start("integer", "int64", {}, 1);
  writer->AppendInteger(entry, 42, 2);
  writer->SetDir(dirs.dest.string(), 100000);
  bool didCopy =
      copied.wait_for(std::chrono::seconds{2}) == std::future_status::ready;
  if (!didCopy) {
    releasePromise.set_value();
    writer.reset();
    FAIL("background transfer did not complete");
  }
  // Hold the writer in the transfer, then append and request another flush.
  auto producer = std::async(std::launch::async, [&] {
    writer->AppendInteger(entry, 43, 3);
    writer->Flush();
  });
  bool appended =
      producer.wait_for(std::chrono::seconds{1}) == std::future_status::ready;
  releasePromise.set_value();
  producer.get();
  writer.reset();
  REQUIRE(appended);

  auto buffer =
      wpi::util::MemoryBuffer::GetFile((dirs.dest / "test.wpilog").string());
  REQUIRE(buffer.has_value());
  wpi::log::DataLogReader reader{std::move(*buffer)};
  REQUIRE(reader.IsValid());
  std::vector<int64_t> values;
  for (const auto& record : reader) {
    int64_t value;
    if (record.GetEntry() == entry && record.GetInteger(&value)) {
      values.push_back(value);
    }
  }
  CHECK(values == std::vector<int64_t>{42, 43});
}

TEST_CASE("DataLogBackgroundWriterTest LargeHeaderResumesAfterDrain",
          "[datalog][background-writer]") {
  std::string extraHeader(1024 * 1024, 'x');
  MessageLog msglog;
  std::vector<uint8_t> output;
  std::promise<void> headerWrittenPromise;
  auto headerWritten = headerWrittenPromise.get_future();
  bool headerWasWritten = false;
  int entry;
  {
    wpi::log::DataLogBackgroundWriter writer{
        msglog.logger,
        [&](std::span<const uint8_t> data) {
          output.insert(output.end(), data.begin(), data.end());
          if (!headerWasWritten && output.size() >= extraHeader.size() + 12) {
            headerWasWritten = true;
            headerWrittenPromise.set_value();
          }
        },
        30.0, extraHeader};

    REQUIRE(headerWritten.wait_for(std::chrono::seconds{2}) ==
            std::future_status::ready);
    entry = writer.Start("integer", "int64", {}, 1);
    writer.AppendInteger(entry, 42, 2);
  }

  REQUIRE(msglog.messages.size() == 1);
  CHECK(msglog.messages[0].starts_with("outgoing buffers exceeded threshold"));
  wpi::log::DataLogReader reader{
      wpi::util::MemoryBuffer::GetMemBufferCopy(output, "large-header")};
  REQUIRE(reader.IsValid());
  CHECK(reader.GetExtraHeader() == extraHeader);
  bool found = false;
  for (const auto& record : reader) {
    int64_t value;
    if (record.GetEntry() == entry && record.GetInteger(&value) &&
        value == 42) {
      found = true;
    }
  }
  CHECK(found);
}

TEST_CASE("DataLogBackgroundWriterTest ConcurrentDrainDoesNotDeadlock",
          "[datalog][background-writer]") {
  auto msglog = std::make_shared<MessageLog>();
  auto writer = std::make_unique<wpi::log::DataLogBackgroundWriter>(
      // Keep the logger alive if the writer is leaked on a deadlock.
      msglog->logger, [msglog](std::span<const uint8_t>) {}, 0.0);
  auto* writerPtr = writer.get();
  int entry = writerPtr->Start("raw", "raw", {}, 1);
  auto payload = std::make_shared<std::vector<uint8_t>>(2 * 1024 * 1024);
  std::promise<void> complete;
  auto completed = complete.get_future();
  std::thread producer{
      [writerPtr, payload, entry, complete = std::move(complete)]() mutable {
        for (int i = 0; i < 8; ++i) {
          writerPtr->AppendRaw(entry, *payload, i + 2);
          writerPtr->Flush();
        }
        complete.set_value();
      }};

  if (completed.wait_for(std::chrono::seconds{2}) !=
      std::future_status::ready) {
    producer.detach();
    writer.release();
    FAIL("background producer and writer deadlocked");
  }
  producer.join();
  writer.reset();
  CHECK_FALSE(msglog->messages.empty());
  CHECK(msglog->messages.size() <= 8);
  for (const auto& message : msglog->messages) {
    CHECK(message.starts_with("outgoing buffers exceeded threshold"));
  }
}

TEST_CASE("DataLogBackgroundWriterTest NegativePeriodFlushesAutomatically",
          "[datalog][background-writer]") {
  auto callbackState = std::make_shared<AutomaticOutputState>();
  auto writer = std::make_unique<wpi::log::DataLogBackgroundWriter>(
      [callbackState](std::span<const uint8_t> data) {
        if (!data.empty() && !callbackState->outputReported) {
          callbackState->outputReported = true;
          callbackState->outputStartedPromise.set_value();
        }
      },
      -1.0);

  int entry = writer->Start("raw", "raw", {}, 1);
  writer->AppendRaw(entry, std::vector<uint8_t>{1}, 2);

  bool outputWasPrompt =
      callbackState->outputStarted.wait_for(std::chrono::seconds{1}) ==
      std::future_status::ready;
  writer.reset();
  REQUIRE(outputWasPrompt);
}

TEST_CASE("DataLogBackgroundWriterTest FlushWakesAfterBlockedDrain",
          "[datalog][background-writer]") {
  auto callbackState = std::make_shared<DrainCallbackState>();
  auto startupDrainCompleted =
      callbackState->startupDrainCompleted.get_future();
  auto firstDrainStarted = callbackState->firstDrainStarted.get_future();
  auto secondDrainStarted = callbackState->secondDrainStarted.get_future();
  auto writer = std::make_unique<wpi::log::DataLogBackgroundWriter>(
      [callbackState](std::span<const uint8_t> data) {
        if (data.empty()) {
          return;
        }
        if (++callbackState->drainCount == 1) {
          callbackState->startupDrainCompleted.set_value();
        } else if (callbackState->drainCount == 2) {
          callbackState->firstDrainStarted.set_value();
          callbackState->releaseFirstDrainFuture.wait();
        } else if (callbackState->drainCount == 3) {
          callbackState->secondDrainStarted.set_value();
        }
      },
      30.0);

  auto startupDeadline =
      std::chrono::steady_clock::now() + std::chrono::seconds{2};
  do {
    writer->Flush();
    if (startupDrainCompleted.wait_for(std::chrono::milliseconds{10}) ==
        std::future_status::ready) {
      break;
    }
  } while (std::chrono::steady_clock::now() < startupDeadline);

  if (startupDrainCompleted.wait_for(std::chrono::seconds{0}) !=
      std::future_status::ready) {
    callbackState->releaseFirstDrain.set_value();
    writer.reset();
    FAIL("background writer startup drain did not complete");
  }

  int entry = writer->Start("raw", "raw", {}, 1);
  std::vector<uint8_t> payload{1};
  writer->AppendRaw(entry, payload, 2);
  auto firstDrainDeadline =
      std::chrono::steady_clock::now() + std::chrono::seconds{2};
  do {
    writer->Flush();
    if (firstDrainStarted.wait_for(std::chrono::milliseconds{10}) ==
        std::future_status::ready) {
      break;
    }
  } while (std::chrono::steady_clock::now() < firstDrainDeadline);

  if (firstDrainStarted.wait_for(std::chrono::seconds{0}) !=
      std::future_status::ready) {
    callbackState->releaseFirstDrain.set_value();
    writer.reset();
    FAIL("first data drain did not start");
  }

  writer->AppendRaw(entry, payload, 3);
  writer->Flush();
  callbackState->releaseFirstDrain.set_value();

  bool secondDrainWasPrompt =
      secondDrainStarted.wait_for(std::chrono::seconds{1}) ==
      std::future_status::ready;
  writer.reset();
  REQUIRE(secondDrainWasPrompt);
}
