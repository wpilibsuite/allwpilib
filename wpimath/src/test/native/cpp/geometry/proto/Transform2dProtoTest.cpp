// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <wpi/SmallVector.h>

#include "wpi/math/geometry/Transform2d.h"

using namespace wpi::math;

namespace {

const Transform2d kExpectedData =
    Transform2d{Translation2d{0.191_m, 2.2_m}, Rotation2d{4.4_rad}};
}  // namespace

TEST(Transform2dProtoTest, Roundtrip) {
  wpi::ProtobufMessage<decltype(kExpectedData)> message;
  wpi::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, kExpectedData));
  auto unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());

  EXPECT_EQ(kExpectedData.Translation(), unpacked_data->Translation());
  EXPECT_EQ(kExpectedData.Rotation(), unpacked_data->Rotation());
}
