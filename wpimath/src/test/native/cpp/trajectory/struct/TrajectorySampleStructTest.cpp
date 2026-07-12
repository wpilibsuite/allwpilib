// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpi/math/trajectory/TrajectorySample.hpp"

using namespace wpi::math;

namespace {

using StructType = wpi::util::Struct<TrajectorySample>;
const TrajectorySample kExpectedData{1.23_s};
}  // namespace

TEST(TrajectorySampleStructTest, Roundtrip) {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, kExpectedData);

  TrajectorySample unpacked_data = StructType::Unpack(buffer);

  EXPECT_EQ(kExpectedData.time.value(), unpacked_data.time.value());
}
