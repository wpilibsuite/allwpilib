/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import java.util.logging.Logger;

import edu.wpi.first.wpilibj.fixtures.SampleFixture;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;

import static org.junit.Assert.assertTrue;

/**
 * Sample test for a sample PID controller. This demonstrates the general pattern of how to create a
 * test and use testing fixtures and categories. All tests must extend from {@link
 * AbstractComsSetup} in order to ensure that Network Communications are set up before the tests are
 * run.
 */
public class SampleTest extends AbstractComsSetup {
  private static final Logger logger = Logger.getLogger(SampleTest.class.getName());

  static SampleFixture fixture = new SampleFixture();

  protected Logger getClassLogger() {
    return logger;
  }

  @BeforeClass
  public static void classSetup() {
    // Set up the fixture before the test is created
    fixture.setup();
  }

  @Before
  public void setUp() {
    // Reset the fixture elements before every test
    fixture.reset();
  }

  @AfterClass
  public static void tearDown() {
    // Clean up the fixture after the test
    fixture.teardown();
  }

  /**
   * This is just a sample test that asserts true. Any traditional junit code can be used, these are
   * ordinary junit tests!
   */
  @Test
  public void test() {
    Timer.delay(0.5);
    assertTrue(true);
  }

}
