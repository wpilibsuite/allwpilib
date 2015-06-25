
package $package;

import edu.wpi.first.wpilibj.AnalogInput;
import edu.wpi.first.wpilibj.SampleRobot;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.Victor;

/**
 * This is a sample program to demonstrate the use of a soft potentiometer and proportional 
 * control to reach and maintain position setpoints on an elevator mechanism. A joystick 
 * button is used to switch elevator setpoints.
 * 
 * WARNING: While it may look like a good choice to use for your code if you're inexperienced,
 * don't. Unless you know what you are doing, complex code will be much more difficult under
 * this system. Use IterativeRobot or Command-Based instead if you're new. 
 */
public class Robot extends SampleRobot {
    final int potChannel = 1; //analog input pin
    final int motorChannel = 7; //PWM channel 
    final int joystickChannel = 0; //usb number in DriverStation
    final int buttonNumber = 4; //joystick button
    
    final double setpoints[] = {1.0, 2.6, 4.3}; //bottom, middle, and top elevator setpoints

    final double pGain = 1.0; //proportional speed constant 
    double motorSpeed;
    double currentPosition; //sensor voltage reading corresponding to current elevator position
    
    AnalogInput potentiometer;  
    Victor elevatorMotor;
    Joystick joystick;
    
    public Robot() {
	//make objects for the potentiometer, elevator motor controller, and joystick
	potentiometer = new AnalogInput(potChannel);
	elevatorMotor = new Victor(motorChannel);
	joystick = new Joystick(joystickChannel);
    }

    /**
     * Runs during autonomous.
     */
    public void autonomous() {
    
    }

    /**
     * Moves elevator to a selectable setpoint that can be changed by pressing a button on
     * the joystick. Proportional control is used to reach and maintain the desired setpoint 
     * by obtaining values from the potentiometer and comparing them to the setpoint value. 
     */
    public void operatorControl() {
	boolean buttonState;
	boolean prevButtonState = false;
	
	int index = 0; //setpoint array index
	double currentSetpoint; //holds desired setpoint
	currentSetpoint = setpoints[0]; //set to first setpoint
	
        while (isOperatorControl() && isEnabled()) {    
            buttonState = joystick.getRawButton(buttonNumber); //check if button is pressed
            
            //if button has been pressed and released once 
            if(buttonState && !prevButtonState) { 
        	index = (index + 1) % setpoints.length; //increment set point, reset if at end of array
        	currentSetpoint = setpoints[index]; //set setpoint
            }
            prevButtonState = buttonState; //record previous button state
      
            currentPosition = potentiometer.getAverageVoltage(); //get position value
            motorSpeed = (currentPosition - currentSetpoint)*pGain; //convert position error to speed
            elevatorMotor.set(motorSpeed); //drive elevator motor
        }
    }

    /**
     * Runs during test mode
     */
    public void test() {
	
    }
}
