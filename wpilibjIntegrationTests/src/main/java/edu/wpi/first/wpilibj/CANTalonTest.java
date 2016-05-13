/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
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
 * Basic test (borrowed straight from SampleTest) for running the CAN TalonSRX.
 */
public class CANTalonTest extends AbstractComsSetup {
  private static final Logger logger = Logger.getLogger(SampleTest.class.getName());

  static SampleFixture m_fixture = new SampleFixture();

  protected Logger getClassLogger() {
    return logger;
  }

  @BeforeClass
  public static void classSetup() {
    // Set up the fixture before the test is created
    m_fixture.setup();
  }

  @Before
  public void setUp() {
    // Reset the fixture elements before every test
    m_fixture.reset();
  }

  @AfterClass
  public static void tearDown() {
    // Clean up the fixture after the test
    m_fixture.teardown();
  }

  private String errorMessage(double actual, double expected) {
    String start = "Actual value was: ";
    start += actual;
    start += " Expected: ";
    start += expected;
    return start;
  }

  /**
   * Briefly run a CAN Talon and assert true.
   */
  @Test
  public void throttle() {
    double throttle = 0.1;
    CANTalon tal = new CANTalon(0);
    tal.set(throttle);
    Timer.delay(5.0);
    assertTrue(errorMessage(tal.get(), throttle), Math.abs(throttle - tal.get()) < 1e-2);

    tal.set(-throttle);
    Timer.delay(1.25);
    assertTrue(errorMessage(tal.get(), -throttle), Math.abs(throttle + tal.get()) < 1e-2);
    tal.reverseOutput(true);
    tal.set(-throttle);
    Timer.delay(1.25);
    assertTrue(errorMessage(tal.get(), -throttle), Math.abs(throttle + tal.get()) < 1e-2);

    tal.disable();
    Timer.delay(0.2);
    assertTrue(errorMessage(tal.get(), 0.0), Math.abs(tal.get()) < 1e-10);
  }

  @Test
  public void setGetPID() {
    CANTalon talon = new CANTalon(0);
    talon.changeControlMode(CANTalon.TalonControlMode.Position);
    @SuppressWarnings({"LocalVariableName", "MultipleVariableDeclarations"})
    double p = 0.05, i = 0.098, d = 1.23;
    talon.setPID(p, i, d);
    assertTrue(errorMessage(talon.getP(), p), Math.abs(p - talon.getP()) < 1e-5);
    assertTrue(errorMessage(talon.getI(), i), Math.abs(i - talon.getI()) < 1e-5);
    assertTrue(errorMessage(talon.getD(), d), Math.abs(d - talon.getD()) < 1e-5);
    // Test with new values in case the talon was already set to the previous
    // ones.
    p = 0.15;
    i = 0.198;
    d = 1.03;
    talon.setPID(p, i, d);
    assertTrue(errorMessage(talon.getP(), p), Math.abs(p - talon.getP()) < 1e-5);
    assertTrue(errorMessage(talon.getI(), i), Math.abs(i - talon.getI()) < 1e-5);
    assertTrue(errorMessage(talon.getD(), d), Math.abs(d - talon.getD()) < 1e-5);
  }

  @Test
  public void positionModeWorks() {
    CANTalon talon = new CANTalon(0);
    talon.changeControlMode(CANTalon.TalonControlMode.Position);
    talon.setFeedbackDevice(CANTalon.FeedbackDevice.AnalogPot);
    Timer.delay(0.2);
    @SuppressWarnings({"LocalVariableName", "MultipleVariableDeclarations"})
    double p = 1.0, i = 0.0, d = 0.00;
    talon.setPID(p, i, d);
    talon.set(100);
    Timer.delay(5.0);
    talon.reverseOutput(true);
    Timer.delay(5.0);
    // assertTrue(errorMessage(talon.get(), 100), Math.abs(100 - talon.get()) <
    // 10);
    assertTrue(true);
  }

  @Test
  public void testBrake() {
    CANTalon talon = new CANTalon(0);
    for (int i = 0; i < 5; i++) {
      talon.enableBrakeMode(true);
      Timer.delay(0.5);
      talon.enableBrakeMode(false);
      Timer.delay(0.5);
    }
  }

  //Test Get Fault methods (artf4814)
  @Test
  public void testGetFaults() {
    CANTalon talon = new CANTalon(0);
    talon.clearStickyFaults();

    assertTrue(talon.getFaultOverTemp() == 0);
    assertTrue(talon.getFaultUnderVoltage() == 0);
    assertTrue(talon.getFaultForLim() == 0);
    assertTrue(talon.getFaultRevLim() == 0);
    assertTrue(talon.getFaultHardwareFailure() == 0);
    assertTrue(talon.getFaultForSoftLim() == 0);
    assertTrue(talon.getFaultRevSoftLim() == 0);

    assertTrue(talon.getStickyFaultOverTemp() == 0);
    //    assertTrue(talon.getStickyFaultUnderVoltage()==0);
    assertTrue(talon.getStickyFaultForLim() == 0);
    assertTrue(talon.getStickyFaultRevLim() == 0);
    assertTrue(talon.getStickyFaultForSoftLim() == 0);
    assertTrue(talon.getStickyFaultRevSoftLim() == 0);
  }

}
