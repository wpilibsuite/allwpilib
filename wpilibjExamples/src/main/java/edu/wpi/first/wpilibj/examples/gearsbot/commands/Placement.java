package edu.wpi.first.wpilibj.examples.gearsbot.commands;

import static edu.wpi.first.wpilibj.examples.gearsbot.Constants.Positions.*;

import edu.wpi.first.wpilibj.examples.gearsbot.Constants.Positions.Pickup;
import edu.wpi.first.wpilibj.examples.gearsbot.Constants.Positions.Place;
import edu.wpi.first.wpilibj.examples.gearsbot.subsystems.Claw;
import edu.wpi.first.wpilibj.examples.gearsbot.subsystems.Elevator;
import edu.wpi.first.wpilibj.examples.gearsbot.subsystems.Wrist;
import edu.wpi.first.wpilibj2.command.Command;

/** A factory class to generate pick and place commands */
public final class Placement {
  private final Claw m_claw;
  private final Wrist m_wrist;
  private final Elevator m_elevator;

  public Placement(Claw claw, Wrist wrist, Elevator elevator) {
    m_claw = claw;
    m_wrist = wrist;
    m_elevator = elevator;
  }

  /**
   * Pickup a soda can (if one is between the open claws) and get it in a safe state to drive
   * around.
   */
  public Command pickup() {
    return m_claw
        .close()
        .andThen(
            m_wrist
                .goTo(() -> Pickup.kWristSetpoint)
                .alongWith(m_elevator.goTo(() -> Pickup.kElevatorSetpoint)))
        .withName("pickup");
  }

  /** Place a held soda can onto the platform. */
  public Command place() {
    return m_elevator
        .goTo(() -> Place.kElevatorSetpoint)
        .andThen(m_wrist.goTo(() -> Place.kWristSetpoint))
        .andThen(m_claw.open())
        .withName("place");
  }

  /** Make sure the robot is in a state to pickup soda cans. */
  public Command prepareToPickup() {
    return m_claw
        .open()
        .andThen(
            m_wrist
                .goTo(() -> PrepareToPickup.kWristSetpoint)
                .alongWith(m_elevator.goTo(() -> PrepareToPickup.kElevatorSetpoint)))
        .withName("prepare pickup");
  }
}
