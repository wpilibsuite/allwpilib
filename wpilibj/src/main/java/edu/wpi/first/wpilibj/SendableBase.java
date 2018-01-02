/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.livewindow.LiveWindow;

/**
 * Base class for all sensors. Stores most recent status information as well as containing utility
 * functions for checking channels and error processing.
 */
public abstract class SendableBase implements Sendable {
  private String m_name = "";
  private String m_subsystem = "Ungrouped";

  /**
   * Creates an instance of the sensor base.
   */
  public SendableBase() {
    this(true);
  }

  /**
   * Creates an instance of the sensor base.
   *
   * @param addLiveWindow if true, add this Sendable to LiveWindow
   */
  public SendableBase(boolean addLiveWindow) {
    if (addLiveWindow) {
      LiveWindow.add(this);
    }
  }

  /**
   * Free the resources used by this object.
   */
  public void free() {
    LiveWindow.remove(this);
  }

  @Override
  public final synchronized String getName() {
    return m_name;
  }

  @Override
  public final synchronized void setName(String name) {
    m_name = name;
  }

  /**
   * Sets the name of the sensor with a channel number.
   *
   * @param moduleType A string that defines the module name in the label for the value
   * @param channel    The channel number the device is plugged into
   */
  protected final void setName(String moduleType, int channel) {
    setName(moduleType + "[" + channel + "]");
  }

  /**
   * Sets the name of the sensor with a module and channel number.
   *
   * @param moduleType   A string that defines the module name in the label for the value
   * @param moduleNumber The number of the particular module type
   * @param channel      The channel number the device is plugged into (usually PWM)
   */
  protected final void setName(String moduleType, int moduleNumber, int channel) {
    setName(moduleType + "[" + moduleNumber + "," + channel + "]");
  }

  @Override
  public final synchronized String getSubsystem() {
    return m_subsystem;
  }

  @Override
  public final synchronized void setSubsystem(String subsystem) {
    m_subsystem = subsystem;
  }

  /**
   * Add a child component.
   *
   * @param child child component
   */
  protected final void addChild(Object child) {
    LiveWindow.addChild(this, child);
  }
}
