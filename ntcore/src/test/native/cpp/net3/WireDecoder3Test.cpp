// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <stdint.h>

#include <cfloat>
#include <climits>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <gtest/gtest.h>
#include <wpi/SpanMatcher.h>

#include "../TestPrinters.h"
#include "../ValueMatcher.h"
#include "gmock/gmock.h"
#include "net3/WireDecoder3.h"
#include "networktables/NetworkTableValue.h"

using namespace std::string_view_literals;
using testing::_;
using testing::MockFunction;
using testing::StrictMock;

namespace nt {

class MockMessageHandler3 : public net3::MessageHandler3 {
 public:
  MOCK_METHOD0(KeepAlive, void());
  MOCK_METHOD0(ServerHelloDone, void());
  MOCK_METHOD0(ClientHelloDone, void());
  MOCK_METHOD0(ClearEntries, void());
  MOCK_METHOD1(ProtoUnsup, void(unsigned int proto_rev));
  MOCK_METHOD2(ClientHello,
               void(std::string_view self_id, unsigned int proto_rev));
  MOCK_METHOD2(ServerHello, void(unsigned int flags, std::string_view self_id));
  MOCK_METHOD5(EntryAssign, void(std::string_view name, unsigned int id,
                                 unsigned int seq_num, const Value& value,
                                 unsigned int flags));
  MOCK_METHOD3(EntryUpdate,
               void(unsigned int id, unsigned int seq_num, const Value& value));
  MOCK_METHOD2(FlagsUpdate, void(unsigned int id, unsigned int flags));
  MOCK_METHOD1(EntryDelete, void(unsigned int id));
  MOCK_METHOD3(ExecuteRpc, void(unsigned int id, unsigned int uid,
                                std::span<const uint8_t> params));
  MOCK_METHOD3(RpcResponse, void(unsigned int id, unsigned int uid,
                                 std::span<const uint8_t> result));
};

class WireDecoder3Test : public ::testing::Test {
 protected:
  StrictMock<MockMessageHandler3> handler;
  net3::WireDecoder3 decoder{handler};

  void DecodeComplete(std::span<const uint8_t> in) {
    decoder.Execute(&in);
    EXPECT_TRUE(in.empty());
    ASSERT_EQ(decoder.GetError(), "");
  }
};

TEST_F(WireDecoder3Test, KeepAlive) {
  EXPECT_CALL(handler, KeepAlive());
  DecodeComplete("\x00"_us);
}

TEST_F(WireDecoder3Test, ClientHello) {
  EXPECT_CALL(handler, ClientHello(std::string_view{"hello"}, 0x0300u));
  DecodeComplete("\x01\x03\x00\x05hello"_us);
}

TEST_F(WireDecoder3Test, ProtoUnsup) {
  EXPECT_CALL(handler, ProtoUnsup(0x0300u));
  EXPECT_CALL(handler, ProtoUnsup(0x0200u));
  DecodeComplete("\x02\x03\x00\x02\x02\x00"_us);
}

TEST_F(WireDecoder3Test, ServerHelloDone) {
  EXPECT_CALL(handler, ServerHelloDone());
  DecodeComplete("\x03"_us);
}

TEST_F(WireDecoder3Test, ServerHello) {
  EXPECT_CALL(handler, ServerHello(0x03, std::string_view{"hello"}));
  DecodeComplete("\x04\x03\x05hello"_us);
}

TEST_F(WireDecoder3Test, ClientHelloDone) {
  EXPECT_CALL(handler, ClientHelloDone());
  DecodeComplete("\x05"_us);
}

TEST_F(WireDecoder3Test, FlagsUpdate) {
  EXPECT_CALL(handler, FlagsUpdate(0x5678, 0x03));
  DecodeComplete("\x12\x56\x78\x03"_us);
}

TEST_F(WireDecoder3Test, EntryDelete) {
  EXPECT_CALL(handler, EntryDelete(0x5678));
  DecodeComplete("\x13\x56\x78"_us);
}

TEST_F(WireDecoder3Test, ClearEntries) {
  EXPECT_CALL(handler, ClearEntries());
  DecodeComplete("\x14\xd0\x6c\xb2\x7a"_us);
}

TEST_F(WireDecoder3Test, ClearEntriesInvalid) {
  auto in = "\x14\xd0\x6c\xb2\x7b"_us;
  decoder.Execute(&in);
  EXPECT_EQ(decoder.GetError(), "received incorrect CLEAR_ENTRIES magic value");
}

TEST_F(WireDecoder3Test, ExecuteRpc) {
  EXPECT_CALL(handler, ExecuteRpc(0x5678, 0x1234, wpi::SpanEq("hello"_us)));
  DecodeComplete("\x20\x56\x78\x12\x34\x05hello"_us);
}

TEST_F(WireDecoder3Test, RpcResponse) {
  EXPECT_CALL(handler, RpcResponse(0x5678, 0x1234, wpi::SpanEq("hello"_us)));
  DecodeComplete("\x21\x56\x78\x12\x34\x05hello"_us);
}

TEST_F(WireDecoder3Test, UnknownMessage) {
  auto in = "\x23"_us;
  decoder.Execute(&in);
  EXPECT_EQ(decoder.GetError(), "unrecognized message type: 35");
}

TEST_F(WireDecoder3Test, EntryAssignBoolean) {
  EXPECT_CALL(handler, EntryAssign("test"sv, 0x5678, 0x1234,
                                   Value::MakeBoolean(true), 0x9a));
  DecodeComplete("\x10\x04test\x00\x56\x78\x12\x34\x9a\x01"_us);
}

TEST_F(WireDecoder3Test, EntryAssignDouble) {
  EXPECT_CALL(handler, EntryAssign("test"sv, 0x5678, 0x1234,
                                   Value::MakeDouble(2.3e5), 0x9a));
  DecodeComplete(
      "\x10\x04test\x01\x56\x78\x12\x34"
      "\x9a\x41\x0c\x13\x80\x00\x00\x00\x00"_us);
}

TEST_F(WireDecoder3Test, EntryUpdateBoolean) {
  EXPECT_CALL(handler, EntryUpdate(0x5678, 0x1234, Value::MakeBoolean(true)));
  DecodeComplete("\x11\x56\x78\x12\x34\x00\x01"_us);
}

TEST_F(WireDecoder3Test, EntryUpdateDouble) {
  // values except min and max from
  // http://www.binaryconvert.com/result_double.html
  EXPECT_CALL(handler, EntryUpdate(0x5678, 0x1234, Value::MakeDouble(0.0)));
  DecodeComplete("\x11\x56\x78\x12\x34\x01\x00\x00\x00\x00\x00\x00\x00\x00"_us);
  EXPECT_CALL(handler, EntryUpdate(0x5678, 0x1234, Value::MakeDouble(2.3e5)));
  DecodeComplete("\x11\x56\x78\x12\x34\x01\x41\x0c\x13\x80\x00\x00\x00\x00"_us);
  EXPECT_CALL(
      handler,
      EntryUpdate(0x5678, 0x1234,
                  Value::MakeDouble(std::numeric_limits<double>::infinity())));
  DecodeComplete("\x11\x56\x78\x12\x34\x01\x7f\xf0\x00\x00\x00\x00\x00\x00"_us);
  EXPECT_CALL(handler, EntryUpdate(0x5678, 0x1234, Value::MakeDouble(DBL_MIN)));
  DecodeComplete("\x11\x56\x78\x12\x34\x01\x00\x10\x00\x00\x00\x00\x00\x00"_us);
  EXPECT_CALL(handler, EntryUpdate(0x5678, 0x1234, Value::MakeDouble(DBL_MAX)));
  DecodeComplete("\x11\x56\x78\x12\x34\x01\x7f\xef\xff\xff\xff\xff\xff\xff"_us);
}

TEST_F(WireDecoder3Test, EntryUpdateString) {
  EXPECT_CALL(handler,
              EntryUpdate(0x5678, 0x1234, Value::MakeString("hello"sv)));
  DecodeComplete("\x11\x56\x78\x12\x34\x02\x05hello"_us);
}

TEST_F(WireDecoder3Test, EntryUpdateString2) {
  std::vector<uint8_t> in{0x11, 0x56, 0x78, 0x12, 0x34, 0x02, 0x7f};
  in.insert(in.end(), 127, '*');
  std::string out(127, '*');
  EXPECT_CALL(handler,
              EntryUpdate(0x5678, 0x1234, Value::MakeString(std::move(out))));
  DecodeComplete(in);
}

TEST_F(WireDecoder3Test, EntryUpdateStringLarge) {
  std::vector<uint8_t> in{0x11, 0x56, 0x78, 0x12, 0x34, 0x02, 0x80, 0x01};
  in.insert(in.end(), 127, '*');
  in.push_back('x');

  std::string out(127, '*');
  out.push_back('x');

  EXPECT_CALL(handler,
              EntryUpdate(0x5678, 0x1234, Value::MakeString(std::move(out))));
  DecodeComplete(in);
}

TEST_F(WireDecoder3Test, EntryUpdateStringHuge) {
  std::vector<uint8_t> in{0x11, 0x56, 0x78, 0x12, 0x34, 0x02, 0x81, 0x80, 0x04};
  in.insert(in.end(), 65534, '*');
  in.insert(in.end(), 3, 'x');

  std::string out(65534, '*');
  out.append(3, 'x');

  EXPECT_CALL(handler,
              EntryUpdate(0x5678, 0x1234, Value::MakeString(std::move(out))));
  DecodeComplete(in);
}

TEST_F(WireDecoder3Test, EntryUpdateRaw) {
  EXPECT_CALL(handler, EntryUpdate(0x5678, 0x1234, Value::MakeRaw("hello"_us)));
  DecodeComplete("\x11\x56\x78\x12\x34\x03\x05hello"_us);
}

TEST_F(WireDecoder3Test, EntryUpdateBooleanArray) {
  EXPECT_CALL(handler,
              EntryUpdate(0x5678, 0x1234,
                          Value::MakeBooleanArray({false, true, false})));
  DecodeComplete("\x11\x56\x78\x12\x34\x10\x03\x00\x01\x00"_us);
}

TEST_F(WireDecoder3Test, EntryUpdateBooleanArrayLarge) {
  std::vector<uint8_t> in{0x11, 0x56, 0x78, 0x12, 0x34, 0x10, 0xff};
  in.insert(in.end(), 255, 0);
  std::vector<int> out(255, 0);
  EXPECT_CALL(handler, EntryUpdate(0x5678, 0x1234,
                                   Value::MakeBooleanArray(std::move(out))));
  DecodeComplete(in);
}

TEST_F(WireDecoder3Test, EntryUpdateDoubleArray) {
  EXPECT_CALL(handler,
              EntryUpdate(0x5678, 0x1234, Value::MakeDoubleArray({0.5, 0.25})));
  DecodeComplete(
      "\x11\x56\x78\x12\x34\x11\x02"
      "\x3f\xe0\x00\x00\x00\x00\x00\x00"
      "\x3f\xd0\x00\x00\x00\x00\x00\x00"_us);
}

TEST_F(WireDecoder3Test, EntryUpdateDoubleArrayLarge) {
  std::vector<uint8_t> in{0x11, 0x56, 0x78, 0x12, 0x34, 0x11, 0xff};
  in.insert(in.end(), 255 * 8, 0);
  std::vector<double> out(255, 0.0);
  EXPECT_CALL(handler, EntryUpdate(0x5678, 0x1234,
                                   Value::MakeDoubleArray(std::move(out))));
  DecodeComplete(in);
}

TEST_F(WireDecoder3Test, EntryUpdateStringArray) {
  EXPECT_CALL(handler, EntryUpdate(0x5678, 0x1234,
                                   Value::MakeStringArray({"hello", "bye"})));
  DecodeComplete(
      "\x11\x56\x78\x12\x34\x12\x02\x05hello\x03"
      "bye"_us);
}

TEST_F(WireDecoder3Test, EntryUpdateStringArrayLarge) {
  std::vector<uint8_t> in{0x11, 0x56, 0x78, 0x12, 0x34, 0x12, 0xff};
  in.insert(in.end(), 255, 0);
  std::vector<std::string> out(255, "");
  EXPECT_CALL(handler, EntryUpdate(0x5678, 0x1234,
                                   Value::MakeStringArray(std::move(out))));
  DecodeComplete(in);
}

TEST_F(WireDecoder3Test, EntryUpdateRpc) {
  // RPC values are decoded as raw
  EXPECT_CALL(handler, EntryUpdate(0x5678, 0x1234, Value::MakeRaw("hello"_us)));
  DecodeComplete("\x11\x56\x78\x12\x34\x20\x05hello"_us);
}

TEST_F(WireDecoder3Test, EntryUpdateTypeError) {
  auto in = "\x11\x56\x78\x12\x34\x30\x11"_us;
  decoder.Execute(&in);
  ASSERT_EQ(decoder.GetError(), "unrecognized value type");
}

}  // namespace nt
