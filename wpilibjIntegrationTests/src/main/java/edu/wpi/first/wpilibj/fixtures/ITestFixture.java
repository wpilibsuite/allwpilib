// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.fixtures;

import edu.wpi.first.wpilibj.test.TestBench;

/**
 * Common interface for all test fixtures. This ensures that all test fixtures have setup and
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
   */
  void setup();

  /**
   * Resets the fixture back to test start state. This should be called by the test class in the
   * test setup method to ensure that the hardware is in the default state. This differs from {@link
   * ITestFixture#setup()} as that is called once, before the class is constructed, so it may need
   * to start sensors. This method should not have to start anything, just reset sensors and ensure
   * that motors are stopped.
   */
  void reset();

  /**
   * Performs any required teardown after use of the fixture, ensuring that future tests will not
   * run into conflicts.
   */
  void teardown();
}
