/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.GyroBase;
import edu.wpi.first.wpilibj.interfaces.Gyro;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;
import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;
import edu.wpi.first.wpilibj.simulation.SimGyro;
import edu.wpi.first.wpilibj.tables.ITable;

/**
 * Use a rate gyro to return the robots heading relative to a starting position. The AnalogGyro
 * class tracks the robots heading based on the starting position. As the robot rotates the new
 * heading is computed by integrating the rate of rotation returned by the sensor. When the class is
 * instantiated, it does a short calibration routine where it samples the gyro while at rest to
 * determine the default offset. This is subtracted from each sample to determine the heading.
 */
public class AnalogGyro extends GyroBase implements Gyro, PIDSource, LiveWindowSendable {

  private PIDSourceType m_pidSource;
  private SimGyro impl;

  /**
   * Initialize the gyro. Calibrate the gyro by running for a number of samples and computing the
   * center value for this part. Then use the center value as the Accumulator center value for
   * subsequent measurements. It's important to make sure that the robot is not moving while the
   * centering calculations are in progress, this is typically done when the robot is first turned
   * on while it's sitting at rest before the competition starts.
   */
  private void initGyro(int channel) {
    impl = new SimGyro("simulator/analog/" + channel);

    reset();
    setPIDSourceType(PIDSourceType.kDisplacement);

    LiveWindow.addSensor("AnalogGyro", channel, this);
  }

  /**
   * AnalogGyro constructor with only a channel.
   *
   * @param channel The analog channel the gyro is connected to.
   */
  public AnalogGyro(int channel) {
    initGyro(channel);
  }

  /**
   * {@inherit_doc}
   */
  public void calibrate() {
    reset();
  }


  /**
   * AnalogGyro constructor with a precreated analog channel object. Use this
   * constructor when the analog channel needs to be shared. There is no
   * reference counting when an AnalogChannel is passed to the gyro.
   *
   * @param channel
   *            The AnalogChannel object that the gyro is connected to.
   */
  // Not Supported: public AnalogGyro(AnalogChannel channel) {

  /**
   * Reset the gyro. Resets the gyro to a heading of zero. This can be used if there is significant
   * drift in the gyro and it needs to be recalibrated after it has been running.
   */
  public void reset() {
    impl.reset();
  }

  /**
   * Delete (free) the accumulator and the analog components used for the gyro.
   */
  public void free() {
  }

  /**
   * Return the actual angle in degrees that the robot is currently facing.
   *
   * The angle is based on the current accumulator value corrected by the oversampling rate, the
   * gyro type and the A/D calibration values. The angle is continuous, that is can go beyond 360
   * degrees. This make algorithms that wouldn't want to see a discontinuity in the gyro output as
   * it sweeps past 0 on the second time around.
   *
   * @return the current heading of the robot in degrees. This heading is based on integration of
   * the returned rate from the gyro.
   */
  public double getAngle() {
    return impl.getAngle();
  }

  /**
   * Return the rate of rotation of the gyro
   *
   * The rate is based on the most recent reading of the gyro analog value
   *
   * @return the current rate in degrees per second
   */
  public double getRate() {
    return impl.getVelocity();
  }

  /**
   * Set which parameter of the encoder you are using as a process control variable. The AnalogGyro
   * class supports the rate and angle parameters
   *
   * @param pidSource An enum to select the parameter.
   */
  public void setPIDSourceType(PIDSourceType pidSource) {
    m_pidSource = pidSource;
  }

  /**
   * {@inheritDoc}
   */
  public PIDSourceType getPIDSourceType() {
    return m_pidSource;
  }

  /**
   * Get the angle of the gyro for use with PIDControllers
   *
   * @return the current angle according to the gyro
   */
  public double pidGet() {
    switch (m_pidSource) {
      case kRate:
        return getRate();
      case kDisplacement:
        return getAngle();
      default:
        return 0.0;
    }
  }

  /*
   * Live Window code, only does anything if live window is activated.
   */
  public String getSmartDashboardType() {
    return "AnalogGyro";
  }

  private ITable m_table;

  /**
   * {@inheritDoc}
   */
  public void initTable(ITable subtable) {
    m_table = subtable;
    updateTable();
  }

  /**
   * {@inheritDoc}
   */
  public ITable getTable() {
    return m_table;
  }

  /**
   * {@inheritDoc}
   */
  public void updateTable() {
    if (m_table != null) {
      m_table.putNumber("Value", getAngle());
    }
  }

  /**
   * {@inheritDoc}
   */
  public void startLiveWindowMode() {
  }

  /**
   * {@inheritDoc}
   */
  public void stopLiveWindowMode() {
  }
}
