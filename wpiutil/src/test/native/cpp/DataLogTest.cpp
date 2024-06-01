// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <array>
#include <memory>

#include <gtest/gtest.h>

#include "wpi/DataLogWriter.h"
#include "wpi/Logger.h"
#include "wpi/raw_ostream.h"

namespace {
struct ThingA {
  int x = 0;
};

struct ThingB {
  int x = 0;
};

struct ThingC {
  int x = 0;
};

struct Info1 {
  int info = 0;
};

struct Info2 {
  int info = 0;
};
}  // namespace

template <>
struct wpi::Struct<ThingA> {
  static constexpr std::string_view GetTypeString() { return "struct:ThingA"; }
  static constexpr size_t GetSize() { return 1; }
  static constexpr std::string_view GetSchema() { return "uint8 value"; }
  static ThingA Unpack(std::span<const uint8_t> data) {
    return ThingA{.x = data[0]};
  }
  static void Pack(std::span<uint8_t> data, const ThingA& value) {
    data[0] = value.x;
  }
};

template <>
struct wpi::Struct<ThingB, Info1> {
  static constexpr std::string_view GetTypeString(const Info1&) {
    return "struct:ThingB";
  }
  static constexpr size_t GetSize(const Info1&) { return 1; }
  static constexpr std::string_view GetSchema(const Info1&) {
    return "uint8 value";
  }
  static ThingB Unpack(std::span<const uint8_t> data, const Info1&) {
    return ThingB{.x = data[0]};
  }
  static void Pack(std::span<uint8_t> data, const ThingB& value, const Info1&) {
    data[0] = value.x;
  }
};

template <>
struct wpi::Struct<ThingC> {
  static constexpr std::string_view GetTypeString() { return "struct:ThingC"; }
  static constexpr size_t GetSize() { return 1; }
  static constexpr std::string_view GetSchema() { return "uint8 value"; }
  static ThingC Unpack(std::span<const uint8_t> data) {
    return ThingC{.x = data[0]};
  }
  static void Pack(std::span<uint8_t> data, const ThingC& value) {
    data[0] = value.x;
  }
};

template <>
struct wpi::Struct<ThingC, Info1> {
  static constexpr std::string_view GetTypeString(const Info1&) {
    return "struct:ThingC";
  }
  static constexpr size_t GetSize(const Info1&) { return 1; }
  static constexpr std::string_view GetSchema(const Info1&) {
    return "uint8 value";
  }
  static ThingC Unpack(std::span<const uint8_t> data, const Info1&) {
    return ThingC{.x = data[0]};
  }
  static void Pack(std::span<uint8_t> data, const ThingC& value, const Info1&) {
    data[0] = value.x;
  }
};

template <>
struct wpi::Struct<ThingC, Info2> {
  static constexpr std::string_view GetTypeString(const Info2&) {
    return "struct:ThingC";
  }
  static constexpr size_t GetSize(const Info2&) { return 1; }
  static constexpr std::string_view GetSchema(const Info2&) {
    return "uint8 value";
  }
  static ThingC Unpack(std::span<const uint8_t> data, const Info2&) {
    return ThingC{.x = data[0]};
  }
  static void Pack(std::span<uint8_t> data, const ThingC& value, const Info2&) {
    data[0] = value.x;
  }
};

static_assert(wpi::StructSerializable<ThingA>);
static_assert(!wpi::StructSerializable<ThingA, Info1>);

static_assert(!wpi::StructSerializable<ThingB>);
static_assert(wpi::StructSerializable<ThingB, Info1>);
static_assert(!wpi::StructSerializable<ThingB, Info2>);

static_assert(wpi::StructSerializable<ThingC>);
static_assert(wpi::StructSerializable<ThingC, Info1>);
static_assert(wpi::StructSerializable<ThingC, Info2>);

class DataLogTest : public ::testing::Test {
 public:
  wpi::Logger msglog;
  std::vector<uint8_t> data;
  wpi::log::DataLogWriter log{msglog,
                              std::make_unique<wpi::raw_uvector_ostream>(data)};
};

TEST_F(DataLogTest, SimpleInt) {
  int entry = log.Start("test", "int64");
  log.AppendInteger(entry, 1, 0);
  log.Flush();
  ASSERT_EQ(data.size(), 66u);
}

TEST_F(DataLogTest, StructA) {
  [[maybe_unused]]
  wpi::log::StructLogEntry<ThingA> entry0;
  wpi::log::StructLogEntry<ThingA> entry{log, "a", 5};
  entry.Append(ThingA{});
  entry.Append(ThingA{}, 7);
}

TEST_F(DataLogTest, StructArrayA) {
  [[maybe_unused]]
  wpi::log::StructArrayLogEntry<ThingA> entry0;
  wpi::log::StructArrayLogEntry<ThingA> entry{log, "a", 5};
  entry.Append({{ThingA{}, ThingA{}}});
  entry.Append({{ThingA{}, ThingA{}}}, 7);
}

TEST_F(DataLogTest, StructFixedArrayA) {
  [[maybe_unused]]
  wpi::log::StructArrayLogEntry<std::array<ThingA, 2>> entry0;
  wpi::log::StructLogEntry<std::array<ThingA, 2>> entry{log, "a", 5};
  std::array<ThingA, 2> arr;
  entry.Append(arr);
  entry.Append(arr, 7);
}

TEST_F(DataLogTest, StructB) {
  Info1 info;
  [[maybe_unused]]
  wpi::log::StructLogEntry<ThingB, Info1> entry0;
  wpi::log::StructLogEntry<ThingB, Info1> entry{log, "b", info, 5};
  entry.Append(ThingB{});
  entry.Append(ThingB{}, 7);
}

TEST_F(DataLogTest, StructArrayB) {
  Info1 info;
  [[maybe_unused]]
  wpi::log::StructArrayLogEntry<ThingB, Info1> entry0;
  wpi::log::StructArrayLogEntry<ThingB, Info1> entry{log, "a", info, 5};
  entry.Append({{ThingB{}, ThingB{}}});
  entry.Append({{ThingB{}, ThingB{}}}, 7);
}

TEST_F(DataLogTest, StructFixedArrayB) {
  Info1 info;
  wpi::log::StructLogEntry<std::array<ThingB, 2>, Info1> entry{log, "a", info,
                                                               5};
  std::array<ThingB, 2> arr;
  entry.Append(arr);
  entry.Append(arr, 7);
}

TEST_F(DataLogTest, StructC) {
  {
    wpi::log::StructLogEntry<ThingC> entry{log, "c", 5};
    entry.Append(ThingC{});
    entry.Append(ThingC{}, 7);
  }
  {
    Info1 info;
    wpi::log::StructLogEntry<ThingC, Info1> entry{log, "c1", info, 5};
    entry.Append(ThingC{});
    entry.Append(ThingC{}, 7);
  }
  {
    Info2 info;
    wpi::log::StructLogEntry<ThingC, Info2> entry{log, "c2", info, 5};
    entry.Append(ThingC{});
    entry.Append(ThingC{}, 7);
  }
}

TEST_F(DataLogTest, StructArrayC) {
  {
    wpi::log::StructArrayLogEntry<ThingC> entry{log, "c", 5};
    entry.Append({{ThingC{}, ThingC{}}});
    entry.Append({{ThingC{}, ThingC{}}}, 7);
  }
  {
    Info1 info;
    wpi::log::StructArrayLogEntry<ThingC, Info1> entry{log, "c1", info, 5};
    entry.Append({{ThingC{}, ThingC{}}});
    entry.Append({{ThingC{}, ThingC{}}}, 7);
  }
  {
    Info2 info;
    wpi::log::StructArrayLogEntry<ThingC, Info2> entry{log, "c2", info, 5};
    entry.Append({{ThingC{}, ThingC{}}});
    entry.Append({{ThingC{}, ThingC{}}}, 7);
  }
}

TEST_F(DataLogTest, StructFixedArrayC) {
  std::array<ThingC, 2> arr;
  {
    wpi::log::StructLogEntry<std::array<ThingC, 2>> entry{log, "c", 5};
    entry.Append(arr);
    entry.Append(arr, 7);
  }
  {
    Info1 info;
    wpi::log::StructLogEntry<std::array<ThingC, 2>, Info1> entry{log, "c1",
                                                                 info, 5};
    entry.Append(arr);
    entry.Append(arr, 7);
  }
  {
    Info2 info;
    wpi::log::StructLogEntry<std::array<ThingC, 2>, Info2> entry{log, "c2",
                                                                 info, 5};
    entry.Append(arr);
    entry.Append(arr, 7);
  }
}
