// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "opmode/MyAuto.hpp"

#include "Robot.hpp"

/** The Robot instance is passed into the opmode via the constructor. */
MyAuto::MyAuto(Robot& robot) : m_robot{robot} {
  /*
   * Can call the base class constructor with the period to set a different
   * periodic time interval.
   *
   * Additional periodic methods may be configured with AddPeriodic().
   */
}

MyAuto::~MyAuto() {
  /* Called when the opmode is de-selected. */
}

void MyAuto::Start() {
  /* Called once when the robot is first enabled. */
}

void MyAuto::Periodic() {
  /* Called periodically (set time interval) while the robot is enabled. */
}

void MyAuto::End() {
  /* Called when the robot is disabled (after previously being enabled). */
}
