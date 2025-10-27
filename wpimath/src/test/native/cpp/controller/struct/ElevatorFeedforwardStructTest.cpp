// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpi/math/controller/ElevatorFeedforward.hpp"

using namespace frc;

namespace {

using StructType = wpi::Struct<frc::ElevatorFeedforward>;

static constexpr auto Ks = 1.91_V;
static constexpr auto Kg = 2.29_V;
static constexpr auto Kv = 35.04_V * 1_s / 1_m;
static constexpr auto Ka = 1.74_V * 1_s * 1_s / 1_m;

constexpr ElevatorFeedforward kExpectedData{Ks, Kg, Kv, Ka};
}  // namespace

TEST(ElevatorFeedforwardStructTest, Roundtrip) {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, kExpectedData);

  ElevatorFeedforward unpacked_data = StructType::Unpack(buffer);

  EXPECT_EQ(kExpectedData.GetKs().value(), unpacked_data.GetKs().value());
  EXPECT_EQ(kExpectedData.GetKg().value(), unpacked_data.GetKg().value());
  EXPECT_EQ(kExpectedData.GetKv().value(), unpacked_data.GetKv().value());
  EXPECT_EQ(kExpectedData.GetKa().value(), unpacked_data.GetKa().value());
}
