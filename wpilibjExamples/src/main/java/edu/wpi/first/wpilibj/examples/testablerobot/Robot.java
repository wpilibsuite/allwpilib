/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.testablerobot;

import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.DigitalOutput;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.buttons.Button;
import edu.wpi.first.wpilibj.buttons.JoystickButton;
import edu.wpi.first.wpilibj.command.Command;
import edu.wpi.first.wpilibj.command.Scheduler;
import edu.wpi.first.wpilibj.smartdashboard.SendableChooser;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

import edu.wpi.first.wpilibj.examples.testablerobot.commandgroups.SayHelloInTurnCommandGroup;
import edu.wpi.first.wpilibj.examples.testablerobot.commands.SayHelloCommand;
import edu.wpi.first.wpilibj.examples.testablerobot.commands.SayHelloUntilSilencedCommand;
import edu.wpi.first.wpilibj.examples.testablerobot.subsystems.HelloWorldSubsystem;
import edu.wpi.first.wpilibj.examples.testablerobot.subsystems.SilenceableHelloWorldSubsystem;

/**
 * The VM is configured to automatically run this class, and to call the
 * functions corresponding to each mode, as described in the TimedRobot
 * documentation. If you change the name of this class or the package after
 * creating this project, you must also update the build.gradle file in the
 * project.
 * 
 * <p>This is a typical hello world type robot program that flashes an LED0 when button 1
 * is pressed on the default joystick.
 * 
 * <p>It also demonstrates a sequential command group. When button 2 is pressed LED0 
 * will flash. When DigitalInput 3 is asserted, then LED1 will flash and LED0 will
 * turn off. Don't forget your dropping resistors if you hook up LEDs.
 * 
 * <p>This project is based on the CommandRobot VSCode template.  It has been altered 
 * to enable you to test all commands, command groups, and subsystems. 
 * Note the the Robot class itself is not *yet* easily testable. More advanced
 * techniques are called for to get to that stage. But, 
 * when you run "./gradlew build", marvel at the test task output as it tests your
 * subsystems, commands, and commandgroups without a roboRio and/or complex simulators.
 */
public class Robot extends TimedRobot {
  // Declare hardware
  // Declare subsystems
  // Declare commands
  private final SayHelloCommand m_sayHelloCommand;
  // Declare command groups
  private final SayHelloInTurnCommandGroup m_sayHelloCommandGroup;
  // Declare operator interface
  private final Button m_simpleButton;
  private final Button m_commandGroupButton;

  Command m_autonomousCommand;
  SendableChooser<Command> m_chooser = new SendableChooser<>();

  /**
   * Example testable command robot. This will flash a hello world led on digital IO
   * port 0 when the first button is held down on the first joysitck found. This robot
   * class itself is not yet testable, but all commands and subsystems are.
   */
  public Robot() {
    super();
    // Simple hello world
    HelloWorldSubsystem helloWorldSubsystem = 
        new HelloWorldSubsystem(new DigitalOutput(RobotMap.perpetualLED));
    this.m_sayHelloCommand = new SayHelloCommand(helloWorldSubsystem);
    Joystick stick = new Joystick(RobotMap.joystickPort);
    this.m_simpleButton = new JoystickButton(stick, RobotMap.simpleButtonNumber);

    // Additional components for command group demonstration
    SilenceableHelloWorldSubsystem silenceableHelloWorldSubsystem = 
        new SilenceableHelloWorldSubsystem(
            new DigitalOutput(RobotMap.silenceableLED), 
            new DigitalInput(RobotMap.silencer));
    SayHelloUntilSilencedCommand sayHelloUntilSilencedCommand = 
        new SayHelloUntilSilencedCommand(silenceableHelloWorldSubsystem);
    this.m_sayHelloCommandGroup =
        new SayHelloInTurnCommandGroup(sayHelloUntilSilencedCommand, m_sayHelloCommand);
    this.m_commandGroupButton = new JoystickButton(stick, RobotMap.commandGroupButtonNumber);
  }

  /**
   * Instead of using the OI class in the original template, simply add a
   * method here to contain all that logic.
   */
  public void wireUpOperatorInterface() {
    m_simpleButton.whileHeld(m_sayHelloCommand);
    m_commandGroupButton.whileHeld(m_sayHelloCommandGroup);
  }

  /**
   * This function is run when the robot is first started up and should be
   * used for any initialization code.
   */
  @Override
  public void robotInit() {
    wireUpOperatorInterface();
    m_chooser.setDefaultOption("Default Auto", m_sayHelloCommand);
    // chooser.addObject("My Auto", new MyAutoCommand());
    SmartDashboard.putData("Auto mode", m_chooser);
  }

  /**
   * This function is called every robot packet, no matter the mode. Use
   * this for items like diagnostics that you want ran during disabled,
   * autonomous, teleoperated and test.
   *
   * <p>This runs after the mode specific periodic functions, but before
   * LiveWindow and SmartDashboard integrated updating.
   */
  @Override
  public void robotPeriodic() {
  }

  /**
   * This function is called once each time the robot enters Disabled mode.
   * You can use it to reset any subsystem information you want to clear when
   * the robot is disabled.
   */
  @Override
  public void disabledInit() {
  }

  @Override
  public void disabledPeriodic() {
    Scheduler.getInstance().run();
  }

  /**
   * This autonomous (along with the chooser code above) shows how to select
   * between different autonomous modes using the dashboard. The sendable
   * chooser code works with the Java SmartDashboard. If you prefer the
   * LabVIEW Dashboard, remove all of the chooser code and uncomment the
   * getString code to get the auto name from the text box below the Gyro
   *
   * <p>You can add additional auto modes by adding additional commands to the
   * chooser code above (like the commented example) or additional comparisons
   * to the switch structure below with additional strings & commands.
   */
  @Override
  public void autonomousInit() {
    m_autonomousCommand = m_chooser.getSelected();

    /*
     * String autoSelected = SmartDashboard.getString("Auto Selector",
     * "Default"); switch(autoSelected) { case "My Auto": autonomousCommand
     * = new MyAutoCommand(); break; case "Default Auto": default:
     * autonomousCommand = new ExampleCommand(); break; }
     */

    // schedule the autonomous command (example)
    if (m_autonomousCommand != null) {
      m_autonomousCommand.start();
    }
  }

  /**
   * This function is called periodically during autonomous.
   */
  @Override
  public void autonomousPeriodic() {
    Scheduler.getInstance().run();
  }

  @Override
  public void teleopInit() {
    // This makes sure that the autonomous stops running when
    // teleop starts running. If you want the autonomous to
    // continue until interrupted by another command, remove
    // this line or comment it out.
    if (m_autonomousCommand != null) {
      m_autonomousCommand.cancel();
    }
  }

  /**
   * This function is called periodically during operator control.
   */
  @Override
  public void teleopPeriodic() {
    Scheduler.getInstance().run();
  }

  /**
   * This function is called periodically during test mode.
   */
  @Override
  public void testPeriodic() {
  }
}
