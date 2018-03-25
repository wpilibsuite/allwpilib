/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <limits>
#include <mutex>
#include <tuple>

#include "CtrlSys/FuncNode.h"
#include "CtrlSys/INode.h"
#include "Timer.h"

namespace frc {

/**
 * Base class for all types of motion profile controllers.
 */
class MotionProfile {
 public:
  MotionProfile();
  virtual ~MotionProfile() = default;

  INode& GetPositionNode();
  INode& GetVelocityNode();
  INode& GetAccelerationNode();

  virtual void SetGoal(double goal, double currentSource) = 0;
  double GetGoal() const;
  bool AtGoal() const;
  double ProfileTimeTotal() const;

  void Reset();

 protected:
  using State = std::tuple<double, double, double>;

  virtual State UpdateSetpoint(double currentTime) = 0;

  // Use this to make UpdateSetpoint() and SetGoal() thread-safe
  mutable std::mutex m_mutex;

  Timer m_timer;

  double m_goal = 0.0;

  double m_sign;

  // Current reference (displacement, velocity, acceleration)
  State m_ref = std::make_tuple(0.0, 0.0, 0.0);

  frc::FuncNode m_positionNode{[this] {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_ref = UpdateSetpoint(m_timer.Get());
    return std::get<0>(m_ref);
  }};

  frc::FuncNode m_velocityNode{[this] {
    std::lock_guard<std::mutex> lock(m_mutex);
    return std::get<1>(m_ref) * m_sign;
  }};

  frc::FuncNode m_accelerationNode{[this] {
    std::lock_guard<std::mutex> lock(m_mutex);
    return std::get<2>(m_ref) * m_sign;
  }};

  double m_lastTime = 0.0;
  double m_timeTotal = std::numeric_limits<double>::infinity();
};

}  // namespace frc
