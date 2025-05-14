// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <wpi/SmallVector.h>

#include "frc/geometry/Quaternion.h"

using namespace frc;

namespace {

const Quaternion EXPECTED_DATA = Quaternion{1.1, 0.191, 35.04, 19.1};
}  // namespace

TEST(QuaternionProtoTest, Roundtrip) {
  wpi::ProtobufMessage<decltype(EXPECTED_DATA)> message;
  wpi::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, EXPECTED_DATA));
  auto unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());

  EXPECT_EQ(EXPECTED_DATA.W(), unpacked_data->W());
  EXPECT_EQ(EXPECTED_DATA.X(), unpacked_data->X());
  EXPECT_EQ(EXPECTED_DATA.Y(), unpacked_data->Y());
  EXPECT_EQ(EXPECTED_DATA.Z(), unpacked_data->Z());
}
