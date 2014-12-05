/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

/**
 * Vex Robotics Victor SP Speed Controller
 */
public class VictorSP extends Talon {
	
    /**
     * Constructor.
     *
     * @param channel The PWM channel that the VictorSP is attached to.
     */
    public VictorSP(final int channel) {
        super(channel);
    }

}
