/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.driverstation.robotcoms;

/**
 *
 * @author Brendan
 */
public enum ControlMode {
    AUTO("AUTO"), TELEOP("TELEOP"), TEST_MODE("TEST");
    private String name;

    private ControlMode(String name) {
        this.name = name;
    }

    @Override
    public String toString() {
        return name; //To change body of generated methods, choose Tools | Templates.
    }
    
}
