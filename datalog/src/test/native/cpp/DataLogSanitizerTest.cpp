// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <chrono>
#include <cstdint>
#include <memory>
#include <thread>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "wpi/datalog/DataLogReader.hpp"
#include "wpi/datalog/DataLogReaderThread.hpp"
#include "wpi/datalog/DataLogWriter.hpp"
#include "wpi/util/MemoryBuffer.hpp"
#include "wpi/util/raw_ostream.hpp"

namespace {
std::vector<uint8_t> MakeValidSchemaLog() {
  std::vector<uint8_t> output;
  {
    wpi::log::DataLogWriter writer{
        std::make_unique<wpi::util::raw_uvector_ostream>(output)};
    const uint8_t descriptor[] = {0x0a, 0x01, 0x78};
    writer.AddSchema("proto:x", "proto:FileDescriptorProto", descriptor, 1);
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

TEST_CASE("DataLogSanitizerTest ReaderReleasesProtobufResources",
          "[datalog][sanitizer]") {
  auto output = MakeValidSchemaLog();
  for (int i = 0; i < 20; ++i) {
    wpi::log::DataLogReaderThread thread{wpi::log::DataLogReader{
        wpi::util::MemoryBuffer::GetMemBufferCopy(output, "valid-proto")}};
    REQUIRE(WaitForDone(thread, std::chrono::seconds{2}));
  }
}
