// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/estimator/TwoDeadWheelPoseEstimator3d.hpp"

#include "wpi/math/estimator/AngleStatistics.hpp"
#include "wpi/math/util/MathShared.hpp"
#include "wpi/math/util/StateSpaceUtil.hpp"
#include "wpi/util/timestamp.h"

using namespace wpi::math;

wpi::math::TwoDeadWheelPoseEstimator3d::
    TwoDeadWheelPoseEstimator3d(
        wpi::units::meter_t xWheelYPos, wpi::units::meter_t yWheelXPos,
        const Rotation3d& gyroAngle,
        const TwoDeadWheelPositions& wheelPositions,
        const Pose3d& initialPose)
    : TwoDeadWheelPoseEstimator3d{
          xWheelYPos,          yWheelXPos,  gyroAngle,
          wheelPositions,      initialPose, {0.1, 0.1, 0.1, 0.1},
          {0.9, 0.9, 0.9, 0.9}} {}

wpi::math::TwoDeadWheelPoseEstimator3d::
    TwoDeadWheelPoseEstimator3d(
        wpi::units::meter_t xWheelYPos, wpi::units::meter_t yWheelXPos,
        const Rotation3d& gyroAngle,
        const TwoDeadWheelPositions& wheelPositions,
        const Pose3d& initialPose,
        const wpi::util::array<double, 4>& stateStdDevs,
        const wpi::util::array<double, 4>& visionMeasurementStdDevs)
    : PoseEstimator3d(m_odometryImpl, stateStdDevs, visionMeasurementStdDevs),
      m_odometryImpl(xWheelYPos, yWheelXPos, gyroAngle, wheelPositions,
                     initialPose) {
  ResetPose(initialPose);
}
