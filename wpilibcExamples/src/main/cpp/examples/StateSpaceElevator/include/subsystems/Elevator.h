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

#include "subsystems/ElevatorController.h"

class Elevator {
 public:
  Elevator();

  Elevator(const Elevator&) = delete;
  Elevator& operator=(const Elevator&) = delete;

  void Enable();
  void Disable();

  /**
   * Sets the references.
   *
   * @param position Position of the carriage in meters.
   * @param velocity Velocity of the carriage in meters per second.
   */
  void SetReferences(double position, double velocity);

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

  ElevatorController m_elevator;
  frc::Notifier m_thread{&Elevator::Iterate, this};
};
