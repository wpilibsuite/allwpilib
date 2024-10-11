// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static edu.wpi.first.units.Units.Amps;
import static edu.wpi.first.units.Units.Meters;
import static edu.wpi.first.units.Units.MetersPerSecond;
import static edu.wpi.first.units.Units.NewtonMeters;
import static edu.wpi.first.units.Units.Volts;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.StateSpaceUtil;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.numbers.N7;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.NumericalIntegration;
import edu.wpi.first.math.system.plant.DCMotorType;
import edu.wpi.first.math.system.plant.Gearbox;
import edu.wpi.first.math.system.plant.LinearSystemId;
import edu.wpi.first.math.system.plant.Wheel;
import edu.wpi.first.math.util.Units;
import edu.wpi.first.units.measure.Current;
import edu.wpi.first.units.measure.Distance;
import edu.wpi.first.units.measure.LinearVelocity;
import edu.wpi.first.units.measure.MutCurrent;
import edu.wpi.first.units.measure.MutDistance;
import edu.wpi.first.units.measure.MutLinearVelocity;
import edu.wpi.first.units.measure.MutTorque;
import edu.wpi.first.units.measure.MutVoltage;
import edu.wpi.first.units.measure.Voltage;

/** This class simulates the state of the drivetrain. */
public class DifferentialDrivetrainSimBase {
  /** The motor, gearbox, reduction, and wheel radius of one of drivetrain wheels. */
  protected final Wheel m_driveWheel;

  /** The original reduction of the gearboxes. */
  protected final double m_originalGearing;

  /** The current reduction of the gearboxes. */
  protected double m_currentGearing;

  /** The radius of the drivetrain. Half of the distance between the wheels. */
  protected final double m_rb;

  /** The linear system representing the drivetrain wheel velocities. */
  protected final LinearSystem<N2, N2, N2> m_plant;

  /**
   * Standard deviations for measurements, in the form [x, y, heading, left velocity, right
   * velocity, left distance, right distance]ᵀ.
   */
  protected final Matrix<N7, N1> m_measurementStdDevs;

  /** The input matrix. */
  protected Matrix<N2, N1> m_u;

  /** The state matrix. */
  protected Matrix<N7, N1> m_x;

  /** The output matrix. */
  protected Matrix<N7, N1> m_y;

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

  /**
   * Creates a simulated differential drivetrain.
   *
   * @param plant The linear system that represents the drivetrain.
   * @param driveWheel A {@link Wheel} representing one of the drivetrain's wheels.
   * @param trackWidthMeters The robot's track width, or distance between left and right wheels.
   * @param measurementStdDevs Standard deviations for measurements, in the form [x, y, heading,
   *     left velocity, right velocity, left distance, right distance]ᵀ. Can be null if no noise is
   *     desired. Gyro standard deviations of 0.0001 radians, velocity standard deviations of 0.05
   *     m/s, and position measurement standard deviations of 0.005 meters are a reasonable starting
   *     point. If present must have 7 elements matching the aforementioned measurements.
   */
  public DifferentialDrivetrainSimBase(
      LinearSystem<N2, N2, N2> plant,
      Wheel driveWheel,
      double trackWidthMeters,
      Matrix<N7, N1> measurementStdDevs) {
    m_x = new Matrix<>(Nat.N7(), Nat.N1());
    m_u = VecBuilder.fill(0, 0);
    m_y = new Matrix<>(Nat.N7(), Nat.N1());
    m_plant = plant;
    m_driveWheel = driveWheel;
    m_originalGearing = driveWheel.gearbox.reduction;
    m_currentGearing = m_originalGearing;
    m_rb = trackWidthMeters / 2.0;
    m_measurementStdDevs = measurementStdDevs;
  }

  /**
   * Sets the gearing reduction on the drivetrain. This is commonly used for shifting drivetrains.
   *
   * @param newGearRatio The new gear ratio, as output over input.
   */
  public void setCurrentGearing(double newGearRatio) {
    this.m_currentGearing = newGearRatio;
  }

  /**
   * Get the drivetrain gearing.
   *
   * @return the gearing ration
   */
  public double getCurrentGearing() {
    return m_currentGearing;
  }

  /** Returns the full simulated state of the drivetrain. */
  Matrix<N7, N1> getState() {
    return m_x;
  }

  /**
   * Get one of the drivetrain states.
   *
   * @param state the state to get
   * @return the state
   */
  double getState(State state) {
    return m_x.get(state.value, 0);
  }

  /**
   * Returns the direction the robot is pointing.
   *
   * <p>Note that this angle is counterclockwise-positive, while most gyros are clockwise positive.
   *
   * @return The direction the robot is pointing.
   */
  public Rotation2d getHeading() {
    return new Rotation2d(getOutput(State.kHeading));
  }

  /**
   * Returns the current pose.
   *
   * @return The current pose.
   */
  public Pose2d getPose() {
    return new Pose2d(getOutput(State.kX), getOutput(State.kY), getHeading());
  }

  /**
   * Sets the system state.
   *
   * @param state The state.
   */
  public void setState(Matrix<N7, N1> state) {
    m_x = state;
  }

  /**
   * Sets the system pose.
   *
   * @param pose The pose.
   */
  public void setPose(Pose2d pose) {
    m_x.set(State.kX.value, 0, pose.getX());
    m_x.set(State.kY.value, 0, pose.getY());
    m_x.set(State.kHeading.value, 0, pose.getRotation().getRadians());
    m_x.set(State.kLeftPosition.value, 0, 0);
    m_x.set(State.kRightPosition.value, 0, 0);
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

  private double getOutput(State output) {
    return m_y.get(output.value, 0);
  }

  /**
   * The differential drive dynamics function.
   *
   * @param x The state.
   * @param u The input.
   * @return The state derivative with respect to time.
   */
  protected Matrix<N7, N1> getDynamics(Matrix<N7, N1> x, Matrix<N2, N1> u) {
    // Because G can be factored out of B, we can divide by the old ratio and
    // multiply
    // by the new ratio to get a new drivetrain model.
    var B = new Matrix<>(Nat.N4(), Nat.N2());
    B.assignBlock(0, 0, m_plant.getB().times(this.m_currentGearing / this.m_originalGearing));

    // Because G² can be factored out of A, we can divide by the old ratio squared
    // and multiply
    // by the new ratio squared to get a new drivetrain model.
    var A = new Matrix<>(Nat.N4(), Nat.N4());
    A.assignBlock(
        0,
        0,
        m_plant
            .getA()
            .times(
                (this.m_currentGearing * this.m_currentGearing)
                    / (this.m_originalGearing * this.m_originalGearing)));

    A.assignBlock(2, 0, Matrix.eye(Nat.N2()));

    var v = (x.get(State.kLeftVelocity.value, 0) + x.get(State.kRightVelocity.value, 0)) / 2.0;

    var xdot = new Matrix<>(Nat.N7(), Nat.N1());
    xdot.set(0, 0, v * Math.cos(x.get(State.kHeading.value, 0)));
    xdot.set(1, 0, v * Math.sin(x.get(State.kHeading.value, 0)));
    xdot.set(
        2,
        0,
        (x.get(State.kRightVelocity.value, 0) - x.get(State.kLeftVelocity.value, 0))
            / (2.0 * m_rb));
    xdot.assignBlock(3, 0, A.times(x.block(Nat.N4(), Nat.N1(), 3, 0)).plus(B.times(u)));

    return xdot;
  }

  /**
   * Update the drivetrain states with the current time difference.
   *
   * @param dtSeconds the time difference
   */
  public void update(double dtSeconds) {
    m_x = NumericalIntegration.rkdp(this::getDynamics, m_x, m_u, dtSeconds);
    m_y = m_x;
    if (m_measurementStdDevs != null) {
      m_y = m_y.plus(StateSpaceUtil.makeWhiteNoiseVector(m_measurementStdDevs));
    }
    m_leftPosition.mut_replace(getOutput(State.kLeftPosition), Meters);
    m_rightPosition.mut_replace(getOutput(State.kRightPosition), Meters);
    m_leftVelocity.mut_replace(getOutput(State.kLeftVelocity), MetersPerSecond);
    m_rightVelocity.mut_replace(getOutput(State.kRightVelocity), MetersPerSecond);
    m_leftCurrentDraw.mut_replace(
        m_driveWheel.currentAmps(m_x.get(0, 0), m_u.get(0, 0)) * Math.signum(m_u.get(0, 0)), Amps);
    m_rightCurrentDraw.mut_replace(
        m_driveWheel.currentAmps(m_x.get(1, 0), m_u.get(1, 0)) * Math.signum(m_u.get(1, 0)), Amps);
    m_currentDraw.mut_replace(m_leftCurrentDraw.in(Amps) + m_rightCurrentDraw.in(Amps), Amps);
  }

  /** Represents the different states of the drivetrain. */
  enum State {
    kX(0),
    kY(1),
    kHeading(2),
    kLeftVelocity(3),
    kRightVelocity(4),
    kLeftPosition(5),
    kRightPosition(6);

    public final int value;

    State(int i) {
      this.value = i;
    }
  }

  /**
   * Represents a gearing option of the Toughbox mini. 12.75:1 -- 14:50 and 14:50 10.71:1 -- 14:50
   * and 16:48 8.45:1 -- 14:50 and 19:45 7.31:1 -- 14:50 and 21:43 5.95:1 -- 14:50 and 24:40
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
    kDualMiniCIMPerSide(new Gearbox(2, DCMotorType.CIM)),
    /** One Falcon 500 motor per drive side. */
    kSingleFalcon500PerSide(new Gearbox(1, DCMotorType.Falcon500)),
    /** Two Falcon 500 motors per drive side. */
    kDoubleFalcon500PerSide(new Gearbox(2, DCMotorType.Falcon500)),
    /** One NEO motor per drive side. */
    kSingleNEOPerSide(new Gearbox(1, DCMotorType.NEO)),
    /** Two NEO motors per drive side. */
    kDoubleNEOPerSide(new Gearbox(2, DCMotorType.NEO));

    /** KitbotMotor value. */
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

  /**
   * Create a sim for the standard FRC kitbot.
   *
   * @param motor The motors installed in the bot.
   * @param gearing The gearing reduction used.
   * @param wheelSize The wheel size.
   * @param measurementStdDevs Standard deviations for measurements, in the form [x, y, heading,
   *     left velocity, right velocity, left distance, right distance]ᵀ. Can be null if no noise is
   *     desired. Gyro standard deviations of 0.0001 radians, velocity standard deviations of 0.05
   *     m/s, and position measurement standard deviations of 0.005 meters are a reasonable starting
   *     point.
   * @return A sim for the standard FRC kitbot.
   */
  public static DifferentialDrivetrainSim createKitbotSim(
      KitbotGearbox motor,
      KitbotGearing gearing,
      KitbotWheelSize wheelSize,
      Matrix<N7, N1> measurementStdDevs) {
    // MOI estimation -- note that I = mr² for point masses
    var batteryMoi = 12.5 / 2.2 * Math.pow(Units.inchesToMeters(10), 2);
    var gearboxMoi =
        (2.8 /* CIM motor */ * 2 / 2.2 + 2.0 /* Toughbox Mini- ish */)
            * Math.pow(Units.inchesToMeters(26.0 / 2.0), 2);

    return createKitbotSim(motor, gearing, wheelSize, batteryMoi + gearboxMoi, measurementStdDevs);
  }

  /**
   * Create a sim for the standard FRC kitbot.
   *
   * @param gearbox The gearbox installed in the bot.
   * @param gearing The gearing reduction used.
   * @param wheelSize The wheel size.
   * @param jKgMetersSquared The moment of inertia of the drivebase. This can be calculated using
   *     SysId.
   * @param measurementStdDevs Standard deviations for measurements, in the form [x, y, heading,
   *     left velocity, right velocity, left distance, right distance]ᵀ. Can be null if no noise is
   *     desired. Gyro standard deviations of 0.0001 radians, velocity standard deviations of 0.05
   *     m/s, and position measurement standard deviations of 0.005 meters are a reasonable starting
   *     point.
   * @return A sim for the standard FRC kitbot.
   */
  public static DifferentialDrivetrainSim createKitbotSim(
      KitbotGearbox gearbox,
      KitbotGearing gearing,
      KitbotWheelSize wheelSize,
      double jKgMetersSquared,
      Matrix<N7, N1> measurementStdDevs) {
    Wheel driveWheel = new Wheel(gearbox.value.withReduction(gearing.value), wheelSize.value / 2);
    LinearSystem<N2, N2, N2> plant =
        LinearSystemId.createDrivetrainVelocitySystem(
            driveWheel,
            Units.lbsToKilograms(jKgMetersSquared),
            Units.inchesToMeters(26 / 2),
            jKgMetersSquared);
    return new DifferentialDrivetrainSim(
        plant, driveWheel, Units.inchesToMeters(26), measurementStdDevs);
  }
}
