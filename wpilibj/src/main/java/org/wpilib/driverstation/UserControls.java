// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.driverstation;

/**
 * An interface representing user controls such as gamepads or joysticks. If your main robot class
 * has a UserControlsInstance attribute with a class implementing this interface, the constructor is
 * able to receive an instance of that class. Additionally, any OpModes can also receive that same
 * instance.
 *
 * <p>The implementation of this class must have a default constructor
 */
public interface UserControls {}
