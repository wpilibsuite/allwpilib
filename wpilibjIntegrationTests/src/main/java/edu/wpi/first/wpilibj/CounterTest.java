// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.wpilibj.fixtures.FakeCounterFixture;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import edu.wpi.first.wpilibj.test.TestBench;
import java.util.Collection;
import java.util.logging.Logger;
import org.junit.jupiter.api.AfterAll;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Timeout;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.MethodSource;

/** Tests to see if the Counter is working properly. */
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
   * @param input The input Port
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
  public static Collection<Integer[]> generateData() {
    // In this example, the parameter generator returns a List of
    // arrays. Each array has two elements: { Digital input port, Digital output
    // port}.
    // These data are hard-coded into the class, but they could be
    // generated or loaded in any way you like.
    return TestBench.getDIOCrossConnectCollection();
  }

  @BeforeAll
  public static void setUpBeforeAll() {}

  @AfterAll
  public static void tearDownAfterAll() {
    counter.teardown();
    counter = null;
  }

  @BeforeEach
  public void setUp() {
    counter.setup();
  }

  /** Tests the default state of the counter immediately after reset. */
  @ParameterizedTest
  @MethodSource("generateData")
  public void testDefault() {
    assertEquals(0, counter.getCounter().get(), "Counter did not reset to 0");
  }

  @Timeout(5)
  @ParameterizedTest
  @MethodSource("generateData")
  public void testCount() {
    final int goal = 100;
    counter.getFakeCounterSource().setCount(goal);
    counter.getFakeCounterSource().execute();

    final int count = counter.getCounter().get();

    assertTrue(
        count == goal,
        "Fake Counter, Input: "
            + m_input
            + ", Output: "
            + m_output
            + ", did not return "
            + goal
            + " instead got: "
            + count);
  }
}
