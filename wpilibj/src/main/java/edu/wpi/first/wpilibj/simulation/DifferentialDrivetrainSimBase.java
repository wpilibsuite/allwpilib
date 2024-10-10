// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static edu.wpi.first.units.Units.Amps;
import static edu.wpi.first.units.Units.KilogramSquareMeters;
import static edu.wpi.first.units.Units.Kilograms;
import static edu.wpi.first.units.Units.Meters;
import static edu.wpi.first.units.Units.MetersPerSecond;
import static edu.wpi.first.units.Units.NewtonMeters;
import static edu.wpi.first.units.Units.Volts;

import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.numbers.N4;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.plant.DCMotorType;
import edu.wpi.first.math.system.plant.Gearbox;
import edu.wpi.first.math.system.plant.Wheel;
import edu.wpi.first.math.util.Units;
import edu.wpi.first.units.measure.Current;
import edu.wpi.first.units.measure.Distance;
import edu.wpi.first.units.measure.LinearVelocity;
import edu.wpi.first.units.measure.Mass;
import edu.wpi.first.units.measure.MomentOfInertia;
import edu.wpi.first.units.measure.MutCurrent;
import edu.wpi.first.units.measure.MutDistance;
import edu.wpi.first.units.measure.MutLinearVelocity;
import edu.wpi.first.units.measure.MutTorque;
import edu.wpi.first.units.measure.MutVoltage;
import edu.wpi.first.units.measure.Voltage;

/** This class simulates the state of the drivetrain. */
public class DifferentialDrivetrainSimBase extends LinearSystemSim<N4, N2, N4> {
  /**
   * Represents a gearing option of the Toughbox mini. 12.75:1 -- 14:50 and 14:50
   * 10.71:1 -- 14:50
   * and 16:48 8.45:1 -- 14:50 and 19:45 7.31:1 -- 14:50 and 21:43 5.95:1 -- 14:50
   * and 24:40
   */
  public enum KitbotGearing {
    /** Gear ratio of 12.75:1. */
    k12p75(12.75),
    /** Gear ratio of 10.71:1. */
    k10p71(10.71),
    /** Gear ratio of 8.45:1. */
    k8p45(8.45),
    /** Gear ratio of 7.31:1. */
    k7p31(7.31),
    /** Gear ratio of 5.95:1. */
    k5p95(5.95);

    /** KitbotGearing value. */
    public final double value;

    KitbotGearing(double i) {
      this.value = i;
    }
  }

  /** Represents common motor layouts of the kit drivetrain. */
  public enum KitbotGearbox {
    /** One CIM motor per drive side. */
    kSingleCIMPerSide(new Gearbox(1, DCMotorType.CIM)),
    /** Two CIM motors per drive side. */
    kDualCIMPerSide(new Gearbox(2, DCMotorType.CIM)),
    /** One Mini CIM motor per drive side. */
    kSingleMiniCIMPerSide(new Gearbox(1, DCMotorType.MiniCIM)),
    /** Two Mini CIM motors per drive side. */
    kDualMiniCIMPerSide(new Gearbox(2, DCMotorType.MiniCIM)),
    /** One Falcon 500 motor per drive side. */
    kSingleFalcon500PerSide(new Gearbox(1, DCMotorType.Falcon500)),
    /** Two Falcon 500 motors per drive side. */
    kDoubleFalcon500PerSide(new Gearbox(2, DCMotorType.Falcon500)),
    /** One NEO motor per drive side. */
    kSingleNEOPerSide(new Gearbox(1, DCMotorType.NEO)),
    /** Two NEO motors per drive side. */
    kDoubleNEOPerSide(new Gearbox(2, DCMotorType.NEO));

    /** KitbotGearbox value. */
    public final Gearbox value;

    KitbotGearbox(Gearbox i) {
      this.value = i;
    }
  }

  /** Represents common wheel sizes of the kit drivetrain. */
  public enum KitbotWheelSize {
    /** Six inch diameter wheels. */
    kSixInch(Units.inchesToMeters(6)),
    /** Eight inch diameter wheels. */
    kEightInch(Units.inchesToMeters(8)),
    /** Ten inch diameter wheels. */
    kTenInch(Units.inchesToMeters(10));

    /** KitbotWheelSize value. */
    public final double value;

    KitbotWheelSize(double i) {
      this.value = i;
    }
  }

  /** The drive train's wheel. */
  protected final Wheel m_wheel;

  /** The mass of the drivetrain. */
  private final Mass m_mass;

  /** The moment of inertia of the drivetrain about its center. */
  private final MomentOfInertia m_J;

  /** The distance between left and right wheels. */
  private final Distance m_trackWidth;

  /** The velocity of the left wheel. */
  private final MutLinearVelocity m_leftVelocity = MetersPerSecond.mutable(0.0);

  /** The velocity of the right wheel. */
  private final MutLinearVelocity m_rightVelocity = MetersPerSecond.mutable(0.0);

  /** The position of the left wheel. */
  private final MutDistance m_leftPosition = Meters.mutable(0.0);

  /** The position of the right wheel. */
  private final MutDistance m_rightPosition = Meters.mutable(0.0);

  /** The current draw of the left wheel. */
  protected final MutCurrent m_leftCurrentDraw = Amps.mutable(0.0);

  /** The current draw of the right wheel. */
  protected final MutCurrent m_rightCurrentDraw = Amps.mutable(0.0);

  /** The total current draw of the wheels. */
  protected final MutCurrent m_currentDraw = Amps.mutable(0.0);

  /** The voltage of the left wheel. */
  protected final MutVoltage m_leftVoltage = Volts.mutable(0.0);

  /** The voltage of the right wheel. */
  protected final MutVoltage m_rightVoltage = Volts.mutable(0.0);

  /** The torque of the left wheel. */
  protected final MutTorque m_leftTorque = NewtonMeters.mutable(0.0);

  /** The torque of the right wheel. */
  protected final MutTorque m_rightTorque = NewtonMeters.mutable(0.0);

  /** The current reduction of the gearboxes. */
  protected double m_currentGearing;

  /**
   * Creates a simulated differential drivetrain.
   *
   * @param plant              The linear system that represents the drivetrain.
   * @param wheel              A {@link Wheel} representing one of the
   *                           drivetrain's wheel.
   * @param trackWidth         The robot's track width, or distance between left
   *                           and right wheels.
   * @param measurementStdDevs Standard deviations for measurements, in the form
   *                           [x, y, heading,
   *                           left velocity, right velocity, left distance, right
   *                           distance]ᵀ. Can be null if no noise is
   *                           desired. Gyro standard deviations of 0.0001
   *                           radians, velocity standard deviations of 0.05
   *                           m/s, and position measurement standard deviations
   *                           of 0.005 meters are a reasonable starting
   *                           point. If present must have 7 elements matching the
   *                           aforementioned measurements.
   */
  public DifferentialDrivetrainSimBase(
      LinearSystem<N4, N2, N4> plant,
      Wheel wheel,
      Distance trackWidth,
      double... measurementStdDevs) {
    super(plant, measurementStdDevs);
    m_wheel = wheel;
    m_trackWidth = trackWidth;
    double A0 = plant.getA(0, 0);
    double A1 = plant.getA(1, 0);
    var C1 = -wheel.gearbox.numMotors
        * Math.pow(wheel.gearbox.reduction, 2)
        * wheel.gearbox.motorType.KtNMPerAmp
        / (wheel.gearbox.motorType.KvRadPerSecPerVolt
            * wheel.gearbox.motorType.rOhms
            * Math.pow(wheel.radiusMeters, 2));
    double a0massJPart = A0 / C1;
    double a1massJPart = A1 / C1;
    double halfMassRecip = a0massJPart + a1massJPart;
    double massKg = 1.0 / halfMassRecip;
    m_mass = Kilograms.of(massKg);
    double jKgMetersSquared = Math.pow(trackWidth.in(Meters), 2) / (a0massJPart - (1.0 / massKg));
    m_J = KilogramSquareMeters.of(jKgMetersSquared);
  }

  /**
   * Get the drivetrain gearing.
   *
   * @return the gearing ration
   */
  public double getCurrentGearing() {
    return m_currentGearing;
  }

  /**
   * Sets the position of the left and right encoders.
   *
   * @param leftPositionMeters  The new left encoder position in meters.
   * @param rightPositionMeters The new right encoder position in meters.
   */
  public void setEncoderPositions(double leftPositionMeters, double rightPositionMeters) {
    m_x.set(5, 0, leftPositionMeters);
    m_x.set(6, 0, rightPositionMeters);
  }

  /**
   * Sets the position of the left and right encoders.
   *
   * @param leftPosition  The new left encoder position.
   * @param rightPosition The new right encoder position.
   */
  public void setEncoderPositions(Distance leftPosition, Distance rightPosition) {
    setEncoderPositions(leftPosition.in(Meters), rightPosition.in(Meters));
  }

  /**
   * Get the left encoder position.
   *
   * @return The left encoder position.
   */
  public Distance getLeftPosition() {
    return m_leftPosition;
  }

  /**
   * Get the left encoder position in meters.
   *
   * @return The left encoder position in meters.
   */
  public double getLeftPositionMeters() {
    return m_leftPosition.in(Meters);
  }

  /**
   * Get the right encoder position.
   *
   * @return The right encoder position.
   */
  public Distance getRightPosition() {
    return m_rightPosition;
  }

  /**
   * Get the right encoder position in meters.
   *
   * @return The right encoder position in meters.
   */
  public double getRightPositionMeters() {
    return m_rightPosition.in(Meters);
  }

  /**
   * Sets the velocity of the left and right encoders.
   *
   * @param leftVelocityMetersPerSecond  The new left encoder velocity in meters
   *                                     per second.
   * @param rightVelocityMetersPerSecond The new right encoder velocity in meters
   *                                     per second.
   */
  public void setEncoderVelocities(
      double leftVelocityMetersPerSecond, double rightVelocityMetersPerSecond) {
    m_x.set(3, 0, leftVelocityMetersPerSecond);
    m_x.set(4, 0, rightVelocityMetersPerSecond);
  }

  /**
   * Sets the position of the left and right encoders.
   *
   * @param leftVelocity  The new left encoder velocity.
   * @param rightVelocity The new right encoder velocity.
   */
  public void setEncoderVelocities(LinearVelocity leftVelocity, LinearVelocity rightVelocity) {
    setEncoderVelocities(leftVelocity.in(MetersPerSecond), rightVelocity.in(MetersPerSecond));
  }

  /**
   * Get the left encoder velocity.
   *
   * @return The left encoder velocity.
   */
  public LinearVelocity getLeftVelocity() {
    return m_leftVelocity;
  }

  /**
   * Get the left encoder velocity in meters per second.
   *
   * @return The encoder velocity.
   */
  public double getLeftVelocityMetersPerSecond() {
    return m_leftVelocity.in(MetersPerSecond);
  }

  /**
   * Get the right encoder velocity.
   *
   * @return The right encoder velocity.
   */
  public LinearVelocity getRightVelocity() {
    return m_rightVelocity;
  }

  /**
   * Get the right encoder velocity in meters per second.
   *
   * @return The encoder velocity.
   */
  public double getRightVelocityMetersPerSecond() {
    return m_rightVelocity.in(MetersPerSecond);
  }

  /**
   * Returns the mass of the drivetrain in kilograms.
   *
   * @return the drivetrain mass of in kilograms.
   */
  public double getMassKilograms() {
    return m_mass.in(Kilograms);
  }

  /**
   * Returns the mass of the drivetrain.
   *
   * @return the drivetrain mass.
   */
  public Mass getMass() {
    return m_mass;
  }

  /**
   * Returns the moment of inertia in kilograms meters squared around the
   * drivetrain's center.
   *
   * @return The moment of inertia in kilograms meters squared around the
   *         drivetrain's center.
   */
  public double getJKgMetersSquared() {
    return m_J.in(KilogramSquareMeters);
  }

  /**
   * Returns the moment of inertia around the drivetrain's center.
   *
   * @return The moment of inertia around the drivetrain's center.
   */
  public MomentOfInertia getJ() {
    return m_J;
  }

  /**
   * Returns the wheel used by the drivetrain.
   *
   * @return The wheel used by the drivetrain.
   */
  public Wheel getWheel() {
    return m_wheel;
  }

  /**
   * Returns the track width of the drivetrain in meters.
   *
   * @return the track width of the drivetrain in meters.
   */
  public double getTrackWidthMeters() {
    return m_trackWidth.in(Meters);
  }

  /**
   * Returns the track width of the drivetrain.
   *
   * @return the track width of the drivetrain.
   */
  public Distance getTrackWidth() {
    return m_trackWidth;
  }

  /**
   * Get the current draw of the left side of the drivetrain.
   *
   * @return the drivetrain's left side current draw.
   */
  public Current getLeftCurrentDraw() {
    return m_leftCurrentDraw;
  }

  /**
   * Get the current draw of the left side of the drivetrain.
   *
   * @return the drivetrain's left side current draw, in amps
   */
  public double getLeftCurrentDrawAmps() {
    return m_leftCurrentDraw.in(Amps);
  }

  /**
   * Get the current draw of the right side of the drivetrain.
   *
   * @return the drivetrain's right side current draw.
   */
  public Current getRightCurrentDraw() {
    return m_rightCurrentDraw;
  }

  /**
   * Get the current draw of the right side of the drivetrain.
   *
   * @return the drivetrain's right side current draw, in amps
   */
  public double getRightCurrentDrawAmps() {
    return m_rightCurrentDraw.in(Amps);
  }

  /**
   * Get the voltage of the left side of the drivetrain.
   *
   * @return the drivetrain's left side voltage.
   */
  public Voltage getLeftVoltage() {
    return m_leftVoltage;
  }

  /**
   * Get the voltage of the left side of the drivetrain.
   *
   * @return the drivetrain's left side voltage in volts.
   */
  public double getLeftVoltageVolts() {
    return m_leftVoltage.in(Volts);
  }

  /**
   * Get the voltage of the right side of the drivetrain.
   *
   * @return the drivetrain's right side voltage.
   */
  public Voltage getRightVoltage() {
    return m_rightVoltage;
  }

  /**
   * Get the voltage of the right side of the drivetrain.
   *
   * @return the drivetrain's right side voltage in volts.
   */
  public double getRightVoltageVolts() {
    return m_rightVoltage.in(Volts);
  }

  /**
   * Get the current draw of the drivetrain.
   *
   * @return the current draw.
   */
  public Current getCurrentDraw() {
    return m_currentDraw;
  }

  /**
   * Get the current draw of the drivetrain.
   *
   * @return the current draw, in amps
   */
  public double getCurrentDrawAmps() {
    return m_currentDraw.in(Amps);
  }

  /**
   * Update the drivetrain states with the current time difference.
   *
   * @param dtSeconds the time difference
   */
  @Override
  public void update(double dtSeconds) {
    super.update(dtSeconds);
    m_leftPosition.mut_replace(getOutput(2), Meters);
    m_rightPosition.mut_replace(getOutput(3), Meters);
    m_leftVelocity.mut_replace(getOutput(0), MetersPerSecond);
    m_rightVelocity.mut_replace(getOutput(1), MetersPerSecond);
    m_leftCurrentDraw.mut_replace(
        m_wheel.currentAmps(m_x.get(0, 0), m_u.get(0, 0)) * Math.signum(m_u.get(0, 0)), Amps);
    m_rightCurrentDraw.mut_replace(
        m_wheel.currentAmps(m_x.get(1, 0), m_u.get(1, 0)) * Math.signum(m_u.get(1, 0)), Amps);
    m_currentDraw.mut_replace(m_leftCurrentDraw.in(Amps) + m_rightCurrentDraw.in(Amps), Amps);
  }
}
