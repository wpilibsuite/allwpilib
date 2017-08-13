/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

@SuppressWarnings("SummaryJavadoc")
public interface PIDInterface extends Controller {

  @SuppressWarnings("ParameterName")
  void setPID(double p, double i, double d);

  double getP();

  double getI();

  double getD();

  void setSetpoint(double setpoint);

  double getSetpoint();

  double getError();

  void enable();

  void disable();

  boolean isEnabled();

  void reset();
}
