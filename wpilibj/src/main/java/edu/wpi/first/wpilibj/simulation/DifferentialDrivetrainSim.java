// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.wpilibj.RobotController;
import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.math.StateSpaceUtil;
import edu.wpi.first.wpilibj.system.LinearSystem;
import edu.wpi.first.wpilibj.system.NumericalIntegration;
import edu.wpi.first.wpilibj.system.plant.DCMotor;
import edu.wpi.first.wpilibj.system.plant.LinearSystemId;
import edu.wpi.first.wpilibj.util.Units;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.VecBuilder;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N2;
import edu.wpi.first.wpiutil.math.numbers.N7;

/**
 * This class simulates the state of the drivetrain. In simulationPeriodic, users should first set
 * inputs from motors with {@link #setInputs(double, double)}, call {@link #update(double)} to
 * update the simulation, and set estimated encoder and gyro positions, as well as estimated
 * odometry pose. Teams can use {@link edu.wpi.first.wpilibj.smartdashboard.Field2d} to visualize
 * their robot on the Sim GUI's field.
 *
 * <p>Our state-space system is:
 *
 * <p>x = [[x, y, theta, vel_l, vel_r, dist_l, dist_r]]^T in the field coordinate system (dist_* are
 * wheel distances.)
 *
 * <p>u = [[voltage_l, voltage_r]]^T This is typically the control input of the last timestep from a
 * LTVDiffDriveController.
 *
 * <p>y = x
 */
public class DifferentialDrivetrainSim {
  private final DCMotor m_motor;
  private final double m_originalGearing;
  private final Matrix<N7, N1> m_measurementStdDevs;
  private double m_currentGearing;
  private final double m_wheelRadiusMeters;

  @SuppressWarnings("MemberName")
  private Matrix<N2, N1> m_u;

  @SuppressWarnings("MemberName")
  private Matrix<N7, N1> m_x;

  @SuppressWarnings("MemberName")
  private Matrix<N7, N1> m_y;

  private final double m_rb;
  private final LinearSystem<N2, N2, N2> m_plant;

  /**
   * Create a SimDrivetrain.
   *
   * @param driveMotor A {@link DCMotor} representing the left side of the drivetrain.
   * @param gearing The gearing ratio between motor and wheel, as output over input. This must be
   *     the same ratio as the ratio used to identify or create the drivetrainPlant.
   * @param jKgMetersSquared The moment of inertia of the drivetrain about its center.
   * @param massKg The mass of the drivebase.
   * @param wheelRadiusMeters The radius of the wheels on the drivetrain.
   * @param trackWidthMeters The robot's track width, or distance between left and right wheels.
   * @param measurementStdDevs Standard deviations for measurements, in the form [x, y, heading,
   *     left velocity, right velocity, left distance, right distance]^T. Can be null if no noise is
   *     desired. Gyro standard deviations of 0.0001 radians, velocity standard deviations of 0.05
   *     m/s, and position measurement standard deviations of 0.005 meters are a reasonable starting
   *     point.
   */
  @SuppressWarnings("ParameterName")
  public DifferentialDrivetrainSim(
      DCMotor driveMotor,
      double gearing,
      double jKgMetersSquared,
      double massKg,
      double wheelRadiusMeters,
      double trackWidthMeters,
      Matrix<N7, N1> measurementStdDevs) {
    this(
        LinearSystemId.createDrivetrainVelocitySystem(
            driveMotor,
            massKg,
            wheelRadiusMeters,
            trackWidthMeters / 2.0,
            jKgMetersSquared,
            gearing),
        driveMotor,
        gearing,
        trackWidthMeters,
        wheelRadiusMeters,
        measurementStdDevs);
  }

  /**
   * Create a SimDrivetrain .
   *
   * @param drivetrainPlant The {@link LinearSystem} representing the robot's drivetrain. This
   *     system can be created with {@link
   *     edu.wpi.first.wpilibj.system.plant.LinearSystemId#createDrivetrainVelocitySystem(DCMotor,
   *     double, double, double, double, double)} or {@link
   *     edu.wpi.first.wpilibj.system.plant.LinearSystemId#identifyDrivetrainSystem(double, double,
   *     double, double)}.
   * @param driveMotor A {@link DCMotor} representing the drivetrain.
   * @param gearing The gearingRatio ratio of the robot, as output over input. This must be the same
   *     ratio as the ratio used to identify or create the drivetrainPlant.
   * @param trackWidthMeters The distance between the two sides of the drivetrian. Can be found with
   *     frc-characterization.
   * @param wheelRadiusMeters The radius of the wheels on the drivetrain, in meters.
   * @param measurementStdDevs Standard deviations for measurements, in the form [x, y, heading,
   *     left velocity, right velocity, left distance, right distance]^T. Can be null if no noise is
   *     desired. Gyro standard deviations of 0.0001 radians, velocity standard deviations of 0.05
   *     m/s, and position measurement standard deviations of 0.005 meters are a reasonable starting
   *     point.
   */
  public DifferentialDrivetrainSim(
      LinearSystem<N2, N2, N2> drivetrainPlant,
      DCMotor driveMotor,
      double gearing,
      double trackWidthMeters,
      double wheelRadiusMeters,
      Matrix<N7, N1> measurementStdDevs) {
    this.m_plant = drivetrainPlant;
    this.m_rb = trackWidthMeters / 2.0;
    this.m_motor = driveMotor;
    this.m_originalGearing = gearing;
    this.m_measurementStdDevs = measurementStdDevs;
    m_wheelRadiusMeters = wheelRadiusMeters;
    m_currentGearing = m_originalGearing;

    m_x = new Matrix<>(Nat.N7(), Nat.N1());
    m_u = VecBuilder.fill(0, 0);
    m_y = new Matrix<>(Nat.N7(), Nat.N1());
  }

  /**
   * Sets the applied voltage to the drivetrain. Note that positive voltage must make that side of
   * the drivetrain travel forward (+X).
   *
   * @param leftVoltageVolts The left voltage.
   * @param rightVoltageVolts The right voltage.
   */
  public void setInputs(double leftVoltageVolts, double rightVoltageVolts) {
    m_u = clampInput(VecBuilder.fill(leftVoltageVolts, rightVoltageVolts));
  }

  /**
   * Update the drivetrain states with the current time difference.
   *
   * @param dtSeconds the time difference
   */
  @SuppressWarnings("LocalVariableName")
  public void update(double dtSeconds) {

    // Update state estimate with RK4
    m_x = NumericalIntegration.rk4(this::getDynamics, m_x, m_u, dtSeconds);
    m_y = m_x;
    if (m_measurementStdDevs != null) {
      m_y = m_y.plus(StateSpaceUtil.makeWhiteNoiseVector(m_measurementStdDevs));
    }
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

  private double getOutput(State output) {
    return m_y.get(output.value, 0);
  }

  /**
   * Returns the direction the robot is pointing.
   *
   * <p>Note that this angle is counterclockwise-positive, while most gyros are clockwise positive.
   */
  public Rotation2d getHeading() {
    return new Rotation2d(getOutput(State.kHeading));
  }

  /** Returns the current pose. */
  public Pose2d getPose() {
    return new Pose2d(getOutput(State.kX), getOutput(State.kY), getHeading());
  }

  /**
   * Get the right encoder position in meters.
   *
   * @return The encoder position.
   */
  public double getRightPositionMeters() {
    return getOutput(State.kRightPosition);
  }

  /**
   * Get the right encoder velocity in meters per second.
   *
   * @return The encoder velocity.
   */
  public double getRightVelocityMetersPerSecond() {
    return getOutput(State.kRightVelocity);
  }

  /**
   * Get the left encoder position in meters.
   *
   * @return The encoder position.
   */
  public double getLeftPositionMeters() {
    return getOutput(State.kLeftPosition);
  }

  /**
   * Get the left encoder velocity in meters per second.
   *
   * @return The encoder velocity.
   */
  public double getLeftVelocityMetersPerSecond() {
    return getOutput(State.kLeftVelocity);
  }

  /**
   * Get the current draw of the left side of the drivetrain.
   *
   * @return the drivetrain's left side current draw, in amps
   */
  public double getLeftCurrentDrawAmps() {
    var loadIleft =
        m_motor.getCurrent(
                getState(State.kLeftVelocity) * m_currentGearing / m_wheelRadiusMeters,
                m_u.get(0, 0))
            * Math.signum(m_u.get(0, 0));
    return loadIleft;
  }

  /**
   * Get the current draw of the right side of the drivetrain.
   *
   * @return the drivetrain's right side current draw, in amps
   */
  public double getRightCurrentDrawAmps() {
    var loadIright =
        m_motor.getCurrent(
                getState(State.kRightVelocity) * m_currentGearing / m_wheelRadiusMeters,
                m_u.get(1, 0))
            * Math.signum(m_u.get(1, 0));

    return loadIright;
  }

  /**
   * Get the current draw of the drivetrain.
   *
   * @return the current draw, in amps
   */
  public double getCurrentDrawAmps() {
    return getLeftCurrentDrawAmps() + getRightCurrentDrawAmps();
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
   * Sets the gearing reduction on the drivetrain. This is commonly used for shifting drivetrains.
   *
   * @param newGearRatio The new gear ratio, as output over input.
   */
  public void setCurrentGearing(double newGearRatio) {
    this.m_currentGearing = newGearRatio;
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

  @SuppressWarnings({"DuplicatedCode", "LocalVariableName", "ParameterName"})
  protected Matrix<N7, N1> getDynamics(Matrix<N7, N1> x, Matrix<N2, N1> u) {

    // Because G can be factored out of B, we can divide by the old ratio and multiply
    // by the new ratio to get a new drivetrain model.
    var B = new Matrix<>(Nat.N4(), Nat.N2());
    B.assignBlock(0, 0, m_plant.getB().times(this.m_currentGearing / this.m_originalGearing));

    // Because G^2 can be factored out of A, we can divide by the old ratio squared and multiply
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
   * Clamp the input vector such that no element exceeds the given voltage. If any does, the
   * relative magnitudes of the input will be maintained.
   *
   * @param u The input vector.
   * @return The normalized input.
   */
  protected Matrix<N2, N1> clampInput(Matrix<N2, N1> u) {
    return StateSpaceUtil.normalizeInputVector(u, RobotController.getBatteryVoltage());
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

    @SuppressWarnings("MemberName")
    public final int value;

    @SuppressWarnings("ParameterName")
    State(int i) {
      this.value = i;
    }
  }

  /**
   * Represents a gearing option of the Toughbox mini. 12.75:1 -- 14:50 and 14:50 10.71:1 -- 14:50
   * and 16:48 8.45:1 -- 14:50 and 19:45 7.31:1 -- 14:50 and 21:43 5.95:1 -- 14:50 and 24:40
   */
  public enum KitbotGearing {
    k12p75(12.75),
    k10p71(10.71),
    k8p45(8.45),
    k7p31(7.31),
    k5p95(5.95);

    @SuppressWarnings("MemberName")
    public final double value;

    @SuppressWarnings("ParameterName")
    KitbotGearing(double i) {
      this.value = i;
    }
  }

  /** Represents common motor layouts of the kit drivetrain. */
  public enum KitbotMotor {
    kSingleCIMPerSide(DCMotor.getCIM(1)),
    kDualCIMPerSide(DCMotor.getCIM(2)),
    kSingleMiniCIMPerSide(DCMotor.getMiniCIM(1)),
    kDualMiniCIMPerSide(DCMotor.getMiniCIM(2));

    @SuppressWarnings("MemberName")
    public final DCMotor value;

    @SuppressWarnings("ParameterName")
    KitbotMotor(DCMotor i) {
      this.value = i;
    }
  }

  /** Represents common wheel sizes of the kit drivetrain. */
  public enum KitbotWheelSize {
    SixInch(Units.inchesToMeters(6)),
    EightInch(Units.inchesToMeters(8)),
    TenInch(Units.inchesToMeters(10));

    @SuppressWarnings("MemberName")
    public final double value;

    @SuppressWarnings("ParameterName")
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
   *     left velocity, right velocity, left distance, right distance]^T. Can be null if no noise is
   *     desired. Gyro standard deviations of 0.0001 radians, velocity standard deviations of 0.05
   *     m/s, and position measurement standard deviations of 0.005 meters are a reasonable starting
   *     point.
   */
  public static DifferentialDrivetrainSim createKitbotSim(
      KitbotMotor motor,
      KitbotGearing gearing,
      KitbotWheelSize wheelSize,
      Matrix<N7, N1> measurementStdDevs) {
    // MOI estimation -- note that I = m r^2 for point masses
    var batteryMoi = 12.5 / 2.2 * Math.pow(Units.inchesToMeters(10), 2);
    var gearboxMoi =
        (2.8 /* CIM motor */ * 2 / 2.2 + 2.0 /* Toughbox Mini- ish */)
            * Math.pow(Units.inchesToMeters(26.0 / 2.0), 2);

    return createKitbotSim(motor, gearing, wheelSize, batteryMoi + gearboxMoi, measurementStdDevs);
  }

  /**
   * Create a sim for the standard FRC kitbot.
   *
   * @param motor The motors installed in the bot.
   * @param gearing The gearing reduction used.
   * @param wheelSize The wheel size.
   * @param jKgMetersSquared The moment of inertia of the drivebase. This can be calculated using
   *     frc-characterization.
   * @param measurementStdDevs Standard deviations for measurements, in the form [x, y, heading,
   *     left velocity, right velocity, left distance, right distance]^T. Can be null if no noise is
   *     desired. Gyro standard deviations of 0.0001 radians, velocity standard deviations of 0.05
   *     m/s, and position measurement standard deviations of 0.005 meters are a reasonable starting
   *     point.
   */
  @SuppressWarnings("ParameterName")
  public static DifferentialDrivetrainSim createKitbotSim(
      KitbotMotor motor,
      KitbotGearing gearing,
      KitbotWheelSize wheelSize,
      double jKgMetersSquared,
      Matrix<N7, N1> measurementStdDevs) {
    return new DifferentialDrivetrainSim(
        motor.value,
        gearing.value,
        jKgMetersSquared,
        25 / 2.2,
        wheelSize.value / 2.0,
        Units.inchesToMeters(26),
        measurementStdDevs);
  }
}
