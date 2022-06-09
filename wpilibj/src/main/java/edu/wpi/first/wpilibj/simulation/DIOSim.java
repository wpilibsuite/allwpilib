// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.DigitalOutput;

/**
 * Class to control a simulated digital input or output.
 *
 * @deprecated Use {@link edu.wpi.first.wpilibj.simulation.DigitalSim}. instead.
 */
@Deprecated(since = "2022", forRemoval = true)
public class DIOSim extends DigitalSim {
  /**
   * Constructs from a DigitalInput object.
   *
   * @param input DigitalInput to simulate
   */
  public DIOSim(DigitalInput input) {
    super(input);
  }

  /**
   * Constructs from a DigitalOutput object.
   *
   * @param output DigitalOutput to simulate
   */
  public DIOSim(DigitalOutput output) {
    super(output);
  }

  /**
   * Constructs from an digital I/O channel number.
   *
   * @param channel Channel number
   */
  public DIOSim(int channel) {
    super(channel);
  }
}
