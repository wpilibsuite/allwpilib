// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/controller/ElevatorFeedforward.h"

using namespace frc;

namespace {

using StructType = wpi::Struct<frc::ElevatorFeedforward>;

static constexpr auto S = 1.91_V;
static constexpr auto G = 2.29_V;
static constexpr auto V = 35.04_V * 1_s / 1_m;
static constexpr auto A = 1.74_V * 1_s * 1_s / 1_m;

constexpr ElevatorFeedforward EXPECTED_DATA{S, G, V, A};
}  // namespace

TEST(ElevatorFeedforwardStructTest, Roundtrip) {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, EXPECTED_DATA);

  ElevatorFeedforward unpacked_data = StructType::Unpack(buffer);

  EXPECT_EQ(EXPECTED_DATA.GetKs().value(), unpacked_data.GetKs().value());
  EXPECT_EQ(EXPECTED_DATA.GetKg().value(), unpacked_data.GetKg().value());
  EXPECT_EQ(EXPECTED_DATA.GetKv().value(), unpacked_data.GetKv().value());
  EXPECT_EQ(EXPECTED_DATA.GetKa().value(), unpacked_data.GetKa().value());
}
