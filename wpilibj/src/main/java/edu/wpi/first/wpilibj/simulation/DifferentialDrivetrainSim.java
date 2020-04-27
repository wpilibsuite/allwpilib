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
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.VecBuilder;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N10;
import edu.wpi.first.wpiutil.math.numbers.N2;
import edu.wpi.first.wpiutil.math.numbers.N4;
import edu.wpi.first.wpiutil.math.numbers.N7;
import org.ejml.simple.SimpleMatrix;

/**
 * This class simulates the state of the drivetrain. In simulationPeriodic, users should first set inputs from motors with
 * {@link #setInputs(double, double)}, call {@link #update(double)} to update the simulation,
 * and set estimated encoder and gyro positions, as well as estimated odometry pose, . Teams can use {@link edu.wpi.first.wpilibj.simulation.Field2d} to
 * visualize their robot on the Sim GUI's field.
 */
public class DifferentialDrivetrainSim {
  private final DCMotor m_leftMotor;
  private final DCMotor m_rightMotor;
  private final double m_leftGearing;
  private final double m_rightGearing;
  @SuppressWarnings("MemberName")
  private Matrix<N2, N1> m_u;
  @SuppressWarnings("MemberName")
  private Matrix<N10, N1> m_x;

  private final double m_rb;
  private final LinearSystem<N2, N2, N2> m_plant;

  /**
   * Create a SimDrivetrain.
   *
   * @param drivetrainPlant The {@link LinearSystem} representing the robot's drivetrain. This
   *                        system can be created with {@link edu.wpi.first.wpilibj.system.plant.LinearSystemId#createDrivetrainVelocitySystem(DCMotor, double, double, double, double, double)}
   *                        or {@link edu.wpi.first.wpilibj.system.plant.LinearSystemId#identifyDrivetrainSystem(double, double, double, double)}.
   * @param kinematics      A {@link DifferentialDriveKinematics} object representing the
   *                        differential drivetrain's kinematics.
   */
  public DifferentialDrivetrainSim(LinearSystem<N2, N2, N2> drivetrainPlant,
                                   DifferentialDriveKinematics kinematics,
                                   DCMotor leftGearbox, double leftGearing,
                                   DCMotor rightGearbox, double rightGearing) {
    this.m_plant = drivetrainPlant;
    this.m_rb = kinematics.trackWidthMeters / 2.0;
    this.m_leftMotor = leftGearbox;
    this.m_rightMotor = rightGearbox;
    this.m_leftGearing = leftGearing;
    this.m_rightGearing = rightGearing;

    m_x = new Matrix<>(new SimpleMatrix(10, 1));
  }

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
    var loadIleft = m_leftMotor.getCurrent(
        getState(State.kLeftVelocity) * m_leftGearing / m_rb,
        m_u.get(0, 0)) * Math.signum(getState(State.kLeftVelocity));

    var loadIright = m_rightMotor.getCurrent(
        getState(State.kRightVelocity) * m_rightGearing / m_rb,
        m_u.get(1, 0)) * Math.signum(getState(State.kRightVelocity));

    return loadIleft + loadIright;
  }

  @SuppressWarnings({"DuplicatedCode", "LocalVariableName"})
  protected Matrix<N10, N1> getDynamics(Matrix<N10, N1> x, Matrix<N2, N1> u) {
    Matrix<N4, N2> B = new Matrix<>(new SimpleMatrix(4, 2));
    B.getStorage().insertIntoThis(0, 0, m_plant.getB().getStorage());
    B.getStorage().insertIntoThis(2, 0, new SimpleMatrix(2, 2));

    Matrix<N4, N7> A = new Matrix<>(new SimpleMatrix(4, 7));
    A.getStorage().insertIntoThis(0, 0, m_plant.getA().getStorage());

    A.getStorage().insertIntoThis(2, 0, SimpleMatrix.identity(2));
    A.getStorage().insertIntoThis(0, 2, new SimpleMatrix(4, 2));
    A.getStorage().insertIntoThis(0, 4, B.getStorage());
    A.getStorage().setColumn(6, 0, 0, 0, 1, -1);

    var v = (x.get(State.kLeftVelocity.value, 0) + x.get(State.kRightVelocity.value, 0)) / 2.0;

    var result = new Matrix<N10, N1>(new SimpleMatrix(10, 1));
    result.set(0, 0, v * Math.cos(x.get(State.kHeading.value, 0)));
    result.set(1, 0, v * Math.sin(x.get(State.kHeading.value, 0)));
    result.set(2, 0, (x.get(State.kRightVelocity.value, 0)
        - x.get(State.kLeftVelocity.value, 0)) / (2.0 * m_rb));

    result.getStorage().insertIntoThis(3, 0, A.times(new Matrix<N7, N1>(
        x.getStorage().extractMatrix(3, 10, 0, 1))).plus(B.times(u)).getStorage());
    result.getStorage().insertIntoThis(7, 0, new SimpleMatrix(3, 1));
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
    kAngularVelocityError(9);

    @SuppressWarnings("MemberName")
    public final int value;

    State(int i) {
      this.value = i;
    }
  }
}
