// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static edu.wpi.first.units.Units.Amps;
import static edu.wpi.first.units.Units.KilogramSquareMeters;
import static edu.wpi.first.units.Units.Kilograms;
import static edu.wpi.first.units.Units.Meters;
import static edu.wpi.first.units.Units.MetersPerSecondPerSecond;
import static edu.wpi.first.units.Units.NewtonMeters;
import static edu.wpi.first.units.Units.Radians;
import static edu.wpi.first.units.Units.RadiansPerSecond;
import static edu.wpi.first.units.Units.RadiansPerSecondPerSecond;
import static edu.wpi.first.units.Units.Volts;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.NumericalIntegration;
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.math.system.plant.Gearbox;
import edu.wpi.first.units.measure.Angle;
import edu.wpi.first.units.measure.AngularAcceleration;
import edu.wpi.first.units.measure.AngularVelocity;
import edu.wpi.first.units.measure.Current;
import edu.wpi.first.units.measure.Distance;
import edu.wpi.first.units.measure.LinearAcceleration;
import edu.wpi.first.units.measure.Mass;
import edu.wpi.first.units.measure.MomentOfInertia;
import edu.wpi.first.units.measure.MutAngle;
import edu.wpi.first.units.measure.MutAngularAcceleration;
import edu.wpi.first.units.measure.MutAngularVelocity;
import edu.wpi.first.units.measure.MutCurrent;
import edu.wpi.first.units.measure.MutTorque;
import edu.wpi.first.units.measure.MutVoltage;
import edu.wpi.first.units.measure.Torque;
import edu.wpi.first.units.measure.Voltage;

/** Represents a simulated single jointed arm mechanism. */
public class SingleJointedArmSimBase extends LinearSystemSim<N2, N1, N2> {
  /** The gearbox for the arm. */
  private final Gearbox m_gearbox;

  /** The length of the arm. */
  private final Distance m_armLength;

  /** The pivot distance from the center of mass. */
  private final Distance m_pivotDistance;

  /** The mass of the arm. */
  private final Mass m_mass;

  /** The moment of inertia of the arm. */
  private final MomentOfInertia m_j;

  // The minimum angle that the arm is capable of.
  private final Angle m_minAngle;

  // The maximum angle that the arm is capable of.
  private final Angle m_maxAngle;

  /** The acceleration due to gravity. */
  private final LinearAcceleration m_g;

  /** The angle of the arm with 0 being the horizontal. */
  private final MutAngle m_angle = Radians.mutable(0.0);

  /** The angular velocity of the arm. */
  private final MutAngularVelocity m_angularVelocity = RadiansPerSecond.mutable(0.0);

  /** The angular acceleration of the arm. */
  private final MutAngularAcceleration m_acceleration = RadiansPerSecondPerSecond.mutable(0.0);

  /** The current draw of arm. */
  protected final MutCurrent m_currentDraw = Amps.mutable(0.0);

  /** The voltage of the arm. */
  protected final MutVoltage m_voltage = Volts.mutable(0.0);

  /** The torque on the arm. */
  protected final MutTorque m_torque = NewtonMeters.mutable(0.0);

  /**
   * Creates a simulated arm mechanism.
   *
   * @param plant The linear system that represents the arm. This system can be created with {@link
   *     edu.wpi.first.math.system.plant.LinearSystemId#createSingleJointedArmSystem(DCMotor,
   *     double, double)}.
   * @param gearbox The type of and number of motors in the arm gearbox.
   * @param armLength The length of the arm.
   * @param pivotDistance The distance of the pivot from the center of mass.
   * @param mass The mass of the arm.
   * @param j The moment of inertia of the arm.
   * @param minAngle The minimum angle that the arm is capable of.
   * @param maxAngle The maximum angle that the arm is capable of.
   * @param g The acceleration due to gravity.
   * @param startingAngle The initial position of the Arm simulation.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 1 element for position.
   */
  @SuppressWarnings("this-escape")
  public SingleJointedArmSimBase(
      LinearSystem<N2, N1, N2> plant,
      Gearbox gearbox,
      Distance armLength,
      Distance pivotDistance,
      Mass mass,
      MomentOfInertia j,
      Angle minAngle,
      Angle maxAngle,
      LinearAcceleration g,
      Angle startingAngle,
      double... measurementStdDevs) {
    super(plant, measurementStdDevs);
    m_gearbox = gearbox;
    m_armLength = armLength;
    m_pivotDistance = pivotDistance;
    m_mass = mass;
    m_j = j;
    m_minAngle = minAngle;
    m_maxAngle = maxAngle;
    m_g = g;

    setState(startingAngle.in(Radians), 0.0);
  }

  /**
   * Sets the arm's state. The new angle will be limited between the minimum and maximum allowed
   * angles.
   *
   * @param positionMeters The new position in meters.
   * @param velocityMetersPerSecond New velocity in meters per second.
   */
  public void setState(double positionMeters, double velocityMetersPerSecond) {
    setState(
        VecBuilder.fill(
            MathUtil.clamp(positionMeters, m_minAngle.in(Radians), m_maxAngle.in(Radians)),
            velocityMetersPerSecond));
  }

  /**
   * Sets the arm's state. The new angle will be limited between the minimum and maximum allowed
   * angles.
   *
   * @param angle The new angle.
   * @param angularVelocity The new angular velocity.
   */
  public void setState(Angle angle, AngularVelocity angularVelocity) {
    setState(angle.in(Radians), angularVelocity.in(RadiansPerSecond));
  }

  /**
   * Sets the arm's position. The new angle will be limited bewtween the minimum and maximum allowed
   * angles.
   *
   * @param angleRadians The new angle in radians.
   */
  public void setAngle(double angleRadians) {
    setState(angleRadians, m_angularVelocity.in(RadiansPerSecond));
  }

  /**
   * Sets the arm's position. The new angle will be limited bewtween the minimum and maximum allowed
   * angles.
   *
   * @param angle The new position.
   */
  public void setAngle(Angle angle) {
    setAngle(angle.in(Radians));
  }

  /**
   * Sets the arm's velocity.
   *
   * @param angularVelocityRadiansPerSecond The new velocity in radiansPerSecond per second.
   */
  public void setAngularVelocity(double angularVelocityRadiansPerSecond) {
    setState(m_angle.in(Radians), angularVelocityRadiansPerSecond);
  }

  /**
   * Sets the arm's velocity.
   *
   * @param angularVelocity The new velocity.
   */
  public void setAngularVelocity(AngularVelocity angularVelocity) {
    setAngularVelocity(angularVelocity.in(RadiansPerSecond));
  }

  /**
   * Returns whether the arm would hit the lower limit.
   *
   * @param currentAngleRads The current arm height.
   * @return Whether the arm would hit the lower limit.
   */
  public boolean wouldHitLowerLimit(double currentAngleRads) {
    return currentAngleRads <= this.m_minAngle.in(Radians);
  }

  /**
   * Returns whether the arm would hit the upper limit.
   *
   * @param currentAngleRads The current arm height.
   * @return Whether the arm would hit the upper limit.
   */
  public boolean wouldHitUpperLimit(double currentAngleRads) {
    return currentAngleRads >= this.m_maxAngle.in(Radians);
  }

  /**
   * Returns whether the arm has hit the lower limit.
   *
   * @return Whether the arm has hit the lower limit.
   */
  public boolean hasHitLowerLimit() {
    return wouldHitLowerLimit(getAngleRads());
  }

  /**
   * Returns whether the arm has hit the upper limit.
   *
   * @return Whether the arm has hit the upper limit.
   */
  public boolean hasHitUpperLimit() {
    return wouldHitUpperLimit(getAngleRads());
  }

  /**
   * Returns the mass of the arm in kilograms.
   *
   * @return the mass of the arm in kilograms.
   */
  public double getMassKilograms() {
    return m_mass.in(Kilograms);
  }

  /**
   * Returns the mass of the arm.
   *
   * @return the mass of the arm.
   */
  public Mass getMass() {
    return m_mass;
  }

  /**
   * Returns the moment of inertia in kilograms meters squared.
   *
   * @return The arm's moment of inertia in kilograms meters squared.
   */
  public double getJKgMetersSquared() {
    return m_j.in(KilogramSquareMeters);
  }

  /**
   * Returns the moment of inertia.
   *
   * @return The arm's moment of inertia.
   */
  public MomentOfInertia getJ() {
    return m_j;
  }

  /**
   * Returns the arm's gearbox.
   *
   * @return The arm's gearbox.
   */
  public Gearbox getGearbox() {
    return m_gearbox;
  }

  /**
   * Returns the acceleration due to gravity in meters per second squared.
   *
   * @return The acceleeration due to gravity in meters per second squared.
   */
  public double getGMetersPerSecondSquared() {
    return m_g.in(MetersPerSecondPerSecond);
  }

  /**
   * Returns the acceleration due to gravity.
   *
   * @return The acceleration due to gravity.
   */
  public LinearAcceleration getG() {
    return m_g;
  }

  /**
   * Returns the length of the arm in meters.
   *
   * @return The length of the arm in meters.
   */
  public double getArmLengthMeters() {
    return m_armLength.in(Meters);
  }

  /**
   * Returns the length of the arm.
   *
   * @return The length of the arm.
   */
  public Distance getArmLength() {
    return m_armLength;
  }

  /**
   * Returns the distance of the pivot to the center of mass in meters.
   *
   * @return The distance of the pivot to the center of mass in meters.
   */
  public double getPivotDistanceMeters() {
    return m_pivotDistance.in(Meters);
  }

  /**
   * Returns the distance of the pivot to the center of mass.
   *
   * @return The distance of the pivot to the center of mass.
   */
  public Distance getPivotDistance() {
    return m_pivotDistance;
  }

  /**
   * Returns the current arm angle in radians.
   *
   * @return The current arm angle in radians.
   */
  public double getAngleRads() {
    return m_angle.in(Radians);
  }

  /**
   * Returns the current arm angle.
   *
   * @return The current arm angle.
   */
  public Angle getAngle() {
    return m_angle;
  }

  /**
   * Returns the current arm velocity in radians per second.
   *
   * @return The current arm velocity in radians per second.
   */
  public double getAngularVelocityRadPerSec() {
    return m_angularVelocity.in(RadiansPerSecond);
  }

  /**
   * Returns the current arm velocity.
   *
   * @return The current arm velocity.
   */
  public AngularVelocity getAngularVelocity() {
    return m_angularVelocity;
  }

  /**
   * Returns the acceleration of the arm in radians per second squared.
   *
   * @return The acceleration of the arm in radians per second squared.
   */
  public double getAccelerationRadPerSecSquared() {
    return m_acceleration.in(RadiansPerSecondPerSecond);
  }

  /**
   * Returns the acceleration of the arm.
   *
   * @return The acceleration of the arm.
   */
  public AngularAcceleration getAcceleration() {
    return m_acceleration;
  }

  /**
   * Returns the arm's current draw in Amps.
   *
   * @return The arm's current draw in Amps.
   */
  public double getCurrentDrawAmps() {
    return m_currentDraw.in(Amps);
  }

  /**
   * Returns the arm's current draw.
   *
   * @return The arm's current draw.
   */
  public Current getCurrentDraw() {
    return m_currentDraw;
  }

  /**
   * Returns the torque on the arm in Newton-Meters.
   *
   * @return The torque on the arm in Newton-Meters.
   */
  public double getTorqueNewtonMeters() {
    return m_torque.in(NewtonMeters);
  }

  /**
   * Returns the torque on the arm.
   *
   * @return The torque on the arm.
   */
  public Torque getTorque() {
    return m_torque;
  }

  /**
   * Returns the voltage of the arm in volts.
   *
   * @return The voltage of the arm in volts.
   */
  public double getVoltageVolts() {
    return m_voltage.in(Volts);
  }

  /**
   * Returns the voltage of the arm.
   *
   * @return The voltage of the arm.
   */
  public Voltage getVoltage() {
    return m_voltage;
  }

  /**
   * Updates the state of the arm.
   *
   * @param currentXhat The current state estimate.
   * @param u The system inputs (voltage).
   * @param dtSeconds The time difference between controller updates.
   */
  @Override
  protected Matrix<N2, N1> updateX(Matrix<N2, N1> currentXhat, Matrix<N1, N1> u, double dtSeconds) {
    // The torque on the arm is given by τ = F⋅r, where F is the force applied by
    // gravity and r the distance from pivot to center of mass. Recall from
    // dynamics that the sum of torques for a rigid body is τ = J⋅α, were τ is
    // torque on the arm, J is the mass-moment of inertia about the pivot axis,
    // and α is the angular acceleration in rad/s². Rearranging yields: α = F⋅r/J
    //
    // We substitute in F = m⋅g⋅cos(θ), where θ is the angle from horizontal:
    //
    // α = (m⋅g⋅cos(θ))⋅r/J
    //
    // Multiply RHS by cos(θ) to account for the arm angle. Further, we know the
    // arm mass-moment of inertia J of our arm is given by J=1/3 mL², modeled as a
    // rod rotating about it's end, where L is the overall rod length. The mass
    // distribution is assumed to be uniform. Substitute r=L/2 to find:
    //
    // α = (m⋅g⋅cos(θ))⋅r/J
    // α = m⋅g⋅r⋅cos(θ)/J
    //
    // This acceleration is next added to the linear system dynamics ẋ=Ax+Bu
    //
    // f(x, u) = Ax + Bu + [0 α]ᵀ
    // f(x, u) = Ax + Bu + [0 m⋅g⋅r⋅cos(θ)/J]ᵀ

    Matrix<N2, N1> updatedXhat =
        NumericalIntegration.rkdp(
            (Matrix<N2, N1> x, Matrix<N1, N1> _u) -> {
              Matrix<N2, N1> xdot = m_plant.getA().times(x).plus(m_plant.getB().times(_u));
              double alphaGrav =
                  m_mass.in(Kilograms)
                      * m_g.in(MetersPerSecondPerSecond)
                      * m_pivotDistance.in(Meters)
                      * Math.cos(x.get(0, 0))
                      / m_j.in(KilogramSquareMeters);
              xdot = xdot.plus(VecBuilder.fill(0, alphaGrav));
              return xdot;
            },
            currentXhat,
            u,
            dtSeconds);

    // We check for collision after updating xhat
    if (wouldHitLowerLimit(updatedXhat.get(0, 0))) {
      return VecBuilder.fill(m_minAngle.in(Radians), 0);
    }
    if (wouldHitUpperLimit(updatedXhat.get(0, 0))) {
      return VecBuilder.fill(m_maxAngle.in(Radians), 0);
    }
    return updatedXhat;
  }

  @Override
  public void update(double dtSeconds) {
    super.update(dtSeconds);
    m_angle.mut_replace(getOutput(0), Radians);
    m_angularVelocity.mut_replace(getOutput(1), RadiansPerSecond);
    m_acceleration.mut_replace(
        (m_plant.getA().times(m_x)).plus(m_plant.getB().times(m_u)).get(0, 0),
        RadiansPerSecondPerSecond);
    m_currentDraw.mut_replace(
        m_gearbox.currentAmps(m_x.get(1, 0), m_u.get(0, 0)) * Math.signum(m_u.get(0, 0)), Amps);
  }
}
