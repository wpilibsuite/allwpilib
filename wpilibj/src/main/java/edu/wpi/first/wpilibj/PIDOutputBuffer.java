/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

public class PIDOutputBuffer implements PIDOutput {
  private volatile double m_output = 0.0;

  @Override
  public void pidWrite(double output) {
    m_output = output;
  }

  public double getOutput() {
    return m_output;
  }
}
