/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <Eigen/Core>
#include <frc/Encoder.h>
#include <frc/Notifier.h>
#include <frc/Spark.h>

#include "subsystems/SingleJointedArmController.h"

class SingleJointedArm {
 public:
  SingleJointedArm();

  SingleJointedArm(const SingleJointedArm&) = delete;
  SingleJointedArm& operator=(const SingleJointedArm&) = delete;

  void Enable();
  void Disable();

  /**
   * Sets the references.
   *
   * @param angle           Angle of arm in radians.
   * @param angularVelocity Velocity of arm in radians per second.
   */
  void SetReferences(double angle, double angularVelocity);

  bool AtReference() const;

  /**
   * Iterates the elevator control loop one cycle.
   */
  void Iterate();

  /**
   * Returns controller output.
   */
  double ControllerVoltage() const;

  void Reset();

 private:
  frc::Spark m_motor{1};
  frc::Encoder m_encoder{1, 2};

  SingleJointedArmController m_singleJointedArm;
  frc::Notifier m_thread{&SingleJointedArm::Iterate, this};
};
