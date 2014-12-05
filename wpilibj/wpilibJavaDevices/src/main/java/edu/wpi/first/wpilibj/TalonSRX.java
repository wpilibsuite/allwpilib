/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

/**
 * Cross the Road Electronics (CTRE) Talon SRX Speed Controller with PWM control
 * @see CANTalon CANTalon for CAN control of Talon SRX
 */
public class TalonSRX extends Talon {
    /**
     * Constructor.
     *
     * @param channel The PWM channel that the Talon SRX is attached to.
     */
    public TalonSRX(final int channel) {
        super(channel);
    }
}
