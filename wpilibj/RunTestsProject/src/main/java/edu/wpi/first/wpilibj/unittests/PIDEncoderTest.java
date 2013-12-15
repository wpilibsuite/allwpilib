/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.wpilibj.unittests;

import Assemblies.MagneticRotaryAssembly;
import edu.wpi.first.testing.TestClass;
import edu.wpi.first.wpilibj.DigitalOutput;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.Jaguar;
import edu.wpi.first.wpilibj.PIDController;
import edu.wpi.first.wpilibj.Timer;

/**
 * 
 * @author Fred
 */
public class PIDEncoderTest extends TestClass {

	private Encoder encoder;
	private Jaguar jag;
	private PIDController PID;
	private DigitalOutput fwdLimit;
	private DigitalOutput revLimit;

	public String getName() {
		return "PID Encoder Test";
	}

	public String[] getTags() {
		return new String[] { RunTests.Tags.Lifecycle.INPRODUCTION,
				RunTests.Tags.Type.DIGITAL, RunTests.Tags.Type.PWM };
	}

	public void setup() {
		encoder = MagneticRotaryAssembly.getEncoder();
		jag = MagneticRotaryAssembly.getJaguar();
		encoder.setPIDSourceParameter(Encoder.PIDSourceParameter.kDistance);
		PID = new PIDController(.01, 0, 0, encoder, jag);
		fwdLimit = MagneticRotaryAssembly.getLimitSWFwd();
		revLimit = MagneticRotaryAssembly.getLimitSWRev();
		fwdLimit.set(false);
		revLimit.set(false);
	}

	public void teardown() {
		encoder.free();
		jag.free();
		PID.disable();
		fwdLimit.free();
		revLimit.free();
	}

	{
		new Test("Set PID Setpoint to 1000 and confirm running forward") {
			private void runPID() {
				PID.setSetpoint(1000);
				Timer.delay(3);
			}

			public void run() {
				encoder.reset();
				encoder.start();
				PID.reset();
				PID.enable();
				runPID();
				// while (true) {
				// System.out.println("Setpoint: " + PID.getSetpoint()
				// + " Value: " + PID.get() + " Encoder: "
				// + encoder.getDistance() + " Jaguar: " + jag.get());
				// }
				assertEquals(1000, encoder.getDistance(), 15);
			}
		};
		new Test("Set PID Setpoint to 0 and confirm running backwards") {
			private void reversePID() {
				PID.setSetpoint(0);
				Timer.delay(3);
			}

			public void run() {
				reversePID();
				assertEquals(0, encoder.getDistance(), 15);
			}
		};
	}
}
