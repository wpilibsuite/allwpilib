// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/datalog/DataLogReader.hpp"

#include <array>
#include <cstdint>

#include <catch2/catch_test_macros.hpp>

#include "wpi/util/MemoryBuffer.hpp"

TEST_CASE("DataLogReaderTest RejectsOverflowingExtraHeader",
          "[datalog][data-log]") {
  constexpr std::array<uint8_t, 12> data{'W',  'P',  'I',  'L',  'O',  'G',
                                         0x00, 0x01, 0xff, 0xff, 0xff, 0xff};
  wpi::log::DataLogReader reader{
      wpi::util::MemoryBuffer::GetMemBufferCopy(data)};

  CHECK_FALSE(reader.IsValid());
  CHECK(reader.GetVersion() == 0);
  CHECK(reader.GetExtraHeader().empty());
  CHECK(reader.begin() == reader.end());
}

TEST_CASE("DataLogReaderTest RejectsTruncatedExtraHeader",
          "[datalog][data-log]") {
  constexpr std::array<uint8_t, 12> data{'W',  'P',  'I',  'L',  'O',  'G',
                                         0x00, 0x01, 0x01, 0x00, 0x00, 0x00};
  wpi::log::DataLogReader reader{
      wpi::util::MemoryBuffer::GetMemBufferCopy(data)};

  CHECK_FALSE(reader.IsValid());
  CHECK(reader.GetVersion() == 0);
  CHECK(reader.GetExtraHeader().empty());
  CHECK(reader.begin() == reader.end());
}

TEST_CASE("DataLogReaderTest ConvertsFileTimestampToNanoseconds",
          "[datalog][data-log]") {
  constexpr std::array<uint8_t, 20> data{
      'W',  'P',  'I',  'L', 'O', 'G', 0x00, 0x01, 0x00, 0x00,
      0x00, 0x00, 0x00, 1,   4,   4,   'D',  'A',  'T',  'A'};
  wpi::log::DataLogReader reader{
      wpi::util::MemoryBuffer::GetMemBufferCopy(data)};

  REQUIRE(reader.IsValid());
  auto it = reader.begin();
  REQUIRE(it != reader.end());
  CHECK(it->GetEntry() == 1);
  CHECK(it->GetTimestamp() == 4000);
  CHECK(it->GetSize() == 4);
  CHECK(it->GetRaw()[0] == 'D');
}
