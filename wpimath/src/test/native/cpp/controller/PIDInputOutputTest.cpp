// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/controller/PIDController.hpp"

TEST_CASE("PIDInputOutputTest ContinuousInput", "[wpimath]") {
  wpi::math::PIDController controller{0.0, 0.0, 0.0};

  controller.SetP(1);
  controller.EnableContinuousInput(-180, 180);
  CHECK(controller.Calculate(-179, 179) == Catch::Approx(-2));

  controller.EnableContinuousInput(0, 360);
  CHECK(controller.Calculate(1, 359) == Catch::Approx(-2));
}

TEST_CASE("PIDInputOutputTest ProportionalGainOutput", "[wpimath]") {
  wpi::math::PIDController controller{0.0, 0.0, 0.0};

  controller.SetP(4);

  CHECK(-0.1 == Catch::Approx(controller.Calculate(0.025, 0)));
}

TEST_CASE("PIDInputOutputTest IntegralGainOutput", "[wpimath]") {
  wpi::math::PIDController controller{0.0, 0.0, 0.0};

  controller.SetI(4);

  double out = 0;

  for (int i = 0; i < 5; i++) {
    out = controller.Calculate(0.025, 0);
  }

  CHECK(-0.5 * controller.GetPeriod().value() == Catch::Approx(out));
}

TEST_CASE("PIDInputOutputTest DerivativeGainOutput", "[wpimath]") {
  wpi::math::PIDController controller{0.0, 0.0, 0.0};

  controller.SetD(4);

  controller.Calculate(0, 0);

  CHECK(-10_ms / controller.GetPeriod() ==
        Catch::Approx(controller.Calculate(0.0025, 0)));
}

TEST_CASE("PIDInputOutputTest IZoneNoOutput", "[wpimath]") {
  wpi::math::PIDController controller{0.0, 0.0, 0.0};

  controller.SetI(1);
  controller.SetIZone(1);

  double out = controller.Calculate(2, 0);

  CHECK(0 == Catch::Approx(out));
}

TEST_CASE("PIDInputOutputTest IZoneOutput", "[wpimath]") {
  wpi::math::PIDController controller{0.0, 0.0, 0.0};

  controller.SetI(1);
  controller.SetIZone(1);

  double out = controller.Calculate(1, 0);

  CHECK(-1 * controller.GetPeriod().value() == Catch::Approx(out));
}
