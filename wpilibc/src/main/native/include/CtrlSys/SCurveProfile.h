/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "CtrlSys/MotionProfile.h"

namespace frc {

/**
 * Provides trapezoidal acceleration control.
 *
 * Constant acceleration until target (max) velocity is reached, sets
 * acceleration to zero for a calculated time, then decelerates at a constant
 * acceleration with a slope equal to the negative slope of the initial
 * acceleration.
 */
class SCurveProfile : public MotionProfile {
 public:
  SCurveProfile(double maxV, double maxA, double timeToMaxA);

  void SetGoal(double goal, double currentSource = 0.0) override;

  void SetMaxVelocity(double v);
  double GetMaxVelocity() const;
  void SetMaxAcceleration(double a);
  void SetTimeToMaxA(double timeToMaxA);

 protected:
  State UpdateSetpoint(double currentTime) override;

 private:
  double m_acceleration;
  double m_maxVelocity;
  double m_profileMaxVelocity;
  double m_timeToMaxA;

  double m_jerk;
  double m_t2;
  double m_t3;
  double m_t4;
  double m_t5;
  double m_t6;
  double m_t7;
};

}  // namespace frc
