// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/interpolation/TimeInterpolatableBuffer.h"

namespace frc {

// Template specialization to ensure that Pose2d uses pose exponential
template <>
TimeInterpolatableBuffer<Pose2d>::TimeInterpolatableBuffer(
    units::second_t historySize)
    : m_historySize(historySize),
      m_interpolatingFunc([](const Pose2d& start, const Pose2d& end, double t) {
        if (t < 0) {
          return start;
        } else if (t >= 1) {
          return end;
        } else {
          Twist2d twist = start.Log(end);
          Twist2d scaledTwist = twist * t;
          return start.Exp(scaledTwist);
        }
      }) {}

}  // namespace frc
