/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.pacgoat.triggers;

import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.buttons.Trigger;

/**
 * A custom button that is triggered when two buttons on a Joystick are
 * simultaneously pressed.
 */
public class DoubleButton extends Trigger {
	private Joystick m_joy;
	private int m_button1;
	private int m_button2;

	public DoubleButton(Joystick joy, int button1, int button2) {
		this.m_joy = joy;
		this.m_button1 = button1;
		this.m_button2 = button2;
	}

	@Override
	public boolean get() {
		return m_joy.getRawButton(m_button1) && m_joy.getRawButton(m_button2);
	}
}
