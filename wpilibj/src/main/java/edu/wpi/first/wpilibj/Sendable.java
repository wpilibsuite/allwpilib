// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;
import edu.wpi.first.wpilibj.smartdashboard.SendableRegistry;

/** The base interface for objects that can be sent over the network through network tables. */
public interface Sendable {
  /**
   * Gets the name of this {@link Sendable} object.
   *
   * @return Name
   * @deprecated Use SendableRegistry.getName()
   */
  @Deprecated(since = "2020", forRemoval = true)
  default String getName() {
    return SendableRegistry.getName(this);
  }

  /**
   * Sets the name of this {@link Sendable} object.
   *
   * @param name name
   * @deprecated Use SendableRegistry.setName()
   */
  @Deprecated(since = "2020", forRemoval = true)
  default void setName(String name) {
    SendableRegistry.setName(this, name);
  }

  /**
   * Sets both the subsystem name and device name of this {@link Sendable} object.
   *
   * @param subsystem subsystem name
   * @param name device name
   * @deprecated Use SendableRegistry.setName()
   */
  @Deprecated(since = "2020", forRemoval = true)
  default void setName(String subsystem, String name) {
    SendableRegistry.setName(this, subsystem, name);
  }

  /**
   * Sets the name of the sensor with a channel number.
   *
   * @param moduleType A string that defines the module name in the label for the value
   * @param channel The channel number the device is plugged into
   * @deprecated Use SendableRegistry.setName()
   */
  @Deprecated(since = "2020", forRemoval = true)
  default void setName(String moduleType, int channel) {
    SendableRegistry.setName(this, moduleType, channel);
  }

  /**
   * Sets the name of the sensor with a module and channel number.
   *
   * @param moduleType A string that defines the module name in the label for the value
   * @param moduleNumber The number of the particular module type
   * @param channel The channel number the device is plugged into (usually PWM)
   * @deprecated Use SendableRegistry.setName()
   */
  @Deprecated(since = "2020", forRemoval = true)
  default void setName(String moduleType, int moduleNumber, int channel) {
    SendableRegistry.setName(this, moduleType, moduleNumber, channel);
  }

  /**
   * Gets the subsystem name of this {@link Sendable} object.
   *
   * @return Subsystem name
   * @deprecated Use SendableRegistry.getSubsystem()
   */
  @Deprecated(since = "2020", forRemoval = true)
  default String getSubsystem() {
    return SendableRegistry.getSubsystem(this);
  }

  /**
   * Sets the subsystem name of this {@link Sendable} object.
   *
   * @param subsystem subsystem name
   * @deprecated Use SendableRegistry.setSubsystem()
   */
  @Deprecated(since = "2020", forRemoval = true)
  default void setSubsystem(String subsystem) {
    SendableRegistry.setSubsystem(this, subsystem);
  }

  /**
   * Add a child component.
   *
   * @param child child component
   * @deprecated Use SendableRegistry.addChild()
   */
  @Deprecated(since = "2020", forRemoval = true)
  default void addChild(Object child) {
    SendableRegistry.addChild(this, child);
  }

  /**
   * Initializes this {@link Sendable} object.
   *
   * @param builder sendable builder
   */
  void initSendable(SendableBuilder builder);
}
