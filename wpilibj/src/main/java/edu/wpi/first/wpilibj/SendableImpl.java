/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.livewindow.LiveWindow;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;

/**
 * The base interface for objects that can be sent over the network through network tables.
 */
public class SendableImpl implements Sendable, AutoCloseable {
  private String m_name = "";
  private String m_subsystem = "Ungrouped";

  /**
   * Creates an instance of the sensor base.
   */
  public SendableImpl() {
    this(true);
  }

  /**
   * Creates an instance of the sensor base.
   *
   * @param addLiveWindow if true, add this Sendable to LiveWindow
   */
  public SendableImpl(boolean addLiveWindow) {
    if (addLiveWindow) {
      LiveWindow.add(this);
    }
  }

  @Deprecated
  public void free() {
    close();
  }

  @Override
  public void close() {
    LiveWindow.remove(this);
  }

  @Override
  public synchronized String getName() {
    return m_name;
  }

  @Override
  public synchronized void setName(String name) {
    m_name = name;
  }

  /**
   * Sets the name of the sensor with a channel number.
   *
   * @param moduleType A string that defines the module name in the label for the value
   * @param channel    The channel number the device is plugged into
   */
  public void setName(String moduleType, int channel) {
    setName(moduleType + "[" + channel + "]");
  }

  /**
   * Sets the name of the sensor with a module and channel number.
   *
   * @param moduleType   A string that defines the module name in the label for the value
   * @param moduleNumber The number of the particular module type
   * @param channel      The channel number the device is plugged into (usually PWM)
   */
  public void setName(String moduleType, int moduleNumber, int channel) {
    setName(moduleType + "[" + moduleNumber + "," + channel + "]");
  }

  @Override
  public synchronized String getSubsystem() {
    return m_subsystem;
  }

  @Override
  public synchronized void setSubsystem(String subsystem) {
    m_subsystem = subsystem;
  }

  @Override
  public void initSendable(SendableBuilder builder) {
  }

  /**
   * Add a child component.
   *
   * @param child child component
   */
  public void addChild(Object child) {
    LiveWindow.addChild(this, child);
  }
}
