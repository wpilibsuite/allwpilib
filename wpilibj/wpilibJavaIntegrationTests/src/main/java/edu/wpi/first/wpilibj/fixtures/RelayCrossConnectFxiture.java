/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj.fixtures;

import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.Relay;
import edu.wpi.first.wpilibj.Relay.Direction;
import edu.wpi.first.wpilibj.Relay.Value;

/**
 * @author jonathanleitschuh
 *
 */
public abstract class RelayCrossConnectFxiture implements ITestFixture {
	
	private DigitalInput inputOne;
	private DigitalInput inputTwo;
	private Relay relay;
	
	private boolean initialized = false;
	private boolean freed = false;
	
	
	
	protected abstract Relay giveRelay();
	
	protected abstract DigitalInput giveInputOne();
	
	protected abstract DigitalInput giveInputTwo();
	
	private void initialize(){
		synchronized(this){
			if(!initialized){
				relay = giveRelay();
				inputOne = giveInputOne();
				inputTwo = giveInputTwo();
				initialized = true;
			}
		}
	}
	
	public Relay getRelay(){
		initialize();
		return relay;
	}
	
	public DigitalInput getInputOne(){
		initialize();
		return inputOne;
	}
	
	public DigitalInput getInputTwo(){
		initialize();
		return inputTwo;
	}

	/* (non-Javadoc)
	 * @see edu.wpi.first.wpilibj.fixtures.ITestFixture#setup()
	 */
	@Override
	public boolean setup() {
		initialize();
		return true;
	}

	/* (non-Javadoc)
	 * @see edu.wpi.first.wpilibj.fixtures.ITestFixture#reset()
	 */
	@Override
	public boolean reset() {
		initialize();
		return true;
	}

	/* (non-Javadoc)
	 * @see edu.wpi.first.wpilibj.fixtures.ITestFixture#teardown()
	 */
	@Override
	public boolean teardown() {
		if(!freed){
			relay.free();
			inputOne.free();
			inputTwo.free();
			freed = true;
		} else {
			throw new RuntimeException("You attempted to free the " + RelayCrossConnectFxiture.class.getSimpleName() + " multiple times");
		}
		return true;
	}
}
