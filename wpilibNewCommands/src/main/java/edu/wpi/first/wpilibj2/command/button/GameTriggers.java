package edu.wpi.first.wpilibj2.command.button;

import edu.wpi.first.wpilibj.DriverStation;

public class GameTriggers {
    // TODO: Add documentation
    // Utility class
    private GameTriggers() { }

    public static Trigger autonomous() {
        return new Trigger(DriverStation::isAutonomousEnabled);
    }

    public static Trigger teleop() {
        return new Trigger(DriverStation::isTeleopEnabled);
    }

    public static Trigger disabled() {
        return new Trigger(DriverStation::isDisabled);
    }
}
