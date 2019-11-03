/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
