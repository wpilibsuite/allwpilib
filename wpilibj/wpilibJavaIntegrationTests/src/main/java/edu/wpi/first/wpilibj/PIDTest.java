/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj;

import static org.hamcrest.CoreMatchers.is;
import static org.hamcrest.CoreMatchers.not;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertThat;
import static org.junit.Assert.assertTrue;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.logging.Logger;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import edu.wpi.first.wpilibj.fixtures.MotorEncoderFixture;
import edu.wpi.first.wpilibj.networktables.NetworkTable;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import edu.wpi.first.wpilibj.test.TestBench;


/**
 * @author Kacper Puczydlowski
 * @author Jonathan Leitschuh
 *
 */

@RunWith(Parameterized.class)
public class PIDTest extends AbstractComsSetup {
	private static final Logger logger = Logger.getLogger(PIDTest.class.getName());
	private NetworkTable table;
	
	private static final double absoluteTollerance = 20;
	private static final double outputRange = 0.3;	
	
	private PIDController controller = null;
	private static MotorEncoderFixture me = null;
	
	private final Double k_p, k_i, k_d;
	
	@Override
	protected Logger getClassLogger(){
		return logger;
	}

	
	public PIDTest(Double p, Double i, Double d, MotorEncoderFixture mef){
		logger.fine("Constructor with: " + mef.getType());
		if(PIDTest.me != null && !PIDTest.me.equals(mef)) PIDTest.me.teardown();
		PIDTest.me = mef;
		this.k_p = p;
		this.k_i = i;
		this.k_d = d;
	}
	
	
	@Parameters
	public static Collection<Object[]> generateData(){
		//logger.fine("Loading the MotorList");
		Collection<Object[]> data = new ArrayList<Object[]>();
		double kp = 0.001;
		double ki = 0.0005;
		double kd = 0.0;
		for(int i = 0; i < 1; i++){
			data.addAll(Arrays.asList(new Object[][]{
				 {kp, ki, kd, TestBench.getInstance().getTalonPair()},
				 {kp, ki, kd, TestBench.getInstance().getVictorPair()},
				 {kp, ki, kd, TestBench.getInstance().getJaguarPair()}}));
		}
		return data;
	}

	/**
	 * @throws java.lang.Exception
	 */
	@BeforeClass
	public static void setUpBeforeClass() throws Exception {
	}

	/**
	 * @throws java.lang.Exception
	 */
	@AfterClass
	public static void tearDownAfterClass() throws Exception {
		logger.fine("TearDownAfterClass: " + me.getType());
		me.teardown();
		me=null;
	}

	/**
	 * @throws java.lang.Exception
	 */
	@Before
	public void setUp() throws Exception {
		logger.fine("Setup: " + me.getType());
		me.setup();
		table = NetworkTable.getTable("TEST_PID");
		controller = new PIDController(k_p, k_i, k_d, me.getEncoder(), me.getMotor());
		controller.initTable(table);
	}

	/**
	 * @throws java.lang.Exception
	 */
	@After
	public void tearDown() throws Exception {
		logger.fine("Teardown: " + me.getType());
		controller.disable();
		controller.free();
		controller = null;
		me.reset();
	}
	
	private void setupAbsoluteTollerance(){
		controller.setAbsoluteTolerance(absoluteTollerance);
	}
	private void setupOutputRange(){
		controller.setOutputRange(-outputRange, outputRange);
	}
	
	@Test
	public void testInitialSettings(){
		setupAbsoluteTollerance();
		setupOutputRange();
		double setpoint = 2500.0;
		controller.setSetpoint(setpoint);
		assertFalse("PID did not begin disabled", controller.isEnable());
		assertEquals("PID.getError() did not start at " + setpoint, setpoint, controller.getError(), 0);
		assertEquals(k_p, table.getNumber("p"), 0);
		assertEquals(k_i, table.getNumber("i"), 0);
		assertEquals(k_d, table.getNumber("d"), 0);
		assertEquals(setpoint, table.getNumber("setpoint"), 0);
		assertFalse(table.getBoolean("enabled"));
	}
	
	@Test
	public void testRestartAfterEnable(){
		setupAbsoluteTollerance();
		setupOutputRange();
		double setpoint = 2500.0;
		controller.setSetpoint(setpoint);
		controller.enable();
		Timer.delay(.5);
		assertTrue(table.getBoolean("enabled"));
		assertTrue(controller.isEnable());
		assertThat(0.0, is(not(me.getMotor().get())));
		controller.reset();
		assertFalse(table.getBoolean("enabled"));
		assertFalse(controller.isEnable());
		assertEquals(0, me.getMotor().get(), 0);
	}
	
	@Test
	public void testSetSetpoint(){
		setupAbsoluteTollerance();
		setupOutputRange();
		Double setpoint = 2500.0;
		controller.disable();
		controller.setSetpoint(setpoint);
		controller.enable();
		assertEquals("Did not correctly set set-point",setpoint, new Double(controller.getSetpoint()));
	}

	@Test (timeout = 15000)
	public void testRotateToTarget() {
		setupAbsoluteTollerance();
		setupOutputRange();
		double setpoint = 2500.0;
		assertEquals(pidData() + "did not start at 0", 0, controller.get(), 0);
		controller.setSetpoint(setpoint);
		assertEquals(pidData() +"did not have an error of " + setpoint, setpoint, controller.getError(), 0);
		controller.enable();
		Timer.delay(10);
		controller.disable();
		assertTrue(pidData() + "Was not on Target. Controller Error: " + controller.getError(), controller.onTarget());
	}
	
	private String pidData(){
		return me.getType() + " PID {P:" +controller.getP() + " I:" + controller.getI() + " D:" + controller.getD() +"} ";
	}
	
	
	@Test(expected = RuntimeException.class)
	public void testOnTargetNoTolleranceSet(){
		setupOutputRange();
		controller.onTarget();
	}
}
