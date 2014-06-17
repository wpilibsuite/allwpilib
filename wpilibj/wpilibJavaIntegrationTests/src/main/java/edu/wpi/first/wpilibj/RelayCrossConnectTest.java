/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.util.logging.Logger;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import edu.wpi.first.wpilibj.Relay.Direction;
import edu.wpi.first.wpilibj.Relay.InvalidValueException;
import edu.wpi.first.wpilibj.Relay.Value;
import edu.wpi.first.wpilibj.fixtures.RelayCrossConnectFxiture;
import edu.wpi.first.wpilibj.networktables.NetworkTable;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import edu.wpi.first.wpilibj.test.TestBench;

/**
 * @author jonathanleitschuh
 *
 */
public class RelayCrossConnectTest extends AbstractComsSetup {
	private static final Logger logger = Logger.getLogger(RelayCrossConnectTest.class.getName());
	private static final NetworkTable table = NetworkTable.getTable("_RELAY_CROSS_CONNECT_TEST_");
	private RelayCrossConnectFxiture relayFixture;
	
	
	/**
	 * @throws java.lang.Exception
	 */
	@Before
	public void setUp() throws Exception {
		relayFixture = TestBench.getRelayCrossConnectFixture();
		relayFixture.setup();
		relayFixture.getRelay().initTable(table);
	}

	/**
	 * @throws java.lang.Exception
	 */
	@After
	public void tearDown() throws Exception {
		relayFixture.reset();
		relayFixture.teardown();
	}

	@Test
	public void testBothHigh() {
		relayFixture.getRelay().setDirection(Direction.kBoth);
		relayFixture.getRelay().set(Value.kOn);
		relayFixture.getRelay().updateTable();
		assertTrue("Input one was not high when relay set both high", relayFixture.getInputOne().get());
		assertTrue("Input two was not high when relay set both high", relayFixture.getInputTwo().get());
		assertEquals("On", table.getString("Value"));
	}
	
	@Test
	public void testFirstHigh() {
		relayFixture.getRelay().setDirection(Direction.kBoth);
		relayFixture.getRelay().set(Value.kForward);
		relayFixture.getRelay().updateTable();
		assertFalse("Input one was not low when relay set Value.kForward", relayFixture.getInputOne().get());
		assertTrue("Input two was not high when relay set Value.kForward", relayFixture.getInputTwo().get());
		assertEquals("Forward", table.getString("Value"));
	}
	
	@Test
	public void testSecondHigh() {
		relayFixture.getRelay().setDirection(Direction.kBoth);
		relayFixture.getRelay().set(Value.kReverse);
		relayFixture.getRelay().updateTable();
		assertTrue("Input one was not high when relay set Value.kReverse", relayFixture.getInputOne().get());
		assertFalse("Input two was not low when relay set Value.kReverse", relayFixture.getInputTwo().get());
		assertEquals("Reverse", table.getString("Value"));
	}
	
	@Test(expected=InvalidValueException.class)
	public void testSetValueForwardWithDirectionReverseThrowingException(){
		relayFixture.getRelay().setDirection(Direction.kForward);
		relayFixture.getRelay().set(Value.kReverse);
	}
	
	@Test(expected=InvalidValueException.class)
	public void testSetValueReverseWithDirectionForwardThrowingException(){
		relayFixture.getRelay().setDirection(Direction.kReverse);
		relayFixture.getRelay().set(Value.kForward);
	}
	
	@Test
	public void testInitialSettings(){
		assertEquals(Value.kOff, relayFixture.getRelay().get());
		//Initially both outputs should be off
		assertFalse(relayFixture.getInputOne().get());
		assertFalse(relayFixture.getInputTwo().get());
		assertEquals("Off", table.getString("Value"));
	}

	@Override
	protected Logger getClassLogger() {
		return logger;
	}

}
