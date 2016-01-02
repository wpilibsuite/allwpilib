/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.interfaces;

import edu.wpi.first.wpilibj.PIDSource;

/**
 *
 * @author alex
 */
public interface Potentiometer extends PIDSource {
  double get();
}
