// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/ProtoHelper.h>

#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "TestProtoInner.h"
#include "wpi/protobuf/ProtobufCallbacks.h"
#include "wpiutil.npb.h"

struct TestProto {
  double double_msg{1};
  float float_msg{2};
  int32_t int32_msg{3};
  int64_t int64_msg{4};
  uint32_t uint32_msg{5};
  uint64_t uint64_msg{6};
  int32_t sint32_msg{7};
  int64_t sint64_msg{8};
  uint32_t fixed32_msg{9};
  uint64_t fixed64_msg{10};
  int32_t sfixed32_msg{11};
  int64_t sfixed64_msg{12};
  bool bool_msg{true};
  std::string string_msg;
  std::vector<uint8_t> bytes_msg;
  TestProtoInner TestProtoInner_msg;
};

template <>
struct wpi::Protobuf<TestProto> {
  using MessageStruct = wpi_proto_TestProto;
  using InputStream = wpi::ProtoInputStream<TestProto>;
  using OutputStream = wpi::ProtoOutputStream<TestProto>;
  static std::optional<TestProto> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const TestProto& value);
};

std::optional<TestProto> wpi::Protobuf<TestProto>::Unpack(InputStream& stream) {
  wpi::UnpackCallback<std::string> str;
  wpi::UnpackCallback<std::vector<uint8_t>> bytes;
  wpi::UnpackCallback<TestProtoInner> inner;
  wpi_proto_TestProto msg;
  msg.string_msg = str.Callback();
  msg.bytes_msg = bytes.Callback();
  msg.TestProtoInner_msg = inner.Callback();
  if (!stream.Decode(msg)) {
    return {};
  }

  auto istr = str.Items();
  auto ibytes = bytes.Items();
  auto iinner = inner.Items();

  if (istr.empty() || ibytes.empty() || iinner.empty()) {
    return {};
  }

  return TestProto{
      .double_msg = msg.double_msg,
      .float_msg = msg.float_msg,
      .int32_msg = msg.int32_msg,
      .int64_msg = msg.int64_msg,
      .uint32_msg = msg.uint32_msg,
      .uint64_msg = msg.uint64_msg,
      .sint32_msg = msg.sint32_msg,
      .sint64_msg = msg.sint64_msg,
      .fixed32_msg = msg.fixed32_msg,
      .fixed64_msg = msg.fixed64_msg,
      .sfixed32_msg = msg.sfixed32_msg,
      .sfixed64_msg = msg.sfixed64_msg,
      .bool_msg = msg.bool_msg,
      .string_msg = std::move(istr[0]),
      .bytes_msg = std::move(ibytes[0]),
      .TestProtoInner_msg = std::move(iinner[0]),
  };
}

bool wpi::Protobuf<TestProto>::Pack(OutputStream& stream,
                                    const TestProto& value) {
  wpi::PackCallback str{&value.string_msg};
  wpi::PackCallback bytes{&value.bytes_msg};
  wpi::PackCallback inner{&value.TestProtoInner_msg};
  wpi_proto_TestProto msg{
      .double_msg = value.double_msg,
      .float_msg = value.float_msg,
      .int32_msg = value.int32_msg,
      .int64_msg = value.int64_msg,
      .uint32_msg = value.uint32_msg,
      .uint64_msg = value.uint64_msg,
      .sint32_msg = value.sint32_msg,
      .sint64_msg = value.sint64_msg,
      .fixed32_msg = value.fixed32_msg,
      .fixed64_msg = value.fixed64_msg,
      .sfixed32_msg = value.sfixed32_msg,
      .sfixed64_msg = value.sfixed64_msg,
      .bool_msg = value.bool_msg,
      .string_msg = str.Callback(),
      .bytes_msg = bytes.Callback(),
      .TestProtoInner_msg = inner.Callback(),
  };
  return stream.Encode(msg);
}

namespace {
using ProtoType = wpi::Protobuf<TestProto>;
}  // namespace

TEST(TestProtoTest, RoundtripNanopb) {
  const TestProto kExpectedData = TestProto{};

  wpi::ProtobufMessage<TestProto> message;
  wpi::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, kExpectedData));
  std::optional<TestProto> unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());
  ASSERT_TRUE(unpacked_data.has_value());
}
