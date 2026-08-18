// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <algorithm>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "TestPrinters.hpp"
#include "Value_internal.hpp"
#include "wpi/nt/NetworkTableValue.hpp"
#include "wpi/util/string.hpp"

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

namespace wpi::nt {

TEST_CASE("ValueTest ConstructEmpty", "[ntcore][value]") {
  Value v;
  REQUIRE(NT_UNASSIGNED == v.type());
}

TEST_CASE("ValueTest Boolean", "[ntcore][value]") {
  auto v = Value::MakeBoolean(false);
  REQUIRE(NT_BOOLEAN == v.type());
  REQUIRE_FALSE(v.GetBoolean());
  NT_Value cv;
  NT_InitValue(&cv);
  ConvertToC(v, &cv);
  REQUIRE(NT_BOOLEAN == cv.type);
  REQUIRE(0 == cv.data.v_boolean);

  v = Value::MakeBoolean(true);
  REQUIRE(NT_BOOLEAN == v.type());
  REQUIRE(v.GetBoolean());
  NT_DisposeValue(&cv);
  ConvertToC(v, &cv);
  REQUIRE(NT_BOOLEAN == cv.type);
  REQUIRE(1 == cv.data.v_boolean);

  NT_DisposeValue(&cv);
}

TEST_CASE("ValueTest Double", "[ntcore][value]") {
  auto v = Value::MakeDouble(0.5);
  REQUIRE(NT_DOUBLE == v.type());
  REQUIRE(0.5 == v.GetDouble());
  NT_Value cv;
  NT_InitValue(&cv);
  ConvertToC(v, &cv);
  REQUIRE(NT_DOUBLE == cv.type);
  REQUIRE(0.5 == cv.data.v_double);

  v = Value::MakeDouble(0.25);
  REQUIRE(NT_DOUBLE == v.type());
  REQUIRE(0.25 == v.GetDouble());
  NT_DisposeValue(&cv);
  ConvertToC(v, &cv);
  REQUIRE(NT_DOUBLE == cv.type);
  REQUIRE(0.25 == cv.data.v_double);

  NT_DisposeValue(&cv);
}

TEST_CASE("ValueTest String", "[ntcore][value]") {
  auto v = Value::MakeString("hello");
  REQUIRE(NT_STRING == v.type());
  REQUIRE("hello" == v.GetString());
  NT_Value cv;
  NT_InitValue(&cv);
  ConvertToC(v, &cv);
  REQUIRE(NT_STRING == cv.type);
  REQUIRE("hello"sv == wpi::util::to_string_view(&cv.data.v_string));
  REQUIRE(5u == cv.data.v_string.len);

  v = Value::MakeString("goodbye");
  REQUIRE(NT_STRING == v.type());
  REQUIRE("goodbye" == v.GetString());
  NT_DisposeValue(&cv);
  ConvertToC(v, &cv);
  REQUIRE(NT_STRING == cv.type);
  REQUIRE("goodbye"sv == wpi::util::to_string_view(&cv.data.v_string));
  REQUIRE(7u == cv.data.v_string.len);

  NT_DisposeValue(&cv);
}

TEST_CASE("ValueTest Raw", "[ntcore][value]") {
  std::vector<uint8_t> arr{5, 4, 3, 2, 1};
  auto v = Value::MakeRaw(arr);
  REQUIRE(NT_RAW == v.type());
  REQUIRE(std::span<const uint8_t>(arr) == v.GetRaw());
  NT_Value cv;
  NT_InitValue(&cv);
  ConvertToC(v, &cv);
  REQUIRE(NT_RAW == cv.type);
  REQUIRE(5u == cv.data.v_raw.size);
  REQUIRE(std::span(reinterpret_cast<const uint8_t*>("\5\4\3\2\1"), 5) ==
          std::span(cv.data.v_raw.data, 5));

  std::vector<uint8_t> arr2{1, 2, 3, 4, 5, 6};
  v = Value::MakeRaw(arr2);
  REQUIRE(NT_RAW == v.type());
  REQUIRE(std::span<const uint8_t>(arr2) == v.GetRaw());
  NT_DisposeValue(&cv);
  ConvertToC(v, &cv);
  REQUIRE(NT_RAW == cv.type);
  REQUIRE(6u == cv.data.v_raw.size);
  REQUIRE(std::span(reinterpret_cast<const uint8_t*>("\1\2\3\4\5\6"), 6) ==
          std::span(cv.data.v_raw.data, 6));

  NT_DisposeValue(&cv);
}

TEST_CASE("ValueTest BooleanArray", "[ntcore][value]") {
  std::vector<int> vec{1, 0, 1};
  auto v = Value::MakeBooleanArray(vec);
  REQUIRE(NT_BOOLEAN_ARRAY == v.type());
  REQUIRE(std::span<const int>(vec) == v.GetBooleanArray());
  NT_Value cv;
  NT_InitValue(&cv);
  ConvertToC(v, &cv);
  REQUIRE(NT_BOOLEAN_ARRAY == cv.type);
  REQUIRE(3u == cv.data.arr_boolean.size);
  REQUIRE(vec[0] == cv.data.arr_boolean.arr[0]);
  REQUIRE(vec[1] == cv.data.arr_boolean.arr[1]);
  REQUIRE(vec[2] == cv.data.arr_boolean.arr[2]);

  // assign with same size
  vec = {0, 1, 0};
  v = Value::MakeBooleanArray(vec);
  REQUIRE(NT_BOOLEAN_ARRAY == v.type());
  REQUIRE(std::span<const int>(vec) == v.GetBooleanArray());
  NT_DisposeValue(&cv);
  ConvertToC(v, &cv);
  REQUIRE(NT_BOOLEAN_ARRAY == cv.type);
  REQUIRE(3u == cv.data.arr_boolean.size);
  REQUIRE(vec[0] == cv.data.arr_boolean.arr[0]);
  REQUIRE(vec[1] == cv.data.arr_boolean.arr[1]);
  REQUIRE(vec[2] == cv.data.arr_boolean.arr[2]);

  // assign with different size
  vec = {1, 0};
  v = Value::MakeBooleanArray(vec);
  REQUIRE(NT_BOOLEAN_ARRAY == v.type());
  REQUIRE(std::span<const int>(vec) == v.GetBooleanArray());
  NT_DisposeValue(&cv);
  ConvertToC(v, &cv);
  REQUIRE(NT_BOOLEAN_ARRAY == cv.type);
  REQUIRE(2u == cv.data.arr_boolean.size);
  REQUIRE(vec[0] == cv.data.arr_boolean.arr[0]);
  REQUIRE(vec[1] == cv.data.arr_boolean.arr[1]);

  NT_DisposeValue(&cv);
}

TEST_CASE("ValueTest DoubleArray", "[ntcore][value]") {
  std::vector<double> vec{0.5, 0.25, 0.5};
  auto v = Value::MakeDoubleArray(vec);
  REQUIRE(NT_DOUBLE_ARRAY == v.type());
  REQUIRE(std::span<const double>(vec) == v.GetDoubleArray());
  NT_Value cv;
  NT_InitValue(&cv);
  ConvertToC(v, &cv);
  REQUIRE(NT_DOUBLE_ARRAY == cv.type);
  REQUIRE(3u == cv.data.arr_double.size);
  REQUIRE(vec[0] == cv.data.arr_double.arr[0]);
  REQUIRE(vec[1] == cv.data.arr_double.arr[1]);
  REQUIRE(vec[2] == cv.data.arr_double.arr[2]);

  // assign with same size
  vec = {0.25, 0.5, 0.25};
  v = Value::MakeDoubleArray(vec);
  REQUIRE(NT_DOUBLE_ARRAY == v.type());
  REQUIRE(std::span<const double>(vec) == v.GetDoubleArray());
  NT_DisposeValue(&cv);
  ConvertToC(v, &cv);
  REQUIRE(NT_DOUBLE_ARRAY == cv.type);
  REQUIRE(3u == cv.data.arr_double.size);
  REQUIRE(vec[0] == cv.data.arr_double.arr[0]);
  REQUIRE(vec[1] == cv.data.arr_double.arr[1]);
  REQUIRE(vec[2] == cv.data.arr_double.arr[2]);

  // assign with different size
  vec = {0.5, 0.25};
  v = Value::MakeDoubleArray(vec);
  REQUIRE(NT_DOUBLE_ARRAY == v.type());
  REQUIRE(std::span<const double>(vec) == v.GetDoubleArray());
  NT_DisposeValue(&cv);
  ConvertToC(v, &cv);
  REQUIRE(NT_DOUBLE_ARRAY == cv.type);
  REQUIRE(2u == cv.data.arr_double.size);
  REQUIRE(vec[0] == cv.data.arr_double.arr[0]);
  REQUIRE(vec[1] == cv.data.arr_double.arr[1]);

  NT_DisposeValue(&cv);
}

TEST_CASE("ValueTest StringArray", "[ntcore][value]") {
  std::vector<std::string> vec;
  vec.push_back("hello");
  vec.push_back("goodbye");
  vec.push_back("string");
  auto v = Value::MakeStringArray(std::move(vec));
  REQUIRE(NT_STRING_ARRAY == v.type());
  REQUIRE(3u == v.GetStringArray().size());
  REQUIRE("hello"sv == v.GetStringArray()[0]);
  REQUIRE("goodbye"sv == v.GetStringArray()[1]);
  REQUIRE("string"sv == v.GetStringArray()[2]);
  NT_Value cv;
  NT_InitValue(&cv);
  ConvertToC(v, &cv);
  REQUIRE(NT_STRING_ARRAY == cv.type);
  REQUIRE(3u == cv.data.arr_string.size);
  REQUIRE("hello"sv == wpi::util::to_string_view(&cv.data.arr_string.arr[0]));
  REQUIRE("goodbye"sv ==
          (wpi::util::to_string_view(&cv.data.arr_string.arr[1])));
  REQUIRE("string"sv ==
          (wpi::util::to_string_view(&cv.data.arr_string.arr[2])));

  // assign with same size
  vec.clear();
  vec.push_back("s1");
  vec.push_back("str2");
  vec.push_back("string3");
  v = Value::MakeStringArray(vec);
  REQUIRE(NT_STRING_ARRAY == v.type());
  REQUIRE(3u == v.GetStringArray().size());
  REQUIRE("s1"sv == v.GetStringArray()[0]);
  REQUIRE("str2"sv == v.GetStringArray()[1]);
  REQUIRE("string3"sv == v.GetStringArray()[2]);
  NT_DisposeValue(&cv);
  ConvertToC(v, &cv);
  REQUIRE(NT_STRING_ARRAY == cv.type);
  REQUIRE(3u == cv.data.arr_string.size);
  REQUIRE("s1"sv == wpi::util::to_string_view(&cv.data.arr_string.arr[0]));
  REQUIRE("str2"sv == wpi::util::to_string_view(&cv.data.arr_string.arr[1]));
  REQUIRE("string3"sv ==
          (wpi::util::to_string_view(&cv.data.arr_string.arr[2])));

  // assign with different size
  vec.clear();
  vec.push_back("short");
  vec.push_back("er");
  v = Value::MakeStringArray(std::move(vec));
  REQUIRE(NT_STRING_ARRAY == v.type());
  REQUIRE(2u == v.GetStringArray().size());
  REQUIRE("short"sv == v.GetStringArray()[0]);
  REQUIRE("er"sv == v.GetStringArray()[1]);
  NT_DisposeValue(&cv);
  ConvertToC(v, &cv);
  REQUIRE(NT_STRING_ARRAY == cv.type);
  REQUIRE(2u == cv.data.arr_string.size);
  REQUIRE("short"sv == wpi::util::to_string_view(&cv.data.arr_string.arr[0]));
  REQUIRE("er"sv == wpi::util::to_string_view(&cv.data.arr_string.arr[1]));

  NT_DisposeValue(&cv);
}

TEST_CASE("ValueTest UnassignedComparison", "[ntcore][value]") {
  Value v1, v2;
  REQUIRE(v1 == v2);
}

TEST_CASE("ValueTest MixedComparison", "[ntcore][value]") {
  Value v1;
  auto v2 = Value::MakeBoolean(true);
  REQUIRE(v1 != v2);  // unassigned vs boolean
  auto v3 = Value::MakeDouble(0.5);
  REQUIRE(v2 != v3);  // boolean vs double
}

TEST_CASE("ValueTest BooleanComparison", "[ntcore][value]") {
  auto v1 = Value::MakeBoolean(true);
  auto v2 = Value::MakeBoolean(true);
  REQUIRE(v1 == v2);
  v2 = Value::MakeBoolean(false);
  REQUIRE(v1 != v2);
}

TEST_CASE("ValueTest DoubleComparison", "[ntcore][value]") {
  auto v1 = Value::MakeDouble(0.25);
  auto v2 = Value::MakeDouble(0.25);
  REQUIRE(v1 == v2);
  v2 = Value::MakeDouble(0.5);
  REQUIRE(v1 != v2);
}

TEST_CASE("ValueTest StringComparison", "[ntcore][value]") {
  auto v1 = Value::MakeString("hello");
  auto v2 = Value::MakeString("hello");
  REQUIRE(v1 == v2);
  v2 = Value::MakeString("world");  // different contents
  REQUIRE(v1 != v2);
  v2 = Value::MakeString("goodbye");  // different size
  REQUIRE(v1 != v2);
}

TEST_CASE("ValueTest BooleanArrayComparison", "[ntcore][value]") {
  std::vector<int> vec{1, 0, 1};
  auto v1 = Value::MakeBooleanArray(vec);
  auto v2 = Value::MakeBooleanArray(vec);
  REQUIRE(v1 == v2);

  // different contents
  vec = {1, 1, 1};
  v2 = Value::MakeBooleanArray(vec);
  REQUIRE(v1 != v2);

  // different size
  vec = {1, 0};
  v2 = Value::MakeBooleanArray(vec);
  REQUIRE(v1 != v2);

  // empty
  vec = {};
  v1 = Value::MakeBooleanArray(vec);
  v2 = Value::MakeBooleanArray(vec);
  REQUIRE(v1 == v2);
}

TEST_CASE("ValueTest IntegerArrayComparison", "[ntcore][value]") {
  std::vector<int64_t> vec{-42, 0, 1};
  auto v1 = Value::MakeIntegerArray(vec);
  auto v2 = Value::MakeIntegerArray(vec);
  REQUIRE(v1 == v2);

  // different contents
  vec = {-42, 1, 1};
  v2 = Value::MakeIntegerArray(vec);
  REQUIRE(v1 != v2);

  // different size
  vec = {-42, 0};
  v2 = Value::MakeIntegerArray(vec);
  REQUIRE(v1 != v2);

  // empty
  vec = {};
  v1 = Value::MakeIntegerArray(vec);
  v2 = Value::MakeIntegerArray(vec);
  REQUIRE(v1 == v2);
}

TEST_CASE("ValueTest FloatArrayComparison", "[ntcore][value]") {
  std::vector<float> vec{0.5, 0.25, 0.5};
  auto v1 = Value::MakeFloatArray(vec);
  auto v2 = Value::MakeFloatArray(vec);
  REQUIRE(v1 == v2);

  // different contents
  vec = {0.5, 0.5, 0.5};
  v2 = Value::MakeFloatArray(vec);
  REQUIRE(v1 != v2);

  // different size
  vec = {0.5, 0.25};
  v2 = Value::MakeFloatArray(vec);
  REQUIRE(v1 != v2);

  // empty
  vec = {};
  v1 = Value::MakeFloatArray(vec);
  v2 = Value::MakeFloatArray(vec);
  REQUIRE(v1 == v2);
}

TEST_CASE("ValueTest DoubleArrayComparison", "[ntcore][value]") {
  std::vector<double> vec{0.5, 0.25, 0.5};
  auto v1 = Value::MakeDoubleArray(vec);
  auto v2 = Value::MakeDoubleArray(vec);
  REQUIRE(v1 == v2);

  // different contents
  vec = {0.5, 0.5, 0.5};
  v2 = Value::MakeDoubleArray(vec);
  REQUIRE(v1 != v2);

  // different size
  vec = {0.5, 0.25};
  v2 = Value::MakeDoubleArray(vec);
  REQUIRE(v1 != v2);

  // empty
  vec = {};
  v1 = Value::MakeDoubleArray(vec);
  v2 = Value::MakeDoubleArray(vec);
  REQUIRE(v1 == v2);
}

TEST_CASE("ValueTest StringArrayComparison", "[ntcore][value]") {
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
  REQUIRE(v1 == v2);

  // different contents
  vec.clear();
  vec.push_back("hello");
  vec.push_back("goodby2");
  vec.push_back("string");
  v2 = Value::MakeStringArray(std::move(vec));
  REQUIRE(v1 != v2);

  // different sized contents
  vec.clear();
  vec.push_back("hello");
  vec.push_back("goodbye2");
  vec.push_back("string");
  v2 = Value::MakeStringArray(vec);
  REQUIRE(v1 != v2);

  // different size
  vec.clear();
  vec.push_back("hello");
  vec.push_back("goodbye");
  v2 = Value::MakeStringArray(std::move(vec));
  REQUIRE(v1 != v2);

  // empty
  vec.clear();
  v1 = Value::MakeStringArray(vec);
  v2 = Value::MakeStringArray(std::move(vec));
  REQUIRE(v1 == v2);
}

}  // namespace wpi::nt
