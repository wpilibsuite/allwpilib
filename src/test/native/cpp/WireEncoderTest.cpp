/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WireEncoder.h"

#include "gtest/gtest.h"

#include <cfloat>
#include <climits>
#include <string>

#include "llvm/StringRef.h"

#define BUFSIZE 1024

namespace nt {

class WireEncoderTest : public ::testing::Test {
 protected:
  WireEncoderTest() {
    v_empty = std::make_shared<Value>();
    v_boolean = Value::MakeBoolean(true);
    v_double = Value::MakeDouble(1.0);
    v_string = Value::MakeString(llvm::StringRef("hello"));
    v_raw = Value::MakeRaw(llvm::StringRef("hello"));
    v_boolean_array = Value::MakeBooleanArray(std::vector<int>{0, 1, 0});
    v_boolean_array_big = Value::MakeBooleanArray(std::vector<int>(256));
    v_double_array = Value::MakeDoubleArray(std::vector<double>{0.5, 0.25});
    v_double_array_big = Value::MakeDoubleArray(std::vector<double>(256));

    std::vector<std::string> sa;
    sa.push_back("hello");
    sa.push_back("goodbye");
    v_string_array = Value::MakeStringArray(std::move(sa));

    sa.clear();
    for (int i=0; i<256; ++i)
      sa.push_back("h");
    v_string_array_big = Value::MakeStringArray(std::move(sa));

    s_normal = "hello";

    s_long.clear();
    s_long.append(127, '*');
    s_long.push_back('x');

    s_big.clear();
    s_big.append(65534, '*');
    s_big.append(3, 'x');
  }

  std::shared_ptr<Value> v_empty;
  std::shared_ptr<Value> v_boolean, v_double, v_string, v_raw;
  std::shared_ptr<Value> v_boolean_array, v_boolean_array_big;
  std::shared_ptr<Value> v_double_array, v_double_array_big;
  std::shared_ptr<Value> v_string_array, v_string_array_big;

  std::string s_normal, s_long, s_big;
};

TEST_F(WireEncoderTest, Construct) {
  WireEncoder e(0x0300u);
  EXPECT_EQ(0u, e.size());
  EXPECT_EQ(nullptr, e.error());
  EXPECT_EQ(0x0300u, e.proto_rev());
}

TEST_F(WireEncoderTest, SetProtoRev) {
  WireEncoder e(0x0300u);
  e.set_proto_rev(0x0200u);
  EXPECT_EQ(0x0200u, e.proto_rev());
}

TEST_F(WireEncoderTest, Write8) {
  std::size_t off = BUFSIZE-1;
  WireEncoder e(0x0300u);
  for(std::size_t i=0; i<off; ++i) e.Write8(0u);  // test across Reserve()
  e.Write8(5u);
  e.Write8(0x101u);  // should be truncated
  e.Write8(0u);
  ASSERT_EQ(3u, e.size()-off);
  ASSERT_EQ(llvm::StringRef("\x05\x01\x00", 3),
            llvm::StringRef(e.data(), e.size()).substr(off));
}

TEST_F(WireEncoderTest, Write16) {
  std::size_t off = BUFSIZE-2;
  WireEncoder e(0x0300u);
  for(std::size_t i=0; i<off; ++i) e.Write8(0u);  // test across Reserve()
  e.Write16(5u);
  e.Write16(0x10001u);  // should be truncated
  e.Write16(0x4567u);
  e.Write16(0u);
  ASSERT_EQ(8u, e.size()-off);
  ASSERT_EQ(llvm::StringRef("\x00\x05\x00\x01\x45\x67\x00\x00", 8),
            llvm::StringRef(e.data(), e.size()).substr(off));
}

TEST_F(WireEncoderTest, Write32) {
  std::size_t off = BUFSIZE-4;
  WireEncoder e(0x0300u);
  for(std::size_t i=0; i<off; ++i) e.Write8(0u);  // test across Reserve()
  e.Write32(5ul);
  e.Write32(1ul);
  e.Write32(0xabcdul);
  e.Write32(0x12345678ul);
  e.Write32(0ul);
  ASSERT_EQ(20u, e.size()-off);
  ASSERT_EQ(llvm::StringRef(
                "\x00\x00\x00\x05\x00\x00\x00\x01\x00\x00\xab\xcd"
                "\x12\x34\x56\x78\x00\x00\x00\x00",
                20),
            llvm::StringRef(e.data(), e.size()).substr(off));
}

TEST_F(WireEncoderTest, WriteDouble) {
  std::size_t off = BUFSIZE-8;
  WireEncoder e(0x0300u);
  for(std::size_t i=0; i<off; ++i) e.Write8(0u);  // test across Reserve()
  e.WriteDouble(0.0);
  e.WriteDouble(2.3e5);
  e.WriteDouble(std::numeric_limits<double>::infinity());
  e.WriteDouble(DBL_MIN);
  e.WriteDouble(DBL_MAX);
  ASSERT_EQ(40u, e.size()-off);
  // golden values except min and max from
  // http://www.binaryconvert.com/result_double.html
  ASSERT_EQ(llvm::StringRef(
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x41\x0c\x13\x80\x00\x00\x00\x00"
                "\x7f\xf0\x00\x00\x00\x00\x00\x00"
                "\x00\x10\x00\x00\x00\x00\x00\x00"
                "\x7f\xef\xff\xff\xff\xff\xff\xff",
                40),
            llvm::StringRef(e.data(), e.size()).substr(off));
}

TEST_F(WireEncoderTest, WriteUleb128) {
  std::size_t off = BUFSIZE-2;
  WireEncoder e(0x0300u);
  for(std::size_t i=0; i<off; ++i) e.Write8(0u);  // test across Reserve()
  e.WriteUleb128(0ul);
  e.WriteUleb128(0x7ful);
  e.WriteUleb128(0x80ul);
  ASSERT_EQ(4u, e.size()-off);
  ASSERT_EQ(llvm::StringRef("\x00\x7f\x80\x01", 4),
            llvm::StringRef(e.data(), e.size()).substr(off));
}

TEST_F(WireEncoderTest, WriteType) {
  std::size_t off = BUFSIZE-1;
  WireEncoder e(0x0300u);
  for(std::size_t i=0; i<off; ++i) e.Write8(0u);  // test across Reserve()
  e.WriteType(NT_BOOLEAN);
  e.WriteType(NT_DOUBLE);
  e.WriteType(NT_STRING);
  e.WriteType(NT_RAW);
  e.WriteType(NT_BOOLEAN_ARRAY);
  e.WriteType(NT_DOUBLE_ARRAY);
  e.WriteType(NT_STRING_ARRAY);
  e.WriteType(NT_RPC);
  ASSERT_EQ(nullptr, e.error());
  ASSERT_EQ(8u, e.size()-off);
  ASSERT_EQ(llvm::StringRef("\x00\x01\x02\x03\x10\x11\x12\x20", 8),
            llvm::StringRef(e.data(), e.size()).substr(off));
}

TEST_F(WireEncoderTest, WriteTypeError) {
  WireEncoder e(0x0200u);
  e.WriteType(NT_UNASSIGNED);
  EXPECT_EQ(0u, e.size());
  EXPECT_EQ(std::string("unrecognized type"), e.error());

  e.Reset();
  e.WriteType(NT_RAW);
  EXPECT_EQ(0u, e.size());
  EXPECT_EQ(std::string("raw type not supported in protocol < 3.0"), e.error());

  e.Reset();
  e.WriteType(NT_RPC);
  EXPECT_EQ(0u, e.size());
  EXPECT_EQ(std::string("RPC type not supported in protocol < 3.0"), e.error());
}

TEST_F(WireEncoderTest, Reset) {
  WireEncoder e(0x0300u);
  e.WriteType(NT_UNASSIGNED);
  EXPECT_NE(nullptr, e.error());
  e.Reset();
  EXPECT_EQ(nullptr, e.error());

  e.Write8(0u);
  EXPECT_EQ(1u, e.size());
  e.Reset();
  EXPECT_EQ(0u, e.size());
}

TEST_F(WireEncoderTest, GetValueSize2) {
  WireEncoder e(0x0200u);
  EXPECT_EQ(0u, e.GetValueSize(*v_empty));  // empty
  EXPECT_EQ(1u, e.GetValueSize(*v_boolean));
  EXPECT_EQ(8u, e.GetValueSize(*v_double));
  EXPECT_EQ(7u, e.GetValueSize(*v_string));
  EXPECT_EQ(0u, e.GetValueSize(*v_raw));  // not supported

  EXPECT_EQ(1u+3u, e.GetValueSize(*v_boolean_array));
  // truncated
  EXPECT_EQ(1u+255u, e.GetValueSize(*v_boolean_array_big));

  EXPECT_EQ(1u+2u*8u, e.GetValueSize(*v_double_array));
  // truncated
  EXPECT_EQ(1u+255u*8u, e.GetValueSize(*v_double_array_big));

  EXPECT_EQ(1u+7u+9u, e.GetValueSize(*v_string_array));
  // truncated
  EXPECT_EQ(1u+255u*3u, e.GetValueSize(*v_string_array_big));
}

TEST_F(WireEncoderTest, WriteBooleanValue2) {
  WireEncoder e(0x0200u);
  e.WriteValue(*v_boolean);
  auto v_false = Value::MakeBoolean(false);
  e.WriteValue(*v_false);
  ASSERT_EQ(nullptr, e.error());
  ASSERT_EQ(2u, e.size());
  ASSERT_EQ(llvm::StringRef("\x01\x00", 2),
            llvm::StringRef(e.data(), e.size()));
}

TEST_F(WireEncoderTest, WriteDoubleValue2) {
  WireEncoder e(0x0200u);
  e.WriteValue(*v_double);
  ASSERT_EQ(nullptr, e.error());
  ASSERT_EQ(8u, e.size());
  ASSERT_EQ(llvm::StringRef("\x3f\xf0\x00\x00\x00\x00\x00\x00", 8),
            llvm::StringRef(e.data(), e.size()));
}

TEST_F(WireEncoderTest, WriteStringValue2) {
  WireEncoder e(0x0200u);
  e.WriteValue(*v_string);
  ASSERT_EQ(nullptr, e.error());
  ASSERT_EQ(7u, e.size());
  ASSERT_EQ(llvm::StringRef("\x00\x05hello", 7),
            llvm::StringRef(e.data(), e.size()));
}

TEST_F(WireEncoderTest, WriteBooleanArrayValue2) {
  WireEncoder e(0x0200u);
  e.WriteValue(*v_boolean_array);
  ASSERT_EQ(nullptr, e.error());
  ASSERT_EQ(1u+3u, e.size());
  ASSERT_EQ(llvm::StringRef("\x03\x00\x01\x00", 4),
            llvm::StringRef(e.data(), e.size()));

  // truncated
  e.Reset();
  e.WriteValue(*v_boolean_array_big);
  ASSERT_EQ(nullptr, e.error());
  ASSERT_EQ(1u+255u, e.size());
  ASSERT_EQ(llvm::StringRef("\xff\x00", 2), llvm::StringRef(e.data(), 2));
}

TEST_F(WireEncoderTest, WriteDoubleArrayValue2) {
  WireEncoder e(0x0200u);
  e.WriteValue(*v_double_array);
  ASSERT_EQ(nullptr, e.error());
  ASSERT_EQ(1u+2u*8u, e.size());
  ASSERT_EQ(llvm::StringRef("\x02\x3f\xe0\x00\x00\x00\x00\x00\x00"
                            "\x3f\xd0\x00\x00\x00\x00\x00\x00", 17),
            llvm::StringRef(e.data(), e.size()));

  // truncated
  e.Reset();
  e.WriteValue(*v_double_array_big);
  ASSERT_EQ(nullptr, e.error());
  ASSERT_EQ(1u+255u*8u, e.size());
  ASSERT_EQ(llvm::StringRef("\xff\x00", 2), llvm::StringRef(e.data(), 2));
}

TEST_F(WireEncoderTest, WriteStringArrayValue2) {
  WireEncoder e(0x0200u);
  e.WriteValue(*v_string_array);
  ASSERT_EQ(nullptr, e.error());
  ASSERT_EQ(1u+7u+9u, e.size());
  ASSERT_EQ(llvm::StringRef("\x02\x00\x05hello\x00\x07goodbye", 17),
            llvm::StringRef(e.data(), e.size()));

  // truncated
  e.Reset();
  e.WriteValue(*v_string_array_big);
  ASSERT_EQ(nullptr, e.error());
  ASSERT_EQ(1u+255u*3u, e.size());
  ASSERT_EQ(llvm::StringRef("\xff\x00\x01", 3), llvm::StringRef(e.data(), 3));
}

TEST_F(WireEncoderTest, WriteValueError2) {
  WireEncoder e(0x0200u);
  e.WriteValue(*v_empty);  // empty
  ASSERT_EQ(0u, e.size());
  ASSERT_NE(nullptr, e.error());

  e.Reset();
  e.WriteValue(*v_raw);  // not supported
  ASSERT_EQ(0u, e.size());
  ASSERT_NE(nullptr, e.error());
}

TEST_F(WireEncoderTest, GetValueSize3) {
  WireEncoder e(0x0300u);
  EXPECT_EQ(0u, e.GetValueSize(*v_empty));  // empty
  EXPECT_EQ(1u, e.GetValueSize(*v_boolean));
  EXPECT_EQ(8u, e.GetValueSize(*v_double));
  EXPECT_EQ(6u, e.GetValueSize(*v_string));
  EXPECT_EQ(6u, e.GetValueSize(*v_raw));

  EXPECT_EQ(1u+3u, e.GetValueSize(*v_boolean_array));
  // truncated
  EXPECT_EQ(1u+255u, e.GetValueSize(*v_boolean_array_big));

  EXPECT_EQ(1u+2u*8u, e.GetValueSize(*v_double_array));
  // truncated
  EXPECT_EQ(1u+255u*8u, e.GetValueSize(*v_double_array_big));

  EXPECT_EQ(1u+6u+8u, e.GetValueSize(*v_string_array));
  // truncated
  EXPECT_EQ(1u+255u*2u, e.GetValueSize(*v_string_array_big));
}

TEST_F(WireEncoderTest, WriteBooleanValue3) {
  WireEncoder e(0x0300u);
  e.WriteValue(*v_boolean);
  auto v_false = Value::MakeBoolean(false);
  e.WriteValue(*v_false);
  ASSERT_EQ(nullptr, e.error());
  ASSERT_EQ(2u, e.size());
  ASSERT_EQ(llvm::StringRef("\x01\x00", 2),
            llvm::StringRef(e.data(), e.size()));
}

TEST_F(WireEncoderTest, WriteDoubleValue3) {
  WireEncoder e(0x0300u);
  e.WriteValue(*v_double);
  ASSERT_EQ(nullptr, e.error());
  ASSERT_EQ(8u, e.size());
  ASSERT_EQ(llvm::StringRef("\x3f\xf0\x00\x00\x00\x00\x00\x00", 8),
            llvm::StringRef(e.data(), e.size()));
}

TEST_F(WireEncoderTest, WriteStringValue3) {
  WireEncoder e(0x0300u);
  e.WriteValue(*v_string);
  ASSERT_EQ(nullptr, e.error());
  ASSERT_EQ(6u, e.size());
  ASSERT_EQ(llvm::StringRef("\x05hello", 6),
            llvm::StringRef(e.data(), e.size()));
}

TEST_F(WireEncoderTest, WriteRawValue3) {
  WireEncoder e(0x0300u);
  e.WriteValue(*v_raw);
  ASSERT_EQ(nullptr, e.error());
  ASSERT_EQ(6u, e.size());
  ASSERT_EQ(llvm::StringRef("\x05hello", 6),
            llvm::StringRef(e.data(), e.size()));
}

TEST_F(WireEncoderTest, WriteBooleanArrayValue3) {
  WireEncoder e(0x0300u);
  e.WriteValue(*v_boolean_array);
  ASSERT_EQ(nullptr, e.error());
  ASSERT_EQ(1u+3u, e.size());
  ASSERT_EQ(llvm::StringRef("\x03\x00\x01\x00", 4),
            llvm::StringRef(e.data(), e.size()));

  // truncated
  e.Reset();
  e.WriteValue(*v_boolean_array_big);
  ASSERT_EQ(nullptr, e.error());
  ASSERT_EQ(1u+255u, e.size());
  ASSERT_EQ(llvm::StringRef("\xff\x00", 2), llvm::StringRef(e.data(), 2));
}

TEST_F(WireEncoderTest, WriteDoubleArrayValue3) {
  WireEncoder e(0x0300u);
  e.WriteValue(*v_double_array);
  ASSERT_EQ(nullptr, e.error());
  ASSERT_EQ(1u+2u*8u, e.size());
  ASSERT_EQ(llvm::StringRef("\x02\x3f\xe0\x00\x00\x00\x00\x00\x00"
                            "\x3f\xd0\x00\x00\x00\x00\x00\x00", 17),
            llvm::StringRef(e.data(), e.size()));

  // truncated
  e.Reset();
  e.WriteValue(*v_double_array_big);
  ASSERT_EQ(nullptr, e.error());
  ASSERT_EQ(1u+255u*8u, e.size());
  ASSERT_EQ(llvm::StringRef("\xff\x00", 2), llvm::StringRef(e.data(), 2));
}

TEST_F(WireEncoderTest, WriteStringArrayValue3) {
  WireEncoder e(0x0300u);
  e.WriteValue(*v_string_array);
  ASSERT_EQ(nullptr, e.error());
  ASSERT_EQ(1u+6u+8u, e.size());
  ASSERT_EQ(llvm::StringRef("\x02\x05hello\x07goodbye", 15),
            llvm::StringRef(e.data(), e.size()));

  // truncated
  e.Reset();
  e.WriteValue(*v_string_array_big);
  ASSERT_EQ(nullptr, e.error());
  ASSERT_EQ(1u+255u*2u, e.size());
  ASSERT_EQ(llvm::StringRef("\xff\x01", 2), llvm::StringRef(e.data(), 2));
}

TEST_F(WireEncoderTest, WriteValueError3) {
  WireEncoder e(0x0300u);
  e.WriteValue(*v_empty);  // empty
  ASSERT_EQ(0u, e.size());
  ASSERT_NE(nullptr, e.error());
}

TEST_F(WireEncoderTest, GetStringSize2) {
  // 2-byte length
  WireEncoder e(0x0200u);
  EXPECT_EQ(7u, e.GetStringSize(s_normal));
  EXPECT_EQ(130u, e.GetStringSize(s_long));
  // truncated
  EXPECT_EQ(65537u, e.GetStringSize(s_big));
}

TEST_F(WireEncoderTest, WriteString2) {
  WireEncoder e(0x0200u);
  e.WriteString(s_normal);
  EXPECT_EQ(nullptr, e.error());
  EXPECT_EQ(7u, e.size());
  EXPECT_EQ(llvm::StringRef("\x00\x05hello", 7),
            llvm::StringRef(e.data(), e.size()));

  e.Reset();
  e.WriteString(s_long);
  EXPECT_EQ(nullptr, e.error());
  ASSERT_EQ(130u, e.size());
  EXPECT_EQ(llvm::StringRef("\x00\x80**", 4), llvm::StringRef(e.data(), 4));
  EXPECT_EQ('*', e.data()[128]);
  EXPECT_EQ('x', e.data()[129]);

  // truncated
  e.Reset();
  e.WriteString(s_big);
  EXPECT_EQ(nullptr, e.error());
  ASSERT_EQ(65537u, e.size());
  EXPECT_EQ(llvm::StringRef("\xff\xff**", 4), llvm::StringRef(e.data(), 4));
  EXPECT_EQ('*', e.data()[65535]);
  EXPECT_EQ('x', e.data()[65536]);
}

TEST_F(WireEncoderTest, GetStringSize3) {
  // leb128-encoded length
  WireEncoder e(0x0300u);
  EXPECT_EQ(6u, e.GetStringSize(s_normal));
  EXPECT_EQ(130u, e.GetStringSize(s_long));
  EXPECT_EQ(65540u, e.GetStringSize(s_big));
}

TEST_F(WireEncoderTest, WriteString3) {
  WireEncoder e(0x0300u);
  e.WriteString(s_normal);
  EXPECT_EQ(nullptr, e.error());
  EXPECT_EQ(6u, e.size());
  EXPECT_EQ(llvm::StringRef("\x05hello", 6),
            llvm::StringRef(e.data(), e.size()));

  e.Reset();
  e.WriteString(s_long);
  EXPECT_EQ(nullptr, e.error());
  ASSERT_EQ(130u, e.size());
  EXPECT_EQ(llvm::StringRef("\x80\x01**", 4), llvm::StringRef(e.data(), 4));
  EXPECT_EQ('*', e.data()[128]);
  EXPECT_EQ('x', e.data()[129]);

  // NOT truncated
  e.Reset();
  e.WriteString(s_big);
  EXPECT_EQ(nullptr, e.error());
  ASSERT_EQ(65540u, e.size());
  EXPECT_EQ(llvm::StringRef("\x81\x80\x04*", 4), llvm::StringRef(e.data(), 4));
  EXPECT_EQ('*', e.data()[65536]);
  EXPECT_EQ('x', e.data()[65537]);
  EXPECT_EQ('x', e.data()[65538]);
  EXPECT_EQ('x', e.data()[65539]);
}

}  // namespace nt
