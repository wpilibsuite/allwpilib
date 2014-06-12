/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package $package.subsystems;

import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.SpeedController;
import edu.wpi.first.wpilibj.Victor;
import edu.wpi.first.wpilibj.command.Subsystem;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;

/**
 * The claw subsystem is a simple system with a motor for opening and closing.
 * If using stronger motors, you should probably use a sensor so that the
 * motors don't stall. 
 */
public class Claw extends Subsystem {
    private SpeedController motor;
    private DigitalInput contact;

    public Claw() {
        super();
        motor = new Victor(7);
        contact = new DigitalInput(5);

		// Let's show everything on the LiveWindow
        LiveWindow.addActuator("Claw", "Motor", (Victor) motor);
        LiveWindow.addActuator("Claw", "Limit Switch", contact);
    }

    public void initDefaultCommand() {}
    public void log() {}
    
    /**
     * Set the claw motor to move in the open direction.
     */
    public void open() {
        motor.set(-1);
    }

    /**
     * Set the claw motor to move in the close direction.
     */
    public void close() {
        motor.set(1);
    }
    
    /**
     * Stops the claw motor from moving.
     */
    public void stop() {
        motor.set(0);
    }
    
    /**
     * Return true when the robot is grabbing an object hard enough
     * to trigger the limit switch.
     */
    public boolean isGrabbing() {
    	return contact.get();
    }
}
