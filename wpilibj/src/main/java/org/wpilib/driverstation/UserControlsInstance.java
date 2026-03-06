// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.driverstation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * An annotation to specify the UserControls implementation class to be used for a robot. Apply this
 * annotation to your main robot class, providing a class that implements the UserControls
 * interface.
 */
@Target(ElementType.TYPE)
@Retention(RetentionPolicy.RUNTIME)
public @interface UserControlsInstance {
  /**
   * The UserControls implementation class to be used.
   *
   * @return The class that implements UserControls.
   */
  Class<? extends UserControls> value();
}
