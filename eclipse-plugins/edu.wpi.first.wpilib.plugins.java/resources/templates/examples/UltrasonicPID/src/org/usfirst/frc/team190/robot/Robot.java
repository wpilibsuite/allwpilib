
package $package;

import edu.wpi.first.wpilibj.AnalogInput;
import edu.wpi.first.wpilibj.CANTalon;
import edu.wpi.first.wpilibj.PIDOutput;
import edu.wpi.first.wpilibj.SampleRobot;
import edu.wpi.first.wpilibj.RobotDrive;
import edu.wpi.first.wpilibj.PIDController;

/**
 * This is a sample program to demonstrate the use of a PID Controller with an ultrasonic 
 * sensor to reach and maintain a set distance from an object.
 * 
 * WARNING: While it may look like a good choice to use for your code if you're inexperienced,
 * don't. Unless you know what you are doing, complex code will be much more difficult under
 * this system. Use IterativeRobot or Command-Based instead if you're new.
 */
public class Robot extends SampleRobot {
    AnalogInput ultrasonic; //ultrasonic sensor
    RobotDrive myRobot;    
    PIDController pidController;
    
    final int ultrasonicChannel = 3; //analog input 
    
    //channels for motors
    final int leftMotorChannel = 1;
    final int rightMotorChannel = 0;
    final int leftRearMotorChannel = 3;
    final int rightRearMotorChannel = 2;
  
    int holdDistance = 12; //distance in inches the robot wants to stay from an object
    
    //proportional, integral, and derivative speed constants
    //DANGER: when tuning PID constants, high/inappropriate values for pGain, iGain, 
    //and dGain may cause dangerous, uncontrollable, or undesired behavior!
    final double pGain = 7, iGain = .018, dGain = 1.5; 
    
    //conversion factor specific to the sensor being used. For this sensor,
    //the sensor returned values from 0.0V to 5.0V with a resolution of 9.8mV/in.
    final double VoltsToInches = 0.0098;
    
    //internal class to write to myRobot (a RobotDrive object) using a PIDOutput
    public class MyPIDOutput implements PIDOutput {
	    @Override
	    public void pidWrite(double output) {
		myRobot.drive(output, 0); //drive robot from PID output
	    }
    }
 
    public Robot() {
	//make objects for the sensor and drive train
	ultrasonic = new AnalogInput(ultrasonicChannel); 
	myRobot = new RobotDrive(new CANTalon(leftMotorChannel), new CANTalon(leftRearMotorChannel), 
		new CANTalon(rightMotorChannel), new CANTalon(rightRearMotorChannel));
	
	//ultrasonic (AnalogInput) can be used as a PIDSource without modification,
	//PIDOutput is an instance of the internal class MyPIDOutput made earlier
	pidController = new PIDController(pGain, iGain, dGain, ultrasonic, new MyPIDOutput());
    }

    /**
     * Runs during autonomous.
     */
    public void autonomous() {
    }

    /**
     * Drives the robot a set distance from an object using PID control and the 
     * ultrasonic sensor.
     */
    public void operatorControl() {
        pidController.setSetpoint(holdDistance*VoltsToInches); //set setpoint to 12 inches
        
        //Set expected range to 0-24 inches; e.g. at 24 inches from object go 
        //full forward, at 0 inches from object go full backward.
        pidController.setInputRange(0, 24*VoltsToInches); 
    
        while (isOperatorControl() && isEnabled()) {
            pidController.enable(); //begin PID control
        }
    }

    /**
     * Runs during test mode
     */
    public void test() {
    }
}
