// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/controller/ArmFeedforward.hpp"

using namespace wpi::math;

namespace {

using StructType = wpi::util::Struct<wpi::math::ArmFeedforward>;

static constexpr auto Ks = 1.91_V;
static constexpr auto Kg = 2.29_V;
static constexpr auto Kv = 35.04_V * 1_s / 1_rad;
static constexpr auto Ka = 1.74_V * 1_s * 1_s / 1_rad;
const ArmFeedforward kExpectedData{Ks, Kg, Kv, Ka};
}  // namespace

TEST_CASE("ArmFeedforwardStructTest Roundtrip", "[wpimath]") {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, kExpectedData);

  ArmFeedforward unpacked_data = StructType::Unpack(buffer);

  CHECK(kExpectedData.GetKs().value() == unpacked_data.GetKs().value());
  CHECK(kExpectedData.GetKg().value() == unpacked_data.GetKg().value());
  CHECK(kExpectedData.GetKv().value() == unpacked_data.GetKv().value());
  CHECK(kExpectedData.GetKa().value() == unpacked_data.GetKa().value());
}
