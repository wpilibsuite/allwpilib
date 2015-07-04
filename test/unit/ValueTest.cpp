/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ValueTest.h"

namespace ntimpl {

typedef ValueTest ValueDeathTest;

TEST_F(ValueTest, ConstructEmpty) {
  Value v;
  ASSERT_EQ(NT_UNASSIGNED, ToNT(v).type);
  ASSERT_EQ(0u, ToNT(v).last_change);
  ASSERT_EQ(NT_UNASSIGNED, v.type());
}

TEST_F(ValueTest, ConstructMove) {
  Value v;
  v.SetString("hello");
  ASSERT_EQ(NT_STRING, v.type());
  Value v2(std::move(v));
  ASSERT_EQ(NT_UNASSIGNED, v.type());
  ASSERT_EQ(NT_STRING, v2.type());
}

TEST_F(ValueTest, AssignMove) {
  Value v;
  v.SetString("hello");
  ASSERT_EQ(NT_STRING, v.type());
  Value v2;
  v2.SetDouble(0.5);
  ASSERT_EQ(NT_DOUBLE, v2.type());
  v2 = std::move(v);
  ASSERT_EQ(NT_UNASSIGNED, v.type());
  ASSERT_EQ(NT_STRING, v2.type());
}

TEST_F(ValueTest, Boolean) {
  Value v;
  v.SetBoolean(false);
  ASSERT_EQ(NT_BOOLEAN, ToNT(v).type);
  ASSERT_EQ(0, ToNT(v).data.v_boolean);
  ASSERT_EQ(NT_BOOLEAN, v.type());
  ASSERT_FALSE(v.GetBoolean());
  v.SetBoolean(true);
  ASSERT_EQ(NT_BOOLEAN, ToNT(v).type);
  ASSERT_EQ(1, ToNT(v).data.v_boolean);
  ASSERT_EQ(NT_BOOLEAN, v.type());
  ASSERT_TRUE(v.GetBoolean());
}

TEST_F(ValueTest, Double) {
  Value v;
  v.SetDouble(0.5);
  ASSERT_EQ(NT_DOUBLE, ToNT(v).type);
  ASSERT_EQ(0.5, ToNT(v).data.v_double);
  ASSERT_EQ(NT_DOUBLE, v.type());
  ASSERT_EQ(0.5, v.GetDouble());
  v.SetDouble(0.25);
  ASSERT_EQ(NT_DOUBLE, ToNT(v).type);
  ASSERT_EQ(0.25, ToNT(v).data.v_double);
  ASSERT_EQ(NT_DOUBLE, v.type());
  ASSERT_EQ(0.25, v.GetDouble());
}

TEST_F(ValueTest, String) {
  Value v;
  v.SetString("hello");
  ASSERT_EQ(NT_STRING, ToNT(v).type);
  ASSERT_EQ(llvm::StringRef("hello"), ToNT(v).data.v_string.str);
  ASSERT_EQ(5u, ToNT(v).data.v_string.len);
  ASSERT_EQ(NT_STRING, v.type());
  ASSERT_EQ("hello", v.GetString());
  v.SetString("goodbye");
  ASSERT_EQ(NT_STRING, ToNT(v).type);
  ASSERT_EQ(llvm::StringRef("goodbye"), ToNT(v).data.v_string.str);
  ASSERT_EQ(7u, ToNT(v).data.v_string.len);
  ASSERT_EQ(NT_STRING, v.type());
  ASSERT_EQ("goodbye", v.GetString());
}

TEST_F(ValueTest, Raw) {
  Value v;
  v.SetRaw("hello");
  ASSERT_EQ(NT_RAW, ToNT(v).type);
  ASSERT_EQ(llvm::StringRef("hello"), ToNT(v).data.v_string.str);
  ASSERT_EQ(5u, ToNT(v).data.v_string.len);
  ASSERT_EQ(NT_RAW, v.type());
  ASSERT_EQ("hello", v.GetRaw());
  v.SetRaw("goodbye");
  ASSERT_EQ(NT_RAW, ToNT(v).type);
  ASSERT_EQ(llvm::StringRef("goodbye"), ToNT(v).data.v_string.str);
  ASSERT_EQ(7u, ToNT(v).data.v_string.len);
  ASSERT_EQ(NT_RAW, v.type());
  ASSERT_EQ("goodbye", v.GetRaw());
}

TEST_F(ValueTest, BooleanArray) {
  Value v;
  std::vector<int> vec{1,0,1};
  v.SetBooleanArray(vec);
  ASSERT_EQ(NT_BOOLEAN_ARRAY, ToNT(v).type);
  ASSERT_EQ(3u, ToNT(v).data.arr_boolean.size);
  ASSERT_EQ(vec[0], ToNT(v).data.arr_boolean.arr[0]);
  ASSERT_EQ(vec[1], ToNT(v).data.arr_boolean.arr[1]);
  ASSERT_EQ(vec[2], ToNT(v).data.arr_boolean.arr[2]);
  ASSERT_EQ(NT_BOOLEAN_ARRAY, v.type());
  ASSERT_EQ(llvm::ArrayRef<int>(vec), v.GetBooleanArray());

  // assign with same size
  vec = {0,1,0};
  v.SetBooleanArray(vec);
  ASSERT_EQ(NT_BOOLEAN_ARRAY, ToNT(v).type);
  ASSERT_EQ(3u, ToNT(v).data.arr_boolean.size);
  ASSERT_EQ(vec[0], ToNT(v).data.arr_boolean.arr[0]);
  ASSERT_EQ(vec[1], ToNT(v).data.arr_boolean.arr[1]);
  ASSERT_EQ(vec[2], ToNT(v).data.arr_boolean.arr[2]);
  ASSERT_EQ(NT_BOOLEAN_ARRAY, v.type());
  ASSERT_EQ(llvm::ArrayRef<int>(vec), v.GetBooleanArray());

  // assign with different size
  vec = {1,0};
  v.SetBooleanArray(vec);
  ASSERT_EQ(NT_BOOLEAN_ARRAY, ToNT(v).type);
  ASSERT_EQ(2u, ToNT(v).data.arr_boolean.size);
  ASSERT_EQ(vec[0], ToNT(v).data.arr_boolean.arr[0]);
  ASSERT_EQ(vec[1], ToNT(v).data.arr_boolean.arr[1]);
  ASSERT_EQ(NT_BOOLEAN_ARRAY, v.type());
  ASSERT_EQ(llvm::ArrayRef<int>(vec), v.GetBooleanArray());
}

TEST_F(ValueTest, DoubleArray) {
  Value v;
  std::vector<double> vec{0.5,0.25,0.5};
  v.SetDoubleArray(vec);
  ASSERT_EQ(NT_DOUBLE_ARRAY, ToNT(v).type);
  ASSERT_EQ(3u, ToNT(v).data.arr_double.size);
  ASSERT_EQ(vec[0], ToNT(v).data.arr_double.arr[0]);
  ASSERT_EQ(vec[1], ToNT(v).data.arr_double.arr[1]);
  ASSERT_EQ(vec[2], ToNT(v).data.arr_double.arr[2]);
  ASSERT_EQ(NT_DOUBLE_ARRAY, v.type());
  ASSERT_EQ(llvm::ArrayRef<double>(vec), v.GetDoubleArray());

  // assign with same size
  vec = {0.25,0.5,0.25};
  v.SetDoubleArray(vec);
  ASSERT_EQ(NT_DOUBLE_ARRAY, ToNT(v).type);
  ASSERT_EQ(3u, ToNT(v).data.arr_double.size);
  ASSERT_EQ(vec[0], ToNT(v).data.arr_double.arr[0]);
  ASSERT_EQ(vec[1], ToNT(v).data.arr_double.arr[1]);
  ASSERT_EQ(vec[2], ToNT(v).data.arr_double.arr[2]);
  ASSERT_EQ(NT_DOUBLE_ARRAY, v.type());
  ASSERT_EQ(llvm::ArrayRef<double>(vec), v.GetDoubleArray());

  // assign with different size
  vec = {0.5,0.25};
  v.SetDoubleArray(vec);
  ASSERT_EQ(NT_DOUBLE_ARRAY, ToNT(v).type);
  ASSERT_EQ(2u, ToNT(v).data.arr_double.size);
  ASSERT_EQ(vec[0], ToNT(v).data.arr_double.arr[0]);
  ASSERT_EQ(vec[1], ToNT(v).data.arr_double.arr[1]);
  ASSERT_EQ(NT_DOUBLE_ARRAY, v.type());
  ASSERT_EQ(llvm::ArrayRef<double>(vec), v.GetDoubleArray());
}

TEST_F(ValueTest, StringArray) {
  Value v;
  std::vector<StringValue> vec;
  vec.push_back(StringValue("hello"));
  vec.push_back(StringValue("goodbye"));
  vec.push_back(StringValue("string"));
  v.SetStringArray(vec);
  ASSERT_TRUE(vec.empty());
  ASSERT_EQ(NT_STRING_ARRAY, ToNT(v).type);
  ASSERT_EQ(3u, ToNT(v).data.arr_string.size);
  ASSERT_EQ(llvm::StringRef("hello"), ToNT(v).data.arr_string.arr[0].str);
  ASSERT_EQ(llvm::StringRef("goodbye"), ToNT(v).data.arr_string.arr[1].str);
  ASSERT_EQ(llvm::StringRef("string"), ToNT(v).data.arr_string.arr[2].str);
  ASSERT_EQ(NT_STRING_ARRAY, v.type());
  ASSERT_EQ(llvm::StringRef("hello"), v.GetStringArray()[0]);
  ASSERT_EQ(llvm::StringRef("goodbye"), v.GetStringArray()[1]);
  ASSERT_EQ(llvm::StringRef("string"), v.GetStringArray()[2]);

  // assign with same size
  vec.clear();
  vec.push_back(StringValue("s1"));
  vec.push_back(StringValue("str2"));
  vec.push_back(StringValue("string3"));
  v.SetStringArray(vec);
  ASSERT_TRUE(vec.empty());
  ASSERT_EQ(NT_STRING_ARRAY, ToNT(v).type);
  ASSERT_EQ(3u, ToNT(v).data.arr_string.size);
  ASSERT_EQ(llvm::StringRef("s1"), ToNT(v).data.arr_string.arr[0].str);
  ASSERT_EQ(llvm::StringRef("str2"), ToNT(v).data.arr_string.arr[1].str);
  ASSERT_EQ(llvm::StringRef("string3"), ToNT(v).data.arr_string.arr[2].str);
  ASSERT_EQ(NT_STRING_ARRAY, v.type());
  ASSERT_EQ(llvm::StringRef("s1"), v.GetStringArray()[0]);
  ASSERT_EQ(llvm::StringRef("str2"), v.GetStringArray()[1]);
  ASSERT_EQ(llvm::StringRef("string3"), v.GetStringArray()[2]);

  // assign with different size
  vec.clear();
  vec.push_back(StringValue("short"));
  vec.push_back(StringValue("er"));
  v.SetStringArray(vec);
  ASSERT_TRUE(vec.empty());
  ASSERT_EQ(NT_STRING_ARRAY, ToNT(v).type);
  ASSERT_EQ(2u, ToNT(v).data.arr_string.size);
  ASSERT_EQ(llvm::StringRef("short"), ToNT(v).data.arr_string.arr[0].str);
  ASSERT_EQ(llvm::StringRef("er"), ToNT(v).data.arr_string.arr[1].str);
  ASSERT_EQ(NT_STRING_ARRAY, v.type());
  ASSERT_EQ(llvm::StringRef("short"), v.GetStringArray()[0]);
  ASSERT_EQ(llvm::StringRef("er"), v.GetStringArray()[1]);
}

TEST_F(ValueDeathTest, GetAssertions) {
  Value v;
  ASSERT_DEATH(v.GetBoolean(), "NT_Value::type == NT_BOOLEAN");
  ASSERT_DEATH(v.GetDouble(), "NT_Value::type == NT_DOUBLE");
  ASSERT_DEATH(v.GetString(), "NT_Value::type == NT_STRING");
  ASSERT_DEATH(v.GetRaw(), "NT_Value::type == NT_RAW");
  ASSERT_DEATH(v.GetBooleanArray(), "NT_Value::type == NT_BOOLEAN_ARRAY");
  ASSERT_DEATH(v.GetDoubleArray(), "NT_Value::type == NT_DOUBLE_ARRAY");
  ASSERT_DEATH(v.GetStringArray(), "NT_Value::type == NT_STRING_ARRAY");
}

TEST_F(ValueTest, UnassignedComparison) {
  Value v1, v2;
  ASSERT_EQ(v1, v2);
}

TEST_F(ValueTest, MixedComparison) {
  Value v1, v2;
  v1.SetBoolean(true);
  ASSERT_NE(v1, v2);  // unassigned vs boolean
  v2.SetDouble(0.5);
  ASSERT_NE(v1, v2);  // boolean vs double
}

TEST_F(ValueTest, BooleanComparison) {
  Value v1, v2;
  v1.SetBoolean(true);
  v2.SetBoolean(true);
  ASSERT_EQ(v1, v2);
  v2.SetBoolean(false);
  ASSERT_NE(v1, v2);
}

TEST_F(ValueTest, DoubleComparison) {
  Value v1, v2;
  v1.SetDouble(0.25);
  v2.SetDouble(0.25);
  ASSERT_EQ(v1, v2);
  v2.SetDouble(0.5);
  ASSERT_NE(v1, v2);
}

TEST_F(ValueTest, StringComparison) {
  Value v1, v2;
  v1.SetString("hello");
  v2.SetString("hello");
  ASSERT_EQ(v1, v2);
  v2.SetString("world");  // different contents
  ASSERT_NE(v1, v2);
  v2.SetString("goodbye");  // different size
  ASSERT_NE(v1, v2);
}

TEST_F(ValueTest, BooleanArrayComparison) {
  Value v1, v2;
  std::vector<int> vec{1,0,1};
  v1.SetBooleanArray(vec);
  v2.SetBooleanArray(vec);
  ASSERT_EQ(v1, v2);

  // different contents
  vec = {1,1,1};
  v2.SetBooleanArray(vec);
  ASSERT_NE(v1, v2);

  // different size
  vec = {1,0};
  v2.SetBooleanArray(vec);
  ASSERT_NE(v1, v2);
}

TEST_F(ValueTest, DoubleArrayComparison) {
  Value v1, v2;
  std::vector<double> vec{0.5,0.25,0.5};
  v1.SetDoubleArray(vec);
  v2.SetDoubleArray(vec);
  ASSERT_EQ(v1, v2);

  // different contents
  vec = {0.5,0.5,0.5};
  v2.SetDoubleArray(vec);
  ASSERT_NE(v1, v2);

  // different size
  vec = {0.5,0.25};
  v2.SetDoubleArray(vec);
  ASSERT_NE(v1, v2);
}

TEST_F(ValueTest, StringArrayComparison) {
  Value v1, v2;
  std::vector<StringValue> vec;
  vec.push_back(StringValue("hello"));
  vec.push_back(StringValue("goodbye"));
  vec.push_back(StringValue("string"));
  v1.SetStringArray(vec);
  vec.clear();
  vec.push_back(StringValue("hello"));
  vec.push_back(StringValue("goodbye"));
  vec.push_back(StringValue("string"));
  v2.SetStringArray(vec);
  ASSERT_EQ(v1, v2);

  // different contents
  vec.clear();
  vec.push_back(StringValue("hello"));
  vec.push_back(StringValue("goodby2"));
  vec.push_back(StringValue("string"));
  v2.SetStringArray(vec);
  ASSERT_NE(v1, v2);

  // different sized contents
  vec.clear();
  vec.push_back(StringValue("hello"));
  vec.push_back(StringValue("goodbye2"));
  vec.push_back(StringValue("string"));
  v2.SetStringArray(vec);
  ASSERT_NE(v1, v2);

  // different size
  vec.clear();
  vec.push_back(StringValue("hello"));
  vec.push_back(StringValue("goodbye"));
  v2.SetStringArray(vec);
  ASSERT_NE(v1, v2);
}

}  // namespace ntimpl
