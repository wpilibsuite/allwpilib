package com.kauailabs.vmx.wpilib.Examples.HelloWorld;

import edu.wpi.first.wpilibj.IterativeRobot;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.PWMTalonSRX;


public class Robot extends IterativeRobot {

	public static PWMTalonSRX talon;
	
	public Joystick joy;
	
     public void robotInit() {
    	 
    	 talon = new PWMTalonSRX(0);
    	 
    	 joy = new Joystick(0);
    	 
    	 System.out.println("The power of the motor is yours once you enable!");
     }
     
     public void teleopInit () {
    	 System.out.println("Motors enabled");
     }
     
     public void teleopPeriodic() {
    	 talon.setSpeed(joy.getX());
     }
     
     public void disabled() {
    	 talon.setSpeed(0);
     }
}

