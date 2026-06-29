// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/Quaternion.hpp"

#include <numbers>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/units/angle.hpp"
#include "wpi/units/math.hpp"

using namespace wpi::math;

TEST_CASE("QuaternionTest Init", "[wpimath]") {
  // Identity
  Quaternion q1;
  CHECK(1.0 == Catch::Approx(q1.W()));
  CHECK(0.0 == Catch::Approx(q1.X()));
  CHECK(0.0 == Catch::Approx(q1.Y()));
  CHECK(0.0 == Catch::Approx(q1.Z()));

  // Normalized
  Quaternion q2{0.5, 0.5, 0.5, 0.5};
  CHECK(0.5 == Catch::Approx(q2.W()));
  CHECK(0.5 == Catch::Approx(q2.X()));
  CHECK(0.5 == Catch::Approx(q2.Y()));
  CHECK(0.5 == Catch::Approx(q2.Z()));

  // Unnormalized
  Quaternion q3{0.75, 0.3, 0.4, 0.5};
  CHECK(0.75 == Catch::Approx(q3.W()));
  CHECK(0.3 == Catch::Approx(q3.X()));
  CHECK(0.4 == Catch::Approx(q3.Y()));
  CHECK(0.5 == Catch::Approx(q3.Z()));

  q3 = q3.Normalize();
  double norm = std::sqrt(0.75 * 0.75 + 0.3 * 0.3 + 0.4 * 0.4 + 0.5 * 0.5);
  CHECK(0.75 / norm == Catch::Approx(q3.W()));
  CHECK(0.3 / norm == Catch::Approx(q3.X()));
  CHECK(0.4 / norm == Catch::Approx(q3.Y()));
  CHECK(0.5 / norm == Catch::Approx(q3.Z()));
  CHECK(1.0 == Catch::Approx(q3.W() * q3.W() + q3.X() * q3.X() +
                             q3.Y() * q3.Y() + q3.Z() * q3.Z()));
}

TEST_CASE("QuaternionTest Addition", "[wpimath]") {
  Quaternion q{0.1, 0.2, 0.3, 0.4};
  Quaternion p{0.5, 0.6, 0.7, 0.8};

  auto sum = q + p;

  CHECK(q.W() + p.W() == Catch::Approx(sum.W()));
  CHECK(q.X() + p.X() == Catch::Approx(sum.X()));
  CHECK(q.Y() + p.Y() == Catch::Approx(sum.Y()));
  CHECK(q.Z() + p.Z() == Catch::Approx(sum.Z()));
}

TEST_CASE("QuaternionTest Subtraction", "[wpimath]") {
  Quaternion q{0.1, 0.2, 0.3, 0.4};
  Quaternion p{0.5, 0.6, 0.7, 0.8};

  auto difference = q - p;

  CHECK(q.W() - p.W() == Catch::Approx(difference.W()));
  CHECK(q.X() - p.X() == Catch::Approx(difference.X()));
  CHECK(q.Y() - p.Y() == Catch::Approx(difference.Y()));
  CHECK(q.Z() - p.Z() == Catch::Approx(difference.Z()));
}

TEST_CASE("QuaternionTest ScalarMultiplication", "[wpimath]") {
  Quaternion q{0.1, 0.2, 0.3, 0.4};
  auto scalar = 2;

  auto product = q * scalar;

  CHECK(q.W() * scalar == Catch::Approx(product.W()));
  CHECK(q.X() * scalar == Catch::Approx(product.X()));
  CHECK(q.Y() * scalar == Catch::Approx(product.Y()));
  CHECK(q.Z() * scalar == Catch::Approx(product.Z()));
}

TEST_CASE("QuaternionTest ScalarDivision", "[wpimath]") {
  Quaternion q{0.1, 0.2, 0.3, 0.4};
  auto scalar = 2;

  auto product = q / scalar;

  CHECK(q.W() / scalar == Catch::Approx(product.W()));
  CHECK(q.X() / scalar == Catch::Approx(product.X()));
  CHECK(q.Y() / scalar == Catch::Approx(product.Y()));
  CHECK(q.Z() / scalar == Catch::Approx(product.Z()));
}

TEST_CASE("QuaternionTest Multiply", "[wpimath]") {
  // 90° CCW rotations around each axis
  double c = wpi::units::math::cos(90_deg / 2.0);
  double s = wpi::units::math::sin(90_deg / 2.0);
  Quaternion xRot{c, s, 0.0, 0.0};
  Quaternion yRot{c, 0.0, s, 0.0};
  Quaternion zRot{c, 0.0, 0.0, s};

  // 90° CCW X rotation, 90° CCW Y rotation, and 90° CCW Z rotation should
  // produce a 90° CCW Y rotation
  auto expected = yRot;
  auto actual = zRot * yRot * xRot;
  CHECK(expected.W() == Catch::Approx(actual.W()).margin(1e-9));
  CHECK(expected.X() == Catch::Approx(actual.X()).margin(1e-9));
  CHECK(expected.Y() == Catch::Approx(actual.Y()).margin(1e-9));
  CHECK(expected.Z() == Catch::Approx(actual.Z()).margin(1e-9));

  // Identity
  Quaternion q{0.72760687510899891, 0.29104275004359953, 0.38805700005813276,
               0.48507125007266594};
  actual = q * q.Inverse();
  CHECK(1.0 == Catch::Approx(actual.W()).margin(1e-9));
  CHECK(0.0 == Catch::Approx(actual.X()).margin(1e-9));
  CHECK(0.0 == Catch::Approx(actual.Y()).margin(1e-9));
  CHECK(0.0 == Catch::Approx(actual.Z()).margin(1e-9));
}

TEST_CASE("QuaternionTest Conjugate", "[wpimath]") {
  Quaternion q{0.72760687510899891, 0.29104275004359953, 0.38805700005813276,
               0.48507125007266594};
  auto conj = q.Conjugate();

  CHECK(q.W() == Catch::Approx(conj.W()));
  CHECK(-q.X() == Catch::Approx(conj.X()));
  CHECK(-q.Y() == Catch::Approx(conj.Y()));
  CHECK(-q.Z() == Catch::Approx(conj.Z()));
}

TEST_CASE("QuaternionTest Inverse", "[wpimath]") {
  Quaternion q{0.72760687510899891, 0.29104275004359953, 0.38805700005813276,
               0.48507125007266594};
  auto norm = q.Norm();

  auto inv = q.Inverse();

  CHECK(q.W() / (norm * norm) == Catch::Approx(inv.W()));
  CHECK(-q.X() / (norm * norm) == Catch::Approx(inv.X()));
  CHECK(-q.Y() / (norm * norm) == Catch::Approx(inv.Y()));
  CHECK(-q.Z() / (norm * norm) == Catch::Approx(inv.Z()));
}

TEST_CASE("QuaternionTest Norm", "[wpimath]") {
  Quaternion q{3, 4, 12, 84};
  auto norm = q.Norm();

  CHECK(85 == Catch::Approx(norm).margin(1e-9));
}

TEST_CASE("QuaternionTest Exponential", "[wpimath]") {
  Quaternion q{1.1, 2.2, 3.3, 4.4};
  Quaternion expect{2.81211398529184, -0.392521193481878, -0.588781790222817,
                    -0.785042386963756};

  auto q_exp = q.Exp();

  CHECK(expect == q_exp);
}

TEST_CASE("QuaternionTest Logarithm", "[wpimath]") {
  Quaternion q{1.1, 2.2, 3.3, 4.4};
  Quaternion expect{1.7959088706354, 0.515190292664085, 0.772785438996128,
                    1.03038058532817};

  auto q_log = q.Log();

  CHECK(expect == q_log);

  Quaternion zero{0, 0, 0, 0};
  Quaternion one{1, 0, 0, 0};
  Quaternion i{0, 1, 0, 0};
  Quaternion j{0, 0, 1, 0};
  Quaternion k{0, 0, 0, 1};
  Quaternion ln_half{std::log(0.5), -std::numbers::pi, 0, 0};

  CHECK(zero == one.Log());
  CHECK(i * std::numbers::pi / 2 == i.Log());
  CHECK(j * std::numbers::pi / 2 == j.Log());
  CHECK(k * std::numbers::pi / 2 == k.Log());

  CHECK(i * -std::numbers::pi == (one * -1).Log());
  CHECK(ln_half == (one * -0.5).Log());
}

TEST_CASE("QuaternionTest LogarithmAndExponentialInverse", "[wpimath]") {
  Quaternion q{1.1, 2.2, 3.3, 4.4};

  // These operations are order-dependent: ln(exp(q)) is congruent but not
  // necessarily equal to exp(ln(q)) due to the multi-valued nature of the
  // complex logarithm.

  auto q_log_exp = q.Log().Exp();

  CHECK(q == q_log_exp);

  Quaternion start{1, 2, 3, 4};
  Quaternion expect{5, 6, 7, 8};

  auto twist = (expect * start.Inverse()).Log();
  auto actual = twist.Exp() * start;

  CHECK(expect == actual);
}

TEST_CASE("QuaternionTest DotProduct", "[wpimath]") {
  Quaternion q{1.1, 2.2, 3.3, 4.4};
  Quaternion p{5.5, 6.6, 7.7, 8.8};

  CHECK(q.W() * p.W() + q.X() * p.X() + q.Y() * p.Y() + q.Z() * p.Z() ==
        Catch::Approx(q.Dot(p)).margin(1e-9));
}

TEST_CASE("QuaternionTest DotProductAsEquality", "[wpimath]") {
  Quaternion q{1.1, 2.2, 3.3, 4.4};
  auto q_conj = q.Conjugate();

  CHECK(q.Dot(q) == Catch::Approx(q.Norm() * q.Norm()).margin(1e-9));
  CHECK(std::abs(q.Dot(q_conj) - q.Norm() * q_conj.Norm()) > 1e-9);
}
