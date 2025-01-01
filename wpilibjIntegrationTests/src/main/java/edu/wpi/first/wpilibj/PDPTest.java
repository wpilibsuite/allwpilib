// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.Matchers.greaterThan;
import static org.hamcrest.Matchers.is;
import static org.junit.Assert.assertEquals;

import edu.wpi.first.hal.can.CANMessageNotFoundException;
import edu.wpi.first.wpilibj.fixtures.MotorEncoderFixture;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import edu.wpi.first.wpilibj.test.TestBench;
import java.util.Collection;
import java.util.List;
import java.util.logging.Logger;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

/** Test that covers the {@link PowerDistribution}. */
@RunWith(Parameterized.class)
public class PDPTest extends AbstractComsSetup {
  private static final Logger logger = Logger.getLogger(PDPTest.class.getName());

  private static PowerDistribution pdp;
  private static MotorEncoderFixture<?> me;
  private final double m_expectedStoppedCurrentDraw;

  @BeforeClass
  public static void setUpBeforeClass() {
    pdp = new PowerDistribution();
  }

  @AfterClass
  public static void tearDownAfterClass() {
    pdp = null;
    me.teardown();
    me = null;
  }

  /**
   * PDPTest constructor.
   *
   * @param mef Motor encoder fixture.
   * @param expectedCurrentDraw Expected current draw in Amps.
   */
  public PDPTest(MotorEncoderFixture<?> mef, double expectedCurrentDraw) {
    logger.fine("Constructor with: " + mef.getType());
    if (me != null && !me.equals(mef)) {
      me.teardown();
    }
    me = mef;
    me.setup();

    m_expectedStoppedCurrentDraw = expectedCurrentDraw;
  }

  @Parameters(name = "{index}: {0}, Expected Stopped Current Draw: {1}")
  public static Collection<Object[]> generateData() {
    // logger.fine("Loading the MotorList");
    return List.of(new Object[][] {{TestBench.getTalonPair(), 0.0}});
  }

  @After
  public void tearDown() {
    me.reset();
  }

  /** Test if the current changes when the motor is driven using a talon. */
  @Test
  public void checkStoppedCurrentForMotorController() throws CANMessageNotFoundException {
    Timer.delay(0.25);

    /* The Current should be 0 */
    assertEquals(
        "The low current was not within the expected range.",
        m_expectedStoppedCurrentDraw,
        pdp.getCurrent(me.getPDPChannel()),
        0.001);
  }

  /** Test if the current changes when the motor is driven using a talon. */
  @Test
  public void checkRunningCurrentForMotorController() throws CANMessageNotFoundException {
    /* Set the motor to full forward */
    me.getMotor().set(1.0);
    Timer.delay(2);

    /* The current should now be greater than the low current */
    assertThat(
        "The driven current is not greater than the resting current.",
        pdp.getCurrent(me.getPDPChannel()),
        is(greaterThan(m_expectedStoppedCurrentDraw)));
  }

  @Override
  protected Logger getClassLogger() {
    return logger;
  }
}
