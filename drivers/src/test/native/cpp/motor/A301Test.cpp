// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/drivers/motor/A301.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/hal/Errors.h"

using namespace wpi;

TEST_CASE("A301 high-level API", "[drivers][a301]") {
  A301 a301{CANBusMap::CAN_S0, 31};
  CHECK(a301.GetBusId() == 0);
  CHECK(a301.GetDeviceId() == 31);

  a301.SetThrottle(0.25);
  CHECK(a301.GetThrottle() == Catch::Approx(0.25));
  a301.SetVoltage(wpi::units::volt_t{6.0});
  CHECK(a301.GetThrottle() == Catch::Approx(0.5));

  CHECK(a301.SetVelocity(125.0) == A301Error::kOk);
  CHECK(a301.SetRelativePosition(3.0) == A301Error::kOk);
  CHECK(a301.SetRelativePositionWithSpeed(3.0, 50.0) == A301Error::kOk);
  CHECK(a301.SetAbsolutePosition(0.25) == A301Error::kOk);
  CHECK(a301.SetAbsolutePositionWithSpeed(0.25, 50.0) == A301Error::kOk);
  CHECK(a301.SetCurrent(10.0) == A301Error::kOk);
  CHECK(a301.SetRelativeEncoderPosition(2.0) == A301Error::kOk);
  CHECK(a301.SetAbsoluteEncoderPosition(0.1) == A301Error::kOk);
  CHECK(a301.SetIdleMode(A301::IdleMode::kBrake) == A301Error::kOk);
  CHECK(a301.EnableAbsolutePositionContinuousInput() == A301Error::kOk);
  CHECK(a301.DisableAbsolutePositionContinuousInput() == A301Error::kOk);
  CHECK(a301.ClearFaults() == A301Error::kOk);

  a301.SetInverted(true);
  CHECK(a301.GetInverted());
  CHECK(a301.SetAbsoluteEncoderRangeOffset(0.25) == A301Error::kOk);
  CHECK(a301.SetAbsoluteEncoderRangeOffset(0.75) ==
        A301Error::kParamInvalid);

  auto voltage = a301.GetBusVoltage();
  CHECK_FALSE(voltage.IsValid());
  CHECK(voltage.GetError() == A301Error::kTimeout);

  a301.Disable();
  CHECK(a301.GetThrottle() == Catch::Approx(0.0));
}

TEST_CASE("A301 status signal maps HAL errors", "[drivers][a301]") {
  A301StatusSignal<int> valid{4, 0, 301};
  CHECK(valid.IsValid());
  CHECK(valid.Get() == 4);
  CHECK(valid.Get(8) == 4);
  CHECK(valid.GetTimestamp() == 301);
  CHECK(valid.GetStatus() == 0);
  CHECK(valid.Map([](int value) { return value * 2; }).Get() == 8);

  A301StatusSignal<int> timeout{4, HAL_CAN_TIMEOUT, 302};
  CHECK_FALSE(timeout.IsValid());
  CHECK(timeout.Get(8) == 8);
  CHECK(timeout.GetError() == A301Error::kTimeout);
}
