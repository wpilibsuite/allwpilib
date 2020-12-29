/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.romireference.commands;
import edu.wpi.first.wpilibj2.command.CommandBase;
import edu.wpi.first.wpilibj.examples.romireference.subsystems.Drivetrain;

/*
 * Creates a new DriveTime command. This command will drive your robot for a 
 * desired speed and time.
 */
public class DriveTime extends CommandBase {
    private final double m_duration;
    private final double m_speed;
    private final Drivetrain m_drive;
    private long m_startTime;

    /**
     * Creates a new DriveTime.
     *
     * @param speed   The speed which the robot will drive. Negative is in reverse.
     * @param time   How long to turn for.
     * @param drive   The drivetrain subsystem on which this command will run
    */
    public DriveTime(double speed, double time, Drivetrain drive) {
        m_speed = speed;
        m_duration = time * 1000;
        m_drive = drive;
        addRequirements(drive);
    }

    // Called when the command is initially scheduled.
    @Override
    public void initialize() {
        m_startTime = System.currentTimeMillis();
        m_drive.arcadeDrive(0, 0);
    }

    // Called every time the scheduler runs while the command is scheduled.
    @Override
    public void execute() {
        m_drive.arcadeDrive(m_speed, 0);
    }

    // Called once the command ends or is interrupted.
    @Override
    public void end(boolean interrupted) {
        m_drive.arcadeDrive(0, 0);
    }

    // Returns true when the command should end.
    @Override
    public boolean isFinished() {
        return (System.currentTimeMillis() - m_startTime) >= m_duration;
    }

}
