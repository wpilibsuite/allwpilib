// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/estimator/ParticleFilter.hpp"

#include <cmath>
#include <cstddef>
#include <numbers>
#include <random>
#include <stdexcept>
#include <vector>

#include <Eigen/QR>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/TestAssertions.hpp"
#include "wpi/math/estimator/AngleStatistics.hpp"
#include "wpi/math/estimator/ExtendedKalmanFilter.hpp"
#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/linalg/EigenCore.hpp"
#include "wpi/math/system/DCMotor.hpp"
#include "wpi/math/system/NumericalJacobian.hpp"
#include "wpi/math/trajectory/DrivetrainSplineTrajectoryGenerator.hpp"
#include "wpi/math/util/MathUtil.hpp"
#include "wpi/math/util/StateSpaceUtil.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/base.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/mass.hpp"
#include "wpi/units/moment_of_inertia.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/units/voltage.hpp"

namespace {

wpi::math::Vectord<5> DrivetrainDynamics(const wpi::math::Vectord<5>& x,
                                         const wpi::math::Vectord<2>& u) {
  static const auto motors = wpi::math::DCMotor::CIM(2);

  constexpr double Ghigh = 7.08;       // High gear ratio
  constexpr auto rb = 0.8382_m / 2.0;  // Robot radius
  constexpr auto r = 0.0746125_m;      // Wheel radius
  constexpr auto m = 63.503_kg;        // Robot mass
  constexpr auto J = 5.6_kg_sq_m;      // Robot moment of inertia

  auto C1 = -std::pow(Ghigh, 2) * motors.Kt /
            (motors.Kv * motors.R * wpi::units::math::pow<2>(r));
  auto C2 = Ghigh * motors.Kt / (motors.R * r);
  auto k1 = (1 / m + wpi::units::math::pow<2>(rb) / J);
  auto k2 = (1 / m - wpi::units::math::pow<2>(rb) / J);

  wpi::units::meters_per_second_t vl{x(3)};
  wpi::units::meters_per_second_t vr{x(4)};
  wpi::units::volt_t Vl{u(0)};
  wpi::units::volt_t Vr{u(1)};

  auto v = 0.5 * (vl + vr);
  return wpi::math::Vectord<5>{
      v.value() * std::cos(x(2)), v.value() * std::sin(x(2)),
      ((vr - vl) / (2.0 * rb)).value(),
      k1.value() * ((C1 * vl).value() + (C2 * Vl).value()) +
          k2.value() * ((C1 * vr).value() + (C2 * Vr).value()),
      k2.value() * ((C1 * vl).value() + (C2 * Vl).value()) +
          k1.value() * ((C1 * vr).value() + (C2 * Vr).value())};
}

wpi::math::Vectord<3> DrivetrainLocalMeasurementModel(
    const wpi::math::Vectord<5>& x,
    [[maybe_unused]] const wpi::math::Vectord<2>& u) {
  return wpi::math::Vectord<3>{x(2), x(3), x(4)};
}

wpi::math::Vectord<2> DrivetrainPositionMeasurementModel(
    const wpi::math::Vectord<5>& x,
    [[maybe_unused]] const wpi::math::Vectord<2>& u) {
  return wpi::math::Vectord<2>{x(0), x(1)};
}

wpi::math::Vectord<5> DrivetrainGlobalMeasurementModel(
    const wpi::math::Vectord<5>& x,
    [[maybe_unused]] const wpi::math::Vectord<2>& u) {
  return wpi::math::Vectord<5>{x(0), x(1), x(2), x(3), x(4)};
}

// First-order linear system ẋ = −x + u
wpi::math::Vectord<1> LinearDynamics(const wpi::math::Vectord<1>& x,
                                     const wpi::math::Vectord<1>& u) {
  return -x + u;
}

wpi::math::Vectord<1> LinearMeasurementModel(
    const wpi::math::Vectord<1>& x,
    [[maybe_unused]] const wpi::math::Vectord<1>& u) {
  return x;
}

// Heading with a commanded angular velocity θ̇ = u
wpi::math::Vectord<1> HeadingDynamics(
    [[maybe_unused]] const wpi::math::Vectord<1>& x,
    const wpi::math::Vectord<1>& u) {
  return u;
}

wpi::math::Vectord<1> HeadingMeasurementModel(
    const wpi::math::Vectord<1>& x,
    [[maybe_unused]] const wpi::math::Vectord<1>& u) {
  return x;
}

// Returns a vector of normally distributed noise from a seeded generator so
// tests are deterministic
template <int N>
wpi::math::Vectord<N> SeededNoise(std::mt19937& generator,
                                  const wpi::math::Vectord<N>& stdDevs) {
  std::normal_distribution<double> distr{0.0, 1.0};
  wpi::math::Vectord<N> result;
  for (int i = 0; i < N; ++i) {
    result(i) = stdDevs(i) * distr(generator);
  }
  return result;
}

}  // namespace

TEST_CASE("ParticleFilterTest Init", "[wpimath]") {
  constexpr wpi::units::second_t dt = 5_ms;

  wpi::math::ParticleFilter<5, 2, 3> observer{DrivetrainDynamics,
                                              DrivetrainLocalMeasurementModel,
                                              {0.5, 0.5, 10.0, 1.0, 1.0},
                                              {0.0001, 0.01, 0.01},
                                              100,
                                              dt};
  CHECK(observer.NumParticles() == 100);

  wpi::math::Vectord<2> u{12.0, 12.0};
  observer.Predict(u, dt);

  auto localY = DrivetrainLocalMeasurementModel(observer.Xhat(), u);
  observer.Correct(u, localY);

  auto globalY = DrivetrainGlobalMeasurementModel(observer.Xhat(), u);
  auto R = wpi::math::CovarianceMatrix(0.01, 0.01, 0.0001, 0.01, 0.01);
  observer.Correct<5>(u, globalY, DrivetrainGlobalMeasurementModel, R);

  CHECK(observer.Xhat().allFinite());
  CHECK(observer.P().allFinite());
  CHECK_NEAR(1.0, observer.Weights().sum(), 1e-12);
}

TEST_CASE("ParticleFilterTest Invalid particle count", "[wpimath]") {
  CHECK_THROWS_AS(
      (wpi::math::ParticleFilter<1, 1, 1>{
          LinearDynamics, LinearMeasurementModel, {0.1}, {0.1}, 0, 10_ms}),
      std::invalid_argument);
  CHECK_THROWS_AS(
      (wpi::math::ParticleFilter<1, 1, 1>{
          LinearDynamics, LinearMeasurementModel, {0.1}, {0.1}, -1, 10_ms}),
      std::invalid_argument);
}

TEST_CASE("ParticleFilterTest Convergence", "[wpimath]") {
  constexpr wpi::units::second_t dt = 5_ms;
  constexpr auto rb = 0.8382_m / 2.0;  // Robot radius

  // ExtendedKalmanFilterTest uses a heading measurement standard deviation of
  // 0.0001, but with a heading process noise this large, almost every particle
  // would be inconsistent with each heading measurement. The filter would then
  // resample down to a handful of ancestors every timestep and lose what it
  // learned about the other states.
  wpi::math::ParticleFilter<5, 2, 3> observer{DrivetrainDynamics,
                                              DrivetrainLocalMeasurementModel,
                                              {0.5, 0.5, 10.0, 1.0, 1.0},
                                              {0.01, 0.5, 0.5},
                                              200,
                                              dt};
  observer.SetSeed(42);
  std::mt19937 generator{1};

  auto waypoints = std::vector<wpi::math::Pose2d>{
      wpi::math::Pose2d{2.75_m, 22.521_m, 0_rad},
      wpi::math::Pose2d{24.73_m, 19.68_m, 5.846_rad}};
  auto trajectory = wpi::math::DrivetrainSplineTrajectoryGenerator::Generate(
      waypoints, {8.8_mps, 0.1_mps_sq});

  wpi::math::Vectord<5> r = wpi::math::Vectord<5>::Zero();
  wpi::math::Vectord<2> u = wpi::math::Vectord<2>::Zero();

  auto B = wpi::math::NumericalJacobianU<5, 5, 2>(
      DrivetrainDynamics, wpi::math::Vectord<5>::Zero(),
      wpi::math::Vectord<2>::Zero());

  observer.SetXhat(wpi::math::Vectord<5>{
      trajectory.Start().pose.Translation().X().value(),
      trajectory.Start().pose.Translation().Y().value(),
      trajectory.Start().pose.Rotation().Radians().value(), 0.0, 0.0});

  // The local measurements don't observe x and y, so without a global
  // measurement their estimates would only dead reckon. A particle filter
  // resamples often, which makes its estimate of an unobserved state wander
  // like a single sample of the process noise random walk instead of tracking
  // the posterior mean like a Kalman filter. Add a noisy position measurement
  // every 100 ms, like a vision system would provide.
  constexpr int kGlobalMeasurementPeriod = 20;
  const auto R = wpi::math::CovarianceMatrix(0.01, 0.01);

  auto duration = trajectory.Duration();
  for (size_t i = 0; i < (duration / dt).value(); ++i) {
    auto ref = trajectory.SampleAt(dt * i);
    wpi::units::meters_per_second_t vl =
        ref.ForwardVelocity() * (1 - (ref.curvature * rb).value());
    wpi::units::meters_per_second_t vr =
        ref.ForwardVelocity() * (1 + (ref.curvature * rb).value());

    wpi::math::Vectord<5> nextR{
        ref.pose.Translation().X().value(), ref.pose.Translation().Y().value(),
        ref.pose.Rotation().Radians().value(), vl.value(), vr.value()};

    auto localY =
        DrivetrainLocalMeasurementModel(nextR, wpi::math::Vectord<2>::Zero());
    observer.Correct(u, localY + SeededNoise<3>(generator, {0.0001, 0.5, 0.5}));

    if (i % kGlobalMeasurementPeriod == 0) {
      wpi::math::Vectord<2> globalY =
          DrivetrainPositionMeasurementModel(nextR,
                                             wpi::math::Vectord<2>::Zero()) +
          SeededNoise<2>(generator, {0.05, 0.05});
      observer.Correct<2>(u, globalY, DrivetrainPositionMeasurementModel, R);
    }

    wpi::math::Vectord<5> rdot = (nextR - r) / dt.value();
    u = B.householderQr().solve(
        rdot - DrivetrainDynamics(r, wpi::math::Vectord<2>::Zero()));

    observer.Predict(u, dt);

    r = nextR;
  }

  auto finalPosition = trajectory.SampleAt(trajectory.Duration());
  // The random number sequences differ between standard libraries, so these
  // tolerances are loose enough to pass for any seed
  REQUIRE_NEAR(finalPosition.pose.Translation().X().value(), observer.Xhat(0),
               0.3);
  REQUIRE_NEAR(finalPosition.pose.Translation().Y().value(), observer.Xhat(1),
               0.3);
  REQUIRE_NEAR(finalPosition.pose.Rotation().Radians().value(),
               observer.Xhat(2), 0.15);
  REQUIRE_NEAR(0.0, observer.Xhat(3), 1.0);
  REQUIRE_NEAR(0.0, observer.Xhat(4), 1.0);
}

TEST_CASE("ParticleFilterTest Matches Kalman filter on linear system",
          "[wpimath]") {
  constexpr wpi::units::second_t dt = 10_ms;
  constexpr double stateStdDev = 0.5;
  constexpr double measurementStdDev = 0.01;

  // An extended Kalman filter is optimal for a linear system with Gaussian
  // noise, so the particle filter should approach it
  wpi::math::ExtendedKalmanFilter<1, 1, 1> ekf{LinearDynamics,
                                               LinearMeasurementModel,
                                               {stateStdDev},
                                               {measurementStdDev},
                                               dt};
  wpi::math::ParticleFilter<1, 1, 1> pf{LinearDynamics, LinearMeasurementModel,
                                        {stateStdDev},  {measurementStdDev},
                                        2000,           dt};
  pf.SetSeed(42);
  pf.SetP(ekf.P());

  std::mt19937 generator{1};

  // Discrete process and measurement noise standard deviations for simulating
  // the true system
  const double discStateStdDev = stateStdDev * std::sqrt(dt.value());
  const double discMeasurementStdDev =
      measurementStdDev / std::sqrt(dt.value());

  constexpr int kSteps = 500;
  double sumSquaredNormalizedError = 0.0;

  wpi::math::Vectord<1> x{0.0};
  for (int i = 0; i < kSteps; ++i) {
    wpi::math::Vectord<1> u{std::sin(i * dt.value())};

    x = wpi::math::RK4(LinearDynamics, x, u, dt) +
        SeededNoise<1>(generator, wpi::math::Vectord<1>{discStateStdDev});
    ekf.Predict(u, dt);
    pf.Predict(u, dt);

    wpi::math::Vectord<1> y =
        x +
        SeededNoise<1>(generator, wpi::math::Vectord<1>{discMeasurementStdDev});
    ekf.Correct(u, y);
    pf.Correct(u, y);

    // The estimates should agree to within a fraction of the EKF's standard
    // deviation. The particle filter's Monte Carlo error makes individual
    // timesteps deviate more than the average, so the per-timestep bound is
    // looser than the RMS bound below.
    const double normalizedError =
        (pf.Xhat(0) - ekf.Xhat(0)) / std::sqrt(ekf.P(0, 0));
    REQUIRE(std::abs(normalizedError) < 1.0);
    sumSquaredNormalizedError += normalizedError * normalizedError;
  }

  CHECK(std::sqrt(sumSquaredNormalizedError / kSteps) < 0.1);
  CHECK_NEAR(ekf.P(0, 0), pf.P(0, 0), 0.2 * ekf.P(0, 0));
}

TEST_CASE("ParticleFilterTest Angle wrapping", "[wpimath]") {
  constexpr wpi::units::second_t dt = 10_ms;

  wpi::math::ParticleFilter<1, 1, 1> pf{
      HeadingDynamics,
      HeadingMeasurementModel,
      {0.1},
      {0.01},
      wpi::math::AngleMean<1, Eigen::Dynamic>(0),
      wpi::math::AngleResidual<1>(0),
      wpi::math::AngleResidual<1>(0),
      wpi::math::AngleAdd<1>(0),
      500,
      dt};
  pf.SetSeed(42);

  // Start just below π and rotate counterclockwise across the ±π boundary
  double theta = std::numbers::pi - 0.5;
  pf.SetXhat(wpi::math::Vectord<1>{theta});
  pf.SetP(wpi::math::Vectord<1>{0.01}.asDiagonal());

  const wpi::math::Vectord<1> u{1.0};
  for (int i = 0; i < 100; ++i) {
    theta = wpi::math::AngleModulus(wpi::units::radian_t{theta + dt.value()})
                .value();

    pf.Predict(u, dt);
    pf.Correct(u, wpi::math::Vectord<1>{theta});

    // The estimate must stay close to the true heading on the circle, and the
    // particle spread must not blow up from naive arithmetic across the
    // boundary
    const double error = wpi::math::AngleResidual<1>(
        wpi::math::Vectord<1>{theta}, pf.Xhat(), 0)(0);
    REQUIRE_NEAR(0.0, error, 0.05);
    REQUIRE(pf.P(0, 0) < 0.01);
  }

  // The heading wrapped past π
  CHECK(theta < 0.0);
  CHECK(pf.Xhat(0) < 0.0);
}

TEST_CASE("ParticleFilterTest SetP and SetXhat", "[wpimath]") {
  wpi::math::ParticleFilter<2, 1, 1> pf{
      [](const wpi::math::Vectord<2>& x,
         [[maybe_unused]] const wpi::math::Vectord<1>& u) {
        return wpi::math::Vectord<2>{x(1), 0.0};
      },
      [](const wpi::math::Vectord<2>& x,
         [[maybe_unused]] const wpi::math::Vectord<1>& u) {
        return wpi::math::Vectord<1>{x(0)};
      },
      {0.1, 0.1},
      {0.1},
      10000,
      10_ms};
  pf.SetSeed(42);

  // Particles start at the zero state
  CHECK(pf.Xhat().isZero());
  CHECK(pf.P().isZero());

  pf.SetXhat(wpi::math::Vectord<2>{1.0, 2.0});
  CHECK_NEAR(1.0, pf.Xhat(0), 1e-12);
  CHECK_NEAR(2.0, pf.Xhat(1), 1e-12);
  CHECK(pf.P().isZero());

  // SetP() redraws the particles from a Gaussian with the given covariance,
  // including correlation between states. The tolerances are about five
  // standard errors of the sample moments for 10,000 particles.
  wpi::math::Matrixd<2, 2> P{{4.0, 1.0}, {1.0, 2.0}};
  pf.SetP(P);
  CHECK_NEAR(1.0, pf.Xhat(0), 0.1);
  CHECK_NEAR(2.0, pf.Xhat(1), 0.1);
  CHECK_NEAR(4.0, pf.P(0, 0), 0.3);
  CHECK_NEAR(1.0, pf.P(0, 1), 0.15);
  CHECK_NEAR(1.0, pf.P(1, 0), 0.15);
  CHECK_NEAR(2.0, pf.P(1, 1), 0.15);

  // SetXhat() shifts the particles without changing their spread
  const wpi::math::Matrixd<2, 2> oldP = pf.P();
  pf.SetXhat(1, -3.0);
  CHECK_NEAR(-3.0, pf.Xhat(1), 1e-12);
  CHECK(pf.P().isApprox(oldP, 1e-9));

  // A singular covariance is allowed
  pf.SetP(wpi::math::Vectord<2>{1.0, 0.0}.asDiagonal());
  CHECK_NEAR(1.0, pf.P(0, 0), 0.1);
  CHECK_NEAR(0.0, pf.P(1, 1), 1e-12);

  // A singular covariance with perfectly correlated states is allowed. A
  // Cholesky decomposition (LLT) would silently double the second state's
  // variance here.
  pf.SetP(wpi::math::Matrixd<2, 2>{{1.0, 1.0}, {1.0, 1.0}});
  CHECK_NEAR(1.0, pf.P(0, 0), 0.1);
  CHECK_NEAR(1.0, pf.P(0, 1), 0.1);
  CHECK_NEAR(1.0, pf.P(1, 1), 0.1);
  const double offset = pf.Particles()(0, 0) - pf.Particles()(1, 0);
  for (int i = 0; i < pf.NumParticles(); ++i) {
    REQUIRE_NEAR(offset, pf.Particles()(0, i) - pf.Particles()(1, i), 1e-12);
  }

  pf.Reset();
  CHECK(pf.Xhat().isZero());
  CHECK(pf.P().isZero());
}

TEST_CASE("ParticleFilterTest Resampling", "[wpimath]") {
  constexpr int N = 1000;
  wpi::math::ParticleFilter<1, 1, 1> pf{
      LinearDynamics, LinearMeasurementModel, {0.1}, {0.01}, N, 10_ms};
  pf.SetSeed(42);
  pf.SetP(wpi::math::Vectord<1>{1.0}.asDiagonal());

  // A precise measurement makes most particles unlikely, which collapses the
  // effective sample size and triggers resampling back to equal weights
  pf.Correct(wpi::math::Vectord<1>{0.0}, wpi::math::Vectord<1>{0.5});
  for (int i = 0; i < N; ++i) {
    REQUIRE(pf.Weights()(i) == 1.0 / N);
  }

  // The exact posterior for a Gaussian prior N(0, P) and measurement noise
  // variance R (here R = 0.01² / 0.01 s after discretization) is
  //
  //   mean = P / (P + R) y
  //   variance = PR / (P + R)
  constexpr double P = 1.0;
  constexpr double R = 0.01;
  CHECK_NEAR(P / (P + R) * 0.5, pf.Xhat(0), 0.025);
  CHECK_NEAR(P * R / (P + R), pf.P(0, 0), 0.003);
}

TEST_CASE("ParticleFilterTest Measurement inconsistent with every particle",
          "[wpimath]") {
  constexpr int N = 100;
  wpi::math::ParticleFilter<1, 1, 1> pf{
      LinearDynamics,
      [](const wpi::math::Vectord<1>& x,
         [[maybe_unused]] const wpi::math::Vectord<1>& u) {
        // Every particle produces an invalid measurement
        return wpi::math::Vectord<1>{std::nan("") * x(0)};
      },
      {0.1},
      {0.01},
      N,
      10_ms};
  pf.SetSeed(42);
  pf.SetP(wpi::math::Vectord<1>{1.0}.asDiagonal());

  const auto oldXhat = pf.Xhat();
  pf.Correct(wpi::math::Vectord<1>{0.0}, wpi::math::Vectord<1>{0.5});

  // The filter falls back to equal weights instead of producing NaNs
  CHECK(pf.Xhat().allFinite());
  CHECK(pf.P().allFinite());
  CHECK(pf.Xhat() == oldXhat);
  for (int i = 0; i < N; ++i) {
    REQUIRE(pf.Weights()(i) == 1.0 / N);
  }
}

TEST_CASE("ParticleFilterTest Seed makes output reproducible", "[wpimath]") {
  auto run = [](unsigned int seed) {
    wpi::math::ParticleFilter<1, 1, 1> pf{
        LinearDynamics, LinearMeasurementModel, {0.1}, {0.01}, 100, 10_ms};
    pf.SetSeed(seed);
    pf.SetP(wpi::math::Vectord<1>{1.0}.asDiagonal());
    for (int i = 0; i < 10; ++i) {
      pf.Predict(wpi::math::Vectord<1>{1.0}, 10_ms);
      pf.Correct(wpi::math::Vectord<1>{1.0}, wpi::math::Vectord<1>{0.1 * i});
    }
    return pf.Xhat();
  };

  CHECK(run(1) == run(1));
  CHECK(run(1) != run(2));
}
