/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved. */
/* Open Source Software - may be modified and shared by FRC teams. The code */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project. */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj.fixtures;

import java.util.logging.Logger;

import edu.wpi.first.wpilibj.Gyro;
import edu.wpi.first.wpilibj.Servo;
import edu.wpi.first.wpilibj.Timer;

/**
 * A class to represent the a physical Camera with two servos (tilt and pan)
 * designed to test to see if the gyroscope is operating normally.
 *
 * @author Jonathan Leitschuh
 *
 */
public abstract class TiltPanCameraFixture implements ITestFixture {
  public static final Logger logger = Logger.getLogger(TiltPanCameraFixture.class.getName());

  public static final double RESET_TIME = 2.0;
  private Gyro gyro;
  private Servo tilt;
  private Servo pan;
  private boolean initialized = false;


  protected abstract Gyro giveGyro();

  protected abstract Servo giveTilt();

  protected abstract Servo givePan();

  /**
   * Constructs the TiltPanCamera
   */
  public TiltPanCameraFixture() {}

  @Override
  public boolean setup() {
    boolean wasSetup = false;
    if (!initialized) {
      initialized = true;
      tilt = giveTilt();
      tilt.set(0);
      pan = givePan();
      pan.set(0);
      Timer.delay(RESET_TIME);

      logger.fine("Initializing the gyro");
      gyro = giveGyro();
      gyro.reset();
      wasSetup = true;
    }
    return wasSetup;
  }

  @Override
  public boolean reset() {
    gyro.reset();
    return true;
  }

  public Servo getTilt() {
    return tilt;
  }

  public Servo getPan() {
    return pan;
  }

  public Gyro getGyro() {
    return gyro;
  }

  @Override
  public boolean teardown() {
    tilt.free();
    tilt = null;
    pan.free();
    pan = null;
    gyro.free();
    gyro = null;
    return true;
  }

}
