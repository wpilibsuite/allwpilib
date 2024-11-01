// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include <wpi/SmallVector.h>

#include "frc/geometry/Rotation3d.h"

using namespace frc;

namespace {

using ProtoType = wpi::Protobuf<frc::Rotation3d>;

const Rotation3d kExpectedData =
    Rotation3d{Quaternion{2.29, 0.191, 0.191, 17.4}};
}  // namespace

TEST(Rotation3dProtoTest, Roundtrip) {
  wpi::ProtobufMessage<Rotation3d> message;
  wpi::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, kExpectedData));
  std::optional<Rotation3d> unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());

  EXPECT_EQ(kExpectedData.GetQuaternion(), unpacked_data->GetQuaternion());
}
