/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.fixtures;


/**
 * This is an example of how to use the {@link ITestFixture} interface to create test fixtures for a
 * test.
 */
public class SampleFixture implements ITestFixture {

  @Override
  public boolean setup() {
    /*
     * If this fixture actually accessed the hardware, here is where it would
     * set up the starting state of the test bench. For example, reseting
     * encoders, ensuring motors are stopped, reseting any serial communication
     * if necessary, etc.
     */
    return true;
  }

  @Override
  public boolean reset() {
    /*
     * This is where the fixture would reset any sensors or motors used by the
     * fixture to test default state. This method should not worry about whether
     * or not the sensors have been allocated correctly, that is the job of the
     * setup function.
     */
    return false;
  }

  @Override
  public boolean teardown() {
    /*
     * This is where the fixture would deallocate and reset back to normal
     * conditions any necessary hardware. This includes ensuring motors are
     * stopped, stoppable sensors are actually stopped, ensuring serial
     * communications are ready for the next test, etc.
     */
    return true;
  }
}
