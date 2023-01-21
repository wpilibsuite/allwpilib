// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.servo;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;
import edu.wpi.first.wpilibj.PWM;

/* Common base class for all linear servos */
public abstract class LinearServo extends PWM {
  protected double m_strokeMetres;

  /**
   * Constructor.<br>
   *
   * @param channel The PWM channel to which the servo is attached. 0-9 are on-board, 10-19 are on
   *     the MXP port
   */
  protected LinearServo(final String name, final int channel, final double stroke) {
    super(channel);
    setPeriodMultiplier(PeriodMultiplier.k4X);

    m_strokeMetres = stroke;

    HAL.report(tResourceType.kResourceType_Servo, getChannel() + 1);
    SendableRegistry.setName(this, name, getChannel());
  }

  /**
   * Set the servo position.
   *
   * <p>Servo values range from 0.0 to 1.0 corresponding to the range of full retract to full
   * extend. // todo verify that this is the case.
   *
   * @param value Position from 0.0 to 1.0.
   */
  public void set(double value) {
    setPosition(value);
  }

  /**
   * Get the servo position.
   *
   * <p>Servo values range from 0.0 to 1.0 corresponding to the range of full retract to full
   * extend. This returns the commanded position, not the position that the servo is actually at, as
   * the servo does not report its own position.
   *
   * @return Position from 0.0 to 1.0.
   */
  public double get() {
    return getPosition();
  }

  /**
   * Set the servo extend distance.
   *
   * <p>Distances that are out of the supported range of the servo simply "saturate" in that
   * direction. In other words, if the servo has a stroke of 50mm then distances of less than 0mm
   * result in the servo being fully retracted and distances of greater than 50mm result in the
   * servo being fully extended.
   *
   * @param distanceMetres The extend distance in metres to set the servo.
   */
  public void setExtendDistance(double distanceMetres) {
    if (distanceMetres < 0.0) {
      distanceMetres = 0.0;
    } else if (distanceMetres > m_strokeMetres) {
      distanceMetres = m_strokeMetres;
    }

    setPosition(distanceMetres / m_strokeMetres);
  }

  /**
   * Get the servo extend distance.
   *
   * <p>This returns the commanded distance, not the distance that the servo is actually at, as the
   * servo does not report its own position.
   *
   * @return The distance in metres to which the servo is set.
   */
  public double getExtendDistance() {
    return getPosition() * m_strokeMetres;
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Servo");
    builder.setActuator(true);
    builder.addDoubleProperty("Value", this::get, this::set);
  }
}
