// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <wpi/SmallVector.h>

#include "frc/geometry/Quaternion.h"

using namespace frc;

namespace {

const Quaternion kExpectedData = Quaternion{1.1, 0.191, 35.04, 19.1};
}  // namespace

TEST(QuaternionProtoTest, Roundtrip) {
  wpi::ProtobufMessage<decltype(kExpectedData)> message;
  wpi::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, kExpectedData));
  auto unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());

  EXPECT_EQ(kExpectedData.W(), unpacked_data->W());
  EXPECT_EQ(kExpectedData.X(), unpacked_data->X());
  EXPECT_EQ(kExpectedData.Y(), unpacked_data->Y());
  EXPECT_EQ(kExpectedData.Z(), unpacked_data->Z());
}
