/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.ctrlsys;

import edu.wpi.first.wpilibj.PIDSource;

/**
 * INode adapter for PIDSource subclasses.
 *
 * <p>Wraps a PIDSource object in the INode interface by returning the output of PIDGet() from
 * getOutput().
 */
public class Sensor implements INode {
  private PIDSource m_source;

  public Sensor(PIDSource source) {
    m_source = source;
  }

  @Override
  public double getOutput() {
    return m_source.pidGet();
  }
}
