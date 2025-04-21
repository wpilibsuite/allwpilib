// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

import edu.wpi.first.wpilibj.fixtures.FakeCounterFixture;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import edu.wpi.first.wpilibj.test.TestBench;
import java.util.Collection;
import java.util.logging.Logger;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

/** Tests to see if the Counter is working properly. */
@RunWith(Parameterized.class)
public class CounterTest extends AbstractComsSetup {
  private static FakeCounterFixture counter = null;
  private static final Logger logger = Logger.getLogger(CounterTest.class.getName());

  int m_input;
  int m_output;

  @Override
  protected Logger getClassLogger() {
    return logger;
  }

  /**
   * Constructs a Counter Test with the given inputs.
   *
   * @param input The input Port
   * @param output The output Port
   */
  public CounterTest(int input, int output) {
    m_input = input;
    m_output = output;
    // System.out.println("Counter Test: Input: " + input + " Output: " +
    // output);
    if (counter != null) {
      counter.teardown();
    }
    counter = new FakeCounterFixture(input, output);
  }

  /**
   * Test data generator. This method is called the the JUnit parameterized test runner and returns
   * a Collection of Arrays. For each Array in the Collection, each array element corresponds to a
   * parameter in the constructor.
   */
  @Parameters
  public static Collection<Integer[]> generateData() {
    // In this example, the parameter generator returns a List of
    // arrays. Each array has two elements: { Digital input port, Digital output
    // port}.
    // These data are hard-coded into the class, but they could be
    // generated or loaded in any way you like.
    return TestBench.getDIOCrossConnectCollection();
  }

  @BeforeClass
  public static void setUpBeforeClass() {}

  @AfterClass
  public static void tearDownAfterClass() {
    counter.teardown();
    counter = null;
  }

  @Before
  public void setUp() {
    counter.setup();
  }

  /** Tests the default state of the counter immediately after reset. */
  @Test
  public void testDefault() {
    assertEquals("Counter did not reset to 0", 0, counter.getCounter().get());
  }

  @Test(timeout = 5000)
  public void testCount() {
    final int goal = 100;
    counter.getFakeCounterSource().setCount(goal);
    counter.getFakeCounterSource().execute();

    final int count = counter.getCounter().get();

    assertTrue(
        "Fake Counter, Input: "
            + m_input
            + ", Output: "
            + m_output
            + ", did not return "
            + goal
            + " instead got: "
            + count,
        count == goal);
  }
}
