// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include <wpi/SmallVector.h>

#include "frc/geometry/Rotation2d.h"

using namespace frc;

namespace {

using ProtoType = wpi::Protobuf<frc::Rotation2d>;

const Rotation2d kExpectedData = Rotation2d{1.91_rad};
}  // namespace

TEST(Rotation2dProtoTest, Roundtrip) {
  wpi::SmallVector<uint8_t, 64> buf;
  wpi::ProtoOutputStream ostream{buf, ProtoType::Message()};

  ASSERT_TRUE(ProtoType::Pack(ostream, kExpectedData));

  wpi::ProtoInputStream istream{buf, ProtoType::Message()};
  std::optional<Rotation2d> unpacked_data = ProtoType::Unpack(istream);
  ASSERT_TRUE(unpacked_data.has_value());

  EXPECT_EQ(kExpectedData.Radians().value(), unpacked_data->Radians().value());
}
