// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/estimator/ThreeDeadWheelPoseEstimator3d.hpp"

#include "wpi/math/estimator/AngleStatistics.hpp"
#include "wpi/math/util/MathShared.hpp"
#include "wpi/math/util/StateSpaceUtil.hpp"
#include "wpi/util/timestamp.h"

using namespace wpi::math;

wpi::math::ThreeDeadWheelPoseEstimator3d::
    ThreeDeadWheelPoseEstimator3d(
        wpi::units::meter_t x1WheelYPos, wpi::units::meter_t x2WheelYPos,
        wpi::units::meter_t yWheelXPos, const Rotation3d& gyroAngle,
        const ThreeDeadWheelPositions& wheelPositions,
        const Pose3d& initialPose)
    : ThreeDeadWheelPoseEstimator3d{
          x1WheelYPos,          x2WheelYPos,
          yWheelXPos,           gyroAngle,
          wheelPositions,       initialPose,
          {0.1, 0.1, 0.1, 0.1}, {0.9, 0.9, 0.9, 0.9}} {}

wpi::math::ThreeDeadWheelPoseEstimator3d::
    ThreeDeadWheelPoseEstimator3d(
        wpi::units::meter_t x1WheelYPos, wpi::units::meter_t x2WheelYPos,
        wpi::units::meter_t yWheelXPos, const Rotation3d& gyroAngle,
        const ThreeDeadWheelPositions& wheelPositions,
        const Pose3d& initialPose,
        const wpi::util::array<double, 4>& stateStdDevs,
        const wpi::util::array<double, 4>& visionMeasurementStdDevs)
    : PoseEstimator3d(m_odometryImpl, stateStdDevs, visionMeasurementStdDevs),
      m_odometryImpl(x1WheelYPos, x2WheelYPos, yWheelXPos, gyroAngle,
                     wheelPositions, initialPose) {
  ResetPose(initialPose);
}
