#include "Value.h"

#include "gtest/gtest.h"

namespace ntimpl {

class ValueTest : public ::testing::Test {
 public:
  NT_Value& ToNT(Value& v) { return v; }
};

typedef ValueTest ValueDeathTest;

TEST_F(ValueTest, ConstructEmpty) {
  Value v;
  ASSERT_EQ(ToNT(v).type, NT_UNASSIGNED);
  ASSERT_EQ(ToNT(v).last_change, 0u);
  ASSERT_EQ(v.type(), NT_UNASSIGNED);
}

TEST_F(ValueTest, ConstructMove) {
  Value v;
  v.SetString("hello");
  ASSERT_EQ(v.type(), NT_STRING);
  Value v2(std::move(v));
  ASSERT_EQ(v.type(), NT_UNASSIGNED);
  ASSERT_EQ(v2.type(), NT_STRING);
}

TEST_F(ValueTest, AssignMove) {
  Value v;
  v.SetString("hello");
  ASSERT_EQ(v.type(), NT_STRING);
  Value v2;
  v2.SetDouble(0.5);
  ASSERT_EQ(v2.type(), NT_DOUBLE);
  v2 = std::move(v);
  ASSERT_EQ(v.type(), NT_UNASSIGNED);
  ASSERT_EQ(v2.type(), NT_STRING);
}

TEST_F(ValueTest, Boolean) {
  Value v;
  v.SetBoolean(false);
  ASSERT_EQ(ToNT(v).type, NT_BOOLEAN);
  ASSERT_EQ(ToNT(v).data.v_boolean, false);
  ASSERT_EQ(v.type(), NT_BOOLEAN);
  ASSERT_EQ(v.GetBoolean(), false);
  v.SetBoolean(true);
  ASSERT_EQ(ToNT(v).type, NT_BOOLEAN);
  ASSERT_EQ(ToNT(v).data.v_boolean, true);
  ASSERT_EQ(v.type(), NT_BOOLEAN);
  ASSERT_EQ(v.GetBoolean(), true);
}

TEST_F(ValueTest, Double) {
  Value v;
  v.SetDouble(0.5);
  ASSERT_EQ(ToNT(v).type, NT_DOUBLE);
  ASSERT_EQ(ToNT(v).data.v_double, 0.5);
  ASSERT_EQ(v.type(), NT_DOUBLE);
  ASSERT_EQ(v.GetDouble(), 0.5);
  v.SetDouble(0.25);
  ASSERT_EQ(ToNT(v).type, NT_DOUBLE);
  ASSERT_EQ(ToNT(v).data.v_double, 0.25);
  ASSERT_EQ(v.type(), NT_DOUBLE);
  ASSERT_EQ(v.GetDouble(), 0.25);
}

TEST_F(ValueTest, String) {
  Value v;
  v.SetString("hello");
  ASSERT_EQ(ToNT(v).type, NT_STRING);
  ASSERT_EQ(ToNT(v).data.v_string.str, llvm::StringRef("hello"));
  ASSERT_EQ(ToNT(v).data.v_string.len, 5u);
  ASSERT_EQ(v.type(), NT_STRING);
  ASSERT_EQ(v.GetString(), "hello");
  v.SetString("goodbye");
  ASSERT_EQ(ToNT(v).type, NT_STRING);
  ASSERT_EQ(ToNT(v).data.v_string.str, llvm::StringRef("goodbye"));
  ASSERT_EQ(ToNT(v).data.v_string.len, 7u);
  ASSERT_EQ(v.type(), NT_STRING);
  ASSERT_EQ(v.GetString(), "goodbye");
}

TEST_F(ValueTest, Raw) {
  Value v;
  v.SetRaw("hello");
  ASSERT_EQ(ToNT(v).type, NT_RAW);
  ASSERT_EQ(ToNT(v).data.v_string.str, llvm::StringRef("hello"));
  ASSERT_EQ(ToNT(v).data.v_string.len, 5u);
  ASSERT_EQ(v.type(), NT_RAW);
  ASSERT_EQ(v.GetRaw(), "hello");
  v.SetRaw("goodbye");
  ASSERT_EQ(ToNT(v).type, NT_RAW);
  ASSERT_EQ(ToNT(v).data.v_string.str, llvm::StringRef("goodbye"));
  ASSERT_EQ(ToNT(v).data.v_string.len, 7u);
  ASSERT_EQ(v.type(), NT_RAW);
  ASSERT_EQ(v.GetRaw(), "goodbye");
}

TEST_F(ValueTest, BooleanArray) {
  Value v;
  std::vector<int> vec{1,0,1};
  v.SetBooleanArray(vec);
  ASSERT_EQ(ToNT(v).type, NT_BOOLEAN_ARRAY);
  ASSERT_EQ(ToNT(v).data.arr_boolean.size, 3u);
  ASSERT_EQ(ToNT(v).data.arr_boolean.arr[0], vec[0]);
  ASSERT_EQ(ToNT(v).data.arr_boolean.arr[1], vec[1]);
  ASSERT_EQ(ToNT(v).data.arr_boolean.arr[2], vec[2]);
  ASSERT_EQ(v.type(), NT_BOOLEAN_ARRAY);
  ASSERT_EQ(v.GetBooleanArray(), llvm::ArrayRef<int>(vec));

  // assign with same size
  vec = {0,1,0};
  v.SetBooleanArray(vec);
  ASSERT_EQ(ToNT(v).type, NT_BOOLEAN_ARRAY);
  ASSERT_EQ(ToNT(v).data.arr_boolean.size, 3u);
  ASSERT_EQ(ToNT(v).data.arr_boolean.arr[0], vec[0]);
  ASSERT_EQ(ToNT(v).data.arr_boolean.arr[1], vec[1]);
  ASSERT_EQ(ToNT(v).data.arr_boolean.arr[2], vec[2]);
  ASSERT_EQ(v.type(), NT_BOOLEAN_ARRAY);
  ASSERT_EQ(v.GetBooleanArray(), llvm::ArrayRef<int>(vec));

  // assign with different size
  vec = {1,0};
  v.SetBooleanArray(vec);
  ASSERT_EQ(ToNT(v).type, NT_BOOLEAN_ARRAY);
  ASSERT_EQ(ToNT(v).data.arr_boolean.size, 2u);
  ASSERT_EQ(ToNT(v).data.arr_boolean.arr[0], vec[0]);
  ASSERT_EQ(ToNT(v).data.arr_boolean.arr[1], vec[1]);
  ASSERT_EQ(v.type(), NT_BOOLEAN_ARRAY);
  ASSERT_EQ(v.GetBooleanArray(), llvm::ArrayRef<int>(vec));
}

TEST_F(ValueTest, DoubleArray) {
  Value v;
  std::vector<double> vec{0.5,0.25,0.5};
  v.SetDoubleArray(vec);
  ASSERT_EQ(ToNT(v).type, NT_DOUBLE_ARRAY);
  ASSERT_EQ(ToNT(v).data.arr_double.size, 3u);
  ASSERT_EQ(ToNT(v).data.arr_double.arr[0], vec[0]);
  ASSERT_EQ(ToNT(v).data.arr_double.arr[1], vec[1]);
  ASSERT_EQ(ToNT(v).data.arr_double.arr[2], vec[2]);
  ASSERT_EQ(v.type(), NT_DOUBLE_ARRAY);
  ASSERT_EQ(v.GetDoubleArray(), llvm::ArrayRef<double>(vec));

  // assign with same size
  vec = {0.25,0.5,0.25};
  v.SetDoubleArray(vec);
  ASSERT_EQ(ToNT(v).type, NT_DOUBLE_ARRAY);
  ASSERT_EQ(ToNT(v).data.arr_double.size, 3u);
  ASSERT_EQ(ToNT(v).data.arr_double.arr[0], vec[0]);
  ASSERT_EQ(ToNT(v).data.arr_double.arr[1], vec[1]);
  ASSERT_EQ(ToNT(v).data.arr_double.arr[2], vec[2]);
  ASSERT_EQ(v.type(), NT_DOUBLE_ARRAY);
  ASSERT_EQ(v.GetDoubleArray(), llvm::ArrayRef<double>(vec));

  // assign with different size
  vec = {0.5,0.25};
  v.SetDoubleArray(vec);
  ASSERT_EQ(ToNT(v).type, NT_DOUBLE_ARRAY);
  ASSERT_EQ(ToNT(v).data.arr_double.size, 2u);
  ASSERT_EQ(ToNT(v).data.arr_double.arr[0], vec[0]);
  ASSERT_EQ(ToNT(v).data.arr_double.arr[1], vec[1]);
  ASSERT_EQ(v.type(), NT_DOUBLE_ARRAY);
  ASSERT_EQ(v.GetDoubleArray(), llvm::ArrayRef<double>(vec));
}

TEST_F(ValueTest, StringArray) {
  Value v;
  std::vector<StringValue> vec;
  vec.push_back(StringValue("hello"));
  vec.push_back(StringValue("goodbye"));
  vec.push_back(StringValue("string"));
  v.SetStringArray(vec);
  ASSERT_TRUE(vec.empty());
  ASSERT_EQ(ToNT(v).type, NT_STRING_ARRAY);
  ASSERT_EQ(ToNT(v).data.arr_string.size, 3u);
  ASSERT_EQ(ToNT(v).data.arr_string.arr[0].str, llvm::StringRef("hello"));
  ASSERT_EQ(ToNT(v).data.arr_string.arr[1].str, llvm::StringRef("goodbye"));
  ASSERT_EQ(ToNT(v).data.arr_string.arr[2].str, llvm::StringRef("string"));
  ASSERT_EQ(v.type(), NT_STRING_ARRAY);
  ASSERT_EQ(v.GetStringArray()[0], llvm::StringRef("hello"));
  ASSERT_EQ(v.GetStringArray()[1], llvm::StringRef("goodbye"));
  ASSERT_EQ(v.GetStringArray()[2], llvm::StringRef("string"));

  // assign with same size
  vec.clear();
  vec.push_back(StringValue("s1"));
  vec.push_back(StringValue("str2"));
  vec.push_back(StringValue("string3"));
  v.SetStringArray(vec);
  ASSERT_TRUE(vec.empty());
  ASSERT_EQ(ToNT(v).type, NT_STRING_ARRAY);
  ASSERT_EQ(ToNT(v).data.arr_string.size, 3u);
  ASSERT_EQ(ToNT(v).data.arr_string.arr[0].str, llvm::StringRef("s1"));
  ASSERT_EQ(ToNT(v).data.arr_string.arr[1].str, llvm::StringRef("str2"));
  ASSERT_EQ(ToNT(v).data.arr_string.arr[2].str, llvm::StringRef("string3"));
  ASSERT_EQ(v.type(), NT_STRING_ARRAY);
  ASSERT_EQ(v.GetStringArray()[0], llvm::StringRef("s1"));
  ASSERT_EQ(v.GetStringArray()[1], llvm::StringRef("str2"));
  ASSERT_EQ(v.GetStringArray()[2], llvm::StringRef("string3"));

  // assign with different size
  vec.clear();
  vec.push_back(StringValue("short"));
  vec.push_back(StringValue("er"));
  v.SetStringArray(vec);
  ASSERT_TRUE(vec.empty());
  ASSERT_EQ(ToNT(v).type, NT_STRING_ARRAY);
  ASSERT_EQ(ToNT(v).data.arr_string.size, 2u);
  ASSERT_EQ(ToNT(v).data.arr_string.arr[0].str, llvm::StringRef("short"));
  ASSERT_EQ(ToNT(v).data.arr_string.arr[1].str, llvm::StringRef("er"));
  ASSERT_EQ(v.type(), NT_STRING_ARRAY);
  ASSERT_EQ(v.GetStringArray()[0], llvm::StringRef("short"));
  ASSERT_EQ(v.GetStringArray()[1], llvm::StringRef("er"));
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
