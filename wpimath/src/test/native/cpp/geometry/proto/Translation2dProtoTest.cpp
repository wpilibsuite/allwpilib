// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <wpi/SmallVector.h>

#include "frc/geometry/Translation2d.h"

using namespace frc;

namespace {

const Translation2d EXPECTED_DATA = Translation2d{3.504_m, 22.9_m};
}  // namespace

TEST(Translation2dProtoTest, Roundtrip) {
  wpi::ProtobufMessage<decltype(EXPECTED_DATA)> message;
  wpi::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, EXPECTED_DATA));
  auto unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());
  EXPECT_EQ(EXPECTED_DATA.X().value(), unpacked_data->X().value());
  EXPECT_EQ(EXPECTED_DATA.Y().value(), unpacked_data->Y().value());
}
