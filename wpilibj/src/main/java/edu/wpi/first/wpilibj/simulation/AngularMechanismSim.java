// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static edu.wpi.first.units.Units.Radians;
import static edu.wpi.first.units.Units.RadiansPerSecond;

import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.math.system.plant.LinearSystemId;
import edu.wpi.first.math.util.Units;
import edu.wpi.first.units.Angle;
import edu.wpi.first.units.MutableMeasure;
import edu.wpi.first.units.Velocity;
import edu.wpi.first.wpilibj.RobotController;

/** Represents a simulated angular mechanism controlled by a DC motor. */
public class AngularMechanismSim extends LinearSystemSim<N2, N1, N2> {
  // Gearbox for the DC motor.
  private final DCMotor m_gearbox;

  // The gearing from the motors to the output.
  private final double m_gearing;

  // The angular position of the system.
  private final MutableMeasure<Angle> m_angle = MutableMeasure.zero(Radians);

  // The angular velocity of the system.
  private final MutableMeasure<Velocity<Angle>> m_angularVelocity = MutableMeasure.zero(RadiansPerSecond);

  // The mechanism's plant
  private final LinearSystem<N2, N1, N2> m_plant;

  /**
   * Creates a simulated angular mechanism.
   * <p> Note that gravitational effects are not included in this model.  
   *
   * @param plant The linear system representing the angular mechanism.  Use either {@link
   * LinearSystemId#createDCMotorSystem(DCMotor, double, double)} if using physical constants or {@link LinearSystemId#identifyPositionSystem(double, double)}
   * if using system characterization. 
   * @param gearbox The type of and number of motors in the mechanism's gearbox.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 2 elements. The first element is for position. The
   *     second element is for velocity.
   */
  public AngularMechanismSim(
      LinearSystem<N2, N1, N2> plant,
      DCMotor gearbox,
      double... measurementStdDevs) {
    super(plant, measurementStdDevs);
    m_gearbox = gearbox;
    m_gearing = gearbox.KtNMPerAmp * plant.getA(1, 1) / plant.getB(1, 0);
    m_plant = plant;
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
    public void setState(double angularPositionRad) {
     setState(VecBuilder.fill(angularPositionRad, m_x.get(1, 0)));
  }

  /**
   * Returns the DC motor position.
   *
   * @return The DC motor position.
   */
  public double getAngularPositionRad() {
    return getOutput(0);
  }

  /**
   * Returns the DC motor position in rotations.
   *
   * @return The DC motor position in rotations.
   */
  public double getAngularPositionRotations() {
    return Units.radiansToRotations(getAngularPositionRad());
  }

  /**
   * Returns the DC motor velocity.
   *
   * @return The DC motor velocity.
   */
  public double getAngularVelocityRadPerSec() {
    return getOutput(1);
  }

  /**
   * Returns the DC motor velocity in RPM.
   *
   * @return The DC motor velocity in RPM.
   */
  public double getAngularVelocityRPM() {
    return Units.radiansPerSecondToRotationsPerMinute(getAngularVelocityRadPerSec());
  }

  /**
   * Returns the DC motor current draw.
   *
   * @return The DC motor current draw.
   */
  public double getCurrentDrawAmps() {
    // I = V / R - omega / (Kv * R)
    // Reductions are output over input, so a reduction of 2:1 means the motor is spinning
    // 2x faster than the output
    return m_gearbox.getCurrent(m_x.get(1, 0) * m_gearing, m_u.get(0, 0))
        * Math.signum(m_u.get(0, 0));
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
