// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.simulation;

import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.numbers.N2;
import org.wpilib.math.system.DCMotor;
import org.wpilib.math.system.LinearSystem;
import org.wpilib.system.RobotController;

/**
 * Represents a simulated elevator mechanism.
 *
 * <p>Gravity is supplied as a gravity gain kG rather than assumed to act along the full travel, so
 * this also models horizontal rack-and-pinion mechanisms (kG = 0), canted elevators, and elevators
 * with a counterbalance.
 */
public class ElevatorSim extends LinearSystemSim<N2, N1, N2> {
  // Gearbox for the elevator.
  private final DCMotor m_gearbox;

  // The gearing from the motors to the elevator shaft.
  private final double m_gearing;

  // The gravity gain in volts.
  private final double m_kG;

  // The radius of the drum that the elevator spool is wrapped around in meters.
  private final double m_drumRadius;

  // The mass of the elevator carriage in kg.
  private final double m_carriageMass;

  // The min allowable height for the elevator in meters.
  private final double m_minHeight;

  // The max allowable height for the elevator in meters.
  private final double m_maxHeight;

  /**
   * Creates a simulated elevator mechanism.
   *
   * @param plant The linear system that represents the elevator. This system can be created with
   *     {@link org.wpilib.math.system.Models#elevatorFromPhysicalConstants(DCMotor, double, double,
   *     double)} or {@link org.wpilib.math.system.Models#elevatorFromSysId(double, double)}.
   * @param gearbox The type of and number of motors in the elevator gearbox.
   * @param gearing The gear ratio from the motors to the elevator drum, as input over output
   *     (greater than 1 is a reduction).
   * @param kG The gravity gain in volts. Minimum voltage needed to hold an elevator in place. Zero
   *     for a horizontal mechanism, and less than the vertical value for a canted one or one with a
   *     counterbalance.
   * @param minHeight The min allowable height of the elevator in meters.
   * @param maxHeight The max allowable height of the elevator in meters.
   * @param startingHeight The starting height of the elevator in meters.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 1 element for position.
   * @throws IllegalArgumentException if gearing &lt;= 0, minHeight &gt; maxHeight, or the plant's
   *     A(1, 1) or B(1, 0) entry is zero, which leaves the drum radius and carriage mass
   *     undetermined. A(1, 1) is zero for a plant built from SysId constants with kV = 0.
   */
  @SuppressWarnings("this-escape")
  public ElevatorSim(
      LinearSystem<N2, N1, N2> plant,
      DCMotor gearbox,
      double gearing,
      double kG,
      double minHeight,
      double maxHeight,
      double startingHeight,
      double... measurementStdDevs) {
    super(plant, measurementStdDevs);

    if (gearing <= 0.0) {
      throw new IllegalArgumentException("gearing must be greater than zero.");
    }
    if (plant.getA(1, 1) == 0.0) {
      throw new IllegalArgumentException(
          "plant must have nonzero velocity damping A(1, 1); a plant built with kV = 0 doesn't "
              + "determine the drum radius.");
    }
    if (plant.getB(1, 0) == 0.0) {
      throw new IllegalArgumentException("plant must have nonzero input gain B(1, 0).");
    }
    if (minHeight > maxHeight) {
      throw new IllegalArgumentException("minHeight must not be greater than maxHeight.");
    }

    m_gearbox = gearbox;
    m_gearing = gearing;
    m_kG = kG;

    // The elevator state-space model (see Models.elevatorFromPhysicalConstants())
    // has the following nonzero entries, where r is the drum radius and m is the
    // carriage mass:
    //
    //   A₁,₁ = -G²Kₜ/(KᵥRr²m)
    //   B₁,₀ = GKₜ/(Rrm)
    //
    // G and r appear only as the ratio G/r, so the plant alone can't identify
    // them separately; G is taken from the caller.
    //
    // Solve for G/r.
    //
    //   A/B = -G/(Kᵥr)
    //   G/r = -KᵥA/B
    //   r/G = -B/KᵥA
    //
    // Solve for r.
    //
    //   r = G(r/G)
    //   r = -GB/KᵥA
    //
    // Solve for m.
    //
    //   B = GKₜ/(Rrm)
    //   m = GKₜ/(RrB)
    m_drumRadius = -m_gearing * plant.getB(1, 0) / (gearbox.Kv * plant.getA(1, 1));
    m_carriageMass = m_gearing * gearbox.Kt / (gearbox.R * m_drumRadius * plant.getB(1, 0));
    m_minHeight = minHeight;
    m_maxHeight = maxHeight;

    setState(startingHeight, 0);
  }

  /**
   * Sets the elevator's state. The new position will be limited between the minimum and maximum
   * allowed heights.
   *
   * @param position The new position in meters.
   * @param velocity New velocity in meters per second.
   */
  public final void setState(double position, double velocity) {
    setState(VecBuilder.fill(Math.clamp(position, m_minHeight, m_maxHeight), velocity));
  }

  /**
   * Sets the elevator's position. The new position will be limited between the minimum and maximum
   * allowed heights.
   *
   * @param position The new position in meters.
   */
  public void setPosition(double position) {
    setState(position, m_x.get(1, 0));
  }

  /**
   * Sets the elevator's velocity.
   *
   * @param velocity New velocity in meters per second.
   */
  public void setVelocity(double velocity) {
    setState(m_x.get(0, 0), velocity);
  }

  /**
   * Returns whether the elevator would hit the lower limit.
   *
   * @param elevatorHeight The elevator height in meters.
   * @return Whether the elevator would hit the lower limit.
   */
  public boolean wouldHitLowerLimit(double elevatorHeight) {
    return elevatorHeight <= this.m_minHeight;
  }

  /**
   * Returns whether the elevator would hit the upper limit.
   *
   * @param elevatorHeight The elevator height in meters.
   * @return Whether the elevator would hit the upper limit.
   */
  public boolean wouldHitUpperLimit(double elevatorHeight) {
    return elevatorHeight >= this.m_maxHeight;
  }

  /**
   * Returns whether the elevator has hit the lower limit.
   *
   * @return Whether the elevator has hit the lower limit.
   */
  public boolean hasHitLowerLimit() {
    return wouldHitLowerLimit(getPosition());
  }

  /**
   * Returns whether the elevator has hit the upper limit.
   *
   * @return Whether the elevator has hit the upper limit.
   */
  public boolean hasHitUpperLimit() {
    return wouldHitUpperLimit(getPosition());
  }

  /**
   * Returns the gear ratio of the elevator's gearbox.
   *
   * @return The elevator's gearbox gear ratio.
   */
  public double getGearing() {
    return m_gearing;
  }

  /**
   * Returns the drum radius of the elevator.
   *
   * @return The elevator's drum radius in meters.
   */
  public double getDrumRadius() {
    return m_drumRadius;
  }

  /**
   * Returns the mass of the elevator carriage.
   *
   * @return The mass of the elevator carriage in kg.
   */
  public double getCarriageMass() {
    return m_carriageMass;
  }

  /**
   * Returns the gravity gain of the elevator.
   *
   * @return The gravity gain in volts.
   */
  public double getKg() {
    return m_kG;
  }

  /**
   * Returns the gearbox for the elevator.
   *
   * @return The gearbox for the elevator.
   */
  public DCMotor getGearbox() {
    return m_gearbox;
  }

  /**
   * Returns the position of the elevator.
   *
   * @return The position of the elevator in meters.
   */
  public double getPosition() {
    return getOutput(0);
  }

  /**
   * Returns the velocity of the elevator.
   *
   * @return The velocity of the elevator in meters per second.
   */
  public double getVelocity() {
    return getOutput(1);
  }

  /**
   * Returns the acceleration of the elevator.
   *
   * @return The acceleration of the elevator in m/s².
   */
  public double getAcceleration() {
    double acceleration =
        m_plant
            .getA()
            .times(m_x)
            .plus(m_plant.getB().times(m_u.minus(VecBuilder.fill(m_kG))))
            .get(1, 0);

    // updateX() pins the state at a hard stop with zero velocity, so acceleration that points
    // farther into a stop the carriage is already resting against is motion the sim prevents.
    // The stop absorbs it.
    if (wouldHitLowerLimit(m_x.get(0, 0)) && acceleration < 0.0) {
      return 0.0;
    }
    if (wouldHitUpperLimit(m_x.get(0, 0)) && acceleration > 0.0) {
      return 0.0;
    }

    return acceleration;
  }

  /**
   * Returns the elevator's force.
   *
   * @return The elevator's force in Newtons.
   */
  public double getForce() {
    return getAcceleration() * m_carriageMass;
  }

  /**
   * Returns the elevator's current draw.
   *
   * <p>This is the current drawn from the battery, which differs from the current through the motor
   * by the duty cycle the motor controller is applying. A negative value means the elevator is
   * regenerating and returning current to the battery.
   *
   * @return The elevator current draw in amps.
   */
  public double getCurrentDraw() {
    // Reductions are greater than 1, so a reduction of 10:1 would mean the motor is
    // spinning 10x faster than the output. v = rω, so ω = v/r.
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
    double motorVelocity = m_x.get(1, 0) * m_gearing / m_drumRadius;
    double appliedVoltage = m_u.get(0, 0);
    double batteryVoltage = RobotController.getBatteryVoltage();

    // With no battery voltage the controller can't apply any duty cycle, so nothing is drawn.
    if (batteryVoltage == 0.0) {
      return 0.0;
    }

    double dutyCycle = Math.clamp(appliedVoltage / batteryVoltage, -1.0, 1.0);
    return m_gearbox.getCurrent(motorVelocity, appliedVoltage) * dutyCycle;
  }

  /**
   * Gets the input voltage for the elevator.
   *
   * @return The elevator's input voltage.
   */
  public double getInputVoltage() {
    return getInput(0);
  }

  /**
   * Sets the input voltage for the elevator.
   *
   * @param volts The input voltage.
   */
  public void setInputVoltage(double volts) {
    setInput(volts);
    clampInput(RobotController.getBatteryVoltage());
  }

  /**
   * Updates the state of the elevator.
   *
   * @param currentXhat The current state estimate.
   * @param u The system inputs (voltage).
   * @param dt The time difference between controller updates in seconds.
   */
  @Override
  protected Matrix<N2, N1> updateX(Matrix<N2, N1> currentXhat, Matrix<N1, N1> u, double dt) {
    // Gravity enters as a constant voltage offset. Since B = [0  B₁,₀]ᵀ,
    //
    //   ẋ = Ax + Bu + [0  -kG·B₁,₀]ᵀ = Ax + B(u - kG)
    //
    // so the plant's exact discretization applies with the input offset by -kG.
    var updatedXhat = m_plant.calculateX(currentXhat, u.minus(VecBuilder.fill(m_kG)), dt);

    // We check for collisions after updating x-hat.
    if (wouldHitLowerLimit(updatedXhat.get(0, 0))) {
      return VecBuilder.fill(m_minHeight, 0);
    }
    if (wouldHitUpperLimit(updatedXhat.get(0, 0))) {
      return VecBuilder.fill(m_maxHeight, 0);
    }
    return updatedXhat;
  }
}
