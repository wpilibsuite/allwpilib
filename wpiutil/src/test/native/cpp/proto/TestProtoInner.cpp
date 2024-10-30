// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "TestProtoInner.h"
#include <gtest/gtest.h>

#include <wpi/ProtoHelper.h>

#include "wpiutil.npb.h"

#include "wpi/protobuf/ProtobufCallbacks.h"

const pb_msgdesc_t* wpi::Protobuf<TestProtoInner>::Message() {
  return get_wpi_proto_TestProtoInner_msg();
}

std::optional<TestProtoInner> wpi::Protobuf<TestProtoInner>::Unpack(
    wpi::ProtoInputStream& stream) {
  wpi::StringUnpackCallback str;
  wpi_proto_TestProtoInner msg{
      .msg = str.Callback(),
  };
  if (!stream.DecodeNoInit(msg)) {
    return {};
  }

  auto istr = str.Items();

  if (istr.empty()) {
    return {};
  }

  return TestProtoInner{istr[0]};
}

bool wpi::Protobuf<TestProtoInner>::Pack(wpi::ProtoOutputStream& stream,
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

TEST(TestProtoRunner, RoundtripNanopb) {
  const TestProtoInner kExpectedData = TestProtoInner{"Hello!"};

  wpi::SmallVector<uint8_t, 64> buf;
  wpi::ProtoOutputStream ostream{buf, ProtoType::Message()};

  ASSERT_TRUE(ProtoType::Pack(ostream, kExpectedData));

  wpi::ProtoInputStream istream{buf, ProtoType::Message()};
  std::optional<TestProtoInner> unpacked_data = ProtoType::Unpack(istream);
  ASSERT_TRUE(unpacked_data.has_value());

  EXPECT_EQ(kExpectedData.msg, unpacked_data->msg);
}

TEST(TestProtoRunner, RoundtripNanopbEmpty) {
  const TestProtoInner kExpectedData = TestProtoInner{""};

  wpi::SmallVector<uint8_t, 64> buf;
  wpi::ProtoOutputStream ostream{buf, ProtoType::Message()};

  ASSERT_TRUE(ProtoType::Pack(ostream, kExpectedData));

  wpi::ProtoInputStream istream{buf, ProtoType::Message()};
  std::optional<TestProtoInner> unpacked_data = ProtoType::Unpack(istream);
  ASSERT_TRUE(unpacked_data.has_value());

  EXPECT_EQ(kExpectedData.msg, unpacked_data->msg);
}
