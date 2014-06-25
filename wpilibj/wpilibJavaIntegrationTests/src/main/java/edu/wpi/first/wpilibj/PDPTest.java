package edu.wpi.first.wpilibj;

import static org.junit.Assert.*;
import static org.hamcrest.Matchers.*;

import java.util.logging.Logger;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import edu.wpi.first.wpilibj.can.CANMessageNotFoundException;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import edu.wpi.first.wpilibj.test.TestBench;

public class PDPTest extends AbstractComsSetup {
	private static final Logger logger = Logger.getLogger(PCMTest.class.getName());
	/* The current returned when the motor is not being driven */
	protected static final double  kLowCurrent =  1.52;

	protected static final double  kCurrentTolerance =  0.1;

	private static PowerDistributionPanel pdp;
	private static Talon talon;
	private static Victor victor;
	private static Jaguar jaguar;

	@BeforeClass
	public static void setUpBeforeClass() throws Exception {
		pdp = new PowerDistributionPanel();
		talon = new Talon(TestBench.kTalonPDPChannel);
		victor = new Victor(TestBench.kVictorPDPChannel);
		jaguar = new Jaguar(TestBench.kJaguarPDPChannel);
	}

	@AfterClass
	public static void tearDownAfterClass() throws Exception {
		pdp.free();
		talon.free();
		victor.free();
		jaguar.free();
	}

	@Before
	public void setUp() throws Exception {
		/* Reset all speed controllers to 0.0 */
		talon.set(0.0);
		victor.set(0.0);
		jaguar.set(0.0);
	}

	@After
	public void tearDown() throws Exception {
	}

	/**
	 * Test if the current changes when the motor is driven using a talon
	 */
	@Test
	public void CheckCurrentTalon() {
		/* The Current should be kLowCurrent */
		try {
			assertEquals("The low current was not within the expected range.",
					kLowCurrent, pdp.getCurrent(TestBench.kTalonPDPChannel), kCurrentTolerance);
		} catch (CANMessageNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		Timer.delay(0.02);

		/* Set the motor to full forward */
		talon.set(1.0);
		Timer.delay(0.02);
		/* The current should now be greater than the low current */
		try {
			assertThat("The driven current is not greater than the resting current.",
					pdp.getCurrent(TestBench.kTalonPDPChannel), is(greaterThan(kLowCurrent)));
		} catch (CANMessageNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	/**
	 * Test if the current changes when the motor is driven using a victor
	 */
	@Test
	public void CheckCurrentVictor() {
		/* The Current should be kLowCurrent */
		try {
			assertEquals("The low current was not within the expected range.",
					kLowCurrent, pdp.getCurrent(TestBench.kVictorPDPChannel), kCurrentTolerance);
		} catch (CANMessageNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		Timer.delay(0.02);

		/* Set the motor to full forward */
		victor.set(1.0);
		Timer.delay(0.02);
		/* The current should now be greater than the low current */
		try {
			assertThat("The driven current is not greater than the resting current.",
					pdp.getCurrent(TestBench.kVictorPDPChannel), is(greaterThan(kLowCurrent)));
		} catch (CANMessageNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	/**
	 * Test if the current changes when the motor is driven using a jaguar
	 */
	@Test
	public void CheckCurrentJaguar() {
		/* The Current should be kLowCurrent */
		try {
			assertEquals("The low current was not within the expected range.",
					kLowCurrent, pdp.getCurrent(TestBench.kJaguarPDPChannel), kCurrentTolerance);
		} catch (CANMessageNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		Timer.delay(0.02);

		/* Set the motor to full forward */
		jaguar.set(1.0);
		Timer.delay(0.02);

		/* The current should now be greater than the low current */
		try {
			assertThat("The driven current is not greater than the resting current.",
					pdp.getCurrent(TestBench.kJaguarPDPChannel), is(greaterThan(kLowCurrent)));
		} catch (CANMessageNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	@Override
	protected Logger getClassLogger() {
		return logger;
	}
}
