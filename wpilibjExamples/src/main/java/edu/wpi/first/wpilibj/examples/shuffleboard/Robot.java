package edu.wpi.first.wpilibj.examples.shuffleboard;

import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableType;
import edu.wpi.first.wpilibj.AnalogPotentiometer;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.IterativeRobot;
import edu.wpi.first.wpilibj.Spark;
import edu.wpi.first.wpilibj.drive.DifferentialDrive;
import edu.wpi.first.wpilibj.shuffleboard.Shuffleboard;

//import java.util.Map;

@SuppressWarnings({"MemberName", "LineLength", "RegexpSinglelinejava"})
public class Robot extends IterativeRobot {

  private final DifferentialDrive tankDrive = new DifferentialDrive(new Spark(0), new Spark(1));
  private final Encoder leftEncoder = new Encoder(0, 1);
  private final Encoder rightEncoder = new Encoder(2, 3);

  private final Spark elevatorMotor = new Spark(2);
  private final AnalogPotentiometer elevatorPot = new AnalogPotentiometer(0);
  private NetworkTableEntry maxSpeed;

  @Override
  public void robotInit() {
    maxSpeed = Shuffleboard.add("Max Speed", NetworkTableType.kDouble)
        .toTab("Configuration")
        .withWidget("Number Slider")
        //.withProperties(Map.of("min", 0, "max", 1)) // Map.of introduced in Java 9
        .getEntry();

    Shuffleboard.add("Tank Drive", tankDrive)
        .toTab("Drivebase");
    Shuffleboard.add("Left Encoder", leftEncoder)
        .toTab("Drivebase")
        .toLayout("List", "Encoders");
    Shuffleboard.add("Right Encoder", rightEncoder)
        .toTab("Drivebase")
        .toLayout("List", "Encoders");

    Shuffleboard.add("Motor", elevatorMotor)
        .toTab("Elevator");
    Shuffleboard.add("Potentiometer", elevatorPot)
        .toTab("Elevator")
        .withWidget("Voltage View");
  }

  @Override
  public void autonomousInit() {
    tankDrive.setMaxOutput(maxSpeed.getDouble(1.0));
  }

  @Override
  public void robotPeriodic() {
    Shuffleboard.update();
  }

}
