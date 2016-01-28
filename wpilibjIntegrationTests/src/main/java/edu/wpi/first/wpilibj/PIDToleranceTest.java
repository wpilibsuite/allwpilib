package edu.wpi.first.wpilibj;

import java.util.logging.Logger;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import edu.wpi.first.wpilibj.test.AbstractComsSetup;

public class PIDToleranceTest extends AbstractComsSetup {
	private static final Logger logger = Logger.getLogger(PIDToleranceTest.class.getName());
	private PIDController pid;
	private final double setPoint = 50.0;
	private final double tolerance = 10.0;
	private final double range = 200;
	private class fakeInput implements PIDSource{
		public double val;
		public fakeInput(){
			val = 0;
		}
		@Override
		public PIDSourceType getPIDSourceType() {
			return PIDSourceType.kDisplacement;
		}

		@Override
		public double pidGet() {
			return val;
		}

		@Override
		public void setPIDSourceType(PIDSourceType arg0) {}
	};
	private fakeInput inp;
	private PIDOutput out = new PIDOutput(){
		@Override
		public void pidWrite(double out) {
		}
		
	};
	@Override
	protected Logger getClassLogger() {
		return logger;
	}
	@Before
	public void setUp() throws Exception{
		inp = new fakeInput();
		pid = new PIDController(0.05,0.0,0.0,inp,out);
		pid.setInputRange(-range/2, range/2);
	}
	
	@After
	public void tearDown() throws Exception{
		pid.free();
		pid = null;
	}
	
	@Test
	public void testAbsoluteTolerance(){
		pid.setAbsoluteTolerance(tolerance);
		pid.setSetpoint(setPoint);
		pid.enable();
		Timer.delay(1);
		assertFalse("Error was in tolerance when it should not have been. Error was "+pid.getAvgError(),pid.onTarget());
		inp.val = setPoint+tolerance/2;
		Timer.delay(1.0);
		assertTrue("Error was not in tolerance when it should have been. Error was "+pid.getAvgError(),pid.onTarget());
		inp.val = setPoint + 10*tolerance;
		Timer.delay(1.0);
		assertFalse("Error was in tolerance when it should not have been. Error was "+pid.getAvgError(),pid.onTarget());
	}
	@Test
	public void testPercentTolerance(){
		pid.setPercentTolerance(tolerance);
		pid.setSetpoint(setPoint);
		pid.enable();
		assertFalse("Error was in tolerance when it should not have been. Error was "+pid.getAvgError(),pid.onTarget());
		inp.val = setPoint+(tolerance)/200*range;//half of percent tolerance away from setPoint
		Timer.delay(1.0);
		assertTrue("Error was not in tolerance when it should have been. Error was "+pid.getAvgError(),pid.onTarget());
		inp.val = setPoint + (tolerance)/50*range;//double percent tolerance away from setPoint
		Timer.delay(1.0);
		assertFalse("Error was in tolerance when it should not have been. Error was "+pid.getAvgError(),pid.onTarget());
	}
}
