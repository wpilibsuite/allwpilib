// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "opmode/MyTeleop.hpp"

#include "Robot.hpp"

/** The Robot instance is passed into the opmode via the constructor. */
MyTeleop::MyTeleop(Robot& robot) : m_robot{robot} {}

MyTeleop::~MyTeleop() {
  /* Called when the opmode is de-selected. */
}

void MyTeleop::Start() {
  /* Called once when the robot is first enabled. */
}

void MyTeleop::Periodic() {
  /* Called periodically (set time interval) while the robot is enabled. */
}

void MyTeleop::End() {
  /* Called when the robot is disabled (after previously being enabled). */
}
