/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.ctrlsys;

import java.util.function.DoubleSupplier;

/**
 * Converts a callable into a control system node whose output is the callable's return value.
 */
public class FuncNode implements INode {
  private DoubleSupplier m_func;

  public FuncNode(DoubleSupplier func) {
    m_func = func;
  }

  @Override
  public double getOutput() {
    return m_func.getAsDouble();
  }
}
