// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <wpi/SmallVector.h>

#include "frc/geometry/Translation3d.h"

using namespace frc;

namespace {

const Translation3d kExpectedData = Translation3d{35.04_m, 22.9_m, 3.504_m};
}  // namespace

TEST(Translation3dProtoTest, Roundtrip) {
  wpi::ProtobufMessage<decltype(kExpectedData)> message;
  wpi::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, kExpectedData));
  auto unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());

  EXPECT_EQ(kExpectedData.X(), unpacked_data->X());
  EXPECT_EQ(kExpectedData.Y(), unpacked_data->Y());
  EXPECT_EQ(kExpectedData.Z(), unpacked_data->Z());
}
