// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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
