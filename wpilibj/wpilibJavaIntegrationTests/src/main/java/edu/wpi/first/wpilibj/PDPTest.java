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
	/* The current returned when the motor is not being driven */
	protected static final double  kLowCurrent =  1.52;

	protected static final double  kCurrentTolerance =  0.1;

	private static PowerDistributionPanel pdp;
	private static MotorEncoderFixture<?> me;

	@BeforeClass
	public static void setUpBeforeClass() throws Exception {
		pdp = new PowerDistributionPanel();
	}
	
	@AfterClass
	public static void tearDownAfterClass() throws Exception {
		pdp.free();
		me.teardown();
	}

	
	public PDPTest(MotorEncoderFixture<?> mef){
		logger.fine("Constructor with: " + mef.getType());
		if(me != null && !me.equals(mef)) me.teardown();
		me = mef;
		me.setup();
	}
	
	@Parameters(name= "{index}: {0}")
	public static Collection<MotorEncoderFixture<?>[]> generateData(){
		//logger.fine("Loading the MotorList");
		return Arrays.asList(new MotorEncoderFixture<?>[][]{
				 {TestBench.getInstance().getTalonPair()},
				 {TestBench.getInstance().getVictorPair()},
				 {TestBench.getInstance().getJaguarPair()}
		});
	}

	@After
	public void tearDown() throws Exception {
		me.reset();
	}
	
	

	/**
	 * Test if the current changes when the motor is driven using a talon
	 */
	@Test
	public void CheckStoppedCurrentForSpeedController() throws CANMessageNotFoundException{
		/* The Current should be kLowCurrent */
		assertEquals("The low current was not within the expected range.",
				kLowCurrent, pdp.getCurrent(me.getPDPChannel()), kCurrentTolerance);
	}
	
	/**
	 * Test if the current changes when the motor is driven using a talon
	 */
	@Test
	public void CheckRunningCurrentForSpeedController() throws CANMessageNotFoundException{

		/* Set the motor to full forward */
		me.getMotor().set(1.0);
		Timer.delay(0.02);
		/* The current should now be greater than the low current */
		assertThat("The driven current is not greater than the resting current.",
				pdp.getCurrent(me.getPDPChannel()), is(greaterThan(kLowCurrent)));
	}

	@Override
	protected Logger getClassLogger() {
		return logger;
	}
}
