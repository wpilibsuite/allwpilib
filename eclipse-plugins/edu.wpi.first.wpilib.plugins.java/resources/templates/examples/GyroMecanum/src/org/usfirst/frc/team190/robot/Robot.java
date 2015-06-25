
package $package;

import edu.wpi.first.wpilibj.CANTalon;
import edu.wpi.first.wpilibj.Gyro;
import edu.wpi.first.wpilibj.SampleRobot;
import edu.wpi.first.wpilibj.RobotDrive;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.RobotDrive.MotorType;

/**
 * This is a sample program that uses mecanum drive with a gyro sensor to maintian 
 * rotation vectorsin relation to the starting orientation of the robot (field-oriented controls).
 * 
 * WARNING: While it may look like a good choice to use for your code if you're inexperienced,
 * don't. Unless you know what you are doing, complex code will be much more difficult under
 * this system. Use IterativeRobot or Command-Based instead if you're new.
 */
public class Robot extends SampleRobot {
    RobotDrive myRobot;
    Joystick joystick;
    Gyro gyro;
    
    //channels for motors
    final int leftMotorChannel = 1;
    final int rightMotorChannel = 0;
    final int leftRearMotorChannel = 3;
    final int rightRearMotorChannel = 2;
    
    final int gyroChannel = 0; //analog input
  
    //gyro calibration constant, may need to be adjusted so that a gyro value of 360
    //equals 360 degrees
    final double voltsPerDegreePerSecond = .0128; 
    
    public Robot() {
	//make objects for drive train, joystick, and gyro
	myRobot = new RobotDrive(new CANTalon(leftMotorChannel), new CANTalon(leftRearMotorChannel), 
		new CANTalon(rightMotorChannel), new CANTalon(rightRearMotorChannel));
	myRobot.setInvertedMotor(MotorType.kFrontLeft, true); // invert the left side motors
	myRobot.setInvertedMotor(MotorType.kRearLeft, true); // you may need to change or remove this to match your robot
	    	
        joystick = new Joystick(0);
        gyro = new Gyro(gyroChannel);
    }

    /**
     * Runs during autonomous.
     */
    public void autonomous() {
       
    }

    /**
     * Gyro sensitivity is set and mecanum drive is used with the gyro angle as an input.
     */
    public void operatorControl() {
	gyro.setSensitivity(voltsPerDegreePerSecond); //calibrate gyro to have the value equal to degrees
        while (isOperatorControl() && isEnabled()) {  
            myRobot.mecanumDrive_Cartesian(joystick.getX(), joystick.getY(), joystick.getZ(), gyro.getAngle());
            Timer.delay(0.005);	// wait 5ms to avoid hogging CPU cycles
        }
    }

    /**
     * Runs during test mode
     */
    public void test() {
    }
}
