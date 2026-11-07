// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cstddef>
#include <cstdint>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "wpi/datalog/DataLogReader.hpp"
#include "wpi/util/MemoryBuffer.hpp"

namespace {
std::vector<uint8_t> MakeSingleRecordLog() {
  return {'W',  'P',  'I',  'L',  'O',  'G',  0x00, 0x01, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x02, 0x2a};
}

size_t CountRecords(const std::vector<uint8_t>& data) {
  wpi::log::DataLogReader reader{
      wpi::util::MemoryBuffer::GetMemBufferCopy(data, "test")};
  REQUIRE(reader.IsValid());

  size_t count = 0;
  for (const auto& record : reader) {
    CHECK(record.GetEntry() == 1);
    ++count;
  }
  return count;
}
}  // namespace

TEST_CASE("DataLogIteratorTest PartialTrailingHeaderIsIgnored",
          "[datalog][data-log]") {
  auto data = MakeSingleRecordLog();
  data.push_back(0x00);

  CHECK(CountRecords(data) == 1);
}

TEST_CASE("DataLogIteratorTest PartialTrailingPayloadIsIgnored",
          "[datalog][data-log]") {
  auto data = MakeSingleRecordLog();
  data.insert(data.end(), {0x00, 0x02, 0x01, 0x03});

  CHECK(CountRecords(data) == 1);
}
