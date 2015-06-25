
package $package;

import edu.wpi.first.wpilibj.AnalogInput;
import edu.wpi.first.wpilibj.PIDController;
import edu.wpi.first.wpilibj.SampleRobot;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.Victor;

/**
 * This is a sample program to demonstrate how to use a soft potentiometer and a PID
 * controller to reach and maintain position setpoints on an elevator mechanism.
 *  
 * WARNING: While it may look like a good choice to use for your code if you're inexperienced,
 * don't. Unless you know what you are doing, complex code will be much more difficult under
 * this system. Use IterativeRobot or Command-Based instead if you're new.  
 */
public class Robot extends SampleRobot {
    final int potChannel = 1; //analog input pin
    final int motorChannel = 7; //PWM channel 
    final int joystickChannel = 0; //usb number in DriverStation
    final int buttonNumber = 4; //button on joystick
    
    final double setPoints[] = {1.0, 2.6, 4.3}; //bottom, middle, and top elevator setpoints
     
    //proportional, integral, and derivative speed constants; motor inverted 
    //DANGER: when tuning PID constants, high/inappropriate values for pGain, iGain,
    //and dGain may cause dangerous, uncontrollable, or undesired behavior!
    final double pGain = -5.0, iGain = -0.02, dGain = -2.0; //these may need to be positive for a non-inverted motor
  
    PIDController pidController;
    AnalogInput potentiometer;  
    Victor elevatorMotor;
    Joystick joystick;

    public Robot() {
	//make objects for potentiometer, the elevator motor controller, and the joystick
	potentiometer = new AnalogInput(potChannel);
	elevatorMotor = new Victor(motorChannel);
	joystick = new Joystick(joystickChannel);
	
	//potentiometer (AnalogInput) and elevatorMotor (Victor) can be used as a 
	//PIDSource and PIDOutput respectively 
	pidController = new PIDController(pGain, iGain, dGain, potentiometer, elevatorMotor);
    }

    /**
     * Runs during autonomous.
     */
    public void autonomous() {

    }

    /**
     *  Uses a PIDController and an array of setpoints to switch and maintain elevator 
     *  positions. The elevator setpoint is selected by a joystick button.
     */
    public void operatorControl() {
	pidController.setInputRange(0, 5); //0 to 5V 
	pidController.setSetpoint(setPoints[0]); //set to first setpoint
	
	int index = 0;
	boolean currentValue;
	boolean previousValue = false;
	
	while (isOperatorControl() && isEnabled()) {
            pidController.enable(); //begin PID control
            
            //when the button is pressed once, the selected elevator setpoint is incremented 
            currentValue = joystick.getRawButton(buttonNumber);
            if(currentValue && !previousValue){
        	pidController.setSetpoint(setPoints[index]);
        	index = (index + 1) % setPoints.length; //index of elevator setpoint wraps around
            }   
            previousValue = currentValue;
        }
    }

    /**
     * Runs during test mode.
     */
    public void test() {
    
    }
}
