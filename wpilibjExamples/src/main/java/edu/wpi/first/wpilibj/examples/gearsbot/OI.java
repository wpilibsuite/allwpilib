/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.gearsbot;

import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.buttons.JoystickButton;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

import edu.wpi.first.wpilibj.examples.gearsbot.commands.Autonomous;
import edu.wpi.first.wpilibj.examples.gearsbot.commands.CloseClaw;
import edu.wpi.first.wpilibj.examples.gearsbot.commands.OpenClaw;
import edu.wpi.first.wpilibj.examples.gearsbot.commands.Pickup;
import edu.wpi.first.wpilibj.examples.gearsbot.commands.Place;
import edu.wpi.first.wpilibj.examples.gearsbot.commands.PrepareToPickup;
import edu.wpi.first.wpilibj.examples.gearsbot.commands.SetElevatorSetpoint;
import edu.wpi.first.wpilibj.examples.gearsbot.commands.SetWristSetpoint;

/**
 * This class is the glue that binds the controls on the physical operator
 * interface to the commands and command groups that allow control of the robot.
 */
public class OI {
  private final Joystick m_joystick = new Joystick(0);

  /**
   * Construct the OI and all of the buttons on it.
   */
  public OI() {
    // Put Some buttons on the SmartDashboard
    SmartDashboard.putData("Elevator Bottom", new SetElevatorSetpoint(0));
    SmartDashboard.putData("Elevator Platform", new SetElevatorSetpoint(0.2));
    SmartDashboard.putData("Elevator Top", new SetElevatorSetpoint(0.3));

    SmartDashboard.putData("Wrist Horizontal", new SetWristSetpoint(0));
    SmartDashboard.putData("Raise Wrist", new SetWristSetpoint(-45));

    SmartDashboard.putData("Open Claw", new OpenClaw());
    SmartDashboard.putData("Close Claw", new CloseClaw());

    SmartDashboard.putData("Deliver Soda", new Autonomous());

    // Create some buttons
    final JoystickButton dpadUp = new JoystickButton(m_joystick, 5);
    final JoystickButton dpadRight = new JoystickButton(m_joystick, 6);
    final JoystickButton dpadDown = new JoystickButton(m_joystick, 7);
    final JoystickButton dpadLeft = new JoystickButton(m_joystick, 8);
    final JoystickButton l2 = new JoystickButton(m_joystick, 9);
    final JoystickButton r2 = new JoystickButton(m_joystick, 10);
    final JoystickButton l1 = new JoystickButton(m_joystick, 11);
    final JoystickButton r1 = new JoystickButton(m_joystick, 12);

    // Connect the buttons to commands
    dpadUp.whenPressed(new SetElevatorSetpoint(0.2));
    dpadDown.whenPressed(new SetElevatorSetpoint(-0.2));
    dpadRight.whenPressed(new CloseClaw());
    dpadLeft.whenPressed(new OpenClaw());

    r1.whenPressed(new PrepareToPickup());
    r2.whenPressed(new Pickup());
    l1.whenPressed(new Place());
    l2.whenPressed(new Autonomous());
  }

  public Joystick getJoystick() {
    return m_joystick;
  }
}
