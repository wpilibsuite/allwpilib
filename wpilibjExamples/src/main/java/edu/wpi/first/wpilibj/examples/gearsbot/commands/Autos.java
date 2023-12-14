package edu.wpi.first.wpilibj.examples.gearsbot.commands;

import static edu.wpi.first.wpilibj.examples.gearsbot.Constants.AutoConstants.*;

import edu.wpi.first.wpilibj.examples.gearsbot.subsystems.Claw;
import edu.wpi.first.wpilibj.examples.gearsbot.subsystems.Drivetrain;
import edu.wpi.first.wpilibj.examples.gearsbot.subsystems.Elevator;
import edu.wpi.first.wpilibj.examples.gearsbot.subsystems.Wrist;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.Commands;

/** A factory class to generate autonomous commands */
public final class Autos {
  private final Drivetrain m_drivetrain;
  private final Wrist m_wrist;
  private final Claw m_claw;
  private final Placement m_placement;

  public Autos(Drivetrain drivetrain, Elevator elevator, Wrist wrist, Claw claw) {
    m_drivetrain = drivetrain;
    m_wrist = wrist;
    m_claw = claw;
    m_placement = new Placement(claw, wrist, elevator);
  }

  /** The main autonomous command to pickup and deliver the soda to the box. */
  public Command deliverSoda() {
    return Commands.sequence(
        m_placement.prepareToPickup(),
        m_placement.pickup(),
        m_drivetrain.driveDistanceFromObstacle(kDistToBox1),
        // m_drivetrain.driveDistanceEncoders(4), // Use encoders if ultrasonic is broken
        m_placement.place(),
        m_drivetrain.driveDistanceFromObstacle(kDistToBox2),
        // m_drivetrain.driveDistanceEncoders(-2), // Use encoders if ultrasonic is broken
        m_wrist.goTo(() -> kWristSetpoint).asProxy().alongWith(m_claw.close()));
  }
}
