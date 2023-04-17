package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.StateSpaceUtil;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Twist2d;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.numbers.N3;
import edu.wpi.first.math.numbers.N9;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.NumericalIntegration;
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.math.system.plant.HDrivetrainLateralSystemId;
import edu.wpi.first.math.system.plant.LinearSystemId;
import edu.wpi.first.wpilibj.RobotController;

/**
 * This class simulates the state of the drivetrain. In simulationPeriodic, users should first set
 * inputs from motors with {@link #setInputs(double, double, double)}, call {@link #update(double)}
 * to update the simulation, and set estimated encoder and gyro positions, as well as estimated
 * odometry pose. Teams can use {@link edu.wpi.first.wpilibj.smartdashboard.Field2d} to visualize
 * their robot on the Sim GUI's field.
 *
 * <p>Our state-space system is:
 *
 * <p>x = [[x, y, theta, vel_le, vel_ri, vel_la, dist_le, dist_ri, dist_la]]ᵀ in the field
 * coordinate system (dist_* are wheel distances.)
 *
 * <p>u = [[voltage_le, voltage_ri, voltage_la]]ᵀ This is typically the control input of the last
 * timestep from a LTV-HDriveController.
 *
 * <p>y = x
 */
public class HDrivetrainSim {
  private final DCMotor m_motor;
  private final Matrix<N9, N1> m_measurementStdDevs;
  private double m_currentGearing;
  private final double m_wheelRadiusMeters;
  private Matrix<N3, N1> m_u;
  private Matrix<N9, N1> m_x;
  private Matrix<N9, N1> m_y;

  private final double m_rb;
  private final LinearSystem<N2, N2, N2> m_plant;
  private final LinearSystem<N1, N1, N1> m_lateralPlant;

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
   * @param lateralWheelOffsetFromCenterOfGravity The distance from the robots center of gravity
   *     that the lateral wheel is placed.
   * @param measurementStdDevs Standard deviations for measurements, in the form [x, y, heading,
   *     left velocity, right velocity, lateral velocity, left distance, right distance, lateral
   *     distance]ᵀ. Can be null if no noise is desired. Gyro standard deviations of 0.0001 radians,
   *     velocity standard deviations of 0.05 m/s, and position measurement standard deviations of
   *     0.005 meters are a reasonable starting point.
   */
  public HDrivetrainSim(
      DCMotor driveMotor,
      double gearing,
      double jKgMetersSquared,
      double massKg,
      double wheelRadiusMeters,
      double trackWidthMeters,
      double lateralWheelOffsetFromCenterOfGravity,
      Matrix<N9, N1> measurementStdDevs) {
    this(
        LinearSystemId.createDrivetrainVelocitySystem(
            driveMotor,
            massKg,
            wheelRadiusMeters,
            trackWidthMeters / 2.0,
            jKgMetersSquared,
            gearing),
        HDrivetrainLateralSystemId.createHDrivetrainLateralVelocitySystem(
            driveMotor,
            massKg,
            wheelRadiusMeters,
            lateralWheelOffsetFromCenterOfGravity,
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
   *     system can be created with {@link LinearSystemId#createDrivetrainVelocitySystem(DCMotor,
   *     double, double, double, double, double)} or {@link
   *     LinearSystemId#identifyDrivetrainSystem(double, double, double, double)}.
   * @param lateralDrivetrainPlant The {@link LinearSystem} representing the lateral movement system
   *     of the robot. This can be created with {@link
   *     edu.wpi.first.math.system.plant.HDrivetrainLateralSystemId#createHDrivetrainLateralVelocitySystem(
   *     DCMotor, double, double, double, double, double)} or {@link
   *     edu.wpi.first.math.system.plant.HDrivetrainLateralSystemId#identifyLateralVelocitySystem(
   *     double, double, double, double)}
   * @param driveMotor A {@link DCMotor} representing the drivetrain.
   * @param gearing The gearingRatio ratio of the robot, as output over input. This must be the same
   *     ratio as the ratio used to identify or create the drivetrainPlant.
   * @param trackWidthMeters The distance between the two sides of the drivetrian. Can be found with
   *     SysId.
   * @param wheelRadiusMeters The radius of the wheels on the drivetrain, in meters.
   * @param measurementStdDevs Standard deviations for measurements, in the form [x, y, heading,
   *     left velocity, right velocity, lateral velocity, left distance, right distance, lateral
   *     distance]ᵀ. Can be null if no noise is desired. Gyro standard deviations of 0.0001 radians,
   *     velocity standard deviations of 0.05 m/s, and position measurement standard deviations of
   *     0.005 meters are a reasonable starting point.
   */
  public HDrivetrainSim(
      LinearSystem<N2, N2, N2> drivetrainPlant,
      LinearSystem<N1, N1, N1> lateralDrivetrainPlant,
      DCMotor driveMotor,
      double gearing,
      double trackWidthMeters,
      double wheelRadiusMeters,
      Matrix<N9, N1> measurementStdDevs) {
    this.m_plant = drivetrainPlant;
    this.m_lateralPlant = lateralDrivetrainPlant;
    this.m_rb = trackWidthMeters / 2.0;
    this.m_motor = driveMotor;
    this.m_measurementStdDevs = measurementStdDevs;
    m_wheelRadiusMeters = wheelRadiusMeters;
    m_currentGearing = gearing;

    m_x = new Matrix<>(Nat.N9(), Nat.N1());
    m_u = VecBuilder.fill(0, 0, 0);
    m_y = new Matrix<>(Nat.N9(), Nat.N1());
  }

  /**
   * Performs forward kinematics to return the resulting Twist2d from the given left and right side
   * distance deltas. This method is often used for odometry -- determining the robot's position on
   * the field using changes in the distance driven by each wheel on the robot.
   *
   * @param leftDistanceMeters The distance measured by the left side encoder.
   * @param rightDistanceMeters The distance measured by the right side encoder.
   * @param trackWidthMeters The width of the wheelbase in meters.
   * @return The resulting Twist2d.
   */
  public Twist2d toTwist2d(
      double leftDistanceMeters, double rightDistanceMeters, double trackWidthMeters) {
    return new Twist2d(
        (leftDistanceMeters + rightDistanceMeters) / 2,
        0,
        (rightDistanceMeters - leftDistanceMeters) / trackWidthMeters);
  }

  /**
   * Sets the applied voltage to the drivetrain. Note that positive voltage must make that side of
   * the drivetrain travel forward (+X).
   *
   * @param leftVoltageVolts The left voltage.
   * @param rightVoltageVolts The right voltage.
   * @param lateralVoltageVolts The lateral voltage.
   */
  public void setInputs(
      double leftVoltageVolts, double rightVoltageVolts, double lateralVoltageVolts) {
    m_u = clampInput(VecBuilder.fill(leftVoltageVolts, rightVoltageVolts, lateralVoltageVolts));
  }

  /**
   * Update the drivetrain states with the current time difference.
   *
   * @param dtSeconds the time difference
   */
  public void update(double dtSeconds) {
    // Update state estimate with RK4
    m_x = NumericalIntegration.rk4(this::getDynamics, m_x, m_u, dtSeconds);
    m_y = m_x;
    if (m_measurementStdDevs != null) {
      m_y = m_y.plus(StateSpaceUtil.makeWhiteNoiseVector(m_measurementStdDevs));
    }
  }

  /** Returns the full simulated state of the drivetrain. */
  Matrix<N9, N1> getState() {
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
   * Get the lateral encoder position in meters.
   *
   * @return The encoder position.
   */
  public double getLateralPositionMeters() {
    return getOutput(State.kLateralPosition);
  }

  /**
   * Get the lateral encoder velocity in meters per second.
   *
   * @return The encoder velocity.
   */
  public double getLateralVelocityMetersPerSecond() {
    return getOutput(State.kLateralVelocity);
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
    return m_motor.getCurrent(
            getState(State.kLeftVelocity) * m_currentGearing / m_wheelRadiusMeters, m_u.get(0, 0))
        * Math.signum(m_u.get(0, 0));
  }

  /**
   * Get the current draw of the right side of the drivetrain.
   *
   * @return the drivetrain's right side current draw, in amps
   */
  public double getRightCurrentDrawAmps() {

    return m_motor.getCurrent(
            getState(State.kRightVelocity) * m_currentGearing / m_wheelRadiusMeters, m_u.get(1, 0))
        * Math.signum(m_u.get(1, 0));
  }

  /**
   * Get the current draw of the lateral plant of the drivetrain.
   *
   * @return the drivetrain's lateral plant current draw, in amps
   */
  public double getLateralCurrentDrawAmps() {
    return -m_motor.getCurrent(
            getState(State.kLeftVelocity) * m_currentGearing / m_wheelRadiusMeters, m_u.get(2, 0))
        * Math.signum(m_u.get(2, 0));
  }

  /**
   * Get the current draw of the drivetrain.
   *
   * @return the current draw, in amps
   */
  public double getCurrentDrawAmps() {
    return getLeftCurrentDrawAmps() + getRightCurrentDrawAmps() + getLateralCurrentDrawAmps();
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
  public void setState(Matrix<N9, N1> state) {
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

  protected Matrix<N9, N1> getDynamics(Matrix<N9, N1> x, Matrix<N3, N1> u) {
    // Because G can be factored out of B, we can divide by the old ratio and
    // multiply
    // by the new ratio to get a new drivetrain model.
    var B = new Matrix<>(Nat.N6(), Nat.N3());
    B.assignBlock(0, 0, m_plant.getB());
    B.assignBlock(2, 2, m_lateralPlant.getB());

    // Because G² can be factored out of A, we can divide by the old ratio squared
    // and multiply
    // by the new ratio squared to get a new drivetrain model.
    var A = new Matrix<>(Nat.N6(), Nat.N6());
    A.assignBlock(0, 0, m_plant.getA());

    A.assignBlock(2, 2, m_lateralPlant.getA());

    // Make identity matrx for bottom left 3x3
    A.assignBlock(3, 0, Matrix.eye(Nat.N3()));

    var v = (x.get(State.kLeftVelocity.value, 0) + x.get(State.kRightVelocity.value, 0)) / 2.0;

    var xdot = new Matrix<>(Nat.N9(), Nat.N1());
    xdot.set(
        0,
        0,
        v * Math.cos(x.get(State.kHeading.value, 0))
            + (x.get(State.kLateralVelocity.value, 0) * -Math.sin(x.get(State.kHeading.value, 0))));
    xdot.set(
        1,
        0,
        v * Math.sin(x.get(State.kHeading.value, 0))
            + (x.get(State.kLateralVelocity.value, 0) * Math.cos(x.get(State.kHeading.value, 0))));
    xdot.set(
        2,
        0,
        (x.get(State.kRightVelocity.value, 0) - x.get(State.kLeftVelocity.value, 0))
            / (2.0 * m_rb));
    xdot.assignBlock(3, 0, A.times(x.block(Nat.N6(), Nat.N1(), 3, 0)).plus(B.times(u)));

    return xdot;
  }

  /**
   * Clamp the input vector such that no element exceeds the battery voltage. If any does, the
   * relative magnitudes of the input will be maintained.
   *
   * @param u The input vector.
   * @return The normalized input.
   */
  protected Matrix<N3, N1> clampInput(Matrix<N3, N1> u) {
    return StateSpaceUtil.desaturateInputVector(u, RobotController.getBatteryVoltage());
  }

  /** Represents the different states of the drivetrain. */
  enum State {
    kX(0),
    kY(1),
    kHeading(2),
    kLeftVelocity(3),
    kRightVelocity(4),
    kLateralVelocity(5),
    kLeftPosition(6),
    kRightPosition(7),
    kLateralPosition(8);

    public final int value;

    State(int i) {
      this.value = i;
    }
  }

  /**
   * Returns the position of the robot on the field.
   *
   * @return The pose of the robot (x and y are in meters).
   */
  public Pose2d getPoseMeters() {
    return getPose();
  }
}
