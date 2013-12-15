package edu.wpi.first.testing;

import edu.wpi.first.wpilibj.Servo;
import edu.wpi.first.wpilibj.Timer;

/**
 * Servo which moves very slowly.
 *
 * In order to support testing a gyroscope using a hobby servo, we must slow
 * down the turn rate of the hobby servo. Otherwise, the hobby servo would
 * rotate at a speed much higher than that that the gyroscope can measure.
 *
 * @author pmalmsten
 */
public class SlowServo extends Servo {

    public SlowServo(int port) {
        super(port);
    }

    public SlowServo(int module, int port) {
        super(module, port);
    }

    /**
     * Change the servo position moving at a slow rate of speed and block
     * until it gets there.
     *
     * Servo values range from 0.0 to 1.0 corresponding to the range of full left to full right.
     *
     * @param value Position from 0.0 to 1.0.
     */
    public void setPosition(double position) {
        double posToGo = position - getPosition();

        for(int i = 0; i < Math.floor(Math.abs(posToGo) / 0.05); i++) {
            // Apply step
            super.setPosition(getPosition() + ((posToGo > 0)? 0.05:-0.05));

            System.out.println("     At: " + getPosition() + " Going to: " + posToGo);

            // Wait
            Timer.delay(0.5);
        }

        // Apply remainder
        super.setPosition(position);

        System.out.println("     At: " + getPosition() + " Going to: " + posToGo);

        // Wait
        Timer.delay(0.5);
    }

    /**
     * Set the servo position directly and does not block. Should not be used
     * during testing (but may be useful for preparing a test).
     *
     * Servo values range from 0.0 to 1.0 corresponding to the range of full left to full right.
     *
     * @param value Position from 0.0 to 1.0.
     */
    public void fastSet(double position) {
        super.set(position);
    }
}
