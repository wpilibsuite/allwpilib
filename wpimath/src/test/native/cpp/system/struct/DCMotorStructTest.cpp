// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/system/DCMotor.hpp"

using namespace wpi::math;

using StructType = wpi::util::Struct<wpi::math::DCMotor>;

inline constexpr DCMotor kExpectedData =
    DCMotor{1.91_V, 19.1_Nm, 1.74_A, 2.29_A, 2.2_rad_per_s, 2};

TEST_CASE("DCMotorStructTest Roundtrip", "[wpimath]") {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, kExpectedData);

  DCMotor unpacked_data = StructType::Unpack(buffer);

  CHECK(kExpectedData.nominalVoltage.value() ==
        unpacked_data.nominalVoltage.value());
  CHECK(kExpectedData.stallTorque.value() == unpacked_data.stallTorque.value());
  CHECK(kExpectedData.stallCurrent.value() ==
        unpacked_data.stallCurrent.value());
  CHECK(kExpectedData.freeCurrent.value() == unpacked_data.freeCurrent.value());
  CHECK(kExpectedData.freeSpeed.value() == unpacked_data.freeSpeed.value());
  CHECK(kExpectedData.R.value() == unpacked_data.R.value());
  CHECK(kExpectedData.Kv.value() == unpacked_data.Kv.value());
  CHECK(kExpectedData.Kt.value() == unpacked_data.Kt.value());
}
