/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal.sim;

public final class SimValue {
  private boolean m_boolean;
  private long m_long;
  private double m_double;

  private SimValue(boolean b) {
    m_boolean = b;
  }

  private SimValue(double v) {
    m_double = v;
  }

  private SimValue(long v) {
    m_long = v;
  }

  private SimValue() {

  }

  public boolean getBoolean() {
    return m_boolean;
  }

  public long getLong() {
    return m_long;
  }

  public double getDouble() {
    return m_double;
  }

  public static SimValue makeBoolean(boolean value) {
    return new SimValue(value);
  }

  public static SimValue makeEnum(int value) {
    return new SimValue(value);
  }

  public static SimValue makeInt(int value) {
    return new SimValue(value);
  }

  public static SimValue makeLong(long value) {
    return new SimValue(value);
  }

  public static SimValue makeDouble(double value) {
    return new SimValue(value);
  }

  public static SimValue makeUnassigned() {
    return new SimValue();
  }
}
