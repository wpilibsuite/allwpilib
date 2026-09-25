// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/io/WpiLogSource.hpp"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "TestAssertions.hpp"
#include "wpi/datalog/DataLogWriter.hpp"
#include "wpi/util/Logger.hpp"
#include "wpi/util/raw_ostream.hpp"

namespace {

using wpi::filterdesigner::IsNumericType;
using wpi::filterdesigner::Signal;
using wpi::filterdesigner::WpiLogSource;

class WpiLogSourceTest {
 public:
  wpi::util::Logger msglog;
  std::vector<uint8_t> data;
  wpi::log::DataLogWriter log{
      msglog, std::make_unique<wpi::util::raw_uvector_ostream>(data)};
};

TEST_CASE("IsNumericTypeTest RecognizesScalarNumericTypes",
          "[filterdesigner]") {
  CHECK(IsNumericType("double"));
  CHECK(IsNumericType("float"));
  CHECK(IsNumericType("int64"));
  CHECK(IsNumericType("boolean"));
}

TEST_CASE("IsNumericTypeTest RejectsNonScalarTypes", "[filterdesigner]") {
  CHECK_FALSE(IsNumericType("string"));
  CHECK_FALSE(IsNumericType("double[]"));
  CHECK_FALSE(IsNumericType("json"));
  CHECK_FALSE(IsNumericType(""));
}

TEST_CASE_METHOD(WpiLogSourceTest,
                 "WpiLogSourceTest InvalidBufferReturnsNullopt",
                 "[filterdesigner]") {
  std::vector<uint8_t> junk{1, 2, 3, 4};
  auto src = WpiLogSource::FromBuffer(junk);
  CHECK_FALSE(src.has_value());
}

TEST_CASE_METHOD(WpiLogSourceTest, "WpiLogSourceTest ListsAllEntryTypes",
                 "[filterdesigner]") {
  wpi::log::DoubleLogEntry d{log, "/d", 0};
  wpi::log::FloatLogEntry f{log, "/f", 0};
  wpi::log::IntegerLogEntry i{log, "/i", 0};
  wpi::log::BooleanLogEntry b{log, "/b", 0};
  wpi::log::StringLogEntry s{log, "/s", 0};
  d.Append(1.0, 1'000);
  f.Append(1.0f, 1'000);
  i.Append(1, 1'000);
  b.Append(true, 1'000);
  s.Append("hello", 1'000);
  log.Flush();

  auto src = WpiLogSource::FromBuffer(data);
  REQUIRE(src.has_value());
  const auto& entries = src->Entries();
  CHECK(entries.size() == 5u);

  auto find = [&](std::string_view n) {
    return std::find_if(entries.begin(), entries.end(),
                        [&](const auto& e) { return e.name == n; });
  };
  CHECK(find("/d")->numeric);
  CHECK(find("/f")->numeric);
  CHECK(find("/i")->numeric);
  CHECK(find("/b")->numeric);
  CHECK_FALSE(find("/s")->numeric);
}

TEST_CASE_METHOD(WpiLogSourceTest,
                 "WpiLogSourceTest LoadEntryDoubleRecoversValuesAndTimestamps",
                 "[filterdesigner]") {
  wpi::log::DoubleLogEntry d{log, "accel", 0};
  // Timestamps are nanoseconds and evenly spaced, so the loader's resample
  // is the identity and these assertions are about decoding alone.
  d.Append(0.5, 1'000'000);    // 0.001 s
  d.Append(0.75, 2'000'000);   // 0.002 s
  d.Append(1.125, 3'000'000);  // 0.003 s
  log.Flush();

  auto src = WpiLogSource::FromBuffer(data);
  REQUIRE(src.has_value());
  auto sig = src->LoadEntry("accel");
  REQUIRE(sig.has_value());
  CHECK(sig->name == "accel");
  REQUIRE(sig->values.size() == 3u);
  CHECK_DOUBLE_EQ(sig->values[0], 0.5);
  CHECK_DOUBLE_EQ(sig->values[1], 0.75);
  CHECK_DOUBLE_EQ(sig->values[2], 1.125);
  CHECK_NEAR(sig->timestamps[0], 0.001, 1e-12);
  CHECK_NEAR(sig->timestamps[1], 0.002, 1e-12);
  CHECK_NEAR(sig->timestamps[2], 0.003, 1e-12);
}

TEST_CASE_METHOD(WpiLogSourceTest,
                 "WpiLogSourceTest LoadEntryOrdersRecordsByTimestamp",
                 "[filterdesigner]") {
  // A writer may append with timestamps that run backwards; the file keeps
  // append order, and everything downstream assumes time order.
  wpi::log::DoubleLogEntry d{log, "accel", 0};
  d.Append(0.75, 2'000'000);
  d.Append(0.5, 1'000'000);
  d.Append(1.125, 3'000'000);
  log.Flush();

  auto src = WpiLogSource::FromBuffer(data);
  REQUIRE(src.has_value());
  auto sig = src->LoadEntryRaw("accel");
  REQUIRE(sig.has_value());
  REQUIRE(sig->values.size() == 3u);
  CHECK_NEAR(sig->timestamps[0], 0.001, 1e-12);
  CHECK_NEAR(sig->timestamps[1], 0.002, 1e-12);
  CHECK_NEAR(sig->timestamps[2], 0.003, 1e-12);
  UNSCOPED_INFO("values must follow their timestamps");
  CHECK_DOUBLE_EQ(sig->values[0], 0.5);
  CHECK_DOUBLE_EQ(sig->values[1], 0.75);
  CHECK_DOUBLE_EQ(sig->values[2], 1.125);
}

TEST_CASE_METHOD(WpiLogSourceTest,
                 "WpiLogSourceTest LoadEntryRawLeavesTheSamplesAsLogged",
                 "[filterdesigner]") {
  // Jittered timestamps around 100 Hz with one dropped sample. LoadEntry
  // resamples that onto a grid; LoadEntryRaw is what a time-range selection
  // has to cut from, so it must hand back exactly what was written.
  wpi::log::DoubleLogEntry d{log, "raw", 0};
  d.Append(1.0, 10'000'000);
  d.Append(2.0, 20'400'000);
  d.Append(3.0, 29'700'000);
  d.Append(4.0, 50'100'000);
  log.Flush();

  auto src = WpiLogSource::FromBuffer(data);
  REQUIRE(src.has_value());
  auto raw = src->LoadEntryRaw("raw");
  REQUIRE(raw.has_value());
  REQUIRE(raw->timestamps.size() == 4u);
  CHECK_NEAR(raw->timestamps[1], 0.0204, 1e-12);
  CHECK_NEAR(raw->timestamps[3], 0.0501, 1e-12);
  UNSCOPED_INFO("no grid was inferred, because none was built");
  CHECK(raw->sampleRate == 0.0);
  CHECK_FALSE(raw->quality.onGrid);

  auto gridded = src->LoadEntry("raw");
  REQUIRE(gridded.has_value());
  CHECK(gridded->quality.onGrid);
  CHECK(gridded->timestamps.size() > raw->timestamps.size());
}

TEST_CASE_METHOD(WpiLogSourceTest,
                 "WpiLogSourceTest LoadEntryRawRejectsWhatLoadEntryRejects",
                 "[filterdesigner]") {
  wpi::log::StringLogEntry s{log, "text", 0};
  s.Append("hello", 1'000);
  log.Flush();

  auto src = WpiLogSource::FromBuffer(data);
  REQUIRE(src.has_value());
  CHECK_FALSE(src->LoadEntryRaw("text").has_value());
  CHECK_FALSE(src->LoadEntryRaw("nonexistent").has_value());
}

TEST_CASE_METHOD(WpiLogSourceTest,
                 "WpiLogSourceTest LoadEntryIntegerCoercedToDouble",
                 "[filterdesigner]") {
  wpi::log::IntegerLogEntry e{log, "count", 0};
  e.Append(42, 1'000);
  e.Append(-7, 2'000);
  log.Flush();

  auto src = WpiLogSource::FromBuffer(data);
  REQUIRE(src.has_value());
  auto sig = src->LoadEntry("count");
  REQUIRE(sig.has_value());
  REQUIRE(sig->values.size() == 2u);
  CHECK_DOUBLE_EQ(sig->values[0], 42.0);
  CHECK_DOUBLE_EQ(sig->values[1], -7.0);
}

TEST_CASE_METHOD(WpiLogSourceTest,
                 "WpiLogSourceTest LoadEntryBooleanMapsToZeroAndOne",
                 "[filterdesigner]") {
  wpi::log::BooleanLogEntry e{log, "gate", 0};
  e.Append(false, 1'000);
  e.Append(true, 2'000);
  e.Append(false, 3'000);
  log.Flush();

  auto src = WpiLogSource::FromBuffer(data);
  REQUIRE(src.has_value());
  auto sig = src->LoadEntry("gate");
  REQUIRE(sig.has_value());
  REQUIRE(sig->values.size() == 3u);
  CHECK(sig->discrete);
  CHECK_DOUBLE_EQ(sig->values[0], 0.0);
  CHECK_DOUBLE_EQ(sig->values[1], 1.0);
  CHECK_DOUBLE_EQ(sig->values[2], 0.0);
}

TEST_CASE_METHOD(WpiLogSourceTest,
                 "WpiLogSourceTest LoadEntryHoldsBooleanAcrossDroppedSample",
                 "[filterdesigner]") {
  // A boolean is discrete, so the grid holds it across the dropout at 4 us
  // rather than interpolating a 0.5 the topic can never have carried.
  wpi::log::BooleanLogEntry e{log, "gate", 0};
  e.Append(false, 1'000);
  e.Append(false, 2'000);
  e.Append(false, 3'000);  // 4 us dropped
  e.Append(true, 5'000);
  e.Append(true, 6'000);
  log.Flush();

  auto src = WpiLogSource::FromBuffer(data);
  REQUIRE(src.has_value());
  auto sig = src->LoadEntry("gate");
  REQUIRE(sig.has_value());
  CHECK(sig->discrete);
  REQUIRE(sig->values.size() == 6u);
  const std::vector<double> expected{0.0, 0.0, 0.0, 0.0, 1.0, 1.0};
  for (std::size_t i = 0; i < expected.size(); ++i) {
    UNSCOPED_INFO("boolean must be held at i=" << i);
    CHECK_DOUBLE_EQ(sig->values[i], expected[i]);
  }
}

TEST_CASE_METHOD(WpiLogSourceTest,
                 "WpiLogSourceTest LoadEntryTreatsIntegersAsContinuous",
                 "[filterdesigner]") {
  // An int64 topic is more often a count than a state enum, so it is
  // interpolated like any other measurement.
  wpi::log::IntegerLogEntry e{log, "ticks", 0};
  e.Append(0, 1'000);
  e.Append(10, 2'000);
  e.Append(20, 3'000);  // 4 us dropped
  e.Append(40, 5'000);
  e.Append(50, 6'000);
  log.Flush();

  auto src = WpiLogSource::FromBuffer(data);
  REQUIRE(src.has_value());
  auto sig = src->LoadEntry("ticks");
  REQUIRE(sig.has_value());
  CHECK_FALSE(sig->discrete);
  REQUIRE(sig->values.size() == 6u);
  // Halfway across the dropout between 20 and 40.
  CHECK_NEAR(sig->values[3], 30.0, 1e-9);
}

TEST_CASE_METHOD(WpiLogSourceTest,
                 "WpiLogSourceTest LoadEntryFloatPromotedToDouble",
                 "[filterdesigner]") {
  wpi::log::FloatLogEntry e{log, "temp", 0};
  e.Append(0.125f, 1'000);
  e.Append(0.25f, 2'000);
  log.Flush();

  auto src = WpiLogSource::FromBuffer(data);
  REQUIRE(src.has_value());
  auto sig = src->LoadEntry("temp");
  REQUIRE(sig.has_value());
  REQUIRE(sig->values.size() == 2u);
  CHECK_DOUBLE_EQ(sig->values[0], 0.125);
  CHECK_DOUBLE_EQ(sig->values[1], 0.25);
}

TEST_CASE_METHOD(WpiLogSourceTest,
                 "WpiLogSourceTest LoadEntryStringReturnsNullopt",
                 "[filterdesigner]") {
  wpi::log::StringLogEntry e{log, "status", 0};
  e.Append("ok", 1'000);
  log.Flush();

  auto src = WpiLogSource::FromBuffer(data);
  REQUIRE(src.has_value());
  CHECK_FALSE(src->LoadEntry("status").has_value());
}

TEST_CASE_METHOD(WpiLogSourceTest,
                 "WpiLogSourceTest LoadEntryMissingReturnsNullopt",
                 "[filterdesigner]") {
  wpi::log::DoubleLogEntry e{log, "present", 0};
  e.Append(1.0, 1'000);
  log.Flush();

  auto src = WpiLogSource::FromBuffer(data);
  REQUIRE(src.has_value());
  CHECK_FALSE(src->LoadEntry("absent").has_value());
}

TEST_CASE_METHOD(WpiLogSourceTest,
                 "WpiLogSourceTest LoadEntryPopulatesCachedSampleRate",
                 "[filterdesigner]") {
  wpi::log::DoubleLogEntry d{log, "signal", 0};
  for (int i = 0; i < 10; ++i) {
    d.Append(static_cast<double>(i),
             (i + 1) * 1'000'000);  // 1 ms spacing → 1 kHz
  }
  log.Flush();

  auto src = WpiLogSource::FromBuffer(data);
  REQUIRE(src.has_value());
  auto sig = src->LoadEntry("signal");
  REQUIRE(sig.has_value());
  CHECK_NEAR(sig->sampleRate, 1000.0, 1e-6);
}

TEST_CASE_METHOD(WpiLogSourceTest,
                 "WpiLogSourceTest LoadEntryResamplesOntoUniformGrid",
                 "[filterdesigner]") {
  // What a real log looks like: nominally 1 kHz, timestamps off by up to a
  // fifth of a period, and the sample at 6 ms never logged. The loader still
  // hands downstream a uniform grid.
  wpi::log::DoubleLogEntry d{log, "jittery", 0};
  // No zero timestamps — Append treats 0 as "now" rather than as t = 0.
  const int64_t nanos[] = {1'000'000, 2'000'000, 3'000'000, 4'200'000,
                           5'000'000,  // 6 ms dropped
                           7'000'000, 8'000'000, 8'800'000, 10'000'000};
  for (std::size_t i = 0; i < std::size(nanos); ++i) {
    d.Append(static_cast<double>(i), nanos[i]);
  }
  log.Flush();

  auto src = WpiLogSource::FromBuffer(data);
  REQUIRE(src.has_value());
  auto sig = src->LoadEntry("jittery");
  REQUIRE(sig.has_value());

  CHECK_NEAR(sig->sampleRate, 1000.0, 1e-9);
  REQUIRE(sig->values.size() == 10u);
  REQUIRE(sig->timestamps.size() == 10u);
  CHECK(sig->quality.onGrid);
  for (std::size_t i = 0; i < sig->timestamps.size(); ++i) {
    UNSCOPED_INFO("timestamp must land on the grid at i=" << i);
    CHECK_NEAR(sig->timestamps[i], 0.001 + static_cast<double>(i) * 0.001,
               1e-12);
  }
  // Slots landing on a record read it back exactly; the rest sit on the line
  // between their neighbours.
  const std::vector<double> expected{0.0, 1.0, 2.0, 2.0 + 5.0 / 6.0, 4.0,
                                     4.5, 5.0, 6.0, 7.0 + 1.0 / 6.0, 8.0};
  for (std::size_t i = 0; i < expected.size(); ++i) {
    UNSCOPED_INFO("value must be interpolated at i=" << i);
    CHECK_NEAR(sig->values[i], expected[i], 1e-9);
  }
  CHECK_NEAR(sig->quality.filled, 0.1, 1e-12);
  CHECK_NEAR(sig->quality.longestGap, 0.002, 1e-9);
  CHECK_NEAR(sig->quality.jitter, 0.2, 1e-9);
}

TEST_CASE_METHOD(WpiLogSourceTest,
                 "WpiLogSourceTest LoadEntryAcrossMultipleIdsWhenRestarted",
                 "[filterdesigner]") {
  // Finish'd and restarted — same name, different entry IDs.
  wpi::log::DoubleLogEntry first{log, "reopened", 0};
  first.Append(1.0, 10'000);
  first.Append(2.0, 20'000);
  first.Finish(25);
  wpi::log::DoubleLogEntry second{log, "reopened", 30};
  // On the same 10 us grid as the first entry, so the loader's resample
  // doesn't fill across the restart and obscure the merge.
  second.Append(3.0, 30'000);
  second.Append(4.0, 40'000);
  log.Flush();

  auto src = WpiLogSource::FromBuffer(data);
  REQUIRE(src.has_value());
  // Entries list is de-duped by name — the same name collapses to one row.
  const auto& entries = src->Entries();
  auto count = std::count_if(entries.begin(), entries.end(), [](const auto& e) {
    return e.name == "reopened";
  });
  CHECK(count == 1);

  auto sig = src->LoadEntry("reopened");
  REQUIRE(sig.has_value());
  REQUIRE(sig->values.size() == 4u);
  CHECK_DOUBLE_EQ(sig->values[0], 1.0);
  CHECK_DOUBLE_EQ(sig->values[1], 2.0);
  CHECK_DOUBLE_EQ(sig->values[2], 3.0);
  CHECK_DOUBLE_EQ(sig->values[3], 4.0);
}

// Hand-built WPILOG bytes. DataLogWriter never reuses an entry id, but the
// format allows one to be Finish'd and reissued to a different name, and a
// third-party writer may do so.
class RawWpiLog {
 public:
  RawWpiLog() {
    static const uint8_t header[] = {'W', 'P', 'I', 'L', 'O', 'G',
                                     0,   1,   0,   0,   0,   0};
    bytes.insert(bytes.end(), std::begin(header), std::end(header));
  }

  void Start(int id, std::string_view name, std::string_view type,
             int64_t micros) {
    std::vector<uint8_t> payload{0};  // control: start
    U32(payload, static_cast<uint32_t>(id));
    Str(payload, name);
    Str(payload, type);
    Str(payload, "");
    Record(0, micros, payload);
  }

  void Finish(int id, int64_t micros) {
    std::vector<uint8_t> payload{1};  // control: finish
    U32(payload, static_cast<uint32_t>(id));
    Record(0, micros, payload);
  }

  void Double(int id, double value, int64_t micros) {
    std::vector<uint8_t> payload(8);
    std::memcpy(payload.data(), &value, 8);
    Record(id, micros, payload);
  }

  void Int64(int id, int64_t value, int64_t micros) {
    std::vector<uint8_t> payload(8);
    std::memcpy(payload.data(), &value, 8);
    Record(id, micros, payload);
  }

  void Boolean(int id, bool value, int64_t micros) {
    std::vector<uint8_t> payload{static_cast<uint8_t>(value ? 1 : 0)};
    Record(id, micros, payload);
  }

  // A string record's payload is the raw text; the record header carries its
  // length.
  void String(int id, std::string_view value, int64_t micros) {
    std::vector<uint8_t> payload(value.begin(), value.end());
    Record(id, micros, payload);
  }

  std::vector<uint8_t> bytes;

 private:
  static void U32(std::vector<uint8_t>& out, uint32_t v) {
    for (int i = 0; i < 4; ++i) {
      out.push_back(static_cast<uint8_t>(v >> (8 * i)));
    }
  }
  static void Str(std::vector<uint8_t>& out, std::string_view s) {
    U32(out, static_cast<uint32_t>(s.size()));
    out.insert(out.end(), s.begin(), s.end());
  }
  void Record(int id, int64_t micros, const std::vector<uint8_t>& payload) {
    // Fixed-width header: 4-byte id, 4-byte size, 8-byte timestamp.
    bytes.push_back(0x7F);
    U32(bytes, static_cast<uint32_t>(id));
    U32(bytes, static_cast<uint32_t>(payload.size()));
    for (int i = 0; i < 8; ++i) {
      bytes.push_back(static_cast<uint8_t>(micros >> (8 * i)));
    }
    bytes.insert(bytes.end(), payload.begin(), payload.end());
  }
};

TEST_CASE("WpiLogSourceTest ReusedIdOnlyMatchesWhileLiveForName",
          "[filterdesigner]") {
  RawWpiLog raw;
  raw.Start(1, "first", "double", 1'000);
  raw.Double(1, 10.0, 2'000);
  raw.Double(1, 11.0, 3'000);
  raw.Finish(1, 4'000);
  raw.Start(1, "second", "double", 5'000);
  raw.Double(1, 20.0, 6'000);
  raw.Double(1, 21.0, 7'000);

  auto src = WpiLogSource::FromBuffer(raw.bytes);
  REQUIRE(src.has_value());
  CHECK(src->Entries().size() == 2u);

  auto first = src->LoadEntryRaw("first");
  REQUIRE(first.has_value());
  UNSCOPED_INFO("records logged after the Finish belong to the other name");
  REQUIRE(first->values.size() == 2u);
  CHECK_DOUBLE_EQ(first->values[0], 10.0);
  CHECK_DOUBLE_EQ(first->values[1], 11.0);

  auto second = src->LoadEntryRaw("second");
  REQUIRE(second.has_value());
  REQUIRE(second->values.size() == 2u);
  CHECK_DOUBLE_EQ(second->values[0], 20.0);
  CHECK_DOUBLE_EQ(second->values[1], 21.0);
}

TEST_CASE("WpiLogSourceTest NameTurningNumericBecomesSelectable",
          "[filterdesigner]") {
  // The name opens as a string and comes back as a double. Judging it by its
  // first announcement alone greys it out in the picker for good, even
  // though the second half of the file is a perfectly good timeseries.
  RawWpiLog raw;
  raw.Start(1, "s", "string", 1'000);
  raw.String(1, "hello", 2'000);
  raw.Finish(1, 3'000);
  raw.Start(1, "s", "double", 4'000);
  raw.Double(1, 4.5, 5'000);

  auto src = WpiLogSource::FromBuffer(raw.bytes);
  REQUIRE(src.has_value());
  REQUIRE(src->Entries().size() == 1u);
  CHECK(src->Entries()[0].numeric);
  UNSCOPED_INFO("the row is labelled with the type it will actually load");
  CHECK(src->Entries()[0].type == "double");

  auto sig = src->LoadEntryRaw("s");
  REQUIRE(sig.has_value());
  UNSCOPED_INFO("the string lifetime contributes nothing");
  REQUIRE(sig->values.size() == 1u);
  CHECK_DOUBLE_EQ(sig->values[0], 4.5);
}

TEST_CASE("WpiLogSourceTest NameLosingNumericKeepsItsNumericType",
          "[filterdesigner]") {
  // The reverse order: a later non-numeric lifetime must not take the label
  // away from the numeric one that already earned it.
  RawWpiLog raw;
  raw.Start(1, "m", "double", 1'000);
  raw.Double(1, 1.5, 2'000);
  raw.Finish(1, 3'000);
  raw.Start(1, "m", "string", 4'000);
  raw.String(1, "later", 5'000);

  auto src = WpiLogSource::FromBuffer(raw.bytes);
  REQUIRE(src.has_value());
  REQUIRE(src->Entries().size() == 1u);
  CHECK(src->Entries()[0].numeric);
  CHECK(src->Entries()[0].type == "double");

  auto sig = src->LoadEntryRaw("m");
  REQUIRE(sig.has_value());
  REQUIRE(sig->values.size() == 1u);
  CHECK_DOUBLE_EQ(sig->values[0], 1.5);
}

TEST_CASE("WpiLogSourceTest NameThatIsNeverNumericStaysUnloadable",
          "[filterdesigner]") {
  RawWpiLog raw;
  raw.Start(1, "t", "string", 1'000);
  raw.String(1, "one", 2'000);
  raw.Finish(1, 3'000);
  raw.Start(1, "t", "string[]", 4'000);

  auto src = WpiLogSource::FromBuffer(raw.bytes);
  REQUIRE(src.has_value());
  REQUIRE(src->Entries().size() == 1u);
  CHECK_FALSE(src->Entries()[0].numeric);
  CHECK(src->Entries()[0].type == "string");
  CHECK_FALSE(src->LoadEntryRaw("t").has_value());
}

TEST_CASE("WpiLogSourceTest RestartedEntryDecodedWithItsOwnType",
          "[filterdesigner]") {
  // Once an entry is finished its name may start again under another type —
  // DataLog::StartImpl only refuses a mismatch while the entry is still
  // referenced. Both lifetimes here are eight bytes wide, so decoding the
  // second with the first's type does not fail, it just bit-casts a 7 into
  // a denormal near 3.5e-323.
  RawWpiLog raw;
  raw.Start(1, "x", "double", 1'000);
  raw.Double(1, 2.5, 2'000);
  raw.Finish(1, 3'000);
  raw.Start(1, "x", "int64", 4'000);
  raw.Int64(1, 7, 5'000);

  auto src = WpiLogSource::FromBuffer(raw.bytes);
  REQUIRE(src.has_value());

  auto sig = src->LoadEntryRaw("x");
  REQUIRE(sig.has_value());
  REQUIRE(sig->values.size() == 2u);
  CHECK_DOUBLE_EQ(sig->values[0], 2.5);
  CHECK_DOUBLE_EQ(sig->values[1], 7.0);
  UNSCOPED_INFO("a name with a non-boolean lifetime is not discrete");
  CHECK_FALSE(sig->discrete);
}

TEST_CASE("WpiLogSourceTest RestartedEntryOfAnotherWidthIsNotDropped",
          "[filterdesigner]") {
  // A boolean payload is one byte, so decoding it as the double the name
  // opened with fails outright and the samples vanish without a trace.
  RawWpiLog raw;
  raw.Start(1, "y", "double", 1'000);
  raw.Double(1, 1.5, 2'000);
  raw.Finish(1, 3'000);
  raw.Start(1, "y", "boolean", 4'000);
  raw.Boolean(1, true, 5'000);
  raw.Boolean(1, false, 6'000);

  auto src = WpiLogSource::FromBuffer(raw.bytes);
  REQUIRE(src.has_value());

  auto sig = src->LoadEntryRaw("y");
  REQUIRE(sig.has_value());
  REQUIRE(sig->values.size() == 3u);
  CHECK_DOUBLE_EQ(sig->values[0], 1.5);
  CHECK_DOUBLE_EQ(sig->values[1], 1.0);
  CHECK_DOUBLE_EQ(sig->values[2], 0.0);
}

TEST_CASE("WpiLogSourceTest AllBooleanLifetimesStayDiscrete",
          "[filterdesigner]") {
  RawWpiLog raw;
  raw.Start(1, "b", "boolean", 1'000);
  raw.Boolean(1, true, 2'000);
  raw.Finish(1, 3'000);
  raw.Start(1, "b", "boolean", 4'000);
  raw.Boolean(1, false, 5'000);

  auto src = WpiLogSource::FromBuffer(raw.bytes);
  REQUIRE(src.has_value());

  auto sig = src->LoadEntryRaw("b");
  REQUIRE(sig.has_value());
  REQUIRE(sig->values.size() == 2u);
  CHECK(sig->discrete);
}

TEST_CASE_METHOD(WpiLogSourceTest, "WpiLogSourceTest FromFileRoundTrips",
                 "[filterdesigner]") {
  wpi::log::DoubleLogEntry d{log, "disk", 0};
  d.Append(1.5, 1'000);
  d.Append(2.5, 2'000);
  log.Flush();

  auto tmp = std::filesystem::temp_directory_path() /
             ("filterdesigner_test_" +
              std::to_string(
                  std::chrono::steady_clock::now().time_since_epoch().count()) +
              ".wpilog");
  {
    std::ofstream out{tmp, std::ios::binary};
    REQUIRE(out.is_open());
    out.write(reinterpret_cast<const char*>(data.data()),
              static_cast<std::streamsize>(data.size()));
  }

  auto src = WpiLogSource::FromFile(tmp.string());
  REQUIRE(src.has_value());
  auto sig = src->LoadEntry("disk");
  REQUIRE(sig.has_value());
  REQUIRE(sig->values.size() == 2u);
  CHECK_DOUBLE_EQ(sig->values[0], 1.5);
  CHECK_DOUBLE_EQ(sig->values[1], 2.5);

  std::filesystem::remove(tmp);
}

TEST_CASE_METHOD(WpiLogSourceTest,
                 "WpiLogSourceTest FromFileMissingReturnsNullopt",
                 "[filterdesigner]") {
  CHECK_FALSE(
      WpiLogSource::FromFile("/definitely/does/not/exist.wpilog").has_value());
}

TEST_CASE_METHOD(WpiLogSourceTest,
                 "WpiLogSourceTest OnlyMatchingEntryRecordsCollected",
                 "[filterdesigner]") {
  wpi::log::DoubleLogEntry a{log, "a", 0};
  wpi::log::DoubleLogEntry b{log, "b", 0};
  a.Append(1.0, 10'000);
  b.Append(99.0, 20'000);
  a.Append(2.0, 30'000);
  b.Append(98.0, 40'000);
  log.Flush();

  auto src = WpiLogSource::FromBuffer(data);
  REQUIRE(src.has_value());
  auto sigA = src->LoadEntry("a");
  REQUIRE(sigA.has_value());
  REQUIRE(sigA->values.size() == 2u);
  CHECK_DOUBLE_EQ(sigA->values[0], 1.0);
  CHECK_DOUBLE_EQ(sigA->values[1], 2.0);
}

}  // namespace
