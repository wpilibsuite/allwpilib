// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/io/WpiLogSource.hpp"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "wpi/datalog/DataLogWriter.hpp"
#include "wpi/util/Logger.hpp"
#include "wpi/util/raw_ostream.hpp"

namespace {

using wpi::filterdesigner::IsNumericType;
using wpi::filterdesigner::Signal;
using wpi::filterdesigner::WpiLogSource;

class WpiLogSourceTest : public ::testing::Test {
 public:
  wpi::util::Logger msglog;
  std::vector<uint8_t> data;
  wpi::log::DataLogWriter log{
      msglog, std::make_unique<wpi::util::raw_uvector_ostream>(data)};
};

TEST(IsNumericTypeTest, RecognizesScalarNumericTypes) {
  EXPECT_TRUE(IsNumericType("double"));
  EXPECT_TRUE(IsNumericType("float"));
  EXPECT_TRUE(IsNumericType("int64"));
  EXPECT_TRUE(IsNumericType("boolean"));
}

TEST(IsNumericTypeTest, RejectsNonScalarTypes) {
  EXPECT_FALSE(IsNumericType("string"));
  EXPECT_FALSE(IsNumericType("double[]"));
  EXPECT_FALSE(IsNumericType("json"));
  EXPECT_FALSE(IsNumericType(""));
}

TEST_F(WpiLogSourceTest, InvalidBufferReturnsNullopt) {
  std::vector<uint8_t> junk{1, 2, 3, 4};
  auto src = WpiLogSource::FromBuffer(junk);
  EXPECT_FALSE(src.has_value());
}

TEST_F(WpiLogSourceTest, ListsAllEntryTypes) {
  wpi::log::DoubleLogEntry d{log, "/d", 0};
  wpi::log::FloatLogEntry f{log, "/f", 0};
  wpi::log::IntegerLogEntry i{log, "/i", 0};
  wpi::log::BooleanLogEntry b{log, "/b", 0};
  wpi::log::StringLogEntry s{log, "/s", 0};
  d.Append(1.0, 1);
  f.Append(1.0f, 1);
  i.Append(1, 1);
  b.Append(true, 1);
  s.Append("hello", 1);
  log.Flush();

  auto src = WpiLogSource::FromBuffer(data);
  ASSERT_TRUE(src.has_value());
  const auto& entries = src->Entries();
  EXPECT_EQ(entries.size(), 5u);

  auto find = [&](std::string_view n) {
    return std::find_if(entries.begin(), entries.end(),
                        [&](const auto& e) { return e.name == n; });
  };
  EXPECT_TRUE(find("/d")->numeric);
  EXPECT_TRUE(find("/f")->numeric);
  EXPECT_TRUE(find("/i")->numeric);
  EXPECT_TRUE(find("/b")->numeric);
  EXPECT_FALSE(find("/s")->numeric);
}

TEST_F(WpiLogSourceTest, LoadEntryDoubleRecoversValuesAndTimestamps) {
  wpi::log::DoubleLogEntry d{log, "accel", 0};
  d.Append(0.5, 1000);     // 0.001 s
  d.Append(0.75, 2500);    // 0.0025 s
  d.Append(1.125, 10000);  // 0.010 s
  log.Flush();

  auto src = WpiLogSource::FromBuffer(data);
  ASSERT_TRUE(src.has_value());
  auto sig = src->LoadEntry("accel");
  ASSERT_TRUE(sig.has_value());
  EXPECT_EQ(sig->name, "accel");
  ASSERT_EQ(sig->values.size(), 3u);
  EXPECT_DOUBLE_EQ(sig->values[0], 0.5);
  EXPECT_DOUBLE_EQ(sig->values[1], 0.75);
  EXPECT_DOUBLE_EQ(sig->values[2], 1.125);
  EXPECT_DOUBLE_EQ(sig->timestamps[0], 0.001);
  EXPECT_DOUBLE_EQ(sig->timestamps[1], 0.0025);
  EXPECT_DOUBLE_EQ(sig->timestamps[2], 0.010);
}

TEST_F(WpiLogSourceTest, LoadEntryIntegerCoercedToDouble) {
  wpi::log::IntegerLogEntry e{log, "count", 0};
  e.Append(42, 1);
  e.Append(-7, 2);
  log.Flush();

  auto src = WpiLogSource::FromBuffer(data);
  ASSERT_TRUE(src.has_value());
  auto sig = src->LoadEntry("count");
  ASSERT_TRUE(sig.has_value());
  ASSERT_EQ(sig->values.size(), 2u);
  EXPECT_DOUBLE_EQ(sig->values[0], 42.0);
  EXPECT_DOUBLE_EQ(sig->values[1], -7.0);
}

TEST_F(WpiLogSourceTest, LoadEntryBooleanMapsToZeroAndOne) {
  wpi::log::BooleanLogEntry e{log, "gate", 0};
  e.Append(false, 1);
  e.Append(true, 2);
  e.Append(false, 3);
  log.Flush();

  auto src = WpiLogSource::FromBuffer(data);
  ASSERT_TRUE(src.has_value());
  auto sig = src->LoadEntry("gate");
  ASSERT_TRUE(sig.has_value());
  ASSERT_EQ(sig->values.size(), 3u);
  EXPECT_DOUBLE_EQ(sig->values[0], 0.0);
  EXPECT_DOUBLE_EQ(sig->values[1], 1.0);
  EXPECT_DOUBLE_EQ(sig->values[2], 0.0);
}

TEST_F(WpiLogSourceTest, LoadEntryFloatPromotedToDouble) {
  wpi::log::FloatLogEntry e{log, "temp", 0};
  e.Append(0.125f, 1);
  e.Append(0.25f, 2);
  log.Flush();

  auto src = WpiLogSource::FromBuffer(data);
  ASSERT_TRUE(src.has_value());
  auto sig = src->LoadEntry("temp");
  ASSERT_TRUE(sig.has_value());
  ASSERT_EQ(sig->values.size(), 2u);
  EXPECT_DOUBLE_EQ(sig->values[0], 0.125);
  EXPECT_DOUBLE_EQ(sig->values[1], 0.25);
}

TEST_F(WpiLogSourceTest, LoadEntryStringReturnsNullopt) {
  wpi::log::StringLogEntry e{log, "status", 0};
  e.Append("ok", 1);
  log.Flush();

  auto src = WpiLogSource::FromBuffer(data);
  ASSERT_TRUE(src.has_value());
  EXPECT_FALSE(src->LoadEntry("status").has_value());
}

TEST_F(WpiLogSourceTest, LoadEntryMissingReturnsNullopt) {
  wpi::log::DoubleLogEntry e{log, "present", 0};
  e.Append(1.0, 1);
  log.Flush();

  auto src = WpiLogSource::FromBuffer(data);
  ASSERT_TRUE(src.has_value());
  EXPECT_FALSE(src->LoadEntry("absent").has_value());
}

TEST_F(WpiLogSourceTest, EntriesExposeCachedLabel) {
  wpi::log::DoubleLogEntry d{log, "/accel/x", 0};
  d.Append(1.0, 1);
  log.Flush();

  auto src = WpiLogSource::FromBuffer(data);
  ASSERT_TRUE(src.has_value());
  const auto& entries = src->Entries();
  ASSERT_EQ(entries.size(), 1u);
  EXPECT_EQ(entries[0].label, "/accel/x  [double]");
}

TEST_F(WpiLogSourceTest, LoadEntryPopulatesCachedSampleRate) {
  wpi::log::DoubleLogEntry d{log, "signal", 0};
  for (int i = 0; i < 10; ++i) {
    d.Append(static_cast<double>(i), (i + 1) * 1000);  // 1 ms spacing → 1 kHz
  }
  log.Flush();

  auto src = WpiLogSource::FromBuffer(data);
  ASSERT_TRUE(src.has_value());
  auto sig = src->LoadEntry("signal");
  ASSERT_TRUE(sig.has_value());
  EXPECT_NEAR(sig->sampleRate, 1000.0, 1e-6);
}

TEST_F(WpiLogSourceTest, LoadEntryAcrossMultipleIdsWhenRestarted) {
  // Finish'd and restarted — same name, different entry IDs.
  wpi::log::DoubleLogEntry first{log, "reopened", 0};
  first.Append(1.0, 10);
  first.Append(2.0, 20);
  first.Finish(25);
  wpi::log::DoubleLogEntry second{log, "reopened", 30};
  second.Append(3.0, 40);
  second.Append(4.0, 50);
  log.Flush();

  auto src = WpiLogSource::FromBuffer(data);
  ASSERT_TRUE(src.has_value());
  // Entries list is de-duped by name — the same name collapses to one row.
  const auto& entries = src->Entries();
  auto count = std::count_if(entries.begin(), entries.end(), [](const auto& e) {
    return e.name == "reopened";
  });
  EXPECT_EQ(count, 1);

  auto sig = src->LoadEntry("reopened");
  ASSERT_TRUE(sig.has_value());
  ASSERT_EQ(sig->values.size(), 4u);
  EXPECT_DOUBLE_EQ(sig->values[0], 1.0);
  EXPECT_DOUBLE_EQ(sig->values[1], 2.0);
  EXPECT_DOUBLE_EQ(sig->values[2], 3.0);
  EXPECT_DOUBLE_EQ(sig->values[3], 4.0);
}

TEST_F(WpiLogSourceTest, FromFileRoundTrips) {
  wpi::log::DoubleLogEntry d{log, "disk", 0};
  d.Append(1.5, 1);
  d.Append(2.5, 2);
  log.Flush();

  auto tmp = std::filesystem::temp_directory_path() /
             ("filterdesigner_test_" +
              std::to_string(
                  std::chrono::steady_clock::now().time_since_epoch().count()) +
              ".wpilog");
  {
    std::ofstream out{tmp, std::ios::binary};
    ASSERT_TRUE(out.is_open());
    out.write(reinterpret_cast<const char*>(data.data()),
              static_cast<std::streamsize>(data.size()));
  }

  auto src = WpiLogSource::FromFile(tmp.string());
  ASSERT_TRUE(src.has_value());
  auto sig = src->LoadEntry("disk");
  ASSERT_TRUE(sig.has_value());
  ASSERT_EQ(sig->values.size(), 2u);
  EXPECT_DOUBLE_EQ(sig->values[0], 1.5);
  EXPECT_DOUBLE_EQ(sig->values[1], 2.5);

  std::filesystem::remove(tmp);
}

TEST_F(WpiLogSourceTest, FromFileMissingReturnsNullopt) {
  EXPECT_FALSE(
      WpiLogSource::FromFile("/definitely/does/not/exist.wpilog").has_value());
}

TEST_F(WpiLogSourceTest, OnlyMatchingEntryRecordsCollected) {
  wpi::log::DoubleLogEntry a{log, "a", 0};
  wpi::log::DoubleLogEntry b{log, "b", 0};
  a.Append(1.0, 10);
  b.Append(99.0, 20);
  a.Append(2.0, 30);
  b.Append(98.0, 40);
  log.Flush();

  auto src = WpiLogSource::FromBuffer(data);
  ASSERT_TRUE(src.has_value());
  auto sigA = src->LoadEntry("a");
  ASSERT_TRUE(sigA.has_value());
  ASSERT_EQ(sigA->values.size(), 2u);
  EXPECT_DOUBLE_EQ(sigA->values[0], 1.0);
  EXPECT_DOUBLE_EQ(sigA->values[1], 2.0);
}

}  // namespace
