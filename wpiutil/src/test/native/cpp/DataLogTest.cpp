// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <array>
#include <memory>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "wpi/DataLogWriter.h"
#include "wpi/Logger.h"
#include "wpi/raw_ostream.h"

namespace {
struct ThingA {
  int x = 0;
};

inline bool operator==(const ThingA& a, const ThingA& b) {
  return a.x == b.x;
}

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
  static constexpr std::string_view GetTypeName() { return "ThingA"; }
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
  static constexpr std::string_view GetTypeName(const Info1&) {
    return "ThingB";
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
  static constexpr std::string_view GetTypeName() { return "ThingC"; }
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
  static constexpr std::string_view GetTypeName(const Info1&) {
    return "ThingC";
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
  static constexpr std::string_view GetTypeName(const Info2&) {
    return "ThingC";
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
  int entry = log.Start("test", "int64", "", 1);
  log.AppendInteger(entry, 1, 2);
  log.Flush();
  ASSERT_EQ(data.size(), 54u);
}

TEST_F(DataLogTest, BooleanAppend) {
  wpi::log::BooleanLogEntry entry{log, "a", 5};
  entry.Append(false, 7);
  log.Flush();
  ASSERT_EQ(data.size(), 46u);
}

TEST_F(DataLogTest, BooleanUpdate) {
  wpi::log::BooleanLogEntry entry{log, "a", 5};
  ASSERT_FALSE(entry.GetLastValue().has_value());
  entry.Update(false, 7);
  log.Flush();
  ASSERT_EQ(data.size(), 46u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(), false);
  entry.Update(false, 8);
  log.Flush();
  ASSERT_EQ(data.size(), 46u);
  entry.Update(true, 9);
  log.Flush();
  ASSERT_EQ(data.size(), 51u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(), true);
}

TEST_F(DataLogTest, IntegerAppend) {
  wpi::log::IntegerLogEntry entry{log, "a", 5};
  entry.Append(5, 7);
  log.Flush();
  ASSERT_EQ(data.size(), 51u);
}

TEST_F(DataLogTest, IntegerUpdate) {
  wpi::log::IntegerLogEntry entry{log, "a", 5};
  ASSERT_FALSE(entry.GetLastValue().has_value());
  entry.Update(0, 7);
  log.Flush();
  ASSERT_EQ(data.size(), 51u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(), 0);
  entry.Update(0, 8);
  log.Flush();
  ASSERT_EQ(data.size(), 51u);
  entry.Update(2, 9);
  log.Flush();
  ASSERT_EQ(data.size(), 63u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(), 2);
}

TEST_F(DataLogTest, FloatAppend) {
  wpi::log::FloatLogEntry entry{log, "a", 5};
  entry.Append(5.0, 7);
  log.Flush();
  ASSERT_EQ(data.size(), 47u);
}

TEST_F(DataLogTest, FloatUpdate) {
  wpi::log::FloatLogEntry entry{log, "a", 5};
  ASSERT_FALSE(entry.GetLastValue().has_value());
  entry.Update(0.0f, 7);
  log.Flush();
  ASSERT_EQ(data.size(), 47u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(), 0.0f);
  entry.Update(0.0f, 8);
  log.Flush();
  ASSERT_EQ(data.size(), 47u);
  entry.Update(0.1f, 9);
  log.Flush();
  ASSERT_EQ(data.size(), 55u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(), 0.1f);
}

TEST_F(DataLogTest, DoubleAppend) {
  wpi::log::DoubleLogEntry entry{log, "a", 5};
  entry.Append(5.0, 7);
  log.Flush();
  ASSERT_EQ(data.size(), 52u);
}

TEST_F(DataLogTest, DoubleUpdate) {
  wpi::log::DoubleLogEntry entry{log, "a", 5};
  ASSERT_FALSE(entry.GetLastValue().has_value());
  entry.Update(0.0, 7);
  log.Flush();
  ASSERT_EQ(data.size(), 52u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(), 0.0);
  entry.Update(0.0, 8);
  log.Flush();
  ASSERT_EQ(data.size(), 52u);
  entry.Update(0.1, 9);
  log.Flush();
  ASSERT_EQ(data.size(), 64u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(), 0.1);
}

TEST_F(DataLogTest, StringAppend) {
  wpi::log::StringLogEntry entry{log, "a", 5};
  entry.Append("x", 7);
  log.Flush();
  ASSERT_EQ(data.size(), 45u);
}

TEST_F(DataLogTest, StringUpdate) {
  wpi::log::StringLogEntry entry{log, "a", 5};
  ASSERT_FALSE(entry.HasLastValue());

  entry.Update("x", 7);
  log.Flush();
  ASSERT_EQ(data.size(), 45u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(), "x");

  entry.Update("x", 8);
  log.Flush();
  ASSERT_EQ(data.size(), 45u);

  entry.Update("y", 9);
  log.Flush();
  ASSERT_EQ(data.size(), 50u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(), "y");

  entry.Update("yy", 10);
  log.Flush();
  ASSERT_EQ(data.size(), 56u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(), "yy");

  entry.Update("", 11);
  log.Flush();
  ASSERT_EQ(data.size(), 60u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(), "");
}

TEST_F(DataLogTest, RawAppend) {
  wpi::log::RawLogEntry entry{log, "a", 5};
  entry.Append({{5}}, 7);
  log.Flush();
  ASSERT_EQ(data.size(), 42u);
}

TEST_F(DataLogTest, RawUpdate) {
  wpi::log::RawLogEntry entry{log, "a", 5};
  ASSERT_FALSE(entry.HasLastValue());

  entry.Update({{5}}, 7);
  log.Flush();
  ASSERT_EQ(data.size(), 42u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(), std::vector<uint8_t>{5});

  entry.Update({{5}}, 8);
  log.Flush();
  ASSERT_EQ(data.size(), 42u);

  entry.Update({{6}}, 9);
  log.Flush();
  ASSERT_EQ(data.size(), 47u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(), std::vector<uint8_t>{6});

  entry.Update({{6, 6}}, 9);
  log.Flush();
  ASSERT_EQ(data.size(), 53u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(), (std::vector<uint8_t>{6, 6}));

  entry.Update(std::span<const uint8_t>{}, 10);
  log.Flush();
  ASSERT_EQ(data.size(), 57u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(), std::vector<uint8_t>{});
}

TEST_F(DataLogTest, BooleanArrayAppendEmpty) {
  wpi::log::BooleanArrayLogEntry entry{log, "a", 5};
  entry.Append(std::span<const bool>{}, 7);
  log.Flush();
  ASSERT_EQ(data.size(), 47u);
}

TEST_F(DataLogTest, BooleanArrayAppend) {
  wpi::log::BooleanArrayLogEntry entry{log, "a", 5};
  entry.Append({false}, 7);
  log.Flush();
  ASSERT_EQ(data.size(), 48u);
}

TEST_F(DataLogTest, BooleanArrayUpdate) {
  wpi::log::BooleanArrayLogEntry entry{log, "a", 5};
  ASSERT_FALSE(entry.GetLastValue().has_value());
  entry.Update({false}, 7);
  log.Flush();
  ASSERT_EQ(data.size(), 48u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(), std::vector<int>{false});
  entry.Update({false}, 8);
  log.Flush();
  ASSERT_EQ(data.size(), 48u);
  entry.Update({true}, 9);
  log.Flush();
  ASSERT_EQ(data.size(), 53u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(), std::vector<int>{true});
  entry.Update(std::span<const bool>{}, 10);
  log.Flush();
  ASSERT_EQ(data.size(), 57u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(), std::vector<int>{});
}

TEST_F(DataLogTest, IntegerArrayAppendEmpty) {
  wpi::log::IntegerArrayLogEntry entry{log, "a", 5};
  entry.Append(std::span<const int64_t>{}, 7);
  log.Flush();
  ASSERT_EQ(data.size(), 45u);
}

TEST_F(DataLogTest, IntegerArrayAppend) {
  wpi::log::IntegerArrayLogEntry entry{log, "a", 5};
  entry.Append({1}, 7);
  log.Flush();
  ASSERT_EQ(data.size(), 53u);
}

TEST_F(DataLogTest, IntegerArrayUpdate) {
  wpi::log::IntegerArrayLogEntry entry{log, "a", 5};
  ASSERT_FALSE(entry.GetLastValue().has_value());
  entry.Update({1}, 7);
  log.Flush();
  ASSERT_EQ(data.size(), 53u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(), std::vector<int64_t>{1});
  entry.Update({1}, 8);
  log.Flush();
  ASSERT_EQ(data.size(), 53u);
  entry.Update({2}, 9);
  log.Flush();
  ASSERT_EQ(data.size(), 65u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(), std::vector<int64_t>{2});
  entry.Update(std::span<const int64_t>{}, 10);
  log.Flush();
  ASSERT_EQ(data.size(), 69u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(), std::vector<int64_t>{});
}

TEST_F(DataLogTest, DoubleArrayAppendEmpty) {
  wpi::log::DoubleArrayLogEntry entry{log, "a", 5};
  entry.Append(std::span<const double>{}, 7);
  log.Flush();
  ASSERT_EQ(data.size(), 46u);
}

TEST_F(DataLogTest, DoubleArrayAppend) {
  wpi::log::DoubleArrayLogEntry entry{log, "a", 5};
  entry.Append({1.0}, 7);
  log.Flush();
  ASSERT_EQ(data.size(), 54u);
}

TEST_F(DataLogTest, DoubleArrayUpdate) {
  wpi::log::DoubleArrayLogEntry entry{log, "a", 5};
  ASSERT_FALSE(entry.GetLastValue().has_value());
  entry.Update({1.0}, 7);
  log.Flush();
  ASSERT_EQ(data.size(), 54u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(), std::vector<double>{1.0});
  entry.Update({1.0}, 8);
  log.Flush();
  ASSERT_EQ(data.size(), 54u);
  entry.Update({2.0}, 9);
  log.Flush();
  ASSERT_EQ(data.size(), 66u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(), std::vector<double>{2});
  entry.Update(std::span<const double>{}, 10);
  log.Flush();
  ASSERT_EQ(data.size(), 70u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(), std::vector<double>{});
}

TEST_F(DataLogTest, FloatArrayAppendEmpty) {
  wpi::log::FloatArrayLogEntry entry{log, "a", 5};
  entry.Append(std::span<const float>{}, 7);
  log.Flush();
  ASSERT_EQ(data.size(), 45u);
}

TEST_F(DataLogTest, FloatArrayAppend) {
  wpi::log::FloatArrayLogEntry entry{log, "a", 5};
  entry.Append({1.0f}, 7);
  log.Flush();
  ASSERT_EQ(data.size(), 49u);
}

TEST_F(DataLogTest, FloatArrayUpdate) {
  wpi::log::FloatArrayLogEntry entry{log, "a", 5};
  ASSERT_FALSE(entry.GetLastValue().has_value());
  entry.Update({1.0f}, 7);
  log.Flush();
  ASSERT_EQ(data.size(), 49u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(), std::vector<float>{1.0f});
  entry.Update({1.0f}, 8);
  log.Flush();
  ASSERT_EQ(data.size(), 49u);
  entry.Update({2.0f}, 9);
  log.Flush();
  ASSERT_EQ(data.size(), 57u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(), std::vector<float>{2.0f});
  entry.Update(std::span<const float>{}, 10);
  log.Flush();
  ASSERT_EQ(data.size(), 61u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(), std::vector<float>{});
}

TEST_F(DataLogTest, StringArrayAppendEmpty) {
  wpi::log::StringArrayLogEntry entry{log, "a", 5};
  entry.Append(std::span<const std::string>{}, 7);
  entry.Append(std::span<const std::string_view>{}, 7);
  log.Flush();
  ASSERT_EQ(data.size(), 58u);
}

TEST_F(DataLogTest, StringArrayAppend) {
  wpi::log::StringArrayLogEntry entry{log, "a", 5};
  entry.Append({"x"}, 7);
  log.Flush();
  ASSERT_EQ(data.size(), 55u);
}

TEST_F(DataLogTest, StringArrayUpdate) {
  wpi::log::StringArrayLogEntry entry{log, "a", 5};
  ASSERT_FALSE(entry.GetLastValue().has_value());
  entry.Update({"x"}, 7);
  log.Flush();
  ASSERT_EQ(data.size(), 55u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(), std::vector<std::string>{"x"});
  entry.Update({"x"}, 8);
  log.Flush();
  ASSERT_EQ(data.size(), 55u);
  entry.Update({"y"}, 9);
  log.Flush();
  ASSERT_EQ(data.size(), 68u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(), std::vector<std::string>{"y"});
  entry.Update(std::span<const std::string_view>{}, 10);
  log.Flush();
  ASSERT_EQ(data.size(), 76u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(), std::vector<std::string>{});
}

TEST_F(DataLogTest, StructA) {
  [[maybe_unused]]
  wpi::log::StructLogEntry<ThingA> entry0;
  wpi::log::StructLogEntry<ThingA> entry{log, "a", 5};
  entry.Append(ThingA{});
  entry.Append(ThingA{}, 7);
}

TEST_F(DataLogTest, StructUpdate) {
  wpi::log::StructLogEntry<ThingA> entry{log, "a", 5};
  ASSERT_FALSE(entry.GetLastValue().has_value());

  entry.Update(ThingA{}, 7);
  log.Flush();
  ASSERT_EQ(data.size(), 122u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(), ThingA{});

  entry.Update(ThingA{}, 8);
  log.Flush();
  ASSERT_EQ(data.size(), 122u);

  entry.Update(ThingA{.x = 1}, 9);
  log.Flush();
  ASSERT_EQ(data.size(), 127u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(), ThingA{.x = 1});
}

TEST_F(DataLogTest, StructArrayA) {
  [[maybe_unused]]
  wpi::log::StructArrayLogEntry<ThingA> entry0;
  wpi::log::StructArrayLogEntry<ThingA> entry{log, "a", 5};
  entry.Append({{ThingA{}, ThingA{}}});
  entry.Append({{ThingA{}, ThingA{}}}, 7);
}

TEST_F(DataLogTest, StructArrayUpdate) {
  wpi::log::StructArrayLogEntry<ThingA> entry{log, "a", 5};
  ASSERT_FALSE(entry.GetLastValue().has_value());

  entry.Update({{ThingA{}, ThingA{.x = 1}}}, 7);
  log.Flush();
  ASSERT_EQ(data.size(), 125u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(),
            (std::vector<ThingA>{ThingA{}, ThingA{.x = 1}}));

  entry.Update({{ThingA{}, ThingA{.x = 1}}}, 8);
  log.Flush();
  ASSERT_EQ(data.size(), 125u);

  entry.Update({{ThingA{}, ThingA{.x = 2}}}, 9);
  log.Flush();
  ASSERT_EQ(data.size(), 131u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(),
            (std::vector<ThingA>{ThingA{}, ThingA{.x = 2}}));

  entry.Update(std::span<const ThingA>{}, 10);
  log.Flush();
  ASSERT_EQ(data.size(), 135u);
  ASSERT_TRUE(entry.GetLastValue().has_value());
  ASSERT_EQ(entry.GetLastValue().value(), std::vector<ThingA>{});
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
