/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import static org.hamcrest.Matchers.greaterThan;
import static org.hamcrest.Matchers.is;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertThat;

import java.util.Arrays;
import java.util.Collection;
import java.util.logging.Logger;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import edu.wpi.first.wpilibj.can.CANMessageNotFoundException;
import edu.wpi.first.wpilibj.fixtures.MotorEncoderFixture;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import edu.wpi.first.wpilibj.test.TestBench;

@RunWith(Parameterized.class)
public class PDPTest extends AbstractComsSetup {
  private static final Logger logger = Logger.getLogger(PCMTest.class.getName());

  private static PowerDistributionPanel pdp;
  private static MotorEncoderFixture<?> me;
  private final double expectedStoppedCurrentDraw;

  @BeforeClass
  public static void setUpBeforeClass() throws Exception {
    pdp = new PowerDistributionPanel();
  }

  @AfterClass
  public static void tearDownAfterClass() throws Exception {
    pdp.free();
    pdp = null;
    me.teardown();
    me = null;
  }


  public PDPTest(MotorEncoderFixture<?> mef, Double expectedCurrentDraw) {
    logger.fine("Constructor with: " + mef.getType());
    if (me != null && !me.equals(mef))
      me.teardown();
    me = mef;
    me.setup();

    this.expectedStoppedCurrentDraw = expectedCurrentDraw;
  }

  @Parameters(name = "{index}: {0}, Expected Stopped Current Draw: {1}")
  public static Collection<Object[]> generateData() {
    // logger.fine("Loading the MotorList");
    return Arrays.asList(new Object[][] {
        {TestBench.getInstance().getTalonPair(), new Double(0.0)},
        {TestBench.getInstance().getVictorPair(), new Double(0.0)},
        {TestBench.getInstance().getJaguarPair(), new Double(0.0)}});
  }

  @After
  public void tearDown() throws Exception {
    me.reset();
  }



  /**
   * Test if the current changes when the motor is driven using a talon
   */
  @Test
  public void CheckStoppedCurrentForSpeedController() throws CANMessageNotFoundException {
    Timer.delay(0.25);

    /* The Current should be 0 */
    assertEquals("The low current was not within the expected range.", expectedStoppedCurrentDraw,
        pdp.getCurrent(me.getPDPChannel()), 0.001);
  }

  /**
   * Test if the current changes when the motor is driven using a talon
   */
  @Test
  public void CheckRunningCurrentForSpeedController() throws CANMessageNotFoundException {

    /* Set the motor to full forward */
    me.getMotor().set(1.0);
    Timer.delay(0.25);

    /* The current should now be greater than the low current */
    assertThat("The driven current is not greater than the resting current.",
        pdp.getCurrent(me.getPDPChannel()), is(greaterThan(expectedStoppedCurrentDraw)));
  }

  @Override
  protected Logger getClassLogger() {
    return logger;
  }
}
