/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj.fixtures;

import edu.wpi.first.wpilibj.CANJaguar;
import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.DigitalOutput;
import edu.wpi.first.wpilibj.mockhardware.FakeEncoderSource;
import edu.wpi.first.wpilibj.mockhardware.FakePotentiometerSource;

/**
 * @author jonathanleitschuh
 *
 */
public abstract class CANMotorEncoderFixture extends MotorEncoderFixture<CANJaguar> implements ITestFixture {
	private FakePotentiometerSource potSource;
	private DigitalOutput forwardLimit;
	private DigitalOutput reverseLimit;
	private boolean initialized = false;
	
	protected abstract FakePotentiometerSource giveFakePotentiometerSource();
	protected abstract DigitalOutput giveFakeForwardLimit();
	protected abstract DigitalOutput giveFakeReverseLimit();
	
	public CANMotorEncoderFixture(){}
	
	public void initialize(){
		synchronized(this){
			if(!initialized){
				potSource = giveFakePotentiometerSource();
				forwardLimit = giveFakeForwardLimit();
				reverseLimit = giveFakeReverseLimit();
				initialized = true;
			}
		}
	}
	

	@Override
	public boolean setup() {
		initialize();
		return super.setup();
	}

	@Override
	public boolean reset() {
		initialize();
		potSource.reset();
		forwardLimit.set(false);
		reverseLimit.set(false);
		getMotor().setPercentMode(); //Get the Jaguar into a mode where setting the speed means stop
		return super.reset();
	}
	
	@Override
	public boolean teardown() {
		potSource.free();
		forwardLimit.free();
		reverseLimit.free();
		boolean superTornDown = super.teardown();
		getMotor().free();
		return superTornDown;
	}
	
	public FakePotentiometerSource getFakePot(){
		initialize();
		return potSource;
	}
	
	public DigitalOutput getForwardLimit(){
		initialize();
		return forwardLimit;
	}
	
	public DigitalOutput getReverseLimit(){
		initialize();
		return reverseLimit;
	}

}
