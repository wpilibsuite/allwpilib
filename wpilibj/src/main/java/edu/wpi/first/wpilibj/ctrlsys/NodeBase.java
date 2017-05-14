/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.ctrlsys;

/**
 * Provides a common base class for nodes with one input.
 */
public abstract class NodeBase implements INode {
  private final INode m_input;

  public NodeBase(INode input) {
    m_input = input;
  }

  @Override
  public INode getInputNode() {
    return m_input;
  }

  @Override
  public double getOutput() {
    return m_input.getOutput();
  }
}
