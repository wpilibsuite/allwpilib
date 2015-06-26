
package $package;

import edu.wpi.first.wpilibj.CANTalon;
import edu.wpi.first.wpilibj.Gyro;
import edu.wpi.first.wpilibj.SampleRobot;
import edu.wpi.first.wpilibj.RobotDrive;
import edu.wpi.first.wpilibj.Joystick;

/**
 * This is a sample program to demonstrate how to use a gyro sensor to make a robot drive 
 * straight. This program uses a joystick to drive forwards and backwards while the gyro
 * is used for direction keeping.
 * 
 * WARNING: While it may look like a good choice to use for your code if you're inexperienced,
 * don't. Unless you know what you are doing, complex code will be much more difficult under
 * this system. Use IterativeRobot or Command-Based instead if you're new.
 */
public class Robot extends SampleRobot {

    final int gyroChannel = 0; //analog input
    final int joystickChannel = 0; //usb number in DriverStation
    
    //channels for motors
    final int leftMotorChannel = 1;
    final int rightMotorChannel = 0;
    final int leftRearMotorChannel = 3;
    final int rightRearMotorChannel = 2;
    
    double angleSetpoint = 0.0;
    final double pGain = .006; //propotional turning constant
    
    //gyro calibration constant, may need to be adjusted; 
    //gyro value of 360 is set to correspond to one full revolution
    final double voltsPerDegreePerSecond = .0128; 
      	
    RobotDrive myRobot;
    Gyro gyro;
    Joystick joystick;

    public Robot()
    {
	//make objects for the drive train, gyro, and joystick
	myRobot = new RobotDrive(new CANTalon(leftMotorChannel), new CANTalon(
		leftRearMotorChannel), new CANTalon(rightMotorChannel),
		new CANTalon(rightRearMotorChannel));
	gyro = new Gyro(gyroChannel);
	joystick = new Joystick(joystickChannel);
    }

    /**
     * Runs during autonomous.
     */
    public void autonomous() {
     
    }

    /**
     * Sets the gyro sensitivity and drives the robot when the joystick is pushed. The
     * motor speed is set from the joystick while the RobotDrive turning value is assigned
     * from the error between the setpoint and the gyro angle.
     */
    public void operatorControl() {
	double turningValue;
	gyro.setSensitivity(voltsPerDegreePerSecond); //calibrates gyro values to equal degrees
        while (isOperatorControl() && isEnabled()) {
            
            turningValue =  (angleSetpoint - gyro.getAngle())*pGain;
            if(joystick.getY() <= 0)
            {
        	//forwards
        	myRobot.drive(joystick.getY(), turningValue); 
            } else {
        	//backwards
        	myRobot.drive(joystick.getY(), -turningValue); 
            }
        }
    }

    /**
     * Runs during test mode.
     */
    public void test(){
	
    }
}
