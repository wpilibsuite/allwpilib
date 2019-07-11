/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

import edu.wpi.first.wpilibj.SendableBase;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;

/**
 * A mock sendable that marks itself as an actuator.
 */
public class MockActuatorSendable extends SendableBase {
  public MockActuatorSendable(String name) {
    super(false);
    setName(name);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setActuator(true);
  }
}
