/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WireDecoder.h"

#include "gtest/gtest.h"

#include <cfloat>
#include <climits>
#include <string>

#include "llvm/StringRef.h"

namespace nt {

class WireDecoderTest : public ::testing::Test {
 protected:
  WireDecoderTest() {
    v_boolean = Value::MakeBoolean(true);
    v_double = Value::MakeDouble(1.0);
    v_string = Value::MakeString(llvm::StringRef("hello"));
    v_raw = Value::MakeRaw(llvm::StringRef("hello"));
    v_boolean_array = Value::MakeBooleanArray(std::vector<int>{0, 1, 0});
    v_boolean_array_big = Value::MakeBooleanArray(std::vector<int>(255));
    v_double_array = Value::MakeDoubleArray(std::vector<double>{0.5, 0.25});
    v_double_array_big = Value::MakeDoubleArray(std::vector<double>(255));

    std::vector<std::string> sa;
    sa.push_back("hello");
    sa.push_back("goodbye");
    v_string_array = Value::MakeStringArray(std::move(sa));

    sa.clear();
    for (int i=0; i<255; ++i)
      sa.push_back("h");
    v_string_array_big = Value::MakeStringArray(std::move(sa));

    s_normal = std::string("hello");

    s_long.clear();
    s_long.append(127, '*');
    s_long.push_back('x');

    s_big2.clear();
    s_big2.append(65534, '*');
    s_big2.push_back('x');

    s_big3.clear();
    s_big3.append(65534, '*');
    s_big3.append(3, 'x');
  }

  std::shared_ptr<Value> v_boolean, v_double, v_string, v_raw;
  std::shared_ptr<Value> v_boolean_array, v_boolean_array_big;
  std::shared_ptr<Value> v_double_array, v_double_array_big;
  std::shared_ptr<Value> v_string_array, v_string_array_big;

  std::string s_normal, s_long, s_big2, s_big3;
};

TEST_F(WireDecoderTest, Construct) {
  wpi::raw_mem_istream is("", 0);
  WireDecoder d(is, 0x0300u);
  EXPECT_EQ(nullptr, d.error());
  EXPECT_EQ(0x0300u, d.proto_rev());
}

TEST_F(WireDecoderTest, SetProtoRev) {
  wpi::raw_mem_istream is("", 0);
  WireDecoder d(is, 0x0300u);
  d.set_proto_rev(0x0200u);
  EXPECT_EQ(0x0200u, d.proto_rev());
}

TEST_F(WireDecoderTest, Read8) {
  wpi::raw_mem_istream is("\x05\x01\x00", 3);
  WireDecoder d(is, 0x0300u);
  unsigned int val;
  ASSERT_TRUE(d.Read8(&val));
  EXPECT_EQ(5u, val);
  ASSERT_TRUE(d.Read8(&val));
  EXPECT_EQ(1u, val);
  ASSERT_TRUE(d.Read8(&val));
  EXPECT_EQ(0u, val);
  ASSERT_FALSE(d.Read8(&val));
  ASSERT_EQ(nullptr, d.error());
}

TEST_F(WireDecoderTest, Read16) {
  wpi::raw_mem_istream is("\x00\x05\x00\x01\x45\x67\x00\x00", 8);
  WireDecoder d(is, 0x0300u);
  unsigned int val;
  ASSERT_TRUE(d.Read16(&val));
  EXPECT_EQ(5u, val);
  ASSERT_TRUE(d.Read16(&val));
  EXPECT_EQ(1u, val);
  ASSERT_TRUE(d.Read16(&val));
  EXPECT_EQ(0x4567u, val);
  ASSERT_TRUE(d.Read16(&val));
  EXPECT_EQ(0u, val);
  ASSERT_FALSE(d.Read16(&val));
  ASSERT_EQ(nullptr, d.error());
}

TEST_F(WireDecoderTest, Read32) {
  wpi::raw_mem_istream is(
      "\x00\x00\x00\x05\x00\x00\x00\x01\x00\x00\xab\xcd"
      "\x12\x34\x56\x78\x00\x00\x00\x00",
      20);
  WireDecoder d(is, 0x0300u);
  unsigned long val;
  ASSERT_TRUE(d.Read32(&val));
  EXPECT_EQ(5ul, val);
  ASSERT_TRUE(d.Read32(&val));
  EXPECT_EQ(1ul, val);
  ASSERT_TRUE(d.Read32(&val));
  EXPECT_EQ(0xabcdul, val);
  ASSERT_TRUE(d.Read32(&val));
  EXPECT_EQ(0x12345678ul, val);
  ASSERT_TRUE(d.Read32(&val));
  EXPECT_EQ(0ul, val);
  ASSERT_FALSE(d.Read32(&val));
  ASSERT_EQ(nullptr, d.error());
}

TEST_F(WireDecoderTest, ReadDouble) {
  // values except min and max from
  // http://www.binaryconvert.com/result_double.html
  wpi::raw_mem_istream is(
      "\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x41\x0c\x13\x80\x00\x00\x00\x00"
      "\x7f\xf0\x00\x00\x00\x00\x00\x00"
      "\x00\x10\x00\x00\x00\x00\x00\x00"
      "\x7f\xef\xff\xff\xff\xff\xff\xff",
      40);
  WireDecoder d(is, 0x0300u);
  double val;
  ASSERT_TRUE(d.ReadDouble(&val));
  EXPECT_EQ(0.0, val);
  ASSERT_TRUE(d.ReadDouble(&val));
  EXPECT_EQ(2.3e5, val);
  ASSERT_TRUE(d.ReadDouble(&val));
  EXPECT_EQ(std::numeric_limits<double>::infinity(), val);
  ASSERT_TRUE(d.ReadDouble(&val));
  EXPECT_EQ(DBL_MIN, val);
  ASSERT_TRUE(d.ReadDouble(&val));
  EXPECT_EQ(DBL_MAX, val);
  ASSERT_FALSE(d.ReadDouble(&val));
  ASSERT_EQ(nullptr, d.error());
}

TEST_F(WireDecoderTest, ReadUleb128) {
  wpi::raw_mem_istream is("\x00\x7f\x80\x01\x80", 5);
  WireDecoder d(is, 0x0300u);
  unsigned long val;
  ASSERT_TRUE(d.ReadUleb128(&val));
  EXPECT_EQ(0ul, val);
  ASSERT_TRUE(d.ReadUleb128(&val));
  EXPECT_EQ(0x7ful, val);
  ASSERT_TRUE(d.ReadUleb128(&val));
  EXPECT_EQ(0x80ul, val);
  ASSERT_FALSE(d.ReadUleb128(&val)); // partial
  ASSERT_EQ(nullptr, d.error());
}

TEST_F(WireDecoderTest, ReadType) {
  wpi::raw_mem_istream is("\x00\x01\x02\x03\x10\x11\x12\x20", 8);
  WireDecoder d(is, 0x0300u);
  NT_Type val;
  ASSERT_TRUE(d.ReadType(&val));
  EXPECT_EQ(NT_BOOLEAN, val);
  ASSERT_TRUE(d.ReadType(&val));
  EXPECT_EQ(NT_DOUBLE, val);
  ASSERT_TRUE(d.ReadType(&val));
  EXPECT_EQ(NT_STRING, val);
  ASSERT_TRUE(d.ReadType(&val));
  EXPECT_EQ(NT_RAW, val);
  ASSERT_TRUE(d.ReadType(&val));
  EXPECT_EQ(NT_BOOLEAN_ARRAY, val);
  ASSERT_TRUE(d.ReadType(&val));
  EXPECT_EQ(NT_DOUBLE_ARRAY, val);
  ASSERT_TRUE(d.ReadType(&val));
  EXPECT_EQ(NT_STRING_ARRAY, val);
  ASSERT_TRUE(d.ReadType(&val));
  EXPECT_EQ(NT_RPC, val);
  ASSERT_FALSE(d.ReadType(&val));
  ASSERT_EQ(nullptr, d.error());
}

TEST_F(WireDecoderTest, ReadTypeError) {
  wpi::raw_mem_istream is("\x30", 1);
  WireDecoder d(is, 0x0200u);
  NT_Type val;
  ASSERT_FALSE(d.ReadType(&val));
  EXPECT_EQ(NT_UNASSIGNED, val);
  ASSERT_NE(nullptr, d.error());
}

TEST_F(WireDecoderTest, Reset) {
  wpi::raw_mem_istream is("\x30", 1);
  WireDecoder d(is, 0x0200u);
  NT_Type val;
  ASSERT_FALSE(d.ReadType(&val));
  EXPECT_EQ(NT_UNASSIGNED, val);
  ASSERT_NE(nullptr, d.error());
  d.Reset();
  EXPECT_EQ(nullptr, d.error());
}

TEST_F(WireDecoderTest, ReadBooleanValue2) {
  wpi::raw_mem_istream is("\x01\x00", 2);
  WireDecoder d(is, 0x0200u);
  auto val = d.ReadValue(NT_BOOLEAN);
  ASSERT_TRUE(bool(val));
  EXPECT_EQ(*v_boolean, *val);

  auto v_false = Value::MakeBoolean(false);
  val = d.ReadValue(NT_BOOLEAN);
  ASSERT_TRUE(bool(val));
  EXPECT_EQ(*v_false, *val);

  ASSERT_FALSE(d.ReadValue(NT_BOOLEAN));
  ASSERT_EQ(nullptr, d.error());
}

TEST_F(WireDecoderTest, ReadDoubleValue2) {
  wpi::raw_mem_istream is(
      "\x3f\xf0\x00\x00\x00\x00\x00\x00"
      "\x3f\xf0\x00\x00\x00\x00\x00\x00",
      16);
  WireDecoder d(is, 0x0200u);
  auto val = d.ReadValue(NT_DOUBLE);
  ASSERT_TRUE(bool(val));
  EXPECT_EQ(*v_double, *val);

  val = d.ReadValue(NT_DOUBLE);
  ASSERT_TRUE(bool(val));
  EXPECT_EQ(*v_double, *val);

  ASSERT_FALSE(d.ReadValue(NT_DOUBLE));
  ASSERT_EQ(nullptr, d.error());
}

TEST_F(WireDecoderTest, ReadStringValue2) {
  wpi::raw_mem_istream is("\x00\x05hello\x00\x03" "bye\x55", 13);
  WireDecoder d(is, 0x0200u);
  auto val = d.ReadValue(NT_STRING);
  ASSERT_TRUE(bool(val));
  EXPECT_EQ(*v_string, *val);

  auto v_bye = Value::MakeString(llvm::StringRef("bye"));
  val = d.ReadValue(NT_STRING);
  ASSERT_TRUE(bool(val));
  EXPECT_EQ(*v_bye, *val);

  unsigned int b;
  ASSERT_TRUE(d.Read8(&b));
  EXPECT_EQ(0x55u, b);

  ASSERT_FALSE(d.ReadValue(NT_STRING));
  ASSERT_EQ(nullptr, d.error());
}

TEST_F(WireDecoderTest, ReadBooleanArrayValue2) {
  wpi::raw_mem_istream is("\x03\x00\x01\x00\x02\x01\x00\xff", 8);
  WireDecoder d(is, 0x0200u);
  auto val = d.ReadValue(NT_BOOLEAN_ARRAY);
  ASSERT_TRUE(bool(val));
  EXPECT_EQ(*v_boolean_array, *val);

  auto v_boolean_array2 = Value::MakeBooleanArray(std::vector<int>{1, 0});
  val = d.ReadValue(NT_BOOLEAN_ARRAY);
  ASSERT_TRUE(bool(val));
  EXPECT_EQ(*v_boolean_array2, *val);

  ASSERT_FALSE(d.ReadValue(NT_BOOLEAN_ARRAY));
  ASSERT_EQ(nullptr, d.error());
}

TEST_F(WireDecoderTest, ReadBooleanArrayBigValue2) {
  std::string s;
  s.push_back('\xff');
  s.append(255, '\x00');
  wpi::raw_mem_istream is(s.data(), s.size());
  WireDecoder d(is, 0x0200u);
  auto val = d.ReadValue(NT_BOOLEAN_ARRAY);
  ASSERT_TRUE(bool(val));
  EXPECT_EQ(*v_boolean_array_big, *val);

  ASSERT_FALSE(d.ReadValue(NT_BOOLEAN_ARRAY));
  ASSERT_EQ(nullptr, d.error());
}

TEST_F(WireDecoderTest, ReadDoubleArrayValue2) {
  wpi::raw_mem_istream is(
      "\x02\x3f\xe0\x00\x00\x00\x00\x00\x00"
      "\x3f\xd0\x00\x00\x00\x00\x00\x00\x55",
      18);
  WireDecoder d(is, 0x0200u);
  auto val = d.ReadValue(NT_DOUBLE_ARRAY);
  ASSERT_TRUE(bool(val));
  EXPECT_EQ(*v_double_array, *val);

  unsigned int b;
  ASSERT_TRUE(d.Read8(&b));
  EXPECT_EQ(0x55u, b);

  ASSERT_FALSE(d.ReadValue(NT_DOUBLE_ARRAY));
  ASSERT_EQ(nullptr, d.error());
}

TEST_F(WireDecoderTest, ReadDoubleArrayBigValue2) {
  std::string s;
  s.push_back('\xff');
  s.append(255*8, '\x00');
  wpi::raw_mem_istream is(s.data(), s.size());
  WireDecoder d(is, 0x0200u);
  auto val = d.ReadValue(NT_DOUBLE_ARRAY);
  ASSERT_TRUE(bool(val));
  EXPECT_EQ(*v_double_array_big, *val);

  ASSERT_FALSE(d.ReadValue(NT_DOUBLE_ARRAY));
  ASSERT_EQ(nullptr, d.error());
}

TEST_F(WireDecoderTest, ReadStringArrayValue2) {
  wpi::raw_mem_istream is("\x02\x00\x05hello\x00\x07goodbye\x55", 18);
  WireDecoder d(is, 0x0200u);
  auto val = d.ReadValue(NT_STRING_ARRAY);
  ASSERT_TRUE(bool(val));
  EXPECT_EQ(*v_string_array, *val);

  unsigned int b;
  ASSERT_TRUE(d.Read8(&b));
  EXPECT_EQ(0x55u, b);

  ASSERT_FALSE(d.ReadValue(NT_STRING_ARRAY));
  ASSERT_EQ(nullptr, d.error());
}

TEST_F(WireDecoderTest, ReadStringArrayBigValue2) {
  std::string s;
  s.push_back('\xff');
  for (int i=0; i<255; ++i)
    s.append("\x00\x01h", 3);
  wpi::raw_mem_istream is(s.data(), s.size());
  WireDecoder d(is, 0x0200u);
  auto val = d.ReadValue(NT_STRING_ARRAY);
  ASSERT_TRUE(bool(val));
  EXPECT_EQ(*v_string_array_big, *val);

  ASSERT_FALSE(d.ReadValue(NT_STRING_ARRAY));
  ASSERT_EQ(nullptr, d.error());
}

TEST_F(WireDecoderTest, ReadValueError2) {
  wpi::raw_mem_istream is("", 0);
  WireDecoder d(is, 0x0200u);
  ASSERT_FALSE(d.ReadValue(NT_UNASSIGNED));  // unassigned
  ASSERT_NE(nullptr, d.error());

  d.Reset();
  ASSERT_FALSE(d.ReadValue(NT_RAW));  // not supported
  ASSERT_NE(nullptr, d.error());

  d.Reset();
  ASSERT_FALSE(d.ReadValue(NT_RPC));  // not supported
  ASSERT_NE(nullptr, d.error());
}

TEST_F(WireDecoderTest, ReadBooleanValue3) {
  wpi::raw_mem_istream is("\x01\x00", 2);
  WireDecoder d(is, 0x0300u);
  auto val = d.ReadValue(NT_BOOLEAN);
  ASSERT_TRUE(bool(val));
  EXPECT_EQ(*v_boolean, *val);

  auto v_false = Value::MakeBoolean(false);
  val = d.ReadValue(NT_BOOLEAN);
  ASSERT_TRUE(bool(val));
  EXPECT_EQ(*v_false, *val);

  ASSERT_FALSE(d.ReadValue(NT_BOOLEAN));
  ASSERT_EQ(nullptr, d.error());
}

TEST_F(WireDecoderTest, ReadDoubleValue3) {
  wpi::raw_mem_istream is(
      "\x3f\xf0\x00\x00\x00\x00\x00\x00"
      "\x3f\xf0\x00\x00\x00\x00\x00\x00",
      16);
  WireDecoder d(is, 0x0300u);
  auto val = d.ReadValue(NT_DOUBLE);
  ASSERT_TRUE(bool(val));
  EXPECT_EQ(*v_double, *val);

  val = d.ReadValue(NT_DOUBLE);
  ASSERT_TRUE(bool(val));
  EXPECT_EQ(*v_double, *val);

  ASSERT_FALSE(d.ReadValue(NT_DOUBLE));
  ASSERT_EQ(nullptr, d.error());
}

TEST_F(WireDecoderTest, ReadStringValue3) {
  wpi::raw_mem_istream is("\x05hello\x03" "bye\x55", 11);
  WireDecoder d(is, 0x0300u);
  auto val = d.ReadValue(NT_STRING);
  ASSERT_TRUE(bool(val));
  EXPECT_EQ(*v_string, *val);

  auto v_bye = Value::MakeString(llvm::StringRef("bye"));
  val = d.ReadValue(NT_STRING);
  ASSERT_TRUE(bool(val));
  EXPECT_EQ(*v_bye, *val);

  unsigned int b;
  ASSERT_TRUE(d.Read8(&b));
  EXPECT_EQ(0x55u, b);

  ASSERT_FALSE(d.ReadValue(NT_STRING));
  ASSERT_EQ(nullptr, d.error());
}

TEST_F(WireDecoderTest, ReadRawValue3) {
  wpi::raw_mem_istream is("\x05hello\x03" "bye\x55", 11);
  WireDecoder d(is, 0x0300u);
  auto val = d.ReadValue(NT_RAW);
  ASSERT_TRUE(bool(val));
  EXPECT_EQ(*v_raw, *val);

  auto v_bye = Value::MakeRaw(llvm::StringRef("bye"));
  val = d.ReadValue(NT_RAW);
  ASSERT_TRUE(bool(val));
  EXPECT_EQ(*v_bye, *val);

  unsigned int b;
  ASSERT_TRUE(d.Read8(&b));
  EXPECT_EQ(0x55u, b);

  ASSERT_FALSE(d.ReadValue(NT_RAW));
  ASSERT_EQ(nullptr, d.error());
}

TEST_F(WireDecoderTest, ReadBooleanArrayValue3) {
  wpi::raw_mem_istream is("\x03\x00\x01\x00\x02\x01\x00\xff", 8);
  WireDecoder d(is, 0x0300u);
  auto val = d.ReadValue(NT_BOOLEAN_ARRAY);
  ASSERT_TRUE(bool(val));
  EXPECT_EQ(*v_boolean_array, *val);

  auto v_boolean_array2 = Value::MakeBooleanArray(std::vector<int>{1, 0});
  val = d.ReadValue(NT_BOOLEAN_ARRAY);
  ASSERT_TRUE(bool(val));
  EXPECT_EQ(*v_boolean_array2, *val);

  ASSERT_FALSE(d.ReadValue(NT_BOOLEAN_ARRAY));
  ASSERT_EQ(nullptr, d.error());
}

TEST_F(WireDecoderTest, ReadBooleanArrayBigValue3) {
  std::string s;
  s.push_back('\xff');
  s.append(255, '\x00');
  wpi::raw_mem_istream is(s.data(), s.size());
  WireDecoder d(is, 0x0300u);
  auto val = d.ReadValue(NT_BOOLEAN_ARRAY);
  ASSERT_TRUE(bool(val));
  EXPECT_EQ(*v_boolean_array_big, *val);

  ASSERT_FALSE(d.ReadValue(NT_BOOLEAN_ARRAY));
  ASSERT_EQ(nullptr, d.error());
}

TEST_F(WireDecoderTest, ReadDoubleArrayValue3) {
  wpi::raw_mem_istream is(
      "\x02\x3f\xe0\x00\x00\x00\x00\x00\x00"
      "\x3f\xd0\x00\x00\x00\x00\x00\x00\x55",
      18);
  WireDecoder d(is, 0x0300u);
  auto val = d.ReadValue(NT_DOUBLE_ARRAY);
  ASSERT_TRUE(bool(val));
  EXPECT_EQ(*v_double_array, *val);

  unsigned int b;
  ASSERT_TRUE(d.Read8(&b));
  EXPECT_EQ(0x55u, b);

  ASSERT_FALSE(d.ReadValue(NT_DOUBLE_ARRAY));
  ASSERT_EQ(nullptr, d.error());
}

TEST_F(WireDecoderTest, ReadDoubleArrayBigValue3) {
  std::string s;
  s.push_back('\xff');
  s.append(255*8, '\x00');
  wpi::raw_mem_istream is(s.data(), s.size());
  WireDecoder d(is, 0x0300u);
  auto val = d.ReadValue(NT_DOUBLE_ARRAY);
  ASSERT_TRUE(bool(val));
  EXPECT_EQ(*v_double_array_big, *val);

  ASSERT_FALSE(d.ReadValue(NT_DOUBLE_ARRAY));
  ASSERT_EQ(nullptr, d.error());
}

TEST_F(WireDecoderTest, ReadStringArrayValue3) {
  wpi::raw_mem_istream is("\x02\x05hello\x07goodbye\x55", 16);
  WireDecoder d(is, 0x0300u);
  auto val = d.ReadValue(NT_STRING_ARRAY);
  ASSERT_TRUE(bool(val));
  EXPECT_EQ(*v_string_array, *val);

  unsigned int b;
  ASSERT_TRUE(d.Read8(&b));
  EXPECT_EQ(0x55u, b);

  ASSERT_FALSE(d.ReadValue(NT_STRING_ARRAY));
  ASSERT_EQ(nullptr, d.error());
}

TEST_F(WireDecoderTest, ReadStringArrayBigValue3) {
  std::string s;
  s.push_back('\xff');
  for (int i=0; i<255; ++i)
    s.append("\x01h", 2);
  wpi::raw_mem_istream is(s.data(), s.size());
  WireDecoder d(is, 0x0300u);
  auto val = d.ReadValue(NT_STRING_ARRAY);
  ASSERT_TRUE(bool(val));
  EXPECT_EQ(*v_string_array_big, *val);

  ASSERT_FALSE(d.ReadValue(NT_STRING_ARRAY));
  ASSERT_EQ(nullptr, d.error());
}

TEST_F(WireDecoderTest, ReadValueError3) {
  wpi::raw_mem_istream is("", 0);
  WireDecoder d(is, 0x0200u);
  ASSERT_FALSE(d.ReadValue(NT_UNASSIGNED));  // unassigned
  ASSERT_NE(nullptr, d.error());
}

TEST_F(WireDecoderTest, ReadString2) {
  std::string s;
  s.append("\x00\x05", 2);
  s += s_normal;
  s.append("\x00\x80", 2);
  s += s_long;
  s.append("\xff\xff", 2);
  s += s_big2;
  s.push_back('\x55');
  wpi::raw_mem_istream is(s.data(), s.size());
  WireDecoder d(is, 0x0200u);
  std::string outs;
  ASSERT_TRUE(d.ReadString(&outs));
  EXPECT_EQ(s_normal, outs);
  ASSERT_TRUE(d.ReadString(&outs));
  EXPECT_EQ(s_long, outs);
  ASSERT_TRUE(d.ReadString(&outs));
  EXPECT_EQ(s_big2, outs);

  unsigned int b;
  ASSERT_TRUE(d.Read8(&b));
  EXPECT_EQ(0x55u, b);

  ASSERT_FALSE(d.ReadString(&outs));
  ASSERT_EQ(nullptr, d.error());
}

TEST_F(WireDecoderTest, ReadString3) {
  std::string s;
  s.push_back('\x05');
  s += s_normal;
  s.append("\x80\x01", 2);
  s += s_long;
  s.append("\x81\x80\x04", 3);
  s += s_big3;
  s.push_back('\x55');
  wpi::raw_mem_istream is(s.data(), s.size());
  WireDecoder d(is, 0x0300u);
  std::string outs;
  ASSERT_TRUE(d.ReadString(&outs));
  EXPECT_EQ(s_normal, outs);
  ASSERT_TRUE(d.ReadString(&outs));
  EXPECT_EQ(s_long, outs);
  ASSERT_TRUE(d.ReadString(&outs));
  EXPECT_EQ(s_big3, outs);

  unsigned int b;
  ASSERT_TRUE(d.Read8(&b));
  EXPECT_EQ(0x55u, b);

  ASSERT_FALSE(d.ReadString(&outs));
  ASSERT_EQ(nullptr, d.error());
}

}  // namespace nt
