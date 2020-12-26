// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.shuffleboard;

import edu.wpi.first.wpilibj.Sendable;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;
import edu.wpi.first.wpilibj.smartdashboard.SendableRegistry;

/**
 * A mock sendable that marks itself as an actuator.
 */
public class MockActuatorSendable implements Sendable {
  public MockActuatorSendable(String name) {
    SendableRegistry.add(this, name);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setActuator(true);
  }
}
