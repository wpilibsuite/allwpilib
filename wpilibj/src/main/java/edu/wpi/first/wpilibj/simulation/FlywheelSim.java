// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static edu.wpi.first.units.Units.RPM;
import static edu.wpi.first.units.Units.RadiansPerSecond;

import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.units.Angle;
import edu.wpi.first.units.Measure;
import edu.wpi.first.units.MutableMeasure;
import edu.wpi.first.units.Velocity;
import edu.wpi.first.wpilibj.RobotController;

/** Represents a simulated flywheel mechanism. */
public class FlywheelSim extends LinearSystemSim<N1, N1, N1> {
  // Gearbox for the flywheel.
  public final DCMotor m_gearbox;

  // The gearing from the motors to the output.
  public final double m_gearing;

  // The moment of inertia for the flywheel mechanism.
  public final double m_jKgMetersSquared;

  // The angular velocity of the system.
  private final MutableMeasure<Velocity<Angle>> m_angularVelocity =
      MutableMeasure.zero(RadiansPerSecond);

  /**
   * Creates a simulated flywheel mechanism.
   *
   * @param plant The linear system that represents the flywheel. Use either {@link
   *     LinearSystemId#createFlywheelSystem(DCMotor, double, double)} if using physical constants
   *     or {@link LinearSystemId#identifyVelocitySystem(kV, kA)} if using system characterization.
   * @param gearbox The type of and number of motors in the flywheel gearbox.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 1 element for velocity.
   */
  public FlywheelSim(
      LinearSystem<N1, N1, N1> plant, DCMotor gearbox, double... measurementStdDevs) {
    super(plant, measurementStdDevs);
    m_gearbox = gearbox;
    m_gearing = gearbox.KtNMPerAmp * plant.getA(0, 0) / plant.getB(0, 0);
    m_jKgMetersSquared = m_gearing * gearbox.KtNMPerAmp / (gearbox.rOhms * plant.getB(0, 0));
  }

  /**
   * Sets the flywheel's state.
   *
   * @param velocityRadPerSec The new velocity in radians per second.
   */
  public void setState(double velocityRadPerSec) {
    setState(VecBuilder.fill(velocityRadPerSec));
  }

  /**
   * Returns the flywheel velocity.
   *
   * @return The flywheel velocity.
   */
  public double getAngularVelocityRadPerSec() {
    return m_angularVelocity.in(RadiansPerSecond);
  }

  /**
   * Returns the flywheel velocity in RPM.
   *
   * @return The flywheel velocity in RPM.
   */
  public double getAngularVelocityRPM() {
    return m_angularVelocity.in(RPM);
  }

  /**
   * Returns the flywheel velocity.
   *
   * @return The flywheel velocity
   */
  public Measure<Velocity<Angle>> getAngularVelocity() {
    return m_angularVelocity;
  }

  /**
   * Returns the flywheel current draw.
   *
   * @return The flywheel current draw.
   */
  public double getCurrentDrawAmps() {
    // I = V / R - omega / (Kv * R)
    // Reductions are output over input, so a reduction of 2:1 means the motor is spinning
    // 2x faster than the flywheel
    return m_gearbox.getCurrent(m_x.get(0, 0) * m_gearing, m_u.get(0, 0))
        * Math.signum(m_u.get(0, 0));
  }

  /**
   * Gets the input voltage for the flywheel.
   *
   * @return The flywheel input voltage.
   */
  public double getInputVoltage() {
    return getInput(0);
  }

  /**
   * Sets the input voltage for the flywheel.
   *
   * @param volts The input voltage.
   */
  public void setInputVoltage(double volts) {
    setInput(volts);
    clampInput(RobotController.getBatteryVoltage());
  }

  @Override
  public void update(double dtSeconds) {
    super.update(dtSeconds);
    m_angularVelocity.mut_setMagnitude(getOutput(0));
  }
}
