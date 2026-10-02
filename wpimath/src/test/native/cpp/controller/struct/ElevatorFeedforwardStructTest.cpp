// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/controller/ElevatorFeedforward.hpp"

using namespace wpi::math;

namespace {

using StructType = wpi::util::Struct<wpi::math::ElevatorFeedforward>;

static constexpr auto Ks = 1.91_V;
static constexpr auto Kg = 2.29_V;
static constexpr auto Kv = 35.04_V * 1_s / 1_m;
static constexpr auto Ka = 1.74_V * 1_s * 1_s / 1_m;

constexpr ElevatorFeedforward EXPECTED_DATA{Ks, Kg, Kv, Ka};
}  // namespace

TEST_CASE("ElevatorFeedforwardStructTest Roundtrip", "[wpimath]") {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, EXPECTED_DATA);

  ElevatorFeedforward unpacked_data = StructType::Unpack(buffer);

  CHECK(EXPECTED_DATA.GetKs().value() == unpacked_data.GetKs().value());
  CHECK(EXPECTED_DATA.GetKg().value() == unpacked_data.GetKg().value());
  CHECK(EXPECTED_DATA.GetKv().value() == unpacked_data.GetKv().value());
  CHECK(EXPECTED_DATA.GetKa().value() == unpacked_data.GetKa().value());
}
