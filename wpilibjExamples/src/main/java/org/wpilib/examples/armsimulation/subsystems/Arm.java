// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.armsimulation.subsystems;

import org.wpilib.drivers.motor.PWMSparkMax;
import org.wpilib.examples.armsimulation.Constants;
import org.wpilib.hardware.rotation.Encoder;
import org.wpilib.math.controller.PIDController;
import org.wpilib.math.system.DCMotor;
import org.wpilib.math.util.Units;
import org.wpilib.preferences.Preferences;
import org.wpilib.simulation.BatterySim;
import org.wpilib.simulation.EncoderSim;
import org.wpilib.simulation.RoboRioSim;
import org.wpilib.simulation.SingleJointedArmSim;
import org.wpilib.smartdashboard.Mechanism2d;
import org.wpilib.smartdashboard.MechanismLigament2d;
import org.wpilib.smartdashboard.MechanismRoot2d;
import org.wpilib.system.RobotController;
import org.wpilib.telemetry.Telemetry;
import org.wpilib.util.Color;
import org.wpilib.util.Color8Bit;

public class Arm implements AutoCloseable {
  // The P gain for the PID controller that drives this arm.
  private double armKp = Constants.DEFAULT_ARM_KP;
  private double armSetpointDegrees = Constants.DEFAULT_ARM_SETPOINT_DEGREES;

  // The arm gearbox represents a gearbox containing two Vex 775pro motors.
  private final DCMotor armGearbox = DCMotor.getVex775Pro(2);

  // Standard classes for controlling our arm
  private final PIDController controller = new PIDController(armKp, 0, 0);
  private final Encoder encoder =
      new Encoder(Constants.ENCODER_A_CHANNEL, Constants.ENCODER_B_CHANNEL);
  private final PWMSparkMax motor = new PWMSparkMax(Constants.MOTOR_PORT);

  // Simulation classes help us simulate what's going on, including gravity.
  // This arm sim represents an arm that can travel from -75 degrees (rotated down front)
  // to 255 degrees (rotated down in the back).
  private final SingleJointedArmSim armSim =
      new SingleJointedArmSim(
          armGearbox,
          Constants.ARM_REDUCTION,
          SingleJointedArmSim.estimateMOI(Constants.ARM_LENGTH, Constants.ARM_MASS),
          Constants.ARM_LENGTH,
          Constants.MIN_ANGLE_RADS,
          Constants.MAX_ANGLE_RADS,
          true,
          0,
          Constants.ARM_ENCODER_DIST_PER_PULSE,
          0.0 // Add noise with a std-dev of 1 tick
          );
  private final EncoderSim encoderSim = new EncoderSim(encoder);

  // Create a Mechanism2d display of an Arm with a fixed ArmTower and moving Arm.
  private final Mechanism2d mech2d = new Mechanism2d(60, 60);
  private final MechanismRoot2d armPivot = mech2d.getRoot("ArmPivot", 30, 30);
  private final MechanismLigament2d armTower =
      armPivot.append(new MechanismLigament2d("ArmTower", 30, -90));
  private final MechanismLigament2d arm =
      armPivot.append(
          new MechanismLigament2d(
              "Arm",
              30,
              Units.radiansToDegrees(armSim.getAngle()),
              6,
              new Color8Bit(Color.YELLOW)));

  /** Subsystem constructor. */
  public Arm() {
    encoder.setDistancePerPulse(Constants.ARM_ENCODER_DIST_PER_PULSE);

    armTower.setColor(new Color8Bit(Color.BLUE));

    // Set the Arm position setpoint and P constant to Preferences if the keys don't already exist
    Preferences.initDouble(Constants.ARM_POSITION_KEY, armSetpointDegrees);
    Preferences.initDouble(Constants.ARM_P_KEY, armKp);
  }

  /** Update the simulation model. */
  public void simulationPeriodic() {
    // In this method, we update our simulation of what our arm is doing
    // First, we set our "inputs" (voltages)
    armSim.setInput(motor.getThrottle() * RobotController.getBatteryVoltage());

    // Next, we update it. The standard loop time is 20ms.
    armSim.update(0.020);

    // Finally, we set our simulated encoder's readings and simulated battery voltage
    encoderSim.setDistance(armSim.getAngle());
    // SimBattery estimates loaded battery voltages
    RoboRioSim.setVInVoltage(
        BatterySim.calculateDefaultBatteryLoadedVoltage(armSim.getCurrentDraw()));

    // Update the Mechanism Arm angle based on the simulated arm angle
    arm.setAngle(Units.radiansToDegrees(armSim.getAngle()));

    // Put Mechanism 2d to SmartDashboard
    Telemetry.log("Arm Sim", mech2d);
  }

  /** Load setpoint and kP from preferences. */
  public void loadPreferences() {
    // Read Preferences for Arm setpoint and kP on entering Teleop
    armSetpointDegrees = Preferences.getDouble(Constants.ARM_POSITION_KEY, armSetpointDegrees);
    if (armKp != Preferences.getDouble(Constants.ARM_P_KEY, armKp)) {
      armKp = Preferences.getDouble(Constants.ARM_P_KEY, armKp);
      controller.setP(armKp);
    }
  }

  /** Run the control loop to reach and maintain the setpoint from the preferences. */
  public void reachSetpoint() {
    var pidOutput =
        controller.calculate(encoder.getDistance(), Units.degreesToRadians(armSetpointDegrees));
    motor.setVoltage(pidOutput);
  }

  public void stop() {
    motor.setThrottle(0.0);
  }

  @Override
  public void close() {
    motor.close();
    encoder.close();
  }
}
