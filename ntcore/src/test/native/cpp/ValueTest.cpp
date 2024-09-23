// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <algorithm>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "TestPrinters.h"
#include "Value_internal.h"
#include "networktables/NetworkTableValue.h"

using namespace std::string_view_literals;

namespace std {  // NOLINT (clang-tidy.cert-dcl58-cpp)
template <typename T, typename U>
inline bool operator==(std::span<T> lhs, std::span<U> rhs) {
  if (lhs.size() != rhs.size()) {
    return false;
  }
  return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}
}  // namespace std

namespace nt {

class ValueTest : public ::testing::Test {};

using ValueDeathTest = ValueTest;

TEST_F(ValueTest, ConstructEmpty) {
  Value v;
  ASSERT_EQ(NT_UNASSIGNED, v.type());
}

TEST_F(ValueTest, Boolean) {
  auto v = Value::MakeBoolean(false);
  ASSERT_EQ(NT_BOOLEAN, v.type());
  ASSERT_FALSE(v.GetBoolean());
  NT_Value cv;
  NT_InitValue(&cv);
  ConvertToC(v, &cv);
  ASSERT_EQ(NT_BOOLEAN, cv.type);
  ASSERT_EQ(0, cv.data.v_boolean);

  v = Value::MakeBoolean(true);
  ASSERT_EQ(NT_BOOLEAN, v.type());
  ASSERT_TRUE(v.GetBoolean());
  NT_DisposeValue(&cv);
  ConvertToC(v, &cv);
  ASSERT_EQ(NT_BOOLEAN, cv.type);
  ASSERT_EQ(1, cv.data.v_boolean);

  NT_DisposeValue(&cv);
}

TEST_F(ValueTest, Double) {
  auto v = Value::MakeDouble(0.5);
  ASSERT_EQ(NT_DOUBLE, v.type());
  ASSERT_EQ(0.5, v.GetDouble());
  NT_Value cv;
  NT_InitValue(&cv);
  ConvertToC(v, &cv);
  ASSERT_EQ(NT_DOUBLE, cv.type);
  ASSERT_EQ(0.5, cv.data.v_double);

  v = Value::MakeDouble(0.25);
  ASSERT_EQ(NT_DOUBLE, v.type());
  ASSERT_EQ(0.25, v.GetDouble());
  NT_DisposeValue(&cv);
  ConvertToC(v, &cv);
  ASSERT_EQ(NT_DOUBLE, cv.type);
  ASSERT_EQ(0.25, cv.data.v_double);

  NT_DisposeValue(&cv);
}

TEST_F(ValueTest, String) {
  auto v = Value::MakeString("hello");
  ASSERT_EQ(NT_STRING, v.type());
  ASSERT_EQ("hello", v.GetString());
  NT_Value cv;
  NT_InitValue(&cv);
  ConvertToC(v, &cv);
  ASSERT_EQ(NT_STRING, cv.type);
  ASSERT_EQ("hello"sv, wpi::to_string_view(&cv.data.v_string));
  ASSERT_EQ(5u, cv.data.v_string.len);

  v = Value::MakeString("goodbye");
  ASSERT_EQ(NT_STRING, v.type());
  ASSERT_EQ("goodbye", v.GetString());
  NT_DisposeValue(&cv);
  ConvertToC(v, &cv);
  ASSERT_EQ(NT_STRING, cv.type);
  ASSERT_EQ("goodbye"sv, wpi::to_string_view(&cv.data.v_string));
  ASSERT_EQ(7u, cv.data.v_string.len);

  NT_DisposeValue(&cv);
}

TEST_F(ValueTest, Raw) {
  std::vector<uint8_t> arr{5, 4, 3, 2, 1};
  auto v = Value::MakeRaw(arr);
  ASSERT_EQ(NT_RAW, v.type());
  ASSERT_EQ(std::span<const uint8_t>(arr), v.GetRaw());
  NT_Value cv;
  NT_InitValue(&cv);
  ConvertToC(v, &cv);
  ASSERT_EQ(NT_RAW, cv.type);
  ASSERT_EQ(5u, cv.data.v_raw.size);
  ASSERT_EQ(std::span(reinterpret_cast<const uint8_t*>("\5\4\3\2\1"), 5),
            std::span(cv.data.v_raw.data, 5));

  std::vector<uint8_t> arr2{1, 2, 3, 4, 5, 6};
  v = Value::MakeRaw(arr2);
  ASSERT_EQ(NT_RAW, v.type());
  ASSERT_EQ(std::span<const uint8_t>(arr2), v.GetRaw());
  NT_DisposeValue(&cv);
  ConvertToC(v, &cv);
  ASSERT_EQ(NT_RAW, cv.type);
  ASSERT_EQ(6u, cv.data.v_raw.size);
  ASSERT_EQ(std::span(reinterpret_cast<const uint8_t*>("\1\2\3\4\5\6"), 6),
            std::span(cv.data.v_raw.data, 6));

  NT_DisposeValue(&cv);
}

TEST_F(ValueTest, BooleanArray) {
  std::vector<int> vec{1, 0, 1};
  auto v = Value::MakeBooleanArray(vec);
  ASSERT_EQ(NT_BOOLEAN_ARRAY, v.type());
  ASSERT_EQ(std::span<const int>(vec), v.GetBooleanArray());
  NT_Value cv;
  NT_InitValue(&cv);
  ConvertToC(v, &cv);
  ASSERT_EQ(NT_BOOLEAN_ARRAY, cv.type);
  ASSERT_EQ(3u, cv.data.arr_boolean.size);
  ASSERT_EQ(vec[0], cv.data.arr_boolean.arr[0]);
  ASSERT_EQ(vec[1], cv.data.arr_boolean.arr[1]);
  ASSERT_EQ(vec[2], cv.data.arr_boolean.arr[2]);

  // assign with same size
  vec = {0, 1, 0};
  v = Value::MakeBooleanArray(vec);
  ASSERT_EQ(NT_BOOLEAN_ARRAY, v.type());
  ASSERT_EQ(std::span<const int>(vec), v.GetBooleanArray());
  NT_DisposeValue(&cv);
  ConvertToC(v, &cv);
  ASSERT_EQ(NT_BOOLEAN_ARRAY, cv.type);
  ASSERT_EQ(3u, cv.data.arr_boolean.size);
  ASSERT_EQ(vec[0], cv.data.arr_boolean.arr[0]);
  ASSERT_EQ(vec[1], cv.data.arr_boolean.arr[1]);
  ASSERT_EQ(vec[2], cv.data.arr_boolean.arr[2]);

  // assign with different size
  vec = {1, 0};
  v = Value::MakeBooleanArray(vec);
  ASSERT_EQ(NT_BOOLEAN_ARRAY, v.type());
  ASSERT_EQ(std::span<const int>(vec), v.GetBooleanArray());
  NT_DisposeValue(&cv);
  ConvertToC(v, &cv);
  ASSERT_EQ(NT_BOOLEAN_ARRAY, cv.type);
  ASSERT_EQ(2u, cv.data.arr_boolean.size);
  ASSERT_EQ(vec[0], cv.data.arr_boolean.arr[0]);
  ASSERT_EQ(vec[1], cv.data.arr_boolean.arr[1]);

  NT_DisposeValue(&cv);
}

TEST_F(ValueTest, DoubleArray) {
  std::vector<double> vec{0.5, 0.25, 0.5};
  auto v = Value::MakeDoubleArray(vec);
  ASSERT_EQ(NT_DOUBLE_ARRAY, v.type());
  ASSERT_EQ(std::span<const double>(vec), v.GetDoubleArray());
  NT_Value cv;
  NT_InitValue(&cv);
  ConvertToC(v, &cv);
  ASSERT_EQ(NT_DOUBLE_ARRAY, cv.type);
  ASSERT_EQ(3u, cv.data.arr_double.size);
  ASSERT_EQ(vec[0], cv.data.arr_double.arr[0]);
  ASSERT_EQ(vec[1], cv.data.arr_double.arr[1]);
  ASSERT_EQ(vec[2], cv.data.arr_double.arr[2]);

  // assign with same size
  vec = {0.25, 0.5, 0.25};
  v = Value::MakeDoubleArray(vec);
  ASSERT_EQ(NT_DOUBLE_ARRAY, v.type());
  ASSERT_EQ(std::span<const double>(vec), v.GetDoubleArray());
  NT_DisposeValue(&cv);
  ConvertToC(v, &cv);
  ASSERT_EQ(NT_DOUBLE_ARRAY, cv.type);
  ASSERT_EQ(3u, cv.data.arr_double.size);
  ASSERT_EQ(vec[0], cv.data.arr_double.arr[0]);
  ASSERT_EQ(vec[1], cv.data.arr_double.arr[1]);
  ASSERT_EQ(vec[2], cv.data.arr_double.arr[2]);

  // assign with different size
  vec = {0.5, 0.25};
  v = Value::MakeDoubleArray(vec);
  ASSERT_EQ(NT_DOUBLE_ARRAY, v.type());
  ASSERT_EQ(std::span<const double>(vec), v.GetDoubleArray());
  NT_DisposeValue(&cv);
  ConvertToC(v, &cv);
  ASSERT_EQ(NT_DOUBLE_ARRAY, cv.type);
  ASSERT_EQ(2u, cv.data.arr_double.size);
  ASSERT_EQ(vec[0], cv.data.arr_double.arr[0]);
  ASSERT_EQ(vec[1], cv.data.arr_double.arr[1]);

  NT_DisposeValue(&cv);
}

TEST_F(ValueTest, StringArray) {
  std::vector<std::string> vec;
  vec.push_back("hello");
  vec.push_back("goodbye");
  vec.push_back("string");
  auto v = Value::MakeStringArray(std::move(vec));
  ASSERT_EQ(NT_STRING_ARRAY, v.type());
  ASSERT_EQ(3u, v.GetStringArray().size());
  ASSERT_EQ("hello"sv, v.GetStringArray()[0]);
  ASSERT_EQ("goodbye"sv, v.GetStringArray()[1]);
  ASSERT_EQ("string"sv, v.GetStringArray()[2]);
  NT_Value cv;
  NT_InitValue(&cv);
  ConvertToC(v, &cv);
  ASSERT_EQ(NT_STRING_ARRAY, cv.type);
  ASSERT_EQ(3u, cv.data.arr_string.size);
  ASSERT_EQ("hello"sv, wpi::to_string_view(&cv.data.arr_string.arr[0]));
  ASSERT_EQ("goodbye"sv, wpi::to_string_view(&cv.data.arr_string.arr[1]));
  ASSERT_EQ("string"sv, wpi::to_string_view(&cv.data.arr_string.arr[2]));

  // assign with same size
  vec.clear();
  vec.push_back("s1");
  vec.push_back("str2");
  vec.push_back("string3");
  v = Value::MakeStringArray(vec);
  ASSERT_EQ(NT_STRING_ARRAY, v.type());
  ASSERT_EQ(3u, v.GetStringArray().size());
  ASSERT_EQ("s1"sv, v.GetStringArray()[0]);
  ASSERT_EQ("str2"sv, v.GetStringArray()[1]);
  ASSERT_EQ("string3"sv, v.GetStringArray()[2]);
  NT_DisposeValue(&cv);
  ConvertToC(v, &cv);
  ASSERT_EQ(NT_STRING_ARRAY, cv.type);
  ASSERT_EQ(3u, cv.data.arr_string.size);
  ASSERT_EQ("s1"sv, wpi::to_string_view(&cv.data.arr_string.arr[0]));
  ASSERT_EQ("str2"sv, wpi::to_string_view(&cv.data.arr_string.arr[1]));
  ASSERT_EQ("string3"sv, wpi::to_string_view(&cv.data.arr_string.arr[2]));

  // assign with different size
  vec.clear();
  vec.push_back("short");
  vec.push_back("er");
  v = Value::MakeStringArray(std::move(vec));
  ASSERT_EQ(NT_STRING_ARRAY, v.type());
  ASSERT_EQ(2u, v.GetStringArray().size());
  ASSERT_EQ("short"sv, v.GetStringArray()[0]);
  ASSERT_EQ("er"sv, v.GetStringArray()[1]);
  NT_DisposeValue(&cv);
  ConvertToC(v, &cv);
  ASSERT_EQ(NT_STRING_ARRAY, cv.type);
  ASSERT_EQ(2u, cv.data.arr_string.size);
  ASSERT_EQ("short"sv, wpi::to_string_view(&cv.data.arr_string.arr[0]));
  ASSERT_EQ("er"sv, wpi::to_string_view(&cv.data.arr_string.arr[1]));

  NT_DisposeValue(&cv);
}

#ifdef NDEBUG
TEST_F(ValueDeathTest, DISABLED_GetAssertions) {
#else
TEST_F(ValueDeathTest, GetAssertions) {
#endif
  Value v;
  ASSERT_DEATH((void)v.GetBoolean(), "type == NT_BOOLEAN");
  ASSERT_DEATH((void)v.GetDouble(), "type == NT_DOUBLE");
  ASSERT_DEATH((void)v.GetString(), "type == NT_STRING");
  ASSERT_DEATH((void)v.GetRaw(), "type == NT_RAW");
  ASSERT_DEATH((void)v.GetBooleanArray(), "type == NT_BOOLEAN_ARRAY");
  ASSERT_DEATH((void)v.GetDoubleArray(), "type == NT_DOUBLE_ARRAY");
  ASSERT_DEATH((void)v.GetStringArray(), "type == NT_STRING_ARRAY");
}

TEST_F(ValueTest, UnassignedComparison) {
  Value v1, v2;
  ASSERT_EQ(v1, v2);
}

TEST_F(ValueTest, MixedComparison) {
  Value v1;
  auto v2 = Value::MakeBoolean(true);
  ASSERT_NE(v1, v2);  // unassigned vs boolean
  auto v3 = Value::MakeDouble(0.5);
  ASSERT_NE(v2, v3);  // boolean vs double
}

TEST_F(ValueTest, BooleanComparison) {
  auto v1 = Value::MakeBoolean(true);
  auto v2 = Value::MakeBoolean(true);
  ASSERT_EQ(v1, v2);
  v2 = Value::MakeBoolean(false);
  ASSERT_NE(v1, v2);
}

TEST_F(ValueTest, DoubleComparison) {
  auto v1 = Value::MakeDouble(0.25);
  auto v2 = Value::MakeDouble(0.25);
  ASSERT_EQ(v1, v2);
  v2 = Value::MakeDouble(0.5);
  ASSERT_NE(v1, v2);
}

TEST_F(ValueTest, StringComparison) {
  auto v1 = Value::MakeString("hello");
  auto v2 = Value::MakeString("hello");
  ASSERT_EQ(v1, v2);
  v2 = Value::MakeString("world");  // different contents
  ASSERT_NE(v1, v2);
  v2 = Value::MakeString("goodbye");  // different size
  ASSERT_NE(v1, v2);
}

TEST_F(ValueTest, BooleanArrayComparison) {
  std::vector<int> vec{1, 0, 1};
  auto v1 = Value::MakeBooleanArray(vec);
  auto v2 = Value::MakeBooleanArray(vec);
  ASSERT_EQ(v1, v2);

  // different contents
  vec = {1, 1, 1};
  v2 = Value::MakeBooleanArray(vec);
  ASSERT_NE(v1, v2);

  // different size
  vec = {1, 0};
  v2 = Value::MakeBooleanArray(vec);
  ASSERT_NE(v1, v2);

  // empty
  vec = {};
  v1 = Value::MakeBooleanArray(vec);
  v2 = Value::MakeBooleanArray(vec);
  ASSERT_EQ(v1, v2);
}

TEST_F(ValueTest, IntegerArrayComparison) {
  std::vector<int64_t> vec{-42, 0, 1};
  auto v1 = Value::MakeIntegerArray(vec);
  auto v2 = Value::MakeIntegerArray(vec);
  ASSERT_EQ(v1, v2);

  // different contents
  vec = {-42, 1, 1};
  v2 = Value::MakeIntegerArray(vec);
  ASSERT_NE(v1, v2);

  // different size
  vec = {-42, 0};
  v2 = Value::MakeIntegerArray(vec);
  ASSERT_NE(v1, v2);

  // empty
  vec = {};
  v1 = Value::MakeIntegerArray(vec);
  v2 = Value::MakeIntegerArray(vec);
  ASSERT_EQ(v1, v2);
}

TEST_F(ValueTest, FloatArrayComparison) {
  std::vector<float> vec{0.5, 0.25, 0.5};
  auto v1 = Value::MakeFloatArray(vec);
  auto v2 = Value::MakeFloatArray(vec);
  ASSERT_EQ(v1, v2);

  // different contents
  vec = {0.5, 0.5, 0.5};
  v2 = Value::MakeFloatArray(vec);
  ASSERT_NE(v1, v2);

  // different size
  vec = {0.5, 0.25};
  v2 = Value::MakeFloatArray(vec);
  ASSERT_NE(v1, v2);

  // empty
  vec = {};
  v1 = Value::MakeFloatArray(vec);
  v2 = Value::MakeFloatArray(vec);
  ASSERT_EQ(v1, v2);
}

TEST_F(ValueTest, DoubleArrayComparison) {
  std::vector<double> vec{0.5, 0.25, 0.5};
  auto v1 = Value::MakeDoubleArray(vec);
  auto v2 = Value::MakeDoubleArray(vec);
  ASSERT_EQ(v1, v2);

  // different contents
  vec = {0.5, 0.5, 0.5};
  v2 = Value::MakeDoubleArray(vec);
  ASSERT_NE(v1, v2);

  // different size
  vec = {0.5, 0.25};
  v2 = Value::MakeDoubleArray(vec);
  ASSERT_NE(v1, v2);

  // empty
  vec = {};
  v1 = Value::MakeDoubleArray(vec);
  v2 = Value::MakeDoubleArray(vec);
  ASSERT_EQ(v1, v2);
}

TEST_F(ValueTest, StringArrayComparison) {
  std::vector<std::string> vec;
  vec.push_back("hello");
  vec.push_back("goodbye");
  vec.push_back("string");
  auto v1 = Value::MakeStringArray(vec);
  vec.clear();
  vec.push_back("hello");
  vec.push_back("goodbye");
  vec.push_back("string");
  auto v2 = Value::MakeStringArray(std::move(vec));
  ASSERT_EQ(v1, v2);

  // different contents
  vec.clear();
  vec.push_back("hello");
  vec.push_back("goodby2");
  vec.push_back("string");
  v2 = Value::MakeStringArray(std::move(vec));
  ASSERT_NE(v1, v2);

  // different sized contents
  vec.clear();
  vec.push_back("hello");
  vec.push_back("goodbye2");
  vec.push_back("string");
  v2 = Value::MakeStringArray(vec);
  ASSERT_NE(v1, v2);

  // different size
  vec.clear();
  vec.push_back("hello");
  vec.push_back("goodbye");
  v2 = Value::MakeStringArray(std::move(vec));
  ASSERT_NE(v1, v2);

  // empty
  vec.clear();
  v1 = Value::MakeStringArray(vec);
  v2 = Value::MakeStringArray(std::move(vec));
  ASSERT_EQ(v1, v2);
}

}  // namespace nt
