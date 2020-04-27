/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/controller/LTVDiffDriveController.h"

#include <cmath>

#include <units/math.h>
#include <wpi/MathExtras.h>
#include <wpi/math>

#include "frc/controller/LinearQuadraticRegulator.h"
#include "frc/system/NumericalJacobian.h"

using namespace frc;

LTVDiffDriveController::LTVDiffDriveController(
    const LinearSystem<2, 2, 2>& plant,
    const std::array<double, 5>& controllerQ,
    const std::array<double, 2>& controllerR,
    const DifferentialDriveKinematics& kinematics, units::second_t dt)
    : LTVDiffDriveController(plant, controllerQ, 1.0, controllerR, kinematics,
                             dt) {}

LTVDiffDriveController::LTVDiffDriveController(
    const LinearSystem<2, 2, 2>& plant,
    const std::array<double, 5>& controllerQ, const double rho,
    const std::array<double, 2>& controllerR,
    const DifferentialDriveKinematics& kinematics, units::second_t dt)
    : m_plant(plant),
      m_rb(kinematics.trackWidth / 2.0),
      m_kinematics(kinematics) {
  Reset();

  Vector<10> x0;
  x0.setZero();
  x0(State::kLeftVelocity, 0) = 1e-9;
  x0(State::kRightVelocity, 0) = 1e-9;

  Vector<10> x1;
  x1.setZero();
  x1(State::kLeftVelocity, 0) = 1;
  x1(State::kRightVelocity, 0) = 1;

  Vector<2> u0;
  u0.setZero();

  Eigen::Matrix<double, 5, 5> A0 =
      frc::NumericalJacobianX<10, 10, 2>(
          [this](auto& x, auto& u) { return Dynamics(x, u); }, x0, u0)
          .block<5, 5>(0, 0);
  Eigen::Matrix<double, 5, 5> A1 =
      frc::NumericalJacobianX<10, 10, 2>(
          [this](auto& x, auto& u) { return Dynamics(x, u); }, x1, u0)
          .block<5, 5>(0, 0);

  m_B = frc::NumericalJacobianU<10, 10, 2>(
            [this](auto& x, auto& u) { return Dynamics(x, u); }, x0, u0)
            .block<5, 2>(0, 0);

  m_K0 = frc::LinearQuadraticRegulator<5, 2>(A0, m_B, controllerQ, rho,
                                             controllerR, dt)
             .K();
  m_K1 = frc::LinearQuadraticRegulator<5, 2>(A1, m_B, controllerQ, rho,
                                             controllerR, dt)
             .K();
}

bool LTVDiffDriveController::AtReference() const {
  const auto& tolTranslate = m_poseTolerance.Translation();
  const auto& tolRotate = m_poseTolerance.Rotation();
  return std::abs(m_stateError(0)) < tolTranslate.X().to<double>() &&
         std::abs(m_stateError(1)) < tolTranslate.Y().to<double>() &&
         std::abs(m_stateError(2)) < tolRotate.Radians().to<double>() &&
         std::abs(m_stateError(3)) < m_velocityTolerance.to<double>() &&
         std::abs(m_stateError(4)) < m_velocityTolerance.to<double>();
}

const Vector<5>& LTVDiffDriveController::StateError() const {
  return m_stateError;
}

void LTVDiffDriveController::SetTolerance(
    const Pose2d& poseTolerance, units::meters_per_second_t velocityTolerance) {
  m_poseTolerance = poseTolerance;
  m_velocityTolerance = velocityTolerance;
}

const Vector<5>& LTVDiffDriveController::GetReferences() const {
  return m_nextR;
}

const Vector<2>& LTVDiffDriveController::GetInputs() const {
  return m_uncappedU;
}

const Vector<2>& LTVDiffDriveController::Calculate(
    const Vector<5>& currentState, const Vector<5>& stateRef) {
  m_nextR = stateRef;
  m_stateError = m_nextR - currentState;

  m_uncappedU = Controller(currentState, m_nextR);

  return m_uncappedU;
}

const Vector<2>& LTVDiffDriveController::Calculate(
    const Vector<5>& currentState, const Trajectory::State& desiredState) {
  DifferentialDriveWheelSpeeds wheelVelocities = m_kinematics.ToWheelSpeeds(
      ChassisSpeeds{desiredState.velocity, 0_mps,
                    desiredState.velocity * desiredState.curvature});

  Vector<5> stateRef;

  stateRef << desiredState.pose.Translation().X().to<double>(),
      desiredState.pose.Translation().Y().to<double>(),
      desiredState.pose.Rotation().Radians().to<double>(),
      wheelVelocities.left.to<double>(), wheelVelocities.right.to<double>();

  return Calculate(currentState, stateRef);
}

void LTVDiffDriveController::Reset() {
  m_nextR.setZero();
  m_uncappedU.setZero();
}

Vector<2> LTVDiffDriveController::Controller(
    // This implements the linear time-varying differential drive controller in
    // theorem 8.6.2 of https://tavsys.net/controls-in-frc.
    const Vector<5>& x, const Vector<5>& r) {
  double kx = m_K0(0, 0);
  double ky0 = m_K0(0, 1);
  double kvpos0 = m_K0(0, 3);
  double kvneg0 = m_K0(1, 3);
  double ky1 = m_K1(0, 1);
  double ktheta1 = m_K1(0, 2);
  double kvpos1 = m_K1(0, 3);

  double v = (x(State::kLeftVelocity, 0) + x(State::kRightVelocity, 0)) / 2.0;
  double sqrtAbsV = std::sqrt(std::abs(v));

  Eigen::Matrix<double, 2, 5> K;
  K(0, 0) = kx;
  K(0, 1) = (ky0 + (ky1 - ky0) * sqrtAbsV) * wpi::sgn(v);
  K(0, 2) = ktheta1 * sqrtAbsV;
  K(0, 3) = kvpos0 + (kvpos1 - kvpos0) * sqrtAbsV;
  K(0, 4) = kvneg0 - (kvpos1 - kvpos0) * sqrtAbsV;
  K(1, 0) = kx;
  K(1, 1) = -K(0, 1);
  K(1, 2) = -K(0, 2);
  K(1, 3) = K(0, 4);
  K(1, 4) = K(0, 3);

  Eigen::Matrix<double, 5, 5> inRobotFrame =
      Eigen::Matrix<double, 5, 5>::Identity();
  inRobotFrame(0, 0) = std::cos(x(2, 0));
  inRobotFrame(0, 1) = std::sin(x(2, 0));
  inRobotFrame(1, 0) = -std::sin(x(2, 0));
  inRobotFrame(1, 1) = std::cos(x(2, 0));

  Eigen::Matrix<double, 5, 1> error = r - x.block<5, 1>(0, 0);
  error(State::kHeading, 0) =
      units::math::NormalizeAngle(units::radian_t(error(State::kHeading, 0)))
          .to<double>();
  return K * inRobotFrame * error;
}

Vector<10> LTVDiffDriveController::Dynamics(const Vector<10>& x,
                                            const Vector<2>& u) {
  Eigen::Matrix<double, 4, 2> B;
  B.block<2, 2>(0, 0) = m_plant.B();
  B.block<2, 2>(2, 0).setZero();
  Eigen::Matrix<double, 4, 7> A;
  A.block<2, 2>(0, 0) = m_plant.A();

  A.block<2, 2>(2, 0).setIdentity();
  A.block<4, 2>(0, 2).setZero();
  A.block<4, 2>(0, 4) = B;
  A.block<4, 1>(0, 6) << 0, 0, 1, -1;

  double v = (x(State::kLeftVelocity, 0) + x(State::kRightVelocity, 0)) / 2.0;

  Eigen::Matrix<double, 10, 1> result;
  result(0, 0) = v * std::cos(x(State::kHeading, 0));
  result(1, 0) = v * std::sin(x(State::kHeading, 0));
  result(2, 0) = ((x(State::kRightVelocity, 0) - x(State::kLeftVelocity, 0)) /
                  (2.0 * m_rb))
                     .to<double>();
  result.block<4, 1>(3, 0) = A * x.block<7, 1>(3, 0) + B * u;
  result.block<3, 1>(7, 0).setZero();
  return result;
}
