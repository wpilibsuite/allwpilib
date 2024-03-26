// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace wpi2 {

class SendableTableBackend {
  virtual ~SendableTableBackend() = default;


  /**
   * Reinitializes with a different object pointer. Any current callbacks must
   * be immediately invalidated. This is called from the move constructor
   * in SendableHelper, and so the new object is not yet fully
   * moved/initialized and calling Sendable::InitSendable() on this new object
   * must be a deferred operation to avoid UB.
   *
   * @param obj object pointer; nullptr indicates object has been deleted
   */
  virtual void ObjectMove(void* obj) = 0;

  // TODO
};

}  // namespace wpi2
