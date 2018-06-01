/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <limits>
#include <mutex>
#include <tuple>

#include "Notifier.h"
#include "PIDOutput.h"
#include "Timer.h"

namespace frc {

/**
 * Base class for all types of motion profile controllers.
 */
class MotionProfile {
 public:
  explicit MotionProfile(PIDOutput& output, double period = 0.05);
  virtual ~MotionProfile() = default;

  void Enable();
  void Disable();

  virtual void SetGoal(double goal, double currentSource) = 0;
  double GetGoal() const;
  double GetReference() const;
  bool AtGoal() const;
  double GetTotalTime() const;

  void Reset();

 protected:
  using State = std::tuple<double, double, double>;

  // Use this to make UpdateSetpoint() and SetGoal() thread-safe
  mutable std::mutex m_mutex;

  double m_goal = 0.0;

  double m_sign;

  // Current reference (displacement, velocity, acceleration)
  State m_ref = std::make_tuple(0.0, 0.0, 0.0);

  double m_lastTime = 0.0;
  double m_totalTime = std::numeric_limits<double>::infinity();

  virtual State UpdateReference(double currentTime) = 0;

 private:
  PIDOutput& m_output;
  double m_period;
  Timer m_timer;
  Notifier m_notifier{&MotionProfile::Update, this};

  void Update();
};

}  // namespace frc
