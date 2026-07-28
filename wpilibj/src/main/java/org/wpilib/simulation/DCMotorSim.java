// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.simulation;

import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.numbers.N2;
import org.wpilib.math.system.DCMotor;
import org.wpilib.math.system.LinearSystem;
import org.wpilib.system.RobotController;

/** Represents a simulated DC motor mechanism. */
public class DCMotorSim extends LinearSystemSim<N2, N1, N2> {
  // Gearbox for the DC motor.
  private final DCMotor m_gearbox;

  // The gearing from the motors to the output.
  private final double m_gearing;

  // The moment of inertia for the DC motor mechanism in kg-m².
  private final double m_j;

  /**
   * Creates a simulated DC motor mechanism.
   *
   * @param plant The linear system representing the DC motor. This system can be created with
   *     {@link org.wpilib.math.system.Models#singleJointedArmFromPhysicalConstants(DCMotor, double,
   *     double)} or {@link org.wpilib.math.system.Models#singleJointedArmFromSysId(double,
   *     double)}.
   * @param gearbox The type of and number of motors in the DC motor gearbox.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 2 elements. The first element is for position. The
   *     second element is for velocity.
   * @throws IllegalArgumentException if the plant's A(1, 1) or B(1, 0) entry is zero, which leaves
   *     the gearing and moment of inertia undetermined. A(1, 1) is zero for a plant built from
   *     SysId constants with kV = 0.
   */
  public DCMotorSim(LinearSystem<N2, N1, N2> plant, DCMotor gearbox, double... measurementStdDevs) {
    super(plant, measurementStdDevs);

    if (plant.getA(1, 1) == 0.0) {
      throw new IllegalArgumentException(
          "plant must have nonzero velocity damping A(1, 1); a plant built with kV = 0 doesn't "
              + "determine the gearing.");
    }
    if (plant.getB(1, 0) == 0.0) {
      throw new IllegalArgumentException("plant must have nonzero input gain B(1, 0).");
    }

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
    m_gearing = -gearbox.Kv * plant.getA(1, 1) / plant.getB(1, 0);
    m_j = m_gearing * gearbox.Kt / (gearbox.R * plant.getB(1, 0));
  }

  /**
   * Sets the state of the DC motor.
   *
   * @param angularPosition The new position in radians.
   * @param angularVelocity The new velocity in radians per second.
   */
  public void setState(double angularPosition, double angularVelocity) {
    setState(VecBuilder.fill(angularPosition, angularVelocity));
  }

  /**
   * Sets the DC motor's angular position.
   *
   * @param angularPosition The new position in radians.
   */
  public void setAngle(double angularPosition) {
    setState(angularPosition, m_x.get(1, 0));
  }

  /**
   * Sets the DC motor's angular velocity.
   *
   * @param angularVelocity The new velocity in radians per second.
   */
  public void setAngularVelocity(double angularVelocity) {
    setState(m_x.get(0, 0), angularVelocity);
  }

  /**
   * Returns the gear ratio of the DC motor.
   *
   * @return The DC motor's gear ratio.
   */
  public double getGearing() {
    return m_gearing;
  }

  /**
   * Returns the moment of inertia of the DC motor.
   *
   * @return The DC motor's moment of inertia in kg-m².
   */
  public double getJ() {
    return m_j;
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
   * @return The DC motor's position in radians.
   */
  public double getAngularPosition() {
    return getOutput(0);
  }

  /**
   * Returns the DC motor's velocity.
   *
   * @return The DC motor's velocity in radians per second.
   */
  public double getAngularVelocity() {
    return getOutput(1);
  }

  /**
   * Returns the DC motor's acceleration.
   *
   * @return The DC motor's acceleration in rad/s².
   */
  public double getAngularAcceleration() {
    var acceleration = (m_plant.getA().times(m_x)).plus(m_plant.getB().times(m_u));
    return acceleration.get(1, 0);
  }

  /**
   * Returns the DC motor's torque.
   *
   * @return The DC motor's torque in Newton-meters.
   */
  public double getTorque() {
    return getAngularAcceleration() * m_j;
  }

  /**
   * Returns the DC motor's current draw.
   *
   * <p>This is the current drawn from the battery, which differs from the current through the motor
   * by the duty cycle the motor controller is applying. A negative value means the motor is
   * regenerating and returning current to the battery.
   *
   * @return The DC motor's current draw in amps.
   */
  public double getCurrentDraw() {
    // Reductions are greater than 1, so a reduction of 10:1 would mean the motor is
    // spinning 10x faster than the output.
    //
    // The current through the motor is I = V/R - ω/(KᵥR), where V is the voltage across
    // the motor terminals.
    //
    // The motor controller produces V by PWMing the battery voltage at duty cycle
    // D = V/V_batt. An ideal H-bridge conserves power, so V_batt·I_supply = V·I, giving
    //
    //   I_supply = D·I
    //
    // Scaling by D also makes the result continuous through V = 0, where the motor is
    // braking and its circulating current isn't drawn from the battery.
    double motorVelocity = m_x.get(1, 0) * m_gearing;
    var appliedVoltage = m_u.get(0, 0);
    var dutyCycle = appliedVoltage / RobotController.getBatteryVoltage();
    return m_gearbox.getCurrent(motorVelocity, appliedVoltage) * dutyCycle;
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
