// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static edu.wpi.first.units.Units.Radians;
import static edu.wpi.first.units.Units.RadiansPerSecond;
import static edu.wpi.first.units.Units.RadiansPerSecondPerSecond;

import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.math.util.Units;
import edu.wpi.first.units.measure.Angle;
import edu.wpi.first.units.measure.AngularAcceleration;
import edu.wpi.first.units.measure.AngularVelocity;
import edu.wpi.first.wpilibj.RobotController;

/** Represents a simulated DC motor mechanism. */
public class DCMotorSim extends LinearSystemSim<N2, N1, N2> {
  // Gearbox for the DC motor.
  private final DCMotor m_gearbox;

  // The gearing from the motors to the output.
  private final double m_gearing;

  // The moment of inertia for the DC motor mechanism.
  private final double m_jKgMetersSquared;

  /**
   * Creates a simulated DC motor mechanism.
   *
   * @param plant The linear system representing the DC motor. This system can be created with
   *     {@link edu.wpi.first.math.system.plant.LinearSystemId#createDCMotorSystem(DCMotor, double,
   *     double)} or {@link
   *     edu.wpi.first.math.system.plant.LinearSystemId#createDCMotorSystem(double, double)}. If
   *     {@link edu.wpi.first.math.system.plant.LinearSystemId#createDCMotorSystem(double, double)}
   *     is used, the distance unit must be radians.
   * @param gearbox The type of and number of motors in the DC motor gearbox.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 2 elements. The first element is for position. The
   *     second element is for velocity.
   */
  public DCMotorSim(LinearSystem<N2, N1, N2> plant, DCMotor gearbox, double... measurementStdDevs) {
    super(plant, measurementStdDevs);
    m_gearbox = gearbox;

    // By theorem 6.10.1 of https://file.tavsys.net/control/controls-engineering-in-frc.pdf,
    // the DC motor state-space model is:
    //
    //   dx/dt = -G²Kₜ/(KᵥRJ)x + (GKₜ)/(RJ)u
    //   A = -G²Kₜ/(KᵥRJ)
    //   B = GKₜ/(RJ)
    //
    // Solve for G.
    //
    //   A/B = -G/Kᵥ
    //   G = -KᵥA/B
    //
    // Solve for J.
    //
    //   B = GKₜ/(RJ)
    //   J = GKₜ/(RB)
    m_gearing = -gearbox.KvRadPerSecPerVolt * plant.getA(1, 1) / plant.getB(1, 0);
    m_jKgMetersSquared = m_gearing * gearbox.KtNMPerAmp / (gearbox.rOhms * plant.getB(1, 0));
  }

  /**
   * Sets the state of the DC motor.
   *
   * @param angularPositionRad The new position in radians.
   * @param angularVelocityRadPerSec The new velocity in radians per second.
   */
  public void setState(double angularPositionRad, double angularVelocityRadPerSec) {
    setState(VecBuilder.fill(angularPositionRad, angularVelocityRadPerSec));
  }

  /**
   * Sets the DC motor's angular position.
   *
   * @param angularPositionRad The new position in radians.
   */
  public void setAngle(double angularPositionRad) {
    setState(angularPositionRad, getAngularVelocityRadPerSec());
  }

  /**
   * Sets the DC motor's angular velocity.
   *
   * @param angularVelocityRadPerSec The new velocity in radians per second.
   */
  public void setAngularVelocity(double angularVelocityRadPerSec) {
    setState(getAngularPositionRad(), angularVelocityRadPerSec);
  }

  /**
   * Returns the gear ratio of the DC motor.
   *
   * @return the DC motor's gear ratio.
   */
  public double getGearing() {
    return m_gearing;
  }

  /**
   * Returns the moment of inertia of the DC motor.
   *
   * @return The DC motor's moment of inertia.
   */
  public double getJKgMetersSquared() {
    return m_jKgMetersSquared;
  }

  /**
   * Returns the gearbox for the DC motor.
   *
   * @return The DC motor's gearbox.
   */
  public DCMotor getGearbox() {
    return m_gearbox;
  }

  /**
   * Returns the DC motor's position.
   *
   * @return The DC motor's position.
   */
  public double getAngularPositionRad() {
    return getOutput(0);
  }

  /**
   * Returns the DC motor's position in rotations.
   *
   * @return The DC motor's position in rotations.
   */
  public double getAngularPositionRotations() {
    return Units.radiansToRotations(getAngularPositionRad());
  }

  /**
   * Returns the DC motor's position.
   *
   * @return The DC motor's position
   */
  public Angle getAngularPosition() {
    return Radians.of(getAngularPositionRad());
  }

  /**
   * Returns the DC motor's velocity.
   *
   * @return The DC motor's velocity.
   */
  public double getAngularVelocityRadPerSec() {
    return getOutput(1);
  }

  /**
   * Returns the DC motor's velocity in RPM.
   *
   * @return The DC motor's velocity in RPM.
   */
  public double getAngularVelocityRPM() {
    return Units.radiansPerSecondToRotationsPerMinute(getAngularVelocityRadPerSec());
  }

  /**
   * Returns the DC motor's velocity.
   *
   * @return The DC motor's velocity
   */
  public AngularVelocity getAngularVelocity() {
    return RadiansPerSecond.of(getAngularVelocityRadPerSec());
  }

  /**
   * Returns the DC motor's acceleration in Radians Per Second Squared.
   *
   * @return The DC motor's acceleration in Radians Per Second Squared.
   */
  public double getAngularAccelerationRadPerSecSq() {
    var acceleration = (m_plant.getA().times(m_x)).plus(m_plant.getB().times(m_u));
    return acceleration.get(1, 0);
  }

  /**
   * Returns the DC motor's acceleration.
   *
   * @return The DC motor's acceleration.
   */
  public AngularAcceleration getAngularAcceleration() {
    return RadiansPerSecondPerSecond.of(getAngularAccelerationRadPerSecSq());
  }

  /**
   * Returns the DC motor's torque in Newton-Meters.
   *
   * @return The DC motor's torque in Newton-Meters.
   */
  public double getTorqueNewtonMeters() {
    return getAngularAccelerationRadPerSecSq() * m_jKgMetersSquared;
  }

  /**
   * Returns the DC motor's current draw.
   *
   * @return The DC motor's current draw.
   */
  public double getCurrentDrawAmps() {
    // I = V / R - omega / (Kv * R)
    // Reductions are output over input, so a reduction of 2:1 means the motor is spinning
    // 2x faster than the output
    return m_gearbox.getCurrent(m_x.get(1, 0) * m_gearing, m_u.get(0, 0))
        * Math.signum(m_u.get(0, 0));
  }

  /**
   * Gets the input voltage for the DC motor.
   *
   * @return The DC motor's input voltage.
   */
  public double getInputVoltage() {
    return getInput(0);
  }

  /**
   * Sets the input voltage for the DC motor.
   *
   * @param volts The input voltage.
   */
  public void setInputVoltage(double volts) {
    setInput(volts);
    clampInput(RobotController.getBatteryVoltage());
  }
}
