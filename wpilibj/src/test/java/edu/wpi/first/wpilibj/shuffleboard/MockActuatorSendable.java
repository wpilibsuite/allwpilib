// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.wpilibj.shuffleboard;

import org.wpilib.util.sendable.Sendable;
import org.wpilib.util.sendable.SendableBuilder;
import org.wpilib.util.sendable.SendableRegistry;

/** A mock sendable that marks itself as an actuator. */
public class MockActuatorSendable implements Sendable {
  @SuppressWarnings("this-escape")
  public MockActuatorSendable(String name) {
    SendableRegistry.add(this, name);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setActuator(true);
  }
}
