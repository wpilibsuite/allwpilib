// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/controller/ElevatorFeedforward.h"

using namespace frc;

namespace {

using StructType = wpi::Struct<frc::ElevatorFeedforward>;
const ElevatorFeedforward kExpectedData{
    ElevatorFeedforward{1.91, 1.1, 1.1, 0.229}};
}  // namespace

TEST(ElevatorFeedforwardStructTest, Roundtrip) {
  uint8_t buffer[StructType::kSize];
  std::memset(buffer, 0, StructType::kSize);
  StructType::Pack(buffer, kExpectedData);

  ElevatorFeedforward unpacked_data = StructType::Unpack(buffer);

  EXPECT_EQ(kExpectedData.ks.value(), unpacked_data.ks.value());
  EXPECT_EQ(kExpectedData.kg.value(), unpacked_data.kg.value());
  EXPECT_EQ(kExpectedData.kv.value(), unpacked_data.kv.value());
  EXPECT_EQ(kExpectedData.ka.value(), unpacked_data.ka.value());
}
