// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot.subsystems;

import edu.wpi.first.wpilibj.AddressableLED;
import edu.wpi.first.wpilibj.AddressableLEDBuffer;
import frc.robot.PeriodicTask;

public class RobotSignalsLEDbufferLEDSubsystem  {
  private final AddressableLED strip;
  private final AddressableLEDBuffer buffer;
   
  public RobotSignalsLEDbufferLEDSubsystem(int length, int port, PeriodicTask periodicTask) {
    // initialize the physical LED strip length
    strip = new AddressableLED(port);
    strip.setLength(length);

    buffer = new AddressableLEDBuffer(length);

    // start the physical LED strip output
    periodicTask.register(()->strip.start(), 0.1, 0.01);
  }

  public void periodic() {
    strip.setData(buffer);
  }
}
