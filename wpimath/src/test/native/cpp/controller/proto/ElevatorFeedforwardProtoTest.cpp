// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "controller.pb.h"
#include "frc/controller/ElevatorFeedforward.h"

using namespace frc;

namespace {

using ProtoType = wpi::Protobuf<frc::ElevatorFeedforward>;

const ElevatorFeedforward kExpectedData =
    ElevatorFeedforward{1.91, 1.1, 1.1, 0.229};
}  // namespace

TEST(ElevatorFeedforwardProtoTest, Roundtrip) {
  wpi::proto::ProtobufElevatorFeedforward proto;
  ProtoType::Pack(&proto, kExpectedData);

  ElevatorFeedforward unpacked_data = ProtoType::Unpack(proto);
  EXPECT_EQ(kExpectedData.ks.value(), unpacked_data.ks.value());
  EXPECT_EQ(kExpectedData.kg.value(), unpacked_data.kg.value());
  EXPECT_EQ(kExpectedData.kv.value(), unpacked_data.kv.value());
  EXPECT_EQ(kExpectedData.ka.value(), unpacked_data.ka.value());
}
