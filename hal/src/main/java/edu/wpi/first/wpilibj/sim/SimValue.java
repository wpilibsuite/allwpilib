/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.sim;

public class SimValue {
  private boolean v_boolean;
  private long v_long;
  private double v_double;

  private SimValue(boolean b) {
    v_boolean = b;
  }

  private SimValue(double v) {
    v_double = v;
  }

  private SimValue(long v) {
    v_long = v;
  }

  private SimValue() {

  }

  public boolean getBoolean() {
    return v_boolean;
  }

  public long getLong() {
    return v_long;
  }

  public double getDouble() {
    return v_double;
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
