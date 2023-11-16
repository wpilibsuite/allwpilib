// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "controller.pb.h"
#include "frc/controller/ArmFeedforward.h"

using namespace frc;

namespace {

using ProtoType = wpi::Protobuf<frc::ArmFeedforward>;

const ArmFeedforward kExpectedData = ArmFeedforward{0.174, 0.229, 4.4, 4.4};
}  // namespace

TEST(ArmFeedforwardProtoTest, Roundtrip) {
  wpi::proto::ProtobufArmFeedforward proto;
  ProtoType::Pack(&proto, kExpectedData);

  ArmFeedforward unpacked_data = ProtoType::Unpack(proto);
  EXPECT_EQ(kExpectedData.ks.value(), unpacked_data.ks.value());
  EXPECT_EQ(kExpectedData.kg.value(), unpacked_data.kg.value());
  EXPECT_EQ(kExpectedData.kv.value(), unpacked_data.kv.value());
  EXPECT_EQ(kExpectedData.ka.value(), unpacked_data.ka.value());
}
