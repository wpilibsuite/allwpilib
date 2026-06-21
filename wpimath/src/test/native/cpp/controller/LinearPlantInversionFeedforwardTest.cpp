// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/controller/LinearPlantInversionFeedforward.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/TestAssertions.hpp"
#include "wpi/math/linalg/EigenCore.hpp"
#include "wpi/units/time.hpp"

namespace wpi::math {

TEST_CASE("LinearPlantInversionFeedforwardTest Calculate", "[wpimath]") {
  Matrixd<2, 2> A{{1, 0}, {0, 1}};
  Matrixd<2, 1> B{0, 1};

  wpi::math::LinearPlantInversionFeedforward<2, 1> feedforward{A, B, 20_ms};

  Vectord<2> r{2, 2};
  Vectord<2> nextR{3, 3};

  CHECK_NEAR(47.502599, feedforward.Calculate(r, nextR)(0, 0), 0.002);
}

}  // namespace wpi::math
