/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj;

import static org.junit.Assert.assertTrue;

import java.util.Arrays;
import java.util.Collection;
import java.util.logging.Logger;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import edu.wpi.first.wpilibj.fixtures.FilterNoiseFixture;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import edu.wpi.first.wpilibj.test.TestBench;


@RunWith(Parameterized.class)
public class FilterNoiseTest extends AbstractComsSetup {
  private static final Logger logger = Logger.getLogger(FilterNoiseTest.class.getName());

  private static FilterNoiseFixture<?> me = null;

  @Override
  protected Logger getClassLogger() {
    return logger;
  }

  public FilterNoiseTest(FilterNoiseFixture<?> mef) {
    logger.fine("Constructor with: " + mef.getType());
    if (me != null && !me.equals(mef))
      me.teardown();
    me = mef;
  }

  @Parameters(name = "{index}: {0}")
  public static Collection<FilterNoiseFixture<?>[]> generateData() {
    return Arrays.asList(new FilterNoiseFixture<?>[][] {
                         {TestBench.getInstance().getSinglePoleIIRNoiseFixture()},
                         {TestBench.getInstance().getMovAvgNoiseFixture()}});
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
   * Test if the filter reduces the noise produced by a signal generator
   */
  @Test
  public void testNoiseReduce() {
    double theoryData = 0.0;
    double noiseGenError = 0.0;
    double filterError = 0.0;

    FilterNoiseFixture.NoiseGenerator noise = me.getNoiseGenerator();

    noise.reset();
    for (double t = 0; t < TestBench.kFilterTime; t += TestBench.kFilterStep) {
      theoryData = noise.getData(t);
      filterError += Math.abs(me.getFilter().pidGet() - theoryData);
      noiseGenError += Math.abs(noise.get() - theoryData);
    }

    assertTrue(me.getType() + " should have reduced noise accumulation from " + noiseGenError + " but failed. The filter error was " + filterError, noiseGenError > filterError);
  }
}
