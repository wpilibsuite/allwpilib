/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "nt_Value.h"
#include "Value_internal.h"

#include "gtest/gtest.h"

namespace nt {

class ValueTest : public ::testing::Test {};

typedef ValueTest ValueDeathTest;

TEST_F(ValueTest, ConstructEmpty) {
  Value v;
  ASSERT_EQ(NT_UNASSIGNED, v.type());
}

TEST_F(ValueTest, Boolean) {
  auto v = Value::MakeBoolean(false);
  ASSERT_EQ(NT_BOOLEAN, v->type());
  ASSERT_FALSE(v->GetBoolean());
  NT_Value cv;
  NT_InitValue(&cv);
  ConvertToC(*v, &cv);
  ASSERT_EQ(NT_BOOLEAN, cv.type);
  ASSERT_EQ(0, cv.data.v_boolean);

  v = Value::MakeBoolean(true);
  ASSERT_EQ(NT_BOOLEAN, v->type());
  ASSERT_TRUE(v->GetBoolean());
  ConvertToC(*v, &cv);
  ASSERT_EQ(NT_BOOLEAN, cv.type);
  ASSERT_EQ(1, cv.data.v_boolean);

  NT_DisposeValue(&cv);
}

TEST_F(ValueTest, Double) {
  auto v = Value::MakeDouble(0.5);
  ASSERT_EQ(NT_DOUBLE, v->type());
  ASSERT_EQ(0.5, v->GetDouble());
  NT_Value cv;
  NT_InitValue(&cv);
  ConvertToC(*v, &cv);
  ASSERT_EQ(NT_DOUBLE, cv.type);
  ASSERT_EQ(0.5, cv.data.v_double);

  v = Value::MakeDouble(0.25);
  ASSERT_EQ(NT_DOUBLE, v->type());
  ASSERT_EQ(0.25, v->GetDouble());
  ConvertToC(*v, &cv);
  ASSERT_EQ(NT_DOUBLE, cv.type);
  ASSERT_EQ(0.25, cv.data.v_double);

  NT_DisposeValue(&cv);
}

TEST_F(ValueTest, String) {
  auto v = Value::MakeString("hello");
  ASSERT_EQ(NT_STRING, v->type());
  ASSERT_EQ("hello", v->GetString());
  NT_Value cv;
  NT_InitValue(&cv);
  ConvertToC(*v, &cv);
  ASSERT_EQ(NT_STRING, cv.type);
  ASSERT_EQ(llvm::StringRef("hello"), cv.data.v_string.str);
  ASSERT_EQ(5u, cv.data.v_string.len);

  v = Value::MakeString("goodbye");
  ASSERT_EQ(NT_STRING, v->type());
  ASSERT_EQ("goodbye", v->GetString());
  ConvertToC(*v, &cv);
  ASSERT_EQ(NT_STRING, cv.type);
  ASSERT_EQ(llvm::StringRef("goodbye"), cv.data.v_string.str);
  ASSERT_EQ(7u, cv.data.v_string.len);

  NT_DisposeValue(&cv);
}

TEST_F(ValueTest, Raw) {
  auto v = Value::MakeRaw("hello");
  ASSERT_EQ(NT_RAW, v->type());
  ASSERT_EQ("hello", v->GetRaw());
  NT_Value cv;
  NT_InitValue(&cv);
  ConvertToC(*v, &cv);
  ASSERT_EQ(NT_RAW, cv.type);
  ASSERT_EQ(llvm::StringRef("hello"), cv.data.v_string.str);
  ASSERT_EQ(5u, cv.data.v_string.len);

  v = Value::MakeRaw("goodbye");
  ASSERT_EQ(NT_RAW, v->type());
  ASSERT_EQ("goodbye", v->GetRaw());
  ConvertToC(*v, &cv);
  ASSERT_EQ(NT_RAW, cv.type);
  ASSERT_EQ(llvm::StringRef("goodbye"), cv.data.v_string.str);
  ASSERT_EQ(7u, cv.data.v_string.len);

  NT_DisposeValue(&cv);
}

TEST_F(ValueTest, BooleanArray) {
  std::vector<int> vec{1,0,1};
  auto v = Value::MakeBooleanArray(vec);
  ASSERT_EQ(NT_BOOLEAN_ARRAY, v->type());
  ASSERT_EQ(llvm::ArrayRef<int>(vec), v->GetBooleanArray());
  NT_Value cv;
  NT_InitValue(&cv);
  ConvertToC(*v, &cv);
  ASSERT_EQ(NT_BOOLEAN_ARRAY, cv.type);
  ASSERT_EQ(3u, cv.data.arr_boolean.size);
  ASSERT_EQ(vec[0], cv.data.arr_boolean.arr[0]);
  ASSERT_EQ(vec[1], cv.data.arr_boolean.arr[1]);
  ASSERT_EQ(vec[2], cv.data.arr_boolean.arr[2]);

  // assign with same size
  vec = {0,1,0};
  v = Value::MakeBooleanArray(vec);
  ASSERT_EQ(NT_BOOLEAN_ARRAY, v->type());
  ASSERT_EQ(llvm::ArrayRef<int>(vec), v->GetBooleanArray());
  ConvertToC(*v, &cv);
  ASSERT_EQ(NT_BOOLEAN_ARRAY, cv.type);
  ASSERT_EQ(3u, cv.data.arr_boolean.size);
  ASSERT_EQ(vec[0], cv.data.arr_boolean.arr[0]);
  ASSERT_EQ(vec[1], cv.data.arr_boolean.arr[1]);
  ASSERT_EQ(vec[2], cv.data.arr_boolean.arr[2]);

  // assign with different size
  vec = {1,0};
  v = Value::MakeBooleanArray(vec);
  ASSERT_EQ(NT_BOOLEAN_ARRAY, v->type());
  ASSERT_EQ(llvm::ArrayRef<int>(vec), v->GetBooleanArray());
  ConvertToC(*v, &cv);
  ASSERT_EQ(NT_BOOLEAN_ARRAY, cv.type);
  ASSERT_EQ(2u, cv.data.arr_boolean.size);
  ASSERT_EQ(vec[0], cv.data.arr_boolean.arr[0]);
  ASSERT_EQ(vec[1], cv.data.arr_boolean.arr[1]);

  NT_DisposeValue(&cv);
}

TEST_F(ValueTest, DoubleArray) {
  std::vector<double> vec{0.5,0.25,0.5};
  auto v = Value::MakeDoubleArray(vec);
  ASSERT_EQ(NT_DOUBLE_ARRAY, v->type());
  ASSERT_EQ(llvm::ArrayRef<double>(vec), v->GetDoubleArray());
  NT_Value cv;
  NT_InitValue(&cv);
  ConvertToC(*v, &cv);
  ASSERT_EQ(NT_DOUBLE_ARRAY, cv.type);
  ASSERT_EQ(3u, cv.data.arr_double.size);
  ASSERT_EQ(vec[0], cv.data.arr_double.arr[0]);
  ASSERT_EQ(vec[1], cv.data.arr_double.arr[1]);
  ASSERT_EQ(vec[2], cv.data.arr_double.arr[2]);

  // assign with same size
  vec = {0.25,0.5,0.25};
  v = Value::MakeDoubleArray(vec);
  ASSERT_EQ(NT_DOUBLE_ARRAY, v->type());
  ASSERT_EQ(llvm::ArrayRef<double>(vec), v->GetDoubleArray());
  ConvertToC(*v, &cv);
  ASSERT_EQ(NT_DOUBLE_ARRAY, cv.type);
  ASSERT_EQ(3u, cv.data.arr_double.size);
  ASSERT_EQ(vec[0], cv.data.arr_double.arr[0]);
  ASSERT_EQ(vec[1], cv.data.arr_double.arr[1]);
  ASSERT_EQ(vec[2], cv.data.arr_double.arr[2]);

  // assign with different size
  vec = {0.5,0.25};
  v = Value::MakeDoubleArray(vec);
  ASSERT_EQ(NT_DOUBLE_ARRAY, v->type());
  ASSERT_EQ(llvm::ArrayRef<double>(vec), v->GetDoubleArray());
  ConvertToC(*v, &cv);
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
  ASSERT_EQ(NT_STRING_ARRAY, v->type());
  ASSERT_EQ(3u, v->GetStringArray().size());
  ASSERT_EQ(llvm::StringRef("hello"), v->GetStringArray()[0]);
  ASSERT_EQ(llvm::StringRef("goodbye"), v->GetStringArray()[1]);
  ASSERT_EQ(llvm::StringRef("string"), v->GetStringArray()[2]);
  NT_Value cv;
  NT_InitValue(&cv);
  ConvertToC(*v, &cv);
  ASSERT_EQ(NT_STRING_ARRAY, cv.type);
  ASSERT_EQ(3u, cv.data.arr_string.size);
  ASSERT_EQ(llvm::StringRef("hello"), cv.data.arr_string.arr[0].str);
  ASSERT_EQ(llvm::StringRef("goodbye"), cv.data.arr_string.arr[1].str);
  ASSERT_EQ(llvm::StringRef("string"), cv.data.arr_string.arr[2].str);

  // assign with same size
  vec.clear();
  vec.push_back("s1");
  vec.push_back("str2");
  vec.push_back("string3");
  v = Value::MakeStringArray(vec);
  ASSERT_EQ(NT_STRING_ARRAY, v->type());
  ASSERT_EQ(3u, v->GetStringArray().size());
  ASSERT_EQ(llvm::StringRef("s1"), v->GetStringArray()[0]);
  ASSERT_EQ(llvm::StringRef("str2"), v->GetStringArray()[1]);
  ASSERT_EQ(llvm::StringRef("string3"), v->GetStringArray()[2]);
  ConvertToC(*v, &cv);
  ASSERT_EQ(NT_STRING_ARRAY, cv.type);
  ASSERT_EQ(3u, cv.data.arr_string.size);
  ASSERT_EQ(llvm::StringRef("s1"), cv.data.arr_string.arr[0].str);
  ASSERT_EQ(llvm::StringRef("str2"), cv.data.arr_string.arr[1].str);
  ASSERT_EQ(llvm::StringRef("string3"), cv.data.arr_string.arr[2].str);

  // assign with different size
  vec.clear();
  vec.push_back("short");
  vec.push_back("er");
  v = Value::MakeStringArray(std::move(vec));
  ASSERT_EQ(NT_STRING_ARRAY, v->type());
  ASSERT_EQ(2u, v->GetStringArray().size());
  ASSERT_EQ(llvm::StringRef("short"), v->GetStringArray()[0]);
  ASSERT_EQ(llvm::StringRef("er"), v->GetStringArray()[1]);
  ConvertToC(*v, &cv);
  ASSERT_EQ(NT_STRING_ARRAY, cv.type);
  ASSERT_EQ(2u, cv.data.arr_string.size);
  ASSERT_EQ(llvm::StringRef("short"), cv.data.arr_string.arr[0].str);
  ASSERT_EQ(llvm::StringRef("er"), cv.data.arr_string.arr[1].str);

  NT_DisposeValue(&cv);
}

TEST_F(ValueDeathTest, GetAssertions) {
  Value v;
  ASSERT_DEATH(v.GetBoolean(), "type == NT_BOOLEAN");
  ASSERT_DEATH(v.GetDouble(), "type == NT_DOUBLE");
  ASSERT_DEATH(v.GetString(), "type == NT_STRING");
  ASSERT_DEATH(v.GetRaw(), "type == NT_RAW");
  ASSERT_DEATH(v.GetBooleanArray(), "type == NT_BOOLEAN_ARRAY");
  ASSERT_DEATH(v.GetDoubleArray(), "type == NT_DOUBLE_ARRAY");
  ASSERT_DEATH(v.GetStringArray(), "type == NT_STRING_ARRAY");
}

TEST_F(ValueTest, UnassignedComparison) {
  Value v1, v2;
  ASSERT_EQ(v1, v2);
}

TEST_F(ValueTest, MixedComparison) {
  Value v1;
  auto v2 = Value::MakeBoolean(true);
  ASSERT_NE(v1, *v2);  // unassigned vs boolean
  auto v3 = Value::MakeDouble(0.5);
  ASSERT_NE(*v2, *v3);  // boolean vs double
}

TEST_F(ValueTest, BooleanComparison) {
  auto v1 = Value::MakeBoolean(true);
  auto v2 = Value::MakeBoolean(true);
  ASSERT_EQ(*v1, *v2);
  v2 = Value::MakeBoolean(false);
  ASSERT_NE(*v1, *v2);
}

TEST_F(ValueTest, DoubleComparison) {
  auto v1 = Value::MakeDouble(0.25);
  auto v2 = Value::MakeDouble(0.25);
  ASSERT_EQ(*v1, *v2);
  v2 = Value::MakeDouble(0.5);
  ASSERT_NE(*v1, *v2);
}

TEST_F(ValueTest, StringComparison) {
  auto v1 = Value::MakeString("hello");
  auto v2 = Value::MakeString("hello");
  ASSERT_EQ(*v1, *v2);
  v2 = Value::MakeString("world");  // different contents
  ASSERT_NE(*v1, *v2);
  v2 = Value::MakeString("goodbye");  // different size
  ASSERT_NE(*v1, *v2);
}

TEST_F(ValueTest, BooleanArrayComparison) {
  std::vector<int> vec{1,0,1};
  auto v1 = Value::MakeBooleanArray(vec);
  auto v2 = Value::MakeBooleanArray(vec);
  ASSERT_EQ(*v1, *v2);

  // different contents
  vec = {1,1,1};
  v2 = Value::MakeBooleanArray(vec);
  ASSERT_NE(*v1, *v2);

  // different size
  vec = {1,0};
  v2 = Value::MakeBooleanArray(vec);
  ASSERT_NE(*v1, *v2);
}

TEST_F(ValueTest, DoubleArrayComparison) {
  std::vector<double> vec{0.5,0.25,0.5};
  auto v1 = Value::MakeDoubleArray(vec);
  auto v2 = Value::MakeDoubleArray(vec);
  ASSERT_EQ(*v1, *v2);

  // different contents
  vec = {0.5,0.5,0.5};
  v2 = Value::MakeDoubleArray(vec);
  ASSERT_NE(*v1, *v2);

  // different size
  vec = {0.5,0.25};
  v2 = Value::MakeDoubleArray(vec);
  ASSERT_NE(*v1, *v2);
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
  ASSERT_EQ(*v1, *v2);

  // different contents
  vec.clear();
  vec.push_back("hello");
  vec.push_back("goodby2");
  vec.push_back("string");
  v2 = Value::MakeStringArray(std::move(vec));
  ASSERT_NE(*v1, *v2);

  // different sized contents
  vec.clear();
  vec.push_back("hello");
  vec.push_back("goodbye2");
  vec.push_back("string");
  v2 = Value::MakeStringArray(vec);
  ASSERT_NE(*v1, *v2);

  // different size
  vec.clear();
  vec.push_back("hello");
  vec.push_back("goodbye");
  v2 = Value::MakeStringArray(std::move(vec));
  ASSERT_NE(*v1, *v2);
}

}  // namespace nt
