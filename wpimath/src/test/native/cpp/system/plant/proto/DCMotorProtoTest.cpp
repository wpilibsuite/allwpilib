// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/system/plant/DCMotor.h"
#include "plant.pb.h"

using namespace frc;

namespace {

using ProtoType = wpi::Protobuf<frc::DCMotor>;

const DCMotor kExpectedData = DCMotor{units::volt_t{1.91},
                                      units::newton_meter_t{19.1},
                                      units::ampere_t{1.74},
                                      units::ampere_t{2.29},
                                      units::radians_per_second_t{2.2},
                                      2};
}  // namespace

TEST(DCMotorProtoTest, Roundtrip) {
  google::protobuf::Arena arena;
  google::protobuf::Message* proto = ProtoType::New(&arena);
  ProtoType::Pack(proto, kExpectedData);

  DCMotor unpacked_data = ProtoType::Unpack(*proto);
  EXPECT_EQ(kExpectedData.nominalVoltage.value(),
            unpacked_data.nominalVoltage.value());
  EXPECT_EQ(kExpectedData.stallTorque.value(),
            unpacked_data.stallTorque.value());
  EXPECT_EQ(kExpectedData.stallCurrent.value(),
            unpacked_data.stallCurrent.value());
  EXPECT_EQ(kExpectedData.freeCurrent.value(),
            unpacked_data.freeCurrent.value());
  EXPECT_EQ(kExpectedData.freeSpeed.value(), unpacked_data.freeSpeed.value());
  EXPECT_EQ(kExpectedData.R.value(), unpacked_data.R.value());
  EXPECT_EQ(kExpectedData.Kv.value(), unpacked_data.Kv.value());
  EXPECT_EQ(kExpectedData.Kt.value(), unpacked_data.Kt.value());
}
