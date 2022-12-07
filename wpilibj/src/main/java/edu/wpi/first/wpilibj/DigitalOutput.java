// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.DIOJNI;
import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.SimDevice;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;

/**
 * Class to write digital outputs. This class will write digital outputs. Other devices that are
 * implemented elsewhere will automatically allocate digital inputs and outputs as required.
 */
public class DigitalOutput extends DigitalSource implements Sendable {
  private static final int invalidPwmGenerator = 0;
  private int m_pwmGenerator = invalidPwmGenerator;

  private final int m_channel;
  private int m_handle;

  /**
   * Create an instance of a digital output. Create an instance of a digital output given a channel.
   *
   * @param channel the DIO channel to use for the digital output. 0-9 are on-board, 10-25 are on
   *     the MXP
   */
  public DigitalOutput(int channel) {
    SensorUtil.checkDigitalChannel(channel);
    m_channel = channel;

    m_handle = DIOJNI.initializeDIOPort(HAL.getPort((byte) channel), false);

    HAL.report(tResourceType.kResourceType_DigitalOutput, channel + 1);
    SendableRegistry.addLW(this, "DigitalOutput", channel);
  }

  @Override
  public void close() {
    super.close();
    SendableRegistry.remove(this);
    // Disable the pwm only if we have allocated it
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
    DIOJNI.setDIO(m_handle, value);
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
   * Get the GPIO channel number that this object represents.
   *
   * @return The GPIO channel number.
   */
  @Override
  public int getChannel() {
    return m_channel;
  }

  /**
   * Output a single pulse on the digital output line.
   *
   * <p>Send a single pulse on the digital output line where the pulse duration is specified in
   * seconds. Maximum of 65535 microseconds.
   *
   * @param pulseLengthSeconds The pulse length in seconds
   */
  public void pulse(final double pulseLengthSeconds) {
    DIOJNI.pulse(m_handle, pulseLengthSeconds);
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
   * <p>There is only one PWM frequency for all channels.
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
    DIOJNI.setDigitalPWMOutputChannel(m_pwmGenerator, SensorUtil.kDigitalChannels);
    DIOJNI.freeDigitalPWM(m_pwmGenerator);
    m_pwmGenerator = invalidPwmGenerator;
  }

  /**
   * Change the duty-cycle that is being generated on the line.
   *
   * <p>The resolution of the duty cycle is 8-bit for low frequencies (1kHz or less) but is reduced
   * the higher the frequency of the PWM signal is.
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
   * Indicates this input is used by a simulated device.
   *
   * @param device simulated device handle
   */
  public void setSimDevice(SimDevice device) {
    DIOJNI.setDIOSimDevice(m_handle, device.getNativeHandle());
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Digital Output");
    builder.addBooleanProperty("Value", this::get, this::set);
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
   * Get the analog trigger type.
   *
   * @return false
   */
  @Override
  public int getAnalogTriggerTypeForRouting() {
    return 0;
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
}
