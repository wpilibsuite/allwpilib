// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/sendable/Sendable.h"

#include "wpi/sendable/SendableRegistry.h"

using namespace wpi;

Sendable::Sendable(Sendable&& rhs) {
  // it is safe to call Move() multiple times with the same rhs
  SendableRegistry::Move(this, &rhs);
}

Sendable& Sendable::operator=(Sendable&& rhs) {
  // it is safe to call Move() multiple times with the same rhs
  SendableRegistry::Move(this, &rhs);
  return *this;
}

Sendable::~Sendable() {
  // it is safe to call Remove() multiple times with the same object
  SendableRegistry::Remove(this);
}
