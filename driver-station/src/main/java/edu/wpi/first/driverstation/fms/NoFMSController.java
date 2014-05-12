/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.driverstation.fms;

import edu.wpi.first.driverstation.robotcoms.ControlMode;

/**
 *
 * @author Brendan
 */
public class NoFMSController implements FMSInterface {

	@Override
	public char getAllianceColor() {
		return 'B';
	}

	@Override
	public char getDSNumber() {
		return '1';
	}

	@Override
	public boolean isFMSConnected() {
		return false;
	}

	@Override
	public boolean isEnabled() {
		return false;
	}

	@Override
	public ControlMode getControlMode() {
		return ControlMode.TELEOP;
	}

	@Override
	public void updateBattery(char[] batteryData) {
		return;
	}

	@Override
	public void updateRobotFeedback(boolean[] fromRobot) {
		return;
	}

	@Override
	public void setRobotConnected(boolean connected) {
		return;
	}
}
