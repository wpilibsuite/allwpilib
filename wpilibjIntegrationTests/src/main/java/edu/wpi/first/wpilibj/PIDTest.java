/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.function.DoubleSupplier;
import java.util.logging.Logger;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.wpilibj.experimental.controller.AsynchronousControllerRunner;
import edu.wpi.first.wpilibj.experimental.controller.PIDController;
import edu.wpi.first.wpilibj.fixtures.MotorEncoderFixture;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilderImpl;
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
  private SendableBuilderImpl m_builder;

  private static final double absoluteTolerance = 50;
  private static final double outputRange = 0.25;

  private PIDController m_controller = null;
  private AsynchronousControllerRunner m_runner = null;
  private static MotorEncoderFixture me = null;

  @SuppressWarnings({"MemberName", "EmptyLineSeparator", "MultipleVariableDeclarations"})
  private final Double k_p, k_i, k_d;

  private class ReferenceHolder implements DoubleSupplier {
    private double m_reference;

    public void setReference(double reference) {
      m_reference = reference;
    }

    @Override
    public double getAsDouble() {
      return m_reference;
    }
  }

  private ReferenceHolder m_reference = new ReferenceHolder();

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
    m_table = NetworkTableInstance.getDefault().getTable("TEST_PID");
    m_builder = new SendableBuilderImpl();
    m_builder.setTable(m_table);
    m_controller = new PIDController(k_p, k_i, k_d);
    m_runner = new AsynchronousControllerRunner(m_controller,
        m_reference,
        me.getEncoder()::getDistance,
        me.getMotor()::set);
    m_controller.initSendable(m_builder);
  }

  @After
  public void tearDown() throws Exception {
    logger.fine("Teardown: " + me.getType());
    m_runner.disable();
    m_controller.close();
    m_controller = null;
    m_runner = null;
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
    double reference = 2500.0;
    m_reference.setReference(reference);
    assertFalse("PID did not begin disabled", m_runner.isEnabled());
    assertEquals("PID.getError() did not start at " + reference, reference,
        m_controller.getError(), 0);
    m_builder.updateTable();
    assertEquals(k_p, m_table.getEntry("Kp").getDouble(9999999), 0);
    assertEquals(k_i, m_table.getEntry("Ki").getDouble(9999999), 0);
    assertEquals(k_d, m_table.getEntry("Kd").getDouble(9999999), 0);
    assertEquals(reference, m_table.getEntry("reference").getDouble(9999999), 0);
    assertFalse(m_table.getEntry("enabled").getBoolean(true));
  }

  @Test
  public void testRestartAfterEnable() {
    setupAbsoluteTolerance();
    setupOutputRange();
    double reference = 2500.0;
    m_reference.setReference(reference);
    m_runner.enable();
    m_builder.updateTable();
    assertTrue(m_table.getEntry("enabled").getBoolean(false));
    assertTrue(m_runner.isEnabled());
    assertThat(0.0, is(not(me.getMotor().get())));
    m_controller.reset();
    m_builder.updateTable();
    assertFalse(m_table.getEntry("enabled").getBoolean(true));
    assertFalse(m_runner.isEnabled());
    assertEquals(0, me.getMotor().get(), 0);
  }

  @Test
  public void testSetReference() {
    setupAbsoluteTolerance();
    setupOutputRange();
    Double reference = 2500.0;
    m_runner.disable();
    m_reference.setReference(reference);
    m_runner.enable();
    assertEquals("Did not correctly set reference", reference, new Double(m_controller
        .getReference()));
  }

  @Test(timeout = 10000)
  public void testRotateToTarget() {
    setupAbsoluteTolerance();
    setupOutputRange();
    double reference = 1000.0;
    assertEquals(pidData() + "did not start at 0", 0, m_controller.getOutput(), 0);
    m_reference.setReference(reference);
    assertEquals(pidData() + "did not have an error of " + reference, reference,
        m_controller.getError(), 0);
    m_runner.enable();
    Timer.delay(5);
    m_runner.disable();
    assertTrue(pidData() + "Was not on Target. Controller Error: " + m_controller.getError(),
        m_controller.atReference());
  }

  private String pidData() {
    return me.getType() + " PID {P:" + m_controller.getP() + " I:" + m_controller.getI() + " D:"
        + m_controller.getD() + "} ";
  }


  @Test(expected = RuntimeException.class)
  public void testOnTargetNoToleranceSet() {
    setupOutputRange();
    m_controller.atReference();
  }
}
