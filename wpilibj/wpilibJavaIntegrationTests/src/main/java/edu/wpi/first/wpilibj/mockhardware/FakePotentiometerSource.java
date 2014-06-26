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
	private boolean m_init_output;
	private double potMaxAngle;
	private final double defaultPotMaxAngle;
	public FakePotentiometerSource(AnalogOutput output, double defaultPotMaxAngle){
		this.defaultPotMaxAngle = defaultPotMaxAngle;
		potMaxAngle = defaultPotMaxAngle;
		this.output = output;
		m_init_output = false;
	}
	
	public FakePotentiometerSource(int port, double defaultPotMaxAngle){
		this(new AnalogOutput(port), defaultPotMaxAngle);
		m_init_output = true;
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
	
	public void setVoltage(double voltage){
		output.setVoltage(voltage);
	}
	
	public double getVoltage(){
		return output.getVoltage();
	}
	
	public void free(){
		if(m_init_output){
			output.free();
			m_init_output = false;
		}
	}
}
