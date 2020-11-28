/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <gtest/gtest.h>

#include <array>
#include <cmath>

#include "Eigen/Core"
#include "Eigen/QR"
#include "frc/StateSpaceUtil.h"
#include "frc/estimator/AngleStatistics.h"
#include "frc/estimator/UnscentedKalmanFilter.h"
#include "frc/system/NumericalJacobian.h"
#include "frc/system/RungeKutta.h"
#include "frc/system/plant/DCMotor.h"
#include "frc/trajectory/TrajectoryGenerator.h"
#include "units/moment_of_inertia.h"

namespace {

Eigen::Matrix<double, 5, 1> Dynamics(const Eigen::Matrix<double, 5, 1>& x,
                                     const Eigen::Matrix<double, 2, 1>& u) {
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

  Eigen::Matrix<double, 5, 1> result;
  auto v = 0.5 * (vl + vr);
  result(0) = v.to<double>() * std::cos(x(2));
  result(1) = v.to<double>() * std::sin(x(2));
  result(2) = ((vr - vl) / (2.0 * rb)).to<double>();
  result(3) =
      k1.to<double>() * ((C1 * vl).to<double>() + (C2 * Vl).to<double>()) +
      k2.to<double>() * ((C1 * vr).to<double>() + (C2 * Vr).to<double>());
  result(4) =
      k2.to<double>() * ((C1 * vl).to<double>() + (C2 * Vl).to<double>()) +
      k1.to<double>() * ((C1 * vr).to<double>() + (C2 * Vr).to<double>());
  return result;
}

Eigen::Matrix<double, 3, 1> LocalMeasurementModel(
    const Eigen::Matrix<double, 5, 1>& x,
    const Eigen::Matrix<double, 2, 1>& u) {
  static_cast<void>(u);
  Eigen::Matrix<double, 3, 1> y;
  y << x(2), x(3), x(4);
  return y;
}

Eigen::Matrix<double, 5, 1> GlobalMeasurementModel(
    const Eigen::Matrix<double, 5, 1>& x,
    const Eigen::Matrix<double, 2, 1>& u) {
  static_cast<void>(u);
  Eigen::Matrix<double, 5, 1> y;
  y << x(0), x(1), x(2), x(3), x(4);
  return y;
}
}  // namespace

TEST(UnscentedKalmanFilterTest, Init) {
  constexpr auto dt = 0.00505_s;

  frc::UnscentedKalmanFilter<5, 2, 3> observer{Dynamics,
                                               LocalMeasurementModel,
                                               {0.5, 0.5, 10.0, 1.0, 1.0},
                                               {0.0001, 0.01, 0.01},
                                               dt};
  Eigen::Matrix<double, 2, 1> u;
  u << 12.0, 12.0;
  observer.Predict(u, dt);

  auto localY = LocalMeasurementModel(observer.Xhat(), u);
  observer.Correct(u, localY);

  auto globalY = GlobalMeasurementModel(observer.Xhat(), u);
  auto R = frc::MakeCovMatrix(0.01, 0.01, 0.0001, 0.01, 0.01);
  observer.Correct<5>(u, globalY, GlobalMeasurementModel, R,
                      frc::AngleMean<5, 5>(2), frc::AngleResidual<5>(2),
                      frc::AngleResidual<5>(2), frc::AngleAdd<5>(2));
}

TEST(UnscentedKalmanFilterTest, Convergence) {
  constexpr auto dt = 0.00505_s;
  constexpr auto rb = 0.8382_m / 2.0;  // Robot radius

  frc::UnscentedKalmanFilter<5, 2, 3> observer{Dynamics,
                                               LocalMeasurementModel,
                                               {0.5, 0.5, 10.0, 1.0, 1.0},
                                               {0.0001, 0.5, 0.5},
                                               dt};

  auto waypoints =
      std::vector<frc::Pose2d>{frc::Pose2d{2.75_m, 22.521_m, 0_rad},
                               frc::Pose2d{24.73_m, 19.68_m, 5.846_rad}};
  auto trajectory = frc::TrajectoryGenerator::GenerateTrajectory(
      waypoints, {8.8_mps, 0.1_mps_sq});

  Eigen::Matrix<double, 5, 1> r = Eigen::Matrix<double, 5, 1>::Zero();

  Eigen::Matrix<double, 5, 1> nextR;
  Eigen::Matrix<double, 2, 1> u = Eigen::Matrix<double, 2, 1>::Zero();

  auto B = frc::NumericalJacobianU<5, 5, 2>(
      Dynamics, Eigen::Matrix<double, 5, 1>::Zero(),
      Eigen::Matrix<double, 2, 1>::Zero());

  observer.SetXhat(frc::MakeMatrix<5, 1>(
      trajectory.InitialPose().Translation().X().to<double>(),
      trajectory.InitialPose().Translation().Y().to<double>(),
      trajectory.InitialPose().Rotation().Radians().to<double>(), 0.0, 0.0));

  auto trueXhat = observer.Xhat();

  auto totalTime = trajectory.TotalTime();
  for (size_t i = 0; i < (totalTime / dt).to<double>(); ++i) {
    auto ref = trajectory.Sample(dt * i);
    units::meters_per_second_t vl =
        ref.velocity * (1 - (ref.curvature * rb).to<double>());
    units::meters_per_second_t vr =
        ref.velocity * (1 + (ref.curvature * rb).to<double>());

    nextR(0) = ref.pose.Translation().X().to<double>();
    nextR(1) = ref.pose.Translation().Y().to<double>();
    nextR(2) = ref.pose.Rotation().Radians().to<double>();
    nextR(3) = vl.to<double>();
    nextR(4) = vr.to<double>();

    auto localY =
        LocalMeasurementModel(trueXhat, Eigen::Matrix<double, 2, 1>::Zero());
    observer.Correct(u, localY + frc::MakeWhiteNoiseVector(0.0001, 0.5, 0.5));

    Eigen::Matrix<double, 5, 1> rdot = (nextR - r) / dt.to<double>();
    u = B.householderQr().solve(
        rdot - Dynamics(r, Eigen::Matrix<double, 2, 1>::Zero()));

    observer.Predict(u, dt);

    r = nextR;
    trueXhat = frc::RungeKutta(Dynamics, trueXhat, u, dt);
  }

  auto localY = LocalMeasurementModel(trueXhat, u);
  observer.Correct(u, localY);

  auto globalY = GlobalMeasurementModel(trueXhat, u);
  auto R = frc::MakeCovMatrix(0.01, 0.01, 0.0001, 0.5, 0.5);
  observer.Correct<5>(u, globalY, GlobalMeasurementModel, R,
                      frc::AngleMean<5, 5>(2), frc::AngleResidual<5>(2),
                      frc::AngleResidual<5>(2), frc::AngleAdd<5>(2)

  );

  auto finalPosition = trajectory.Sample(trajectory.TotalTime());
  ASSERT_NEAR(finalPosition.pose.Translation().X().template to<double>(),
              observer.Xhat(0), 1.0);
  ASSERT_NEAR(finalPosition.pose.Translation().Y().template to<double>(),
              observer.Xhat(1), 1.0);
  ASSERT_NEAR(finalPosition.pose.Rotation().Radians().template to<double>(),
              observer.Xhat(2), 1.0);
  ASSERT_NEAR(0.0, observer.Xhat(3), 1.0);
  ASSERT_NEAR(0.0, observer.Xhat(4), 1.0);
}
