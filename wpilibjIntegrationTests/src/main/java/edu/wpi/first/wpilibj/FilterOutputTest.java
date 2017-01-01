/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import java.util.Arrays;
import java.util.Collection;
import java.util.logging.Logger;

import edu.wpi.first.wpilibj.fixtures.FilterOutputFixture;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import edu.wpi.first.wpilibj.test.TestBench;

import static org.junit.Assert.assertEquals;


@RunWith(Parameterized.class)
public class FilterOutputTest extends AbstractComsSetup {
  private static final Logger logger = Logger.getLogger(FilterOutputTest.class.getName());

  private double m_expectedOutput;

  private static FilterOutputFixture<?> me = null;

  @Override
  protected Logger getClassLogger() {
    return logger;
  }

  /**
   * Constructs a filter output test.
   *
   * @param mef The fixture under test.
   */
  public FilterOutputTest(FilterOutputFixture<?> mef) {
    logger.fine("Constructor with: " + mef.getType());
    if (me != null && !me.equals(mef)) {
      me.teardown();
    }
    me = mef;
    m_expectedOutput = me.getExpectedOutput();
  }

  @Parameters(name = "{index}: {0}")
  public static Collection<FilterOutputFixture<?>[]> generateData() {
    return Arrays.asList(new FilterOutputFixture<?>[][]{
        {TestBench.getInstance().getSinglePoleIIROutputFixture()},
        {TestBench.getInstance().getHighPassOutputFixture()},
        {TestBench.getInstance().getMovAvgOutputFixture()}});
  }

  @Before
  public void setUp() {
    me.setup();
  }

  @After
  public void tearDown() throws Exception {
    me.reset();
  }

  @AfterClass
  public static void tearDownAfterClass() {
    // Clean up the fixture after the test
    me.teardown();
    me = null;
  }

  /**
   * Test if the filter produces consistent output for a given data set.
   */
  @Test
  public void testOutput() {
    me.reset();

    double filterOutput = 0.0;
    for (double t = 0.0; t < TestBench.kFilterTime; t += TestBench.kFilterStep) {
      filterOutput = me.getFilter().pidGet();
    }

    assertEquals(me.getType() + " output was incorrect.", m_expectedOutput, filterOutput, 0.00005);
  }
}
