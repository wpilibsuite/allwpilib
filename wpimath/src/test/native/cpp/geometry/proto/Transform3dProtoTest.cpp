// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <google/protobuf/arena.h>
#include <gtest/gtest.h>

#include "frc/geometry/Transform3d.h"

using namespace frc;

namespace {

using ProtoType = wpi::Protobuf<frc::Transform3d>;

const Transform3d kExpectedData =
    Transform3d{Translation3d{0.3504_m, 22.9_m, 3.504_m},
                Rotation3d{Quaternion{0.3504, 35.04, 2.29, 0.3504}}};
}  // namespace

TEST(Transform3dProtoTest, Roundtrip) {
  wpi::ProtobufMessage<Transform3d> message;
  wpi::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, kExpectedData));
  std::optional<Transform3d> unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());

  EXPECT_EQ(kExpectedData.Translation(), unpacked_data.Translation());
  EXPECT_EQ(kExpectedData.Rotation(), unpacked_data.Rotation());
}
