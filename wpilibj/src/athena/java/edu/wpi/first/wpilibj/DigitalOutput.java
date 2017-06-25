/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.hal.DIOJNI;
import edu.wpi.first.wpilibj.hal.FRCNetComm.tResourceType;
import edu.wpi.first.wpilibj.hal.HAL;
import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;
import edu.wpi.first.wpilibj.tables.ITable;
import edu.wpi.first.wpilibj.tables.ITableListener;

/**
 * Class to write digital outputs. This class will write digital outputs. Other devices that are
 * implemented elsewhere will automatically allocate digital inputs and outputs as required.
 */
public class DigitalOutput extends DigitalSource implements LiveWindowSendable {

  private static final int invalidPwmGenerator = 0;
  private int m_pwmGenerator = invalidPwmGenerator;

  private int m_channel = 0;
  private int m_handle = 0;

  /**
   * Create an instance of a digital output. Create an instance of a digital output given a
   * channel.
   *
   * @param channel the DIO channel to use for the digital output. 0-9 are on-board, 10-25 are on
   *                the MXP
   */
  public DigitalOutput(int channel) {
    checkDigitalChannel(channel);
    m_channel = channel;

    m_handle = DIOJNI.initializeDIOPort(DIOJNI.getPort((byte) channel), false);

    HAL.report(tResourceType.kResourceType_DigitalOutput, channel);
  }

  /**
   * Free the resources associated with a digital output.
   */
  @Override
  public void free() {
    // disable the pwm only if we have allocated it
    if (m_pwmGenerator != invalidPwmGenerator) {
      disablePWM();
    }
    DIOJNI.freeDIOPort(m_handle);
    m_handle = 0;
  }

  /**
   * Set the value of a digital output.
   *
   * @param value true is on, off is false
   */
  public void set(boolean value) {
    DIOJNI.setDIO(m_handle, (short) (value ? 1 : 0));
  }

  /**
   * Gets the value being output from the Digital Output.
   *
   * @return the state of the digital output.
   */
  public boolean get() {
    return DIOJNI.getDIO(m_handle);
  }

  /**
   * @return The GPIO channel number that this object represents.
   */
  @Override
  public int getChannel() {
    return m_channel;
  }

  /**
   * Generate a single pulse. There can only be a single pulse going at any time.
   *
   * @param pulseLength The length of the pulse.
   */
  public void pulse(final double pulseLength) {
    DIOJNI.pulse(m_handle, pulseLength);
  }

  /**
   * Determine if the pulse is still going. Determine if a previously started pulse is still going.
   *
   * @return true if pulsing
   */
  public boolean isPulsing() {
    return DIOJNI.isPulsing(m_handle);
  }

  /**
   * Change the PWM frequency of the PWM output on a Digital Output line.
   *
   * <p>The valid range is from 0.6 Hz to 19 kHz. The frequency resolution is logarithmic.
   *
   * <p>There is only one PWM frequency for all channnels.
   *
   * @param rate The frequency to output all digital output PWM signals.
   */
  public void setPWMRate(double rate) {
    DIOJNI.setDigitalPWMRate(rate);
  }

  /**
   * Enable a PWM Output on this line.
   *
   * <p>Allocate one of the 6 DO PWM generator resources.
   *
   * <p>Supply the initial duty-cycle to output so as to avoid a glitch when first starting.
   *
   * <p>The resolution of the duty cycle is 8-bit for low frequencies (1kHz or less) but is reduced
   * the higher the frequency of the PWM signal is.
   *
   * @param initialDutyCycle The duty-cycle to start generating. [0..1]
   */
  public void enablePWM(double initialDutyCycle) {
    if (m_pwmGenerator != invalidPwmGenerator) {
      return;
    }
    m_pwmGenerator = DIOJNI.allocateDigitalPWM();
    DIOJNI.setDigitalPWMDutyCycle(m_pwmGenerator, initialDutyCycle);
    DIOJNI.setDigitalPWMOutputChannel(m_pwmGenerator, m_channel);
  }

  /**
   * Change this line from a PWM output back to a static Digital Output line.
   *
   * <p>Free up one of the 6 DO PWM generator resources that were in use.
   */
  public void disablePWM() {
    if (m_pwmGenerator == invalidPwmGenerator) {
      return;
    }
    // Disable the output by routing to a dead bit.
    DIOJNI.setDigitalPWMOutputChannel(m_pwmGenerator, kDigitalChannels);
    DIOJNI.freeDigitalPWM(m_pwmGenerator);
    m_pwmGenerator = invalidPwmGenerator;
  }

  /**
   * Change the duty-cycle that is being generated on the line.
   *
   * <p>The resolution of the duty cycle is 8-bit for low frequencies (1kHz or less) but is reduced
   * the
   * higher the frequency of the PWM signal is.
   *
   * @param dutyCycle The duty-cycle to change to. [0..1]
   */
  public void updateDutyCycle(double dutyCycle) {
    if (m_pwmGenerator == invalidPwmGenerator) {
      return;
    }
    DIOJNI.setDigitalPWMDutyCycle(m_pwmGenerator, dutyCycle);
  }

  /**
   * Get the analog trigger type.
   *
   * @return false
   */
  @Override
  public int getAnalogTriggerTypeForRouting() {
    return 0;
  }

  /**
   * Is this an analog trigger.
   *
   * @return true if this is an analog trigger
   */
  @Override
  public boolean isAnalogTrigger() {
    return false;
  }

  /**
   * Get the HAL Port Handle.
   *
   * @return The HAL Handle to the specified source.
   */
  @Override
  public int getPortHandleForRouting() {
    return m_handle;
  }

  /*
   * Live Window code, only does anything if live window is activated.
   */
  @Override
  public String getSmartDashboardType() {
    return "Digital Output";
  }

  private ITable m_table;
  private ITableListener m_tableListener;


  @Override
  public void initTable(ITable subtable) {
    m_table = subtable;
    updateTable();
  }


  @Override
  public ITable getTable() {
    return m_table;
  }


  @Override
  public void updateTable() {
    // TODO: Put current value.
  }


  @Override
  public void startLiveWindowMode() {
    m_tableListener = (source, key, value, isNew) -> set((boolean) value);
    m_table.addTableListener("Value", m_tableListener, true);
  }


  @Override
  public void stopLiveWindowMode() {
    // TODO: Broken, should only remove the listener from "Value" only.
    m_table.removeTableListener(m_tableListener);
  }
}
