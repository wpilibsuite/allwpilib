/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import org.gazebosim.transport.Msgs;
import org.gazebosim.transport.Publisher;

import gazebo.msgs.GzFloat64.Float64;


public class SimSpeedController {
  private Publisher<Float64> m_pub;
  private double m_speed;

  /**
   * Constructor that assumes the default digital module.
   *
   * @param topic The topic to publish the to.
   */
  public SimSpeedController(String topic) {
    m_pub = MainNode.advertise(topic, Msgs.Float64());
  }

  /**
   * Set the PWM value.
   *
   * <p>he PWM value is set using a range of -1.0 to 1.0, appropriately scaling the value for the
   * FPGA.
   *
   * @param speed     The speed to set.  Value should be between -1.0 and 1.0.
   * @param syncGroup The update group to add this Set() to, pending UpdateSyncGroup().  If 0,
   *                  update immediately.
   * @deprecated For compatibility with CANJaguar
   */
  public void set(double speed, byte syncGroup) {
    set(speed);
  }

  /**
   * Set the PWM value.
   *
   * <p>The PWM value is set using a range of -1.0 to 1.0, appropriately scaling the value for the
   * FPGA.
   *
   * @param speed The speed value between -1.0 and 1.0 to set.
   */
  public void set(double speed) {
    m_pub.publish(Msgs.Float64(speed));
  }

  /**
   * Get the recently set value of the PWM.
   *
   * @return The most recently set value for the PWM between -1.0 and 1.0.
   */
  public double get() {
    return m_speed;
  }

  /**
   * Disable the speed controller.
   */
  public void disable() {
    set(0);
  }

  /**
   * Write out the PID value as seen in the PIDOutput base object.
   *
   * @param output Write out the PWM value as was found in the PIDController
   */
  public void pidWrite(double output) {
    set(output);
  }
}
