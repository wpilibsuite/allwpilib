package wpilib.robot;

import org.wpilib.driverstation.DefaultDriverStation;
import org.wpilib.opmode.PeriodicOpMode;
import org.wpilib.opmode.Teleop;

@Teleop()
public class TeleopOpMode extends PeriodicOpMode {

    public TeleopOpMode(Robot robot, DefaultDriverStation ds) {
        System.out.println("TeleopOpMode initialized");
        System.out.println("Robot: " + robot);
        System.out.println("DriverStation: " + ds);
    }

    @Override
    public void periodic() {
    }

}
