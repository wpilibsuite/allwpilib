// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "controller.pb.h"
#include "frc/controller/ElevatorFeedforward.h"

using namespace frc;

namespace {

using ProtoType = wpi::Protobuf<frc::ElevatorFeedforward>;

static constexpr auto Ks = 1.91_V;
static constexpr auto Kg = 2.29_V;
static constexpr auto Kv = 35.04_V * 1_s / 1_m;
static constexpr auto Ka = 1.74_V * 1_s * 1_s / 1_m;

constexpr ElevatorFeedforward kExpectedData{Ks, Kg, Kv, Ka};
}  // namespace

TEST(ElevatorFeedforwardProtoTest, Roundtrip) {
  google::protobuf::Arena arena;
  google::protobuf::Message* proto = ProtoType::New(&arena);
  ProtoType::Pack(proto, kExpectedData);

  ElevatorFeedforward unpacked_data = ProtoType::Unpack(*proto);
  EXPECT_EQ(kExpectedData.kS.value(), unpacked_data.kS.value());
  EXPECT_EQ(kExpectedData.kG.value(), unpacked_data.kG.value());
  EXPECT_EQ(kExpectedData.kV.value(), unpacked_data.kV.value());
  EXPECT_EQ(kExpectedData.kA.value(), unpacked_data.kA.value());
}
