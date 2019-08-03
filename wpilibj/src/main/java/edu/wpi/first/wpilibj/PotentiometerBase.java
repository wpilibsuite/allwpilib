/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.interfaces.Potentiometer;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;

/**
 * Interface for a Potentiometer that can be displayed on a dashboard.
 */
public abstract class PotentiometerBase extends SendableBase implements Potentiometer {
  @Override
  public abstract void initSendable(SendableBuilder builder);
}
