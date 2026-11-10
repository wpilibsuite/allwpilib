// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/controller/ControlAffinePlantInversionFeedforward.hpp"

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/TestAssertions.hpp"
#include "wpi/math/linalg/EigenCore.hpp"
#include "wpi/units/time.hpp"

namespace wpi::math {

Vectord<2> Dynamics(const Vectord<2>& x, const Vectord<1>& u) {
  return Matrixd<2, 2>{{1.0, 0.0}, {0.0, 1.0}} * x +
         Matrixd<2, 1>{0.0, 1.0} * u;
}

Vectord<2> StateDynamics(const Vectord<2>& x) {
  return Matrixd<2, 2>{{1.0, 0.0}, {0.0, 1.0}} * x;
}

Vectord<2> StateDependentInputDynamics(const Vectord<2>& x,
                                       const Vectord<1>& u) {
  return Matrixd<2, 2>{{1.0, 0.0}, {0.0, 1.0}} * x +
         Matrixd<2, 1>{0.0, 1.0 + x(0) * x(0)} * u;
}

TEST_CASE("ControlAffinePlantInversionFeedforwardTest Calculate", "[wpimath]") {
  wpi::math::ControlAffinePlantInversionFeedforward<2, 1> feedforward{&Dynamics,
                                                                      20_ms};

  Vectord<2> r{2, 2};
  Vectord<2> nextR{3, 3};

  CHECK_NEAR(48, feedforward.Calculate(r, nextR)(0, 0), 1e-6);
}

TEST_CASE("ControlAffinePlantInversionFeedforwardTest CalculateState",
          "[wpimath]") {
  wpi::math::ControlAffinePlantInversionFeedforward<2, 1> feedforward{
      &StateDynamics, Matrixd<2, 1>{{0.0}, {1.0}}, 20_ms};

  Vectord<2> r{2, 2};
  Vectord<2> nextR{3, 3};

  CHECK_NEAR(48, feedforward.Calculate(r, nextR)(0, 0), 1e-6);
}

TEST_CASE("ControlAffinePlantInversionFeedforwardTest CalculateStateDependentB",
          "[wpimath]") {
  wpi::math::ControlAffinePlantInversionFeedforward<2, 1> feedforward{
      &StateDependentInputDynamics, 20_ms};

  // f(x, u) = x + B(x)u
  // B(x) = [0, 1 + x₀²]ᵀ
  // ṙₖ = (rₖ₊₁ − rₖ)/dt
  //
  // ṙₖ = f(rₖ, u)
  // ṙₖ = rₖ + B(rₖ)u
  // (rₖ₊₁ − rₖ)/dt = rₖ + B(rₖ)u
  //
  // rₖ = [2, 2]ᵀ, rₖ₊₁ = [3, 3]ᵀ, dt = 0.02:
  //
  // ([3, 3]ᵀ − [2, 2]ᵀ)/0.02 = [2, 2]ᵀ + B([2, 2]ᵀ)u
  // [1, 1]ᵀ/0.02 = [2, 2]ᵀ + B([2, 2]ᵀ)u
  // [50, 50]ᵀ = [2, 2]ᵀ + B([2, 2]ᵀ)u
  // [50, 50]ᵀ = [2, 2]ᵀ + [0, 1 + 2²]ᵀu
  // [50, 50]ᵀ = [2, 2]ᵀ + [0, 5]ᵀu
  // [0, 5]ᵀu = [50, 50]ᵀ − [2, 2]ᵀ
  // [0, 5]ᵀu = [48, 48]ᵀ
  // u = ([0, 5]ᵀ)⁺[48, 48]ᵀ
  // u = ([0, 5][0, 5]ᵀ)⁻¹[0, 5][48, 48]ᵀ
  // u = (0 ⋅ 48 + 5 ⋅ 48)/(0² + 5²)
  // u = 240/25
  // u = 9.6
  Vectord<2> r{2, 2};
  Vectord<2> nextR{3, 3};
  CHECK_NEAR(9.6, feedforward.Calculate(r, nextR)(0, 0), 1e-6);

  // rₖ = [3, 3]ᵀ, rₖ₊₁ = [4, 4]ᵀ, dt = 0.02:
  //
  // (rₖ₊₁ − rₖ)/dt = rₖ + B(rₖ)u
  // ([4, 4]ᵀ − [3, 3]ᵀ)/0.02 = [3, 3]ᵀ + B([3, 3]ᵀ)u
  // [1, 1]ᵀ/0.02 = [3, 3]ᵀ + B([3, 3]ᵀ)u
  // [50, 50]ᵀ = [3, 3]ᵀ + B([3, 3]ᵀ)u
  // [50, 50]ᵀ = [3, 3]ᵀ + [0, 1 + 3²]ᵀu
  // [50, 50]ᵀ = [3, 3]ᵀ + [0, 10]ᵀu
  // [0, 10]ᵀu = [50, 50]ᵀ − [3, 3]ᵀ
  // [0, 10]ᵀu = [47, 47]ᵀ
  // u = ([0, 10]ᵀ)⁺[47, 47]ᵀ
  // u = ([0, 10][0, 10]ᵀ)⁻¹[0, 10][47, 47]ᵀ
  // u = (0 ⋅ 47 + 10 ⋅ 47)/(0² + 10²)
  // u = 470/100
  // u = 4.7
  r = Vectord<2>{3, 3};
  nextR = Vectord<2>{4, 4};
  CHECK_NEAR(4.7, feedforward.Calculate(r, nextR)(0, 0), 1e-6);
}

}  // namespace wpi::math
