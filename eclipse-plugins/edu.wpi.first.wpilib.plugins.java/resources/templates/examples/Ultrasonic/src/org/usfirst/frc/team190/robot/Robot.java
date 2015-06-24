
package org.usfirst.frc.team190.robot;

import edu.wpi.first.wpilibj.CANTalon;
import edu.wpi.first.wpilibj.SampleRobot;
import edu.wpi.first.wpilibj.RobotDrive;
import edu.wpi.first.wpilibj.AnalogInput;

/**
 * This is a sample program demonstrating how to use an ultrasonic sensor and proportional 
 * control to maintain a set distance from an object.
 */

public class Robot extends SampleRobot {
    AnalogInput ultrasonic; //ultrasonic sensor
    RobotDrive myRobot;
    
    final int ultrasonicChannel = 3; //analog input pin
    
    //channels for motors
    final int leftMotorChannel = 1;
    final int rightMotorChannel = 0;
    final int leftRearMotorChannel = 3;
    final int rightRearMotorChannel = 2;
    
    int holdDistance = 12; //distance in inches the robot wants to stay from an object
    final double valueToInches = 0.125; //factor to convert sensor values to a distance in inches
    final double pGain = 0.05; //proportional speed constant

 
    public Robot() {
	//make objects for the sensor and the drive train
	ultrasonic = new AnalogInput(ultrasonicChannel);   
        myRobot = new RobotDrive(new CANTalon(leftMotorChannel), new CANTalon(leftRearMotorChannel), 
        	new CANTalon(rightMotorChannel), new CANTalon(rightRearMotorChannel));
    }

    /**
     * Runs during autonomous.
     */
    public void autonomous() {

    }

    /**
     * Tells the robot to drive to a set distance (in inches) from an object using proportional control.
     */
    public void operatorControl() {

	double currentDistance; //distance measured from the ultrasonic sensor values
	double currentSpeed; //speed to set the motor
	
	while (isOperatorControl() && isEnabled()) {
            currentDistance = ultrasonic.getValue()*valueToInches; //sensor returns a value from 0-4095 that is scaled to inches 
            currentSpeed = (holdDistance - currentDistance)*pGain; //convert distance error to a motor speed
            myRobot.drive(currentSpeed, 0); //drive robot 
	}
    }

    /**
     * Runs during test mode
     */
    public void test() {
    }
}
