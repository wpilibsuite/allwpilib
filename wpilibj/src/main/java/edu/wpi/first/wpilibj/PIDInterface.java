/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

@Deprecated(since = "2020", forRemoval = true)
@SuppressWarnings("SummaryJavadoc")
public interface PIDInterface {
  @SuppressWarnings("ParameterName")
  void setPID(double p, double i, double d);

  double getP();

  double getI();

  double getD();

  void setSetpoint(double setpoint);

  double getSetpoint();

  double getError();

  void reset();
}
