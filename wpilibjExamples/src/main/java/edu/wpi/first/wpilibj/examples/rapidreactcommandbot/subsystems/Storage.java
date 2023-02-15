// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.rapidreactcommandbot.subsystems;

import static edu.wpi.first.wpilibj.examples.rapidreactcommandbot.Constants.StorageConstants;

import com.fasterxml.jackson.databind.node.JsonNodeFactory;
import edu.wpi.first.wpilibj.shuffleboard.BuiltInWidgets;
import edu.wpi.first.wpilibj.shuffleboard.Shuffleboard;
import edu.wpi.first.wpilibj.telemetry.CommonWidgets;
import edu.wpi.first.wpilibj.telemetry.TelemetryNode;
import edu.wpi.first.wpilibj.telemetry.TelemetryBuilder;
import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.motorcontrol.PWMSparkMax;
import edu.wpi.first.wpilibj.util.Color;
import edu.wpi.first.wpilibj2.command.CommandBase;
import edu.wpi.first.wpilibj2.command.SubsystemBase;

public class Storage extends SubsystemBase implements TelemetryNode {
  private final PWMSparkMax m_motor = new PWMSparkMax(StorageConstants.kMotorPort);
  private final DigitalInput m_ballSensor = new DigitalInput(StorageConstants.kBallSensorPort);

  /** Create a new Storage subsystem. */
  public Storage() {
    // Set default command to turn off the storage motor and then idle
    setDefaultCommand(runOnce(m_motor::disable).andThen(run(() -> {})).withName("Idle"));
    Shuffleboard.getTab("Stowing").addBoolean("isNotEmpty", this::isFull).withWidget(BuiltInWidgets.kToggleSwitch);
  }

  /** Whether the ball storage is full. */
  public boolean isFull() {
    return m_ballSensor.get();
  }

  /** Returns a command that runs the storage motor indefinitely. */
  public CommandBase runCommand() {
    return run(() -> m_motor.set(1)).withName("run");
  }

  @Override
  public void declareTelemetry(TelemetryBuilder builder) {
    builder.selfMetadata(() -> JsonNodeFactory.instance.objectNode().put("type", "ShuffleboardTab"));
    builder.publishBoolean("isFull", this::isFull).withWidget(CommonWidgets.booleanBox(Color.kAzure, Color.kBlueViolet));
  }
}
