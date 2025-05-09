// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <wpi/SmallVector.h>

#include "frc/geometry/Twist3d.h"

using namespace frc;

namespace {

const Twist3d EXPECTED_DATA =
    Twist3d{1.1_m, 2.29_m, 35.04_m, 0.174_rad, 19.1_rad, 4.4_rad};
}  // namespace

TEST(Twist3dProtoTest, Roundtrip) {
  wpi::ProtobufMessage<decltype(EXPECTED_DATA)> message;
  wpi::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, EXPECTED_DATA));
  auto unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());

  EXPECT_EQ(EXPECTED_DATA.dx.value(), unpacked_data->dx.value());
  EXPECT_EQ(EXPECTED_DATA.dy.value(), unpacked_data->dy.value());
  EXPECT_EQ(EXPECTED_DATA.dz.value(), unpacked_data->dz.value());
  EXPECT_EQ(EXPECTED_DATA.rx.value(), unpacked_data->rx.value());
  EXPECT_EQ(EXPECTED_DATA.ry.value(), unpacked_data->ry.value());
  EXPECT_EQ(EXPECTED_DATA.rz.value(), unpacked_data->rz.value());
}
