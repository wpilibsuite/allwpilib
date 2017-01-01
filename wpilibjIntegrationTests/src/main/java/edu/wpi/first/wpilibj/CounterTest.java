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
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import java.util.Collection;
import java.util.logging.Logger;

import edu.wpi.first.wpilibj.fixtures.FakeCounterFixture;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import edu.wpi.first.wpilibj.test.TestBench;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

/**
 * Tests to see if the Counter is working properly.
 */
@RunWith(Parameterized.class)
public class CounterTest extends AbstractComsSetup {
  private static FakeCounterFixture counter = null;
  private static final Logger logger = Logger.getLogger(CounterTest.class.getName());

  Integer m_input;
  Integer m_output;

  @Override
  protected Logger getClassLogger() {
    return logger;
  }

  /**
   * Constructs a Counter Test with the given inputs.
   *
   * @param input  The input Port
   * @param output The output Port
   */
  public CounterTest(Integer input, Integer output) {
    assert input != null;
    assert output != null;

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
    return TestBench.getInstance().getDIOCrossConnectCollection();
  }


  @BeforeClass
  public static void setUpBeforeClass() throws Exception {
  }

  @AfterClass
  public static void tearDownAfterClass() throws Exception {
    counter.teardown();
    counter = null;
  }

  @Before
  public void setUp() throws Exception {
    counter.setup();
  }

  /**
   * Tests the default state of the counter immediately after reset.
   */
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

    assertTrue("Fake Counter, Input: " + m_input + ", Output: " + m_output + ", did not return "
        + goal + " instead got: " + count, count == goal);
  }

}
