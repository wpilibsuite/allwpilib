/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/estimator/DifferentialDriveStateEstimator.h"

#include "frc/StateSpaceUtil.h"
#include "frc2/Timer.h"

using namespace frc;

DifferentialDriveStateEstimator::DifferentialDriveStateEstimator(
    const LinearSystem<2, 2, 2>& plant, const Vector<10>& initialState,
    const Vector<10>& stateStdDevs, const Vector<3>& localMeasurementStdDevs,
    const Vector<3>& globalMeasurementStdDevs,
    const DifferentialDriveKinematics& kinematics, units::second_t nominalDt)
    : m_plant(plant),
      m_rb(kinematics.trackWidth / 2.0),
      m_observer([this](auto& x, auto& u) { return Dynamics(x, u); },
                 &DifferentialDriveStateEstimator::LocalMeasurementModel,
                 StdDevMatrixToArray<10>(stateStdDevs),
                 StdDevMatrixToArray<3>(localMeasurementStdDevs), nominalDt),
      m_nominalDt(nominalDt) {
  m_localY.setZero();
  m_globalY.setZero();

  // Create R (covariances) for global measurements.
  Eigen::Matrix<double, 3, 3> globalContR =
      frc::MakeCovMatrix(StdDevMatrixToArray<3>(globalMeasurementStdDevs));

  Eigen::Matrix<double, 3, 3> globalDiscR =
      frc::DiscretizeR<3>(globalContR, m_nominalDt);

  // Create correction mechanism for global measurements.
  m_globalCorrect = [&](const Vector<2>& u, const Vector<3>& y) {
    m_observer.Correct<3>(
        u, y, &DifferentialDriveStateEstimator::GlobalMeasurementModel,
        globalDiscR);
  };

  Reset(initialState);
}

void DifferentialDriveStateEstimator::ApplyPastGlobalMeasurement(
    const Pose2d& visionRobotPose, units::second_t timestamp) {
  m_latencyCompensator.ApplyPastMeasurement<3>(&m_observer, m_nominalDt,
                                               PoseTo3dVector(visionRobotPose),
                                               m_globalCorrect, timestamp);
}

Vector<10> DifferentialDriveStateEstimator::GetEstimatedState() const {
  return m_observer.Xhat();
}

Vector<10> DifferentialDriveStateEstimator::Update(
    units::radian_t heading, units::meter_t leftPosition,
    units::meter_t rightPosition, const Vector<2>& controlInput) {
  return UpdateWithTime(heading, leftPosition, rightPosition, controlInput,
                        frc2::Timer::GetFPGATimestamp());
}

Vector<10> DifferentialDriveStateEstimator::UpdateWithTime(
    units::radian_t heading, units::meter_t leftPosition,
    units::meter_t rightPosition, const Vector<2>& controlInput,
    units::second_t currentTime) {
  auto dt = m_prevTime >= 0_s ? currentTime - m_prevTime : m_nominalDt;
  m_prevTime = currentTime;

  m_localY(LocalOutput::kHeading) = heading.to<double>();
  m_localY(LocalOutput::kLeftPosition) = leftPosition.to<double>();
  m_localY(LocalOutput::kRightPosition) = rightPosition.to<double>();

  m_latencyCompensator.AddObserverState(m_observer, controlInput, m_localY,
                                        currentTime);

  m_observer.Predict(controlInput, dt);
  m_observer.Correct(controlInput, m_localY);

  return GetEstimatedState();
}

void DifferentialDriveStateEstimator::Reset(const Vector<10>& initialState) {
  m_observer.Reset();

  m_observer.SetXhat(initialState);
}

void DifferentialDriveStateEstimator::Reset() { m_observer.Reset(); }

Vector<10> DifferentialDriveStateEstimator::Dynamics(const Vector<10>& x,
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

Vector<3> DifferentialDriveStateEstimator::LocalMeasurementModel(
    const Vector<10>& x, const Vector<2>& u) {
  static_cast<void>(u);

  Eigen::Matrix<double, 3, 1> y;
  y << x(State::kHeading, 0), x(State::kLeftPosition, 0),
      x(State::kRightPosition, 0);
  return y;
}

Vector<3> DifferentialDriveStateEstimator::GlobalMeasurementModel(
    const Vector<10>& x, const Vector<2>& u) {
  static_cast<void>(u);

  Eigen::Matrix<double, 3, 1> y;
  y << x(State::kX, 0), x(State::kY, 0), x(State::kHeading, 0);
  return y;
}

template <int Dim>
std::array<double, Dim> DifferentialDriveStateEstimator::StdDevMatrixToArray(
    const Vector<Dim>& stdDevs) {
  std::array<double, Dim> array;
  for (size_t i = 0; i < Dim; ++i) {
    array[i] = stdDevs(i);
  }
  return array;
}
