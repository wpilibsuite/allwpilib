/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

import edu.wpi.first.networktables.NetworkTableEntry;

// Used to make sure all the entry builders have a consistent interface for getting the entry
public interface TerminalEntryBuilder {

  NetworkTableEntry getEntry();

}
