/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj;

import static org.junit.Assert.*;

import java.util.logging.Logger;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import edu.wpi.first.wpilibj.fixtures.AnalogCrossConnectFixture;
import edu.wpi.first.wpilibj.mockhardware.FakePotentiometerSource;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import edu.wpi.first.wpilibj.test.TestBench;

/**
 * @author jonathanleitschuh
 *
 */
public class AnalogPotentiometerTest extends AbstractComsSetup {
	private static final Logger logger = Logger.getLogger(AnalogPotentiometerTest.class.getName());
	private AnalogCrossConnectFixture analogIO;
	private FakePotentiometerSource potSource;
	private AnalogPotentiometer pot;
	
	private static final double DOUBLE_COMPARISON_DELTA = 1.0;

	/**
	 * @throws java.lang.Exception
	 */
	@Before
	public void setUp() throws Exception {
		analogIO = TestBench.getAnalogCrossConnectFixture();
		potSource = new FakePotentiometerSource(analogIO.getOutput(), 360);
		pot = new AnalogPotentiometer(analogIO.getInput(), 360.0/5.0, 0);
		
	}

	/**
	 * @throws java.lang.Exception
	 */
	@After
	public void tearDown() throws Exception {
		potSource.reset();
		pot.free();
		analogIO.teardown();
	}
	
	@Override
	protected Logger getClassLogger() {
		return logger;
	}
	
	@Test
	public void testInitialSettings(){
		assertEquals(0, pot.get(), DOUBLE_COMPARISON_DELTA);
	}
	
	@Test
	public void testRangeValues(){
		for(double i = 0.0; i < 360.0; i = i+1.0){
			potSource.setAngle(i);
			assertEquals(i, pot.get(), DOUBLE_COMPARISON_DELTA);
			Timer.delay(.02);
		}
	}


}
