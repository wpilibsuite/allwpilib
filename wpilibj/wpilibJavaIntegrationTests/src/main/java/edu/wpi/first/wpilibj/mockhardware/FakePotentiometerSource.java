/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj.mockhardware;

import edu.wpi.first.wpilibj.AnalogOutput;

/**
 * @author jonathanleitschuh
 *
 */
public class FakePotentiometerSource {
	private AnalogOutput output;
	private double potMaxAngle;
	private final double defaultPotMaxAngle;
	public FakePotentiometerSource(AnalogOutput output, double defaultPotMaxAngle){
		this.defaultPotMaxAngle = defaultPotMaxAngle;
		potMaxAngle = defaultPotMaxAngle;
		this.output = output;
	}
	
	public void setRange(double range){
		potMaxAngle = range;
	}
	
	public void reset(){
		potMaxAngle = defaultPotMaxAngle;
		output.setVoltage(0.0);
	}
	
	public void setAngle(double angle){
		output.setVoltage((5.0/potMaxAngle)*angle);
	}
}
