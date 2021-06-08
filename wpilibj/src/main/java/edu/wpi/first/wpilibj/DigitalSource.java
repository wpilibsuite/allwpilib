// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

/**
 * DigitalSource Interface. The DigitalSource represents all the possible inputs for a counter or a
 * quadrature encoder. The source may be either a digital input or an analog input. If the caller
 * just provides a channel, then a digital input will be constructed and freed when finished for the
 * source. The source can either be a digital input or analog trigger but not both.
 */
public abstract class DigitalSource implements AutoCloseable {
  public abstract boolean isAnalogTrigger();

  public abstract int getChannel();

  /**
   * If this is an analog trigger.
   *
   * @return true if this is an analog trigger.
   */
  public abstract int getAnalogTriggerTypeForRouting();

  /**
   * The channel routing number.
   *
   * @return channel routing number
   */
  public abstract int getPortHandleForRouting();

  @Override
  public void close() {}
}
