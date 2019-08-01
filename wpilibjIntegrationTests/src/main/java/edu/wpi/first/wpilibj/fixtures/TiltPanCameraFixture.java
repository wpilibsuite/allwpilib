// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.fixtures;

import edu.wpi.first.wpilibj.AnalogGyro;
import edu.wpi.first.wpilibj.Servo;
import edu.wpi.first.wpilibj.Timer;
import java.util.logging.Logger;

/**
 * A class to represent the a physical Camera with two servos (tilt and pan) designed to test to see
 * if the gyroscope is operating normally.
 */
public abstract class TiltPanCameraFixture implements ITestFixture {
  public static final Logger logger = Logger.getLogger(TiltPanCameraFixture.class.getName());

  public static final double RESET_TIME = 2.0;
  private AnalogGyro m_gyro;
  private AnalogGyro m_gyroParam;
  private Servo m_tilt;
  private Servo m_pan;
  private boolean m_initialized = false;

  protected abstract AnalogGyro giveGyro();

  protected abstract AnalogGyro giveGyroParam(int center, double offset);

  protected abstract Servo giveTilt();

  protected abstract Servo givePan();

  /** Constructs the TiltPanCamera. */
  public TiltPanCameraFixture() {}

  @Override
  public void setup() {
    if (!m_initialized) {
      m_initialized = true;
      m_tilt = giveTilt();
      m_tilt.set(0);
      m_pan = givePan();
      m_pan.set(0);
      Timer.delay(RESET_TIME);

      logger.fine("Initializing the gyro");
      m_gyro = giveGyro();
      m_gyro.reset();
    }
  }

  @Override
  public void reset() {
    if (m_gyro != null) {
      m_gyro.reset();
    }
  }

  public Servo getTilt() {
    return m_tilt;
  }

  public Servo getPan() {
    return m_pan;
  }

  public AnalogGyro getGyro() {
    return m_gyro;
  }

  public AnalogGyro getGyroParam() {
    return m_gyroParam;
  }

  // Do not call unless all other instances of Gyro have been deallocated
  public void setupGyroParam(int center, double offset) {
    m_gyroParam = giveGyroParam(center, offset);
    m_gyroParam.reset();
  }

  public void freeGyro() {
    m_gyro.close();
    m_gyro = null;
  }

  @Override
  public void teardown() {
    m_tilt.close();
    m_tilt = null;
    m_pan.close();
    m_pan = null;
    if (m_gyro != null) { // in case not freed during gyro tests
      m_gyro.close();
      m_gyro = null;
    }
    if (m_gyroParam != null) { // in case gyro tests failed before getting to this point
      m_gyroParam.close();
      m_gyroParam = null;
    }
  }
}
