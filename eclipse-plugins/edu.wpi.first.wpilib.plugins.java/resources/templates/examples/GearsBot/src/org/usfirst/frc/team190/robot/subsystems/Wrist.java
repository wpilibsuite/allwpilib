/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package $package.subsystems;

import edu.wpi.first.wpilibj.AnalogPotentiometer;
import edu.wpi.first.wpilibj.SpeedController;
import edu.wpi.first.wpilibj.Victor;
import edu.wpi.first.wpilibj.command.PIDSubsystem;
import edu.wpi.first.wpilibj.interfaces.Potentiometer;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

import $package.Robot;

/**
 * The wrist subsystem is like the elevator, but with a rotational joint instead
 * of a linear joint. 
 */
public class Wrist extends PIDSubsystem {
    private SpeedController motor;
    private Potentiometer pot;

    private static final double kP_real = 1,
            kP_simulation = 0.05;
    
    public Wrist() {
        super(kP_real, 0, 0);
        if (Robot.isSimulation()) { // Check for simulation and update PID values
            getPIDController().setPID(kP_simulation, 0, 0, 0);
        }
        setAbsoluteTolerance(2.5);

        motor = new Victor(6);
        
        // Conversion value of potentiometer varies between the real world and simulation
        if (Robot.isReal()) {
            pot = new AnalogPotentiometer(3, -270.0/5);
        } else {
            pot = new AnalogPotentiometer(3); // Defaults to degrees
        }

		// Let's show everything on the LiveWindow
        LiveWindow.addActuator("Wrist", "Motor", (Victor) motor);
        LiveWindow.addSensor("Wrist", "Pot", (AnalogPotentiometer) pot);
        LiveWindow.addActuator("Wrist", "PID", getPIDController());
    }

    public void initDefaultCommand() {}

	/**
	 * The log method puts interesting information to the SmartDashboard.
	 */
    public void log() {
        SmartDashboard.putData("Wrist Angle", (AnalogPotentiometer) pot);
    }

    /**
     * Use the potentiometer as the PID sensor. This method is automatically
     * called by the subsystem.
     */
    protected double returnPIDInput() {
        return pot.get();
    }

    /**
     * Use the motor as the PID output. This method is automatically called by
     * the subsystem.
     */
    protected void usePIDOutput(double d) {
        motor.set(d);
    }
}
