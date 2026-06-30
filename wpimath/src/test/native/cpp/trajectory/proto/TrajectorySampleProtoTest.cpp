// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpi/math/trajectory/TrajectorySample.hpp"
#include "wpi/util/SmallVector.hpp"

using namespace wpi::math;

namespace {
const TrajectorySample kExpectedData =
    TrajectorySample{1.5_s, Pose2d{1.1_m, 2.2_m, 30_deg},
                     ChassisVelocities{1_mps, 2_mps, 3_rad_per_s},
                     ChassisAccelerations{4_mps_sq, 5_mps_sq, 6_rad_per_s_sq}};
}  // namespace

TEST(TrajectorySampleProtoTest, Roundtrip) {
  wpi::util::ProtobufMessage<decltype(kExpectedData)> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, kExpectedData));
  auto unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());

  EXPECT_EQ(kExpectedData, unpacked_data.value());
}
