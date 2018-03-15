/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

/**
 * PDP Data.
 */
@SuppressWarnings("AbbreviationAsWordInName")
public class PDPData {
  /*
   * voltage
   */
  @SuppressWarnings("MemberName")
  public double voltage;
  /*
   * currents
   */
  @SuppressWarnings("MemberName")
  public final double[] currents = new double[16];
  /*
   * temperature
   */
  @SuppressWarnings("MemberName")
  public double temperature;
  /*
   * totalCurrent
   */
  @SuppressWarnings("MemberName")
  public double totalCurrent;
  /*
   * power
   */
  @SuppressWarnings("MemberName")
  public double power;
  /*
   * energy
   */
  @SuppressWarnings("MemberName")
  public double energy;

  /**
   * setData.
   */
  public double[] setData(double voltage, double temperature, double totalCurrent,
                          double power, double energy) {
    this.voltage = voltage;
    this.temperature = temperature;
    this.totalCurrent = totalCurrent;
    this.power = power;
    this.energy = energy;
    return currents;
  }
}
