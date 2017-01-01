/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.mockhardware;

import edu.wpi.first.wpilibj.AnalogOutput;

/**
 * A fake source to provide output to a {@link edu.wpi.first.wpilibj.interfaces.Potentiometer}.
 */
public class FakePotentiometerSource {
  private AnalogOutput m_output;
  private boolean m_initOutput;
  private double m_potMaxAngle;
  private double m_potMaxVoltage = 5.0;
  private final double m_defaultPotMaxAngle;

  /**
   * Constructs the fake source.
   *
   * @param output             The analog port to output the signal on
   * @param defaultPotMaxAngle The default maximum angle the pot supports.
   */
  public FakePotentiometerSource(AnalogOutput output, double defaultPotMaxAngle) {
    m_defaultPotMaxAngle = defaultPotMaxAngle;
    m_potMaxAngle = defaultPotMaxAngle;
    m_output = output;
    m_initOutput = false;
  }

  public FakePotentiometerSource(int port, double defaultPotMaxAngle) {
    this(new AnalogOutput(port), defaultPotMaxAngle);
    m_initOutput = true;
  }

  /**
   * Sets the maximum voltage output. If not the default is 5.0V.
   *
   * @param voltage The voltage that indicates that the pot is at the max value.
   */
  public void setMaxVoltage(double voltage) {
    m_potMaxVoltage = voltage;
  }

  public void setRange(double range) {
    m_potMaxAngle = range;
  }

  public void reset() {
    m_potMaxAngle = m_defaultPotMaxAngle;
    m_output.setVoltage(0.0);
  }

  public void setAngle(double angle) {
    m_output.setVoltage((m_potMaxVoltage / m_potMaxAngle) * angle);
  }

  public void setVoltage(double voltage) {
    m_output.setVoltage(voltage);
  }

  public double getVoltage() {
    return m_output.getVoltage();
  }

  /**
   * Returns the currently set angle.
   *
   * @return the current angle
   */
  public double getAngle() {
    double voltage = m_output.getVoltage();
    if (voltage == 0) { // Removes divide by zero error
      return 0;
    }
    return voltage * (m_potMaxAngle / m_potMaxVoltage);
  }

  /**
   * Frees the resouce.
   */
  public void free() {
    if (m_initOutput) {
      m_output.free();
      m_output = null;
      m_initOutput = false;
    }
  }
}
