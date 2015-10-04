/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved. */
/* Open Source Software - may be modified and shared by FRC teams. The code */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project. */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj.mockhardware;

import edu.wpi.first.wpilibj.AnalogOutput;

/**
 * @author jonathanleitschuh
 *
 */
public class FakePotentiometerSource {
  private AnalogOutput output;
  private boolean m_init_output;
  private double potMaxAngle;
  private double potMaxVoltage = 5.0;
  private final double defaultPotMaxAngle;

  public FakePotentiometerSource(AnalogOutput output, double defaultPotMaxAngle) {
    this.defaultPotMaxAngle = defaultPotMaxAngle;
    potMaxAngle = defaultPotMaxAngle;
    this.output = output;
    m_init_output = false;
  }

  public FakePotentiometerSource(int port, double defaultPotMaxAngle) {
    this(new AnalogOutput(port), defaultPotMaxAngle);
    m_init_output = true;
  }

  /**
   * Sets the maximum voltage output. If not the default is 5.0V
   *$
   * @param voltage The voltage that indicates that the pot is at the max value.
   */
  public void setMaxVoltage(double voltage) {
    potMaxVoltage = voltage;
  }

  public void setRange(double range) {
    potMaxAngle = range;
  }

  public void reset() {
    potMaxAngle = defaultPotMaxAngle;
    output.setVoltage(0.0);
  }

  public void setAngle(double angle) {
    output.setVoltage((potMaxVoltage / potMaxAngle) * angle);
  }

  public void setVoltage(double voltage) {
    output.setVoltage(voltage);
  }

  public double getVoltage() {
    return output.getVoltage();
  }

  /**
   * Returns the currently set angle
   *$
   * @return the current angle
   */
  public double getAngle() {
    double voltage = output.getVoltage();
    if (voltage == 0) { // Removes divide by zero error
      return 0;
    }
    return voltage * (potMaxAngle / potMaxVoltage);
  }

  public void free() {
    if (m_init_output) {
      output.free();
      output = null;
      m_init_output = false;
    }
  }
}
