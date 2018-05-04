/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

import java.util.Map;

public interface Configurable {

  /**
   * Sets the properties of this object. Capitalization and whitespace in property names are
   * ignored.
   *
   * @param properties the properties to configure
   *
   * @return this object
   */
  Configurable withProperties(Map<String, Object> properties);

}
