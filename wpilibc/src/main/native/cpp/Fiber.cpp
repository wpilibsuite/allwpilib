/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/Fiber.h"

using namespace frc;

Fiber::Fiber(double period, std::function<void()> callback)
    : m_period(static_cast<int64_t>(period * 1.0e6)), m_callback(callback) {}

bool Fiber::operator>(const Fiber& rhs) {
  return m_expirationTime > rhs.m_expirationTime;
}
