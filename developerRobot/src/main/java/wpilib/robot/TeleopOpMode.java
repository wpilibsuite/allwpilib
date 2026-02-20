// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package wpilib.robot;

import org.wpilib.driverstation.DefaultUserControls;
import org.wpilib.opmode.PeriodicOpMode;
import org.wpilib.opmode.Teleop;

@Teleop()
public class TeleopOpMode extends PeriodicOpMode {
    public TeleopOpMode(Robot robot, DefaultUserControls ds) {
        System.out.println("TeleopOpMode initialized");
        System.out.println("Robot: " + robot);
        System.out.println("DriverStation: " + ds);
    }

    @Override
    public void periodic() {
    }

}
