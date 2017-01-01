/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.fixtures;

import edu.wpi.first.wpilibj.test.TestBench;

/**
 * Master interface for all test fixtures. This ensures that all test fixtures have setup and
 * teardown methods, to ensure that the tests run properly. Test fixtures should be modeled around
 * the content of a test, rather than the actual physical layout of the testing board. They should
 * obtain references to hardware from the {@link TestBench} class, which is a singleton. Testing
 * Fixtures are responsible for ensuring that the hardware is in an appropriate state for the start
 * of a test, and ensuring that future tests will not be affected by the results of a test.
 */
public interface ITestFixture {

  /**
   * Performs any required setup for this fixture, ensuring that all fixture elements are ready for
   * testing.
   *
   * @return True if the fixture is ready for testing
   */
  boolean setup();

  /**
   * Resets the fixture back to test start state. This should be called by the test class in the
   * test setup method to ensure that the hardware is in the default state. This differs from {@link
   * ITestFixture#setup()} as that is called once, before the class is constructed, so it may need
   * to start sensors. This method should not have to start anything, just reset sensors and ensure
   * that motors are stopped.
   *
   * @return True if the fixture is ready for testing
   */
  boolean reset();

  /**
   * Performs any required teardown after use of the fixture, ensuring that future tests will not
   * run into conflicts.
   *
   * @return True if the teardown succeeded
   */
  boolean teardown();
}
