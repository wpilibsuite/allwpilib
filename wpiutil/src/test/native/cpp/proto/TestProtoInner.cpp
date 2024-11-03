// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "TestProtoInner.h"

#include <string>
#include <utility>

#include <gtest/gtest.h>

#include "wpi/protobuf/ProtobufCallbacks.h"
#include "wpiutil.npb.h"

std::optional<TestProtoInner> wpi::Protobuf<TestProtoInner>::Unpack(
    wpi::ProtoInputStream<TestProtoInner>& stream) {
  wpi::UnpackCallback<std::string> str;
  wpi_proto_TestProtoInner msg{
      .msg = str.Callback(),
  };
  if (!stream.Decode(msg)) {
    return {};
  }

  auto istr = str.Items();

  if (istr.empty()) {
    return {};
  }

  return TestProtoInner{std::move(istr[0])};
}

bool wpi::Protobuf<TestProtoInner>::Pack(
    wpi::ProtoOutputStream<TestProtoInner>& stream,
    const TestProtoInner& value) {
  wpi::PackCallback str{&value.msg};
  wpi_proto_TestProtoInner msg{
      .msg = str.Callback(),
  };
  return stream.Encode(msg);
}

namespace {
using ProtoType = wpi::Protobuf<TestProtoInner>;
}  // namespace

TEST(TestProtoInnerTest, RoundtripNanopb) {
  const TestProtoInner kExpectedData = TestProtoInner{"Hello!"};

  wpi::ProtobufMessage<TestProtoInner> message;
  wpi::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, kExpectedData));
  std::optional<TestProtoInner> unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());
  EXPECT_EQ(kExpectedData.msg, unpacked_data->msg);
}

TEST(TestProtoInnerTest, RoundtripNanopbEmpty) {
  const TestProtoInner kExpectedData = TestProtoInner{"Hello!"};

  wpi::ProtobufMessage<decltype(kExpectedData)> message;
  wpi::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, kExpectedData));
  auto unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());
  EXPECT_EQ(kExpectedData.msg, unpacked_data->msg);
}
