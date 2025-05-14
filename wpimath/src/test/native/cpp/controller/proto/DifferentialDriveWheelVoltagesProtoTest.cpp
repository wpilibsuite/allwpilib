// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <wpi/SmallVector.h>

#include "frc/controller/DifferentialDriveWheelVoltages.h"

using namespace frc;

namespace {

using ProtoType = wpi::Protobuf<frc::DifferentialDriveWheelVoltages>;

const DifferentialDriveWheelVoltages EXPECTED_DATA =
    DifferentialDriveWheelVoltages{0.174_V, 0.191_V};
}  // namespace

TEST(DifferentialDriveWheelVoltagesProtoTest, Roundtrip) {
  wpi::ProtobufMessage<decltype(EXPECTED_DATA)> message;
  wpi::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, EXPECTED_DATA));
  auto unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());
  EXPECT_EQ(EXPECTED_DATA.left.value(), unpacked_data->left.value());
  EXPECT_EQ(EXPECTED_DATA.right.value(), unpacked_data->right.value());
}
