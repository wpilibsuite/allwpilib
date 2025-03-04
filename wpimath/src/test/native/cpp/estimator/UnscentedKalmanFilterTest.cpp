// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>
#include <numbers>
#include <vector>

#include <Eigen/QR>
#include <gtest/gtest.h>

#include "frc/EigenCore.h"
#include "frc/StateSpaceUtil.h"
#include "frc/estimator/AngleStatistics.h"
#include "frc/estimator/UnscentedKalmanFilter.h"
#include "frc/system/NumericalIntegration.h"
#include "frc/system/NumericalJacobian.h"
#include "frc/system/plant/DCMotor.h"
#include "frc/trajectory/TrajectoryGenerator.h"
#include "units/moment_of_inertia.h"

namespace {

// First test system, differential drive
frc::Vectord<5> DriveDynamics(const frc::Vectord<5>& x, const frc::Vectord<2>& u) {
  auto motors = frc::DCMotor::CIM(2);

  // constexpr double Glow = 15.32;    // Low gear ratio
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

frc::Vectord<3> DriveLocalMeasurementModel(
    const frc::Vectord<5>& x, [[maybe_unused]] const frc::Vectord<2>& u) {
  return frc::Vectord<3>{x(2), x(3), x(4)};
}

frc::Vectord<5> DriveGlobalMeasurementModel(
    const frc::Vectord<5>& x, [[maybe_unused]] const frc::Vectord<2>& u) {
  return frc::Vectord<5>{x(0), x(1), x(2), x(3), x(4)};
}


TEST(UnscentedKalmanFilterTest, DriveInit) {
  constexpr auto dt = 5_ms;

  frc::UnscentedKalmanFilter<5, 2, 3> observer{DriveDynamics,
                                               DriveLocalMeasurementModel,
                                               {0.5, 0.5, 10.0, 1.0, 1.0},
                                               {0.0001, 0.01, 0.01},
                                               frc::AngleMean<5, 5>(2),
                                               frc::AngleMean<3, 5>(0),
                                               frc::AngleResidual<5>(2),
                                               frc::AngleResidual<3>(0),
                                               frc::AngleAdd<5>(2),
                                               dt};
  frc::Vectord<2> u{12.0, 12.0};
  observer.Predict(u, dt);

  auto localY = DriveLocalMeasurementModel(observer.Xhat(), u);
  observer.Correct(u, localY);

  auto globalY = DriveGlobalMeasurementModel(observer.Xhat(), u);
  auto R = frc::MakeCovMatrix(0.01, 0.01, 0.0001, 0.01, 0.01);
  observer.Correct<5>(u, globalY, DriveGlobalMeasurementModel, R,
                      frc::AngleMean<5, 5>(2), frc::AngleResidual<5>(2),
                      frc::AngleResidual<5>(2), frc::AngleAdd<5>(2));
}

TEST(UnscentedKalmanFilterTest, DriveConvergence) {
  constexpr auto dt = 5_ms;
  constexpr auto rb = 0.8382_m / 2.0;  // Robot radius

  frc::UnscentedKalmanFilter<5, 2, 3> observer{DriveDynamics,
                                               DriveLocalMeasurementModel,
                                               {0.5, 0.5, 10.0, 1.0, 1.0},
                                               {0.0001, 0.5, 0.5},
                                               frc::AngleMean<5, 5>(2),
                                               frc::AngleMean<3, 5>(0),
                                               frc::AngleResidual<5>(2),
                                               frc::AngleResidual<3>(0),
                                               frc::AngleAdd<5>(2),
                                               dt};

  auto waypoints =
      std::vector<frc::Pose2d>{frc::Pose2d{2.75_m, 22.521_m, 0_rad},
                               frc::Pose2d{24.73_m, 19.68_m, 5.846_rad}};
  auto trajectory = frc::TrajectoryGenerator::GenerateTrajectory(
      waypoints, {8.8_mps, 0.1_mps_sq});

  frc::Vectord<5> r = frc::Vectord<5>::Zero();
  frc::Vectord<2> u = frc::Vectord<2>::Zero();

  auto B = frc::NumericalJacobianU<5, 5, 2>(DriveDynamics, frc::Vectord<5>::Zero(),
                                            frc::Vectord<2>::Zero());

  observer.SetXhat(frc::Vectord<5>{
      trajectory.InitialPose().Translation().X().value(),
      trajectory.InitialPose().Translation().Y().value(),
      trajectory.InitialPose().Rotation().Radians().value(), 0.0, 0.0});

  auto trueXhat = observer.Xhat();

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

    auto localY = DriveLocalMeasurementModel(trueXhat, frc::Vectord<2>::Zero());
    observer.Correct(u, localY + frc::MakeWhiteNoiseVector(0.0001, 0.5, 0.5));

    frc::Vectord<5> rdot = (nextR - r) / dt.value();
    u = B.householderQr().solve(rdot - DriveDynamics(r, frc::Vectord<2>::Zero()));

    observer.Predict(u, dt);

    r = nextR;
    trueXhat = frc::RK4(DriveDynamics, trueXhat, u, dt);
  }

  auto localY = DriveLocalMeasurementModel(trueXhat, u);
  observer.Correct(u, localY);

  auto globalY = DriveGlobalMeasurementModel(trueXhat, u);
  auto R = frc::MakeCovMatrix(0.01, 0.01, 0.0001, 0.5, 0.5);
  observer.Correct<5>(u, globalY, DriveGlobalMeasurementModel, R,
                      frc::AngleMean<5, 5>(2), frc::AngleResidual<5>(2),
                      frc::AngleResidual<5>(2), frc::AngleAdd<5>(2)

  );

  auto finalPosition = trajectory.Sample(trajectory.TotalTime());
  EXPECT_NEAR(finalPosition.pose.Translation().X().value(), observer.Xhat(0),
              0.055);
  EXPECT_NEAR(finalPosition.pose.Translation().Y().value(), observer.Xhat(1),
              0.15);
  EXPECT_NEAR(finalPosition.pose.Rotation().Radians().value(), observer.Xhat(2),
              0.000005);
  EXPECT_NEAR(0.0, observer.Xhat(3), 0.1);
  EXPECT_NEAR(0.0, observer.Xhat(4), 0.1);
}

TEST(UnscentedKalmanFilterTest, RoundTripP) {
  constexpr auto dt = 5_ms;

  frc::UnscentedKalmanFilter<2, 2, 2> observer{
      [](const frc::Vectord<2>& x, const frc::Vectord<2>& u) { return x; },
      [](const frc::Vectord<2>& x, const frc::Vectord<2>& u) { return x; },
      {0.0, 0.0},
      {0.0, 0.0},
      dt};

  frc::Matrixd<2, 2> P({{2, 1}, {1, 2}});
  observer.SetP(P);

  ASSERT_TRUE(observer.P().isApprox(P));
}

// Second system, single motor feedforward estimator
frc::Vectord<4> MotorDynamics(const frc::Vectord<4>& x, const frc::Vectord<1>& u) {
  const double p = x(0);
  const double v = x(1);
  const double kV = x(2);
  const double kA = x(3);
  const double V = u(0);

  const double a = (V - kV * v) / kA;
  return frc::Vectord<4>{v, a, 0, 0};
}

frc::Vectord<3> MotorMeasurementModel(const frc::Vectord<4>& x, const frc::Vectord<1>& u) {
  const double p = x(0);
  const double v = x(1);
  const double kV = x(2);
  const double kA = x(3);
  const double V = u(0);

  const double I = (V - kV * v) / kA;
  return frc::Vectord<3>{p, v, I};
}

double MotorControlInput(const double& t) {
  double u = 8 * std::sin(std::numbers::pi * std::sqrt(2.0) * t)
           + 6 * std::sin(std::numbers::pi * std::sqrt(3.0) * t)
           + 4 * std::sin(std::numbers::pi * std::sqrt(5.0) * t);
  
  if(u > 12) u = 12;
  if(u < -12) u = -12;
  return u;
}

TEST(UnscentedKalmanFilterTest, MotorConvergence) {
  constexpr auto dt = 10_ms;
  constexpr int steps = 500;
  constexpr double true_kV = 3;
  constexpr double true_kA = 0.2;

  double pos_stddev = 0.02;
  double vel_stddev = 0.1;
  double cur_stddev = 0.1;

  frc::UnscentedKalmanFilter<4, 1, 3> observer{MotorDynamics,
                                               MotorMeasurementModel,
                                               wpi::array<double, 4>{0.1, 1.0, 1e-10, 1e-10},
                                               wpi::array<double, 3>{pos_stddev, vel_stddev, cur_stddev},
                                               dt};

  std::vector<frc::Vectord<1>> control_inputs(steps);
  std::vector<frc::Vectord<4>> true_states(steps);
  std::vector<frc::Vectord<3>> true_noisy_measurements(steps);
  true_states[0] = frc::Vectord<4>{0.0, 0.0, true_kV, true_kA};

  for (int i = 1; i < steps; i++) {
    const frc::Vectord<1> u{MotorControlInput(i * (dt.value() / 1000))};
    true_states[i] = frc::RK4(MotorDynamics, (true_states[i - 1]), u, dt);
  }
  for (int i = 0; i < steps; i++) {
    control_inputs[i] = frc::Vectord<1>{MotorControlInput(i * (dt.value() / 1000))};
    true_noisy_measurements[i] = MotorMeasurementModel(true_states[i], control_inputs[i]) + frc::MakeWhiteNoiseVector(pos_stddev, vel_stddev, cur_stddev);
  }

  frc::Vectord<4> P0{0.001, 0.001, 10, 10};

  observer.SetXhat(frc::Vectord<4>{0.0, 0.0, 2.0, 2.0});
  observer.SetP(P0.asDiagonal());

  for (int i = 0; i < steps; i++) {
    observer.Predict(control_inputs[i], dt);
    observer.Correct(control_inputs[i], true_noisy_measurements[i]);
  }

  EXPECT_NEAR(true_kV, observer.Xhat(2), 0.05);
  EXPECT_NEAR(true_kA, observer.Xhat(3), 0.05);
}

}  // namespace