/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.ctrlsys;

/**
 * Interface for control system diagram node.
 *
 * <p>Common interface for control system diagram nodes. Nodes consist of some operation upon an
 * input such as integration, differentiation, multipling by a constant, or summing multiple inputs
 * togeether.
 *
 * <p>Subclasses should take at least one input node in their constructor to be used in getOutput().
 */
public interface INode {
  /**
   * The default period used in control loops in seconds.
   */
  double DEFAULT_PERIOD = 0.05;

  /**
   * Get input node.
   *
   * <p>Returns null if there is no input node.
   */
  default INode getInputNode() {
    return null;
  }

  /**
   * Set callback function.
   *
   * <p>Nodes with no input should override this to store the function Output.outputFunc(), then
   * call it after the node's value changes.
   */
  default void setCallback(Output output) {
    if (getInputNode() != null) {
      getInputNode().setCallback(output);
    }
  }

  /**
   * Performs an operation on the input node's output and returns the result.
   */
  double getOutput();
}
