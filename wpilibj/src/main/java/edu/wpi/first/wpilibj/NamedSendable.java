package edu.wpi.first.wpilibj;


/**
 * The interface for sendable objects that gives the sendable a default name in the Smart Dashboard
 *
 */
public interface NamedSendable extends Sendable {

    /**
     * @return the name of the subtable of SmartDashboard that the Sendable object will use
     */
    public String getName();
}
