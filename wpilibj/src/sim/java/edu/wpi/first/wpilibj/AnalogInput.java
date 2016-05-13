/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.livewindow.LiveWindow;
import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;
import edu.wpi.first.wpilibj.simulation.SimFloatInput;
import edu.wpi.first.wpilibj.tables.ITable;

/**
 * Analog channel class.
 *
 * Each analog channel is read from hardware as a 12-bit number representing -10V to 10V.
 *
 * Connected to each analog channel is an averaging and oversampling engine. This engine accumulates
 * the specified ( by setAverageBits() and setOversampleBits() ) number of samples before returning
 * a new value. This is not a sliding window average. The only difference between the oversampled
 * samples and the averaged samples is that the oversampled samples are simply accumulated
 * effectively increasing the resolution, while the averaged samples are divided by the number of
 * samples to retain the resolution, but get more stable values.
 */
public class AnalogInput extends SensorBase implements PIDSource,
    LiveWindowSendable {

  private SimFloatInput m_impl;
  private int m_channel;
  protected PIDSourceType m_pidSource = PIDSourceType.kDisplacement;

  /**
   * Construct an analog channel.
   *
   * @param channel The channel number to represent.
   */
  public AnalogInput(final int channel) {
    m_channel = channel;
    m_impl = new SimFloatInput("simulator/analog/" + channel);

    LiveWindow.addSensor("AnalogInput", channel, this);
  }

  /**
   * Channel destructor.
   */
  public void free() {
    m_channel = 0;
  }

  /**
   * Get a scaled sample straight from this channel on the module. The value is scaled to units of
   * Volts using the calibrated scaling data from getLSBWeight() and getOffset().
   *
   * @return A scaled sample straight from this channel on the module.
   */
  public double getVoltage() {
    return m_impl.get();
  }

  /**
   * Get a scaled sample from the output of the oversample and average engine for this channel. The
   * value is scaled to units of Volts using the calibrated scaling data from getLSBWeight() and
   * getOffset(). Using oversampling will cause this value to be higher resolution, but it will
   * update more slowly. Using averaging will cause this value to be more stable, but it will update
   * more slowly.
   *
   * @return A scaled sample from the output of the oversample and average engine for this channel.
   */
  public double getAverageVoltage() {
    return getVoltage();
  }

  /**
   * Get the channel number.
   *
   * @return The channel number.
   */
  public int getChannel() {
    return m_channel;
  }

  /**
   * {@inheritDoc}
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
   * Get the average value for use with PIDController
   *
   * @return the average value
   */
  public double pidGet() {
    return getAverageVoltage();
  }

  /**
   * Live Window code, only does anything if live window is activated.
   */
  public String getSmartDashboardType() {
    return "Analog Input";
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
  public void updateTable() {
    if (m_table != null) {
      m_table.putNumber("Value", getAverageVoltage());
    }
  }

  /**
   * {@inheritDoc}
   */
  public ITable getTable() {
    return m_table;
  }

  /**
   * Analog Channels don't have to do anything special when entering the LiveWindow. {@inheritDoc}
   */
  public void startLiveWindowMode() {
  }

  /**
   * Analog Channels don't have to do anything special when exiting the LiveWindow. {@inheritDoc}
   */
  public void stopLiveWindowMode() {
  }
}
