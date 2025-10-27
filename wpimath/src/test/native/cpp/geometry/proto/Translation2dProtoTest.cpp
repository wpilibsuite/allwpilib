// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <wpi/util/SmallVector.hpp>

#include "wpi/math/geometry/Translation2d.hpp"

using namespace wpi::math;

namespace {

const Translation2d kExpectedData = Translation2d{3.504_m, 22.9_m};
}  // namespace

TEST(Translation2dProtoTest, Roundtrip) {
  wpi::util::ProtobufMessage<decltype(kExpectedData)> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, kExpectedData));
  auto unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());
  EXPECT_EQ(kExpectedData.X().value(), unpacked_data->X().value());
  EXPECT_EQ(kExpectedData.Y().value(), unpacked_data->Y().value());
}
