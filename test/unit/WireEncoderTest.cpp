#include "WireEncoder.h"

#include "gtest/gtest.h"

#include <cfloat>
#include <climits>
#include <string>

#include "llvm/StringRef.h"

#define BUFSIZE 1024

namespace ntimpl {

TEST(WireEncoderTest, Construct) {
  WireEncoder e(0x0300u);
  EXPECT_EQ(0u, e.size());
  EXPECT_EQ(nullptr, e.error());
  EXPECT_EQ(0x0300u, e.proto_rev());
}

TEST(WireEncoderTest, SetProtoRev) {
  WireEncoder e(0x0300u);
  e.set_proto_rev(0x0200u);
  EXPECT_EQ(0x0200u, e.proto_rev());
}

TEST(WireEncoderTest, Write8) {
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

TEST(WireEncoderTest, Write16) {
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

TEST(WireEncoderTest, Write32) {
  std::size_t off = BUFSIZE-4;
  WireEncoder e(0x0300u);
  for(std::size_t i=0; i<off; ++i) e.Write8(0u);  // test across Reserve()
  e.Write32(5ul);
  e.Write32(0x100000001ul);  // should be truncated (possibly by compiler)
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

TEST(WireEncoderTest, WriteDouble) {
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

TEST(WireEncoderTest, WriteUleb128) {
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

TEST(WireEncoderTest, WriteType) {
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
  ASSERT_EQ(8u, e.size()-off);
  ASSERT_EQ(llvm::StringRef("\x00\x01\x02\x03\x10\x11\x12\x20", 8),
            llvm::StringRef(e.data(), e.size()).substr(off));
}

TEST(WireEncoderTest, WriteTypeError) {
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

TEST(WireEncoderTest, Reset) {
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

}  // namespace ntimpl
