/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

/**
 * Callback for handling interrupts from the inputs.
 */
@FunctionalInterface
public interface InterruptHandler {
  /**
   * Called when an interrupt occurs.
   *
   * @param rising true if rising edge
   * @param falling true if falling edge
   */
  void onInterrupt(boolean rising, boolean falling);
}
