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
public interface FMSInterface {
    
    public char getAllianceColor();
    public char getDSNumber();
    public boolean isFMSConnected();
    public boolean isEnabled();
    public ControlMode getControlMode();
    public void updateBattery(char[] batteryData);
    public void updateRobotFeedback(boolean[] fromRobot);
    public void setRobotConnected(boolean connected);
    
    
}
