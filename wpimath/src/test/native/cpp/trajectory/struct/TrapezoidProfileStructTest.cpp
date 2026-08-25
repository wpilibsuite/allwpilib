// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/struct/TrapezoidProfileStruct.hpp"

#include <cmath>
#include <cstring>
#include <stdexcept>

#include <catch2/catch_test_macros.hpp>

using namespace wpi::math;

TEST_CASE("TrapezoidProfileStructTest NonBaseDistanceConstraintRoundtrip",
          "[wpimath]") {
  using Constraints = TrapezoidProfile<wpi::units::feet_>::Constraints;
  using StructType = wpi::util::Struct<Constraints>;

  const Constraints expected{Constraints::Velocity_t{3.0},
                             Constraints::Acceleration_t{4.0}};
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());

  StructType::Pack(buffer, expected);

  const double packedVelocity = wpi::util::UnpackStruct<double, 0>(buffer);
  const double packedAcceleration = wpi::util::UnpackStruct<double, 8>(buffer);

  CHECK(std::abs(0.9144 - packedVelocity) <= 1e-12);
  CHECK(std::abs(1.2192 - packedAcceleration) <= 1e-12);

  Constraints unpackedData = StructType::Unpack(buffer);

  CHECK(std::abs(expected.maxVelocity.value() -
                 unpackedData.maxVelocity.value()) <= 1e-12);
  CHECK(std::abs(expected.maxAcceleration.value() -
                 unpackedData.maxAcceleration.value()) <= 1e-12);
}

TEST_CASE("TrapezoidProfileStructTest ConstraintsRejectZeroOnUnpack",
          "[wpimath]") {
  using Constraints = TrapezoidProfile<wpi::units::meters_>::Constraints;
  using StructType = wpi::util::Struct<Constraints>;

  uint8_t zeroVelocityBuffer[StructType::GetSize()];
  std::memset(zeroVelocityBuffer, 0, StructType::GetSize());
  wpi::util::PackStruct<8>(zeroVelocityBuffer, 1.0);
  CHECK_THROWS_AS(StructType::Unpack(zeroVelocityBuffer), std::domain_error);

  uint8_t zeroAccelerationBuffer[StructType::GetSize()];
  std::memset(zeroAccelerationBuffer, 0, StructType::GetSize());
  wpi::util::PackStruct<0>(zeroAccelerationBuffer, 1.0);
  CHECK_THROWS_AS(StructType::Unpack(zeroAccelerationBuffer),
                  std::domain_error);
}
