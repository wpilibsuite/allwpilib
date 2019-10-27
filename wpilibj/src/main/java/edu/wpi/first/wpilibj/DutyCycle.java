/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.DutyCycleJNI;

public class DutyCycle implements AutoCloseable {
  // Explicitly package private
  final int m_handle;

  public DutyCycle(DigitalSource digitalSource) {
    m_handle = DutyCycleJNI.initialize(digitalSource.getPortHandleForRouting(),
                            digitalSource.getAnalogTriggerTypeForRouting());
  }

  @Override
  public void close() {
    DutyCycleJNI.free(m_handle);
  }

  public int getFrequency() {
    return DutyCycleJNI.getFrequency(m_handle);
  }

  public double getOutput() {
    return DutyCycleJNI.getOutput(m_handle);
  }

  public int getOutputRaw() {
    return DutyCycleJNI.getOutputRaw(m_handle);
  }

  public int getOutputScaleFactor() {
    return DutyCycleJNI.getOutputScaleFactor(m_handle);
  }
}
