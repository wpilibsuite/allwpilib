/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.logging.Logger;

import edu.wpi.first.wpilibj.fixtures.MotorEncoderFixture;
import edu.wpi.first.wpilibj.networktables.NetworkTable;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import edu.wpi.first.wpilibj.test.TestBench;

import static org.hamcrest.CoreMatchers.is;
import static org.hamcrest.CoreMatchers.not;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertThat;
import static org.junit.Assert.assertTrue;


/**
 * Test that covers the {@link PIDController}.
 */

@RunWith(Parameterized.class)
public class PIDTest extends AbstractComsSetup {
  private static final Logger logger = Logger.getLogger(PIDTest.class.getName());
  private NetworkTable m_table;

  private static final double absoluteTolerance = 50;
  private static final double outputRange = 0.25;

  private PIDController m_controller = null;
  private static MotorEncoderFixture me = null;

  @SuppressWarnings({"MemberName", "EmptyLineSeparator", "MultipleVariableDeclarations"})
  private final Double k_p, k_i, k_d;

  @Override
  protected Logger getClassLogger() {
    return logger;
  }


  @SuppressWarnings({"ParameterName", "JavadocMethod"})
  public PIDTest(Double p, Double i, Double d, MotorEncoderFixture mef) {
    logger.fine("Constructor with: " + mef.getType());
    if (PIDTest.me != null && !PIDTest.me.equals(mef)) {
      PIDTest.me.teardown();
    }
    PIDTest.me = mef;
    this.k_p = p;
    this.k_i = i;
    this.k_d = d;
  }


  @Parameters
  public static Collection<Object[]> generateData() {
    // logger.fine("Loading the MotorList");
    Collection<Object[]> data = new ArrayList<Object[]>();
    double kp = 0.001;
    double ki = 0.0005;
    double kd = 0.0;
    for (int i = 0; i < 1; i++) {
      data.addAll(Arrays.asList(new Object[][]{
          {kp, ki, kd, TestBench.getInstance().getTalonPair()},
          {kp, ki, kd, TestBench.getInstance().getVictorPair()},
          {kp, ki, kd, TestBench.getInstance().getJaguarPair()}}));
    }
    return data;
  }

  @BeforeClass
  public static void setUpBeforeClass() throws Exception {
  }

  @AfterClass
  public static void tearDownAfterClass() throws Exception {
    logger.fine("TearDownAfterClass: " + me.getType());
    me.teardown();
    me = null;
  }

  @Before
  public void setUp() throws Exception {
    logger.fine("Setup: " + me.getType());
    me.setup();
    m_table = NetworkTable.getTable("TEST_PID");
    m_controller = new PIDController(k_p, k_i, k_d, me.getEncoder(), me.getMotor());
    m_controller.initTable(m_table);
  }

  @After
  public void tearDown() throws Exception {
    logger.fine("Teardown: " + me.getType());
    m_controller.disable();
    m_controller.free();
    m_controller = null;
    me.reset();
  }

  private void setupAbsoluteTolerance() {
    m_controller.setAbsoluteTolerance(absoluteTolerance);
  }

  private void setupOutputRange() {
    m_controller.setOutputRange(-outputRange, outputRange);
  }

  @Test
  public void testInitialSettings() {
    setupAbsoluteTolerance();
    setupOutputRange();
    double setpoint = 2500.0;
    m_controller.setSetpoint(setpoint);
    assertFalse("PID did not begin disabled", m_controller.isEnabled());
    assertEquals("PID.getError() did not start at " + setpoint, setpoint,
        m_controller.getError(), 0);
    assertEquals(k_p, m_table.getNumber("p"), 0);
    assertEquals(k_i, m_table.getNumber("i"), 0);
    assertEquals(k_d, m_table.getNumber("d"), 0);
    assertEquals(setpoint, m_table.getNumber("setpoint"), 0);
    assertFalse(m_table.getBoolean("enabled"));
  }

  @Test
  public void testRestartAfterEnable() {
    setupAbsoluteTolerance();
    setupOutputRange();
    double setpoint = 2500.0;
    m_controller.setSetpoint(setpoint);
    m_controller.enable();
    Timer.delay(.5);
    assertTrue(m_table.getBoolean("enabled"));
    assertTrue(m_controller.isEnabled());
    assertThat(0.0, is(not(me.getMotor().get())));
    m_controller.reset();
    assertFalse(m_table.getBoolean("enabled"));
    assertFalse(m_controller.isEnabled());
    assertEquals(0, me.getMotor().get(), 0);
  }

  @Test
  public void testSetSetpoint() {
    setupAbsoluteTolerance();
    setupOutputRange();
    Double setpoint = 2500.0;
    m_controller.disable();
    m_controller.setSetpoint(setpoint);
    m_controller.enable();
    assertEquals("Did not correctly set set-point", setpoint, new Double(m_controller
        .getSetpoint()));
  }

  @Test(timeout = 10000)
  public void testRotateToTarget() {
    setupAbsoluteTolerance();
    setupOutputRange();
    double setpoint = 1000.0;
    assertEquals(pidData() + "did not start at 0", 0, m_controller.get(), 0);
    m_controller.setSetpoint(setpoint);
    assertEquals(pidData() + "did not have an error of " + setpoint, setpoint,
        m_controller.getError(), 0);
    m_controller.enable();
    Timer.delay(5);
    m_controller.disable();
    assertTrue(pidData() + "Was not on Target. Controller Error: " + m_controller.getError(),
        m_controller.onTarget());
  }

  private String pidData() {
    return me.getType() + " PID {P:" + m_controller.getP() + " I:" + m_controller.getI() + " D:"
        + m_controller.getD() + "} ";
  }


  @Test(expected = RuntimeException.class)
  public void testOnTargetNoToleranceSet() {
    setupOutputRange();
    m_controller.onTarget();
  }
}
