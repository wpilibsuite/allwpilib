// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "TestProtoInner.hpp"

#include <string>
#include <utility>

#include <gtest/gtest.h>

#include "protobuf_wpiutil.npb.h"
#include "wpi/util/protobuf/ProtobufCallbacks.hpp"

std::optional<TestProtoInner> wpi::util::Protobuf<TestProtoInner>::Unpack(
    wpi::util::ProtoInputStream<TestProtoInner>& stream) {
  wpi::util::UnpackCallback<std::string> str;
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

bool wpi::util::Protobuf<TestProtoInner>::Pack(
    wpi::util::ProtoOutputStream<TestProtoInner>& stream,
    const TestProtoInner& value) {
  wpi::util::PackCallback str{&value.msg};
  wpi_proto_TestProtoInner msg{
      .msg = str.Callback(),
  };
  return stream.Encode(msg);
}

namespace {
using ProtoType = wpi::util::Protobuf<TestProtoInner>;
}  // namespace

TEST(TestProtoInnerTest, RoundtripNanopb) {
  const TestProtoInner kExpectedData = TestProtoInner{"Hello!"};

  wpi::util::ProtobufMessage<TestProtoInner> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, kExpectedData));
  std::optional<TestProtoInner> unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());
  EXPECT_EQ(kExpectedData.msg, unpacked_data->msg);
}

TEST(TestProtoInnerTest, RoundtripNanopbEmpty) {
  const TestProtoInner kExpectedData = TestProtoInner{"Hello!"};

  wpi::util::ProtobufMessage<decltype(kExpectedData)> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, kExpectedData));
  auto unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());
  EXPECT_EQ(kExpectedData.msg, unpacked_data->msg);
}
