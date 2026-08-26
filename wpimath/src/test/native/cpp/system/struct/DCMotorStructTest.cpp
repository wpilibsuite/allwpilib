// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/system/DCMotor.hpp"

using namespace wpi::math;

using StructType = wpi::util::Struct<wpi::math::DCMotor>;

inline constexpr DCMotor EXPECTED_DATA =
    DCMotor{1.91_V, 19.1_Nm, 1.74_A, 2.29_A, 2.2_rad_per_s, 2};

TEST_CASE("DCMotorStructTest Roundtrip", "[wpimath]") {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, EXPECTED_DATA);

  DCMotor unpacked_data = StructType::Unpack(buffer);

  CHECK(EXPECTED_DATA.nominalVoltage.value() ==
        unpacked_data.nominalVoltage.value());
  CHECK(EXPECTED_DATA.stallTorque.value() == unpacked_data.stallTorque.value());
  CHECK(EXPECTED_DATA.stallCurrent.value() ==
        unpacked_data.stallCurrent.value());
  CHECK(EXPECTED_DATA.freeCurrent.value() == unpacked_data.freeCurrent.value());
  CHECK(EXPECTED_DATA.freeSpeed.value() == unpacked_data.freeSpeed.value());
  CHECK(EXPECTED_DATA.R.value() == unpacked_data.R.value());
  CHECK(EXPECTED_DATA.Kv.value() == unpacked_data.Kv.value());
  CHECK(EXPECTED_DATA.Kt.value() == unpacked_data.Kt.value());
}
