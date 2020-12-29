/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package frc.robot.commands;
import edu.wpi.first.wpilibj2.command.CommandBase;
import frc.robot.subsystems.Drivetrain;

/*
 * Creates a new TurnTime command. This command will turn your robot for a 
 * desired rotational speed and time.
 */
public class TurnTime extends CommandBase {
    private final double m_duration;
    private final double m_rotational_speed;
    private final Drivetrain m_drive;
    private long m_startTime;

   /**
   * Creates a new TurnTime.
   *
   * @param speed   The speed which the robot will turn. Negative is in reverse.
   * @param duration_in_seconds   How long to turn for.
   * @param drive   The drive subsystem on which this command will run
   */
    public TurnTime(double speed, double duration_in_seconds, Drivetrain drive) {
        m_rotational_speed = speed;
        m_duration = duration_in_seconds * 1000;
        m_drive = drive;
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
        m_drive.arcadeDrive(0, m_rotational_speed);
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



