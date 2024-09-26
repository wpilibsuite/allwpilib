// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>
#include <vector>

#include <Eigen/QR>
#include <gtest/gtest.h>

#include "frc/EigenCore.h"
#include "frc/StateSpaceUtil.h"
#include "frc/estimator/ExtendedKalmanFilter.h"
#include "frc/system/NumericalJacobian.h"
#include "frc/system/plant/DCMotor.h"
#include "frc/trajectory/TrajectoryGenerator.h"
#include "units/moment_of_inertia.h"

namespace {

frc::Vectord<5> Dynamics(const frc::Vectord<5>& x, const frc::Vectord<2>& u) {
  auto motors = frc::DCMotor::CIM(2);

  // constexpr double Glow = 15.32;       // Low gear ratio
  constexpr double Ghigh = 7.08;       // High gear ratio
  constexpr auto rb = 0.8382_m / 2.0;  // Robot radius
  constexpr auto r = 0.0746125_m;      // Wheel radius
  constexpr auto m = 63.503_kg;        // Robot mass
  constexpr auto J = 5.6_kg_sq_m;      // Robot moment of inertia

  auto C1 = -std::pow(Ghigh, 2) * motors.Kt /
            (motors.Kv * motors.R * units::math::pow<2>(r));
  auto C2 = Ghigh * motors.Kt / (motors.R * r);
  auto k1 = (1 / m + units::math::pow<2>(rb) / J);
  auto k2 = (1 / m - units::math::pow<2>(rb) / J);

  units::meters_per_second_t vl{x(3)};
  units::meters_per_second_t vr{x(4)};
  units::volt_t Vl{u(0)};
  units::volt_t Vr{u(1)};

  auto v = 0.5 * (vl + vr);
  return frc::Vectord<5>{
      v.value() * std::cos(x(2)), v.value() * std::sin(x(2)),
      ((vr - vl) / (2.0 * rb)).value(),
      k1.value() * ((C1 * vl).value() + (C2 * Vl).value()) +
          k2.value() * ((C1 * vr).value() + (C2 * Vr).value()),
      k2.value() * ((C1 * vl).value() + (C2 * Vl).value()) +
          k1.value() * ((C1 * vr).value() + (C2 * Vr).value())};
}

frc::Vectord<3> LocalMeasurementModel(
    const frc::Vectord<5>& x, [[maybe_unused]] const frc::Vectord<2>& u) {
  return frc::Vectord<3>{x(2), x(3), x(4)};
}

frc::Vectord<5> GlobalMeasurementModel(
    const frc::Vectord<5>& x, [[maybe_unused]] const frc::Vectord<2>& u) {
  return frc::Vectord<5>{x(0), x(1), x(2), x(3), x(4)};
}
}  // namespace

TEST(ExtendedKalmanFilterTest, Init) {
  constexpr auto dt = 0.00505_s;

  frc::ExtendedKalmanFilter<5, 2, 3> observer{Dynamics,
                                              LocalMeasurementModel,
                                              {0.5, 0.5, 10.0, 1.0, 1.0},
                                              {0.0001, 0.01, 0.01},
                                              dt};
  frc::Vectord<2> u{12.0, 12.0};
  observer.Predict(u, dt);

  auto localY = LocalMeasurementModel(observer.Xhat(), u);
  observer.Correct(u, localY);

  auto globalY = GlobalMeasurementModel(observer.Xhat(), u);
  auto R = frc::MakeCovMatrix(0.01, 0.01, 0.0001, 0.01, 0.01);
  observer.Correct<5>(u, globalY, GlobalMeasurementModel, R);
}

TEST(ExtendedKalmanFilterTest, Convergence) {
  constexpr auto dt = 0.00505_s;
  constexpr auto rb = 0.8382_m / 2.0;  // Robot radius

  frc::ExtendedKalmanFilter<5, 2, 3> observer{Dynamics,
                                              LocalMeasurementModel,
                                              {0.5, 0.5, 10.0, 1.0, 1.0},
                                              {0.0001, 0.5, 0.5},
                                              dt};

  auto waypoints =
      std::vector<frc::Pose2d>{frc::Pose2d{2.75_m, 22.521_m, 0_rad},
                               frc::Pose2d{24.73_m, 19.68_m, 5.846_rad}};
  auto trajectory = frc::TrajectoryGenerator::GenerateTrajectory(
      waypoints, {8.8_mps, 0.1_mps_sq});

  frc::Vectord<5> r = frc::Vectord<5>::Zero();
  frc::Vectord<2> u = frc::Vectord<2>::Zero();

  auto B = frc::NumericalJacobianU<5, 5, 2>(Dynamics, frc::Vectord<5>::Zero(),
                                            frc::Vectord<2>::Zero());

  observer.SetXhat(frc::Vectord<5>{
      trajectory.InitialPose().Translation().X().value(),
      trajectory.InitialPose().Translation().Y().value(),
      trajectory.InitialPose().Rotation().Radians().value(), 0.0, 0.0});

  auto totalTime = trajectory.TotalTime();
  for (size_t i = 0; i < (totalTime / dt).value(); ++i) {
    auto ref = trajectory.Sample(dt * i);
    units::meters_per_second_t vl =
        ref.velocity * (1 - (ref.curvature * rb).value());
    units::meters_per_second_t vr =
        ref.velocity * (1 + (ref.curvature * rb).value());

    frc::Vectord<5> nextR{
        ref.pose.Translation().X().value(), ref.pose.Translation().Y().value(),
        ref.pose.Rotation().Radians().value(), vl.value(), vr.value()};

    auto localY = LocalMeasurementModel(nextR, frc::Vectord<2>::Zero());
    observer.Correct(u, localY + frc::MakeWhiteNoiseVector(0.0001, 0.5, 0.5));

    frc::Vectord<5> rdot = (nextR - r) / dt.value();
    u = B.householderQr().solve(rdot - Dynamics(r, frc::Vectord<2>::Zero()));

    observer.Predict(u, dt);

    r = nextR;
  }

  auto localY = LocalMeasurementModel(observer.Xhat(), u);
  observer.Correct(u, localY);

  auto globalY = GlobalMeasurementModel(observer.Xhat(), u);
  auto R = frc::MakeCovMatrix(0.01, 0.01, 0.0001, 0.5, 0.5);
  observer.Correct<5>(u, globalY, GlobalMeasurementModel, R);

  auto finalPosition = trajectory.Sample(trajectory.TotalTime());
  ASSERT_NEAR(finalPosition.pose.Translation().X().value(), observer.Xhat(0),
              1.0);
  ASSERT_NEAR(finalPosition.pose.Translation().Y().value(), observer.Xhat(1),
              1.0);
  ASSERT_NEAR(finalPosition.pose.Rotation().Radians().value(), observer.Xhat(2),
              1.0);
  ASSERT_NEAR(0.0, observer.Xhat(3), 1.0);
  ASSERT_NEAR(0.0, observer.Xhat(4), 1.0);
}
