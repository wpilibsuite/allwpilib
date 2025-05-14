// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <wpi/SmallVector.h>

#include "frc/geometry/Ellipse2d.h"

using namespace frc;

namespace {

const Ellipse2d EXPECTED_DATA{
    Pose2d{Translation2d{0.191_m, 2.2_m}, Rotation2d{22.9_rad}}, 1.2_m, 2.3_m};
}  // namespace

TEST(Ellipse2dProtoTest, Roundtrip) {
  wpi::ProtobufMessage<decltype(EXPECTED_DATA)> message;
  wpi::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, EXPECTED_DATA));
  auto unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());

  EXPECT_EQ(EXPECTED_DATA.Center(), unpacked_data->Center());
  EXPECT_EQ(EXPECTED_DATA.XSemiAxis(), unpacked_data->XSemiAxis());
  EXPECT_EQ(EXPECTED_DATA.YSemiAxis(), unpacked_data->YSemiAxis());
}
