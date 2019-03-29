package edu.wpi.first.wpilibj.examples.frisbeebot.subsystems;

import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.Spark;
import edu.wpi.first.wpilibj.experimental.command.SynchronousPIDSubsystem;
import edu.wpi.first.wpilibj.experimental.controller.PIDController;

import static edu.wpi.first.wpilibj.examples.frisbeebot.Constants.ShooterConstants.*;

public class ShooterSubsystem extends SynchronousPIDSubsystem {

  private Spark m_shooterMotor = new Spark(kShooterMotorPort);
  private Spark m_feederMotor = new Spark(kFeederMotorPort);

  private double m_reference;

  public ShooterSubsystem() {

    //TODO: Fix this to actually use encoder distance per pulse; requires changes to PIDController
    super(new PIDController(kP, kI, kD,
        new Encoder(kEncoderPorts[0], kEncoderPorts[1], kEncoderReversed)::getRate));
  }

  @Override
  public void useOutput(double output) {
    m_shooterMotor.set(output);
  }

  @Override
  public double getReference() {
    return kShooterRPS;
  }

  public boolean atReference() {
    return m_controller.atReference();
  }

  public void runFeeder() {
    m_feederMotor.set(kFeederSpeed);
  }

  public void stopFeeder() {
    m_feederMotor.set(0);
  }
}
