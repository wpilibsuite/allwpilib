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
import edu.wpi.first.math.system.plant.LinearSystemId;
import edu.wpi.first.math.util.Units;
import edu.wpi.first.units.Angle;
import edu.wpi.first.units.Measure;
import edu.wpi.first.units.MutableMeasure;
import edu.wpi.first.units.Velocity;
import edu.wpi.first.wpilibj.RobotController;

/** Represents a simulated angular mechanism controlled by a DC motor. */
public class AngularMechanismSim extends LinearSystemSim<N2, N1, N2> {
  /** Gearbox for the DC motor. */
  protected final DCMotor m_gearbox;

  /**  The gearing from the motors to the output. */
  protected final double m_gearing;

  /**  The moment of inertia of the mechanism. */
  protected final double m_jKgMetersSquared;

  /**  The angular position of the system. */
  private final MutableMeasure<Angle> m_angle = MutableMeasure.zero(Radians);

  /**  The angular velocity of the system. */
  private final MutableMeasure<Velocity<Angle>> m_angularVelocity =
      MutableMeasure.zero(RadiansPerSecond);

  /**  The angular acceleration of the system. */
  private final MutableMeasure<Velocity<Velocity<Angle>>> m_angularAcceleration =
      MutableMeasure.zero(RadiansPerSecondPerSecond);

  /**
   * Creates a simulated angular mechanism.
   *
   * <p>This class is useful for simulating mechanisms like turrets, wrists, single motors, swerve
   * module steers, etc. Note that gravitational effects are not included in this model.
   *
   * @param plant The linear system representing the angular mechanism. Use either {@link
   *     LinearSystemId#createAngularSystem(DCMotor, double, double)} if using physical constants or
   *     {@link LinearSystemId#identifyPositionSystem(double, double)} if using system
   *     characterization.
   * @param gearbox The type of and number of motors in the mechanism's gearbox.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 2 elements. The first element is for position. The
   *     second element is for velocity.
   */
  public AngularMechanismSim(
      LinearSystem<N2, N1, N2> plant, DCMotor gearbox, double... measurementStdDevs) {
    super(plant, measurementStdDevs);
    m_gearbox = gearbox;

    // By theorem 6.10.1 of https://file.tavsys.net/control/controls-engineering-in-frc.pdf,
    // the flywheel state-space model is:
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
    m_jKgMetersSquared = m_gearing * m_gearbox.KtNMPerAmp / (m_gearbox.rOhms * m_plant.getB(1, 0));
  }

  /**
   * Sets the position and velocity of the mechanism.
   *
   * @param angularPositionRad The new position in radians.
   * @param angularVelocityRadPerSec The new velocity in radians per second.
   */
  public void setState(double angularPositionRad, double angularVelocityRadPerSec) {
    setState(VecBuilder.fill(angularPositionRad, angularVelocityRadPerSec));
  }

  /**
   * Sets the position of the mechanism.
   *
   * @param angularPositionRad The new position in radians.
   */
  public void setPosition(double angularPositionRad) {
    setState(VecBuilder.fill(angularPositionRad, m_x.get(1, 0)));
  }

  /**
   * Sets the velocity of the mechanism.
   *
   * @param angularVelocityRadPerSec The new velocity in radians per second.
   */
  public void setVelocity(double angularVelocityRadPerSec) {
    setState(VecBuilder.fill(m_x.get(0, 0), angularVelocityRadPerSec));
  }

  /**
   * Returns the gear ratio of the mechanism.
   *
   * @return the mechanism's gear ratio.
   */
  public double getGearing() {
    return m_gearing;
  }

  /**
   * Returns the moment of inertia in kilograms meters squared.
   *
   * @return The mechanisms's moment of inertia.
   */
  public double getJKgMetersSquared() {
    return m_jKgMetersSquared;
  }

  /**
   * Returns the gearbox for the angular system.
   *
   * @return The angular system's gearbox.
   */
  public DCMotor getGearBox() {
    return m_gearbox;
  }

  /**
   * Returns the position of the mechanism in radians.
   *
   * @return The mechanism's position in radians.
   */
  public double getAngularPositionRad() {
    return getOutput(0);
  }

  /**
   * Returns the position of the mechanism in rotations.
   *
   * @return The mechanism's position in rotations.
   */
  public double getAngularPositionRotations() {
    return Units.radiansToRotations(getAngularPositionRad());
  }

  /**
   * Returns the position of the mechanism.
   *
   * @return The mechanism's position
   */
  public Measure<Angle> getAngularPosition() {
    m_angle.mut_setMagnitude(getAngularPositionRad());
    return m_angle;
  }

  /**
   * Returns the velocity of the mechanism in radians per second.
   *
   * @return The mechanism's velocity in radians per second.
   */
  public double getAngularVelocityRadPerSec() {
    return getOutput(1);
  }

  /**
   * Returns the velocity of the mechanism in RPM.
   *
   * @return The mechanism's velocity in RPM.
   */
  public double getAngularVelocityRPM() {
    return Units.radiansPerSecondToRotationsPerMinute(getAngularVelocityRadPerSec());
  }

  /**
   * Returns the velocity of the mechanism.
   *
   * @return The mechanism's velocity.
   */
  public Measure<Velocity<Angle>> getAngularVelocity() {
    m_angularVelocity.mut_setMagnitude(getAngularPositionRad());
    return m_angularVelocity;
  }

  /**
   * Returns the acceleration of the mechanism in radians per second squared.
   *
   * @return the mechanism's acceleration in radians per second squared.
   */
  public double getAngularAccelerationRadPerSecSq() {
    var acceleration = (m_plant.getA().times(m_x)).plus(m_plant.getB().times(m_u));
    return acceleration.get(1, 0);
  }

  /**
   * Returns the acceleration of the mechanism.
   *
   * @return the mechanism's acceleration.
   */
  public Measure<Velocity<Velocity<Angle>>> getAngularAcceleration() {
    m_angularAcceleration.mut_setMagnitude(getAngularAccelerationRadPerSecSq());
    return m_angularAcceleration;
  }

  /**
   * Returns the mechanism's torque in Newton-Meters.
   *
   * @return The mechanism's torque in Newton-Meters.
   */
  public double getTorqueNewtonMeters() {
    return getAngularAccelerationRadPerSecSq() * getJKgMetersSquared();
  }

  /**
   * Returns the mechanism's DC motor current draw.
   *
   * @return The mechanism's DC motor current draw.
   */
  public double getCurrentDrawAmps() {
    // I = V / R - omega / (Kv * R)
    // Reductions are output over input, so a reduction of 2:1 means the motor is spinning
    // 2x faster than the output
    return m_gearbox.getCurrent(m_x.get(1, 0) * m_gearing, m_u.get(0, 0))
        * Math.signum(m_u.get(0, 0));
  }

  /**
   * Gets the input voltage for the mechanism's DC motor.
   *
   * @return The mechanism's DC motor's voltage.
   */
  public double getInputVoltage() {
    return getInput(0);
  }

  /**
   * Sets the input voltage for the mechanism's DC motor.
   *
   * @param volts The input voltage.
   */
  public void setInputVoltage(double volts) {
    setInput(volts);
    clampInput(RobotController.getBatteryVoltage());
  }
}
