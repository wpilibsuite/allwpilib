/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.kinematics.DifferentialDriveKinematics;
import edu.wpi.first.wpilibj.system.LinearSystem;
import edu.wpi.first.wpilibj.system.RungeKutta;
import edu.wpi.first.wpilibj.system.plant.DCMotor;
import edu.wpi.first.wpilibj.system.plant.LinearSystemId;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.VecBuilder;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N10;
import edu.wpi.first.wpiutil.math.numbers.N2;
import org.ejml.simple.SimpleMatrix;

/**
 * This class simulates the state of the drivetrain. In simulationPeriodic, users should first set inputs from motors with
 * {@link #setInputs(double, double)}, call {@link #update(double)} to update the simulation,
 * and set estimated encoder and gyro positions, as well as estimated odometry pose. Teams can use {@link edu.wpi.first.wpilibj.simulation.Field2d} to
 * visualize their robot on the Sim GUI's field.
 *
 *  <p>Our state-space system is:
 *
 *  <p>x = [[x, y, theta, vel_l, vel_r, dist_l, dist_r, voltError_l, voltError_r, headingError]]^T
 *  in the field coordinate system (dist_* are wheel distances.)
 *
 *  <p>u = [[voltage_l, voltage_r]]^T This is typically the control input of the last timestep
 *  from a LTVDiffDriveController.
 *
 *  <p>y = [[x, y, theta]]^T from a global measurement source(ex. vision),
 *  or y = [[dist_l, dist_r, theta]] from encoders and gyro.
 *
 */
public class DifferentialDrivetrainSim {
  private final DCMotor m_motor;
  private final double m_gearing;
  private double m_wheelRadiusMeters;
  @SuppressWarnings("MemberName")
  private Matrix<N2, N1> m_u;
  @SuppressWarnings("MemberName")
  private Matrix<N10, N1> m_x;

  private final double m_rb;
  private final LinearSystem<N2, N2, N2> m_plant;

  /**
   * Create a SimDrivetrain.
   *
   * @param drivetrainPlant   The {@link LinearSystem} representing the robot's drivetrain. This
   *                          system can be created with {@link edu.wpi.first.wpilibj.system.plant.LinearSystemId#createDrivetrainVelocitySystem(DCMotor, double, double, double, double, double)}
   *                          or {@link edu.wpi.first.wpilibj.system.plant.LinearSystemId#identifyDrivetrainSystem(double, double, double, double)}.
   * @param kinematics        A {@link DifferentialDriveKinematics} object representing the
   *                          differential drivetrain's kinematics.
   * @param driveMotor        A {@link DCMotor} representing the left side of the drivetrain.
   * @param gearingRatio      The gearingRatio ratio of the left side, as output over input.
   * @param wheelRadiusMeters The radius of the wheels on the drivetrain, in meters.
   */
  public DifferentialDrivetrainSim(LinearSystem<N2, N2, N2> drivetrainPlant,
                                   DifferentialDriveKinematics kinematics,
                                   DCMotor driveMotor, double gearingRatio,
                                   double wheelRadiusMeters) {
    this.m_plant = drivetrainPlant;
    this.m_rb = kinematics.trackWidthMeters / 2.0;
    this.m_motor = driveMotor;
    this.m_gearing = gearingRatio;
    m_wheelRadiusMeters = wheelRadiusMeters;

    m_x = new Matrix<>(new SimpleMatrix(10, 1));
  }

  /**
   * Create a SimDrivetrain.
   *
   * @param driveMotor        A {@link DCMotor} representing the left side of the drivetrain.
   * @param massKg            The mass of the drivebase.
   * @param wheelRadiusMeters The radius of the wheels on the drivetrain.
   * @param jKgMetersSquared  The moment of inertia of the drivetrain about its center.
   * @param gearing           The gearing on the drive between motor and wheel, as output over input.
   * @param trackWidthMeters  The robot's track width, or distance between left and right wheels.
   */
  public DifferentialDrivetrainSim(DCMotor driveMotor, double massKg,
                                   double wheelRadiusMeters, double jKgMetersSquared, double gearing,
                                   double trackWidthMeters) {
    this(LinearSystemId.createDrivetrainVelocitySystem(driveMotor, massKg, wheelRadiusMeters,
        trackWidthMeters / 2, jKgMetersSquared, gearing),
        new DifferentialDriveKinematics(trackWidthMeters),
        driveMotor, gearing, wheelRadiusMeters);
  }

  /**
   * Set the applied voltage to the drivetrain. Note that positive voltage must make that
   * side of the drivetrain travel forward (+X).
   *
   * @param leftVoltageVolts  The left voltage.
   * @param rightVoltageVolts The right voltage.
   */
  public void setInputs(double leftVoltageVolts, double rightVoltageVolts) {
    m_u = VecBuilder.fill(leftVoltageVolts, rightVoltageVolts);
  }

  @SuppressWarnings("LocalVariableName")
  public void update(double dtSeconds) {

    // Update state estimate with RK4
    m_x = RungeKutta.rungeKutta(this::getDynamics, m_x, m_u, dtSeconds);
  }

  public double getState(State state) {
    return m_x.get(state.value, 0);
  }

  public Matrix<N10, N1> getState() {
    return m_x;
  }

  /**
   * Get the estimated direction the robot is pointing. Note that this angle is
   * counterclockwise-positive, while most gyros are clockwise positive.
   */
  public Rotation2d getHeading() {
    return new Rotation2d(getState(State.kHeading));
  }

  public Pose2d getEstimatedPosition() {
    return new Pose2d(m_x.get(0, 0),
      m_x.get(1, 0),
      new Rotation2d(m_x.get(2, 0)));
  }

  public double getCurrentDrawAmps() {
    var loadIleft = m_motor.getCurrent(
        getState(State.kLeftVelocity) * m_gearing / m_wheelRadiusMeters,
        m_u.get(0, 0)) * Math.signum(m_u.get(0, 0));

    var loadIright = m_motor.getCurrent(
        getState(State.kRightVelocity) * m_gearing / m_wheelRadiusMeters,
        m_u.get(1, 0)) * Math.signum(m_u.get(1, 0));

    return loadIleft + loadIright;
  }

  @SuppressWarnings({"DuplicatedCode", "LocalVariableName"})
  protected Matrix<N10, N1> getDynamics(Matrix<N10, N1> x, Matrix<N2, N1> u) {
    var B = new Matrix<>(Nat.N4(), Nat.N2());
    B.assignBlock(0, 0, m_plant.getB());

    var A = new Matrix<>(Nat.N4(), Nat.N7());
    A.assignBlock(0, 0, m_plant.getA());

    A.assignBlock(2, 0, Matrix.eye(Nat.N2()));
    A.assignBlock(0, 4, B);
    A.setColumn(6, VecBuilder.fill(0, 0, 1, -1));

    var v = (x.get(State.kLeftVelocity.value, 0) + x.get(State.kRightVelocity.value, 0)) / 2.0;

    var result = new Matrix<>(Nat.N10(), Nat.N1());
    result.set(0, 0, v * Math.cos(x.get(State.kHeading.value, 0)));
    result.set(1, 0, v * Math.sin(x.get(State.kHeading.value, 0)));
    result.set(2, 0, (x.get(State.kRightVelocity.value, 0)
        - x.get(State.kLeftVelocity.value, 0)) / (2.0 * m_rb));

    result.assignBlock(3, 0,
        A.times(x.block(Nat.N7(), Nat.N1(), 0, 0))
        .plus(B.times(u)));

    return result;
  }

  public enum State {
    kX(0),
    kY(1),
    kHeading(2),
    kLeftVelocity(3),
    kRightVelocity(4),
    kLeftPosition(5),
    kRightPosition(6),
    kLeftVoltageError(7),
    kRightVoltageError(8),
    kHeadingError(9);

    @SuppressWarnings("MemberName")
    public final int value;

    State(int i) {
      this.value = i;
    }
  }
}
