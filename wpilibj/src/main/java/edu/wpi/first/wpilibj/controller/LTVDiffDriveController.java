//package edu.wpi.first.wpilibj.controller;
//
//import edu.wpi.first.wpilibj.estimator.ExtendedKalmanFilter;
//import edu.wpi.first.wpilibj.geometry.Pose2d;
//import edu.wpi.first.wpilibj.kinematics.ChassisSpeeds;
//import edu.wpi.first.wpilibj.kinematics.DifferentialDriveKinematics;
//import edu.wpi.first.wpilibj.math.StateSpaceUtils;
//import edu.wpi.first.wpilibj.system.LinearSystem;
//import edu.wpi.first.wpilibj.system.NumericalJacobian;
//import edu.wpi.first.wpilibj.trajectory.Trajectory;
//import edu.wpi.first.wpiutil.math.MatBuilder;
//import edu.wpi.first.wpiutil.math.Matrix;
//import edu.wpi.first.wpiutil.math.MatrixUtils;
//import edu.wpi.first.wpiutil.math.Nat;
//import edu.wpi.first.wpiutil.math.numbers.*;
//import org.ejml.simple.SimpleMatrix;
//
///**
// * A Linear Time-Varying Cascaded Unicycle Controller for differential drive
// * robots. Similar to RAMSETE, this controller combines feedback and feedforward
// * to output ChassisSpeeds to guide a robot along a trajectory. However, this
// * controller utilizes tolerances grounded in reality to pick gains rather than
// * magical Beta and Zeta gains.
// */
//public class LTVDiffDriveController {
//
//    private final double kPositionToleranceMeters;
//    private final double kVelocityToleranceMps;
//    private final double kAngleToleranceRad;
//    private final double rb;
//    private final LinearSystem m_plant;
//    private final double m_dtSeconds;
//
//    private Matrix<N2, N1> m_appliedU;
//    private Matrix<N3, N1> m_localY;
//    private Matrix<N6, N1> m_globalY;
//
//    private Matrix<N10, N1> m_r;
//    private Matrix<N10, N1> m_nextR;
//    private Matrix<N2, N1> m_cappedU;
//
//    private Matrix<N5, N2> m_B;
//    private Matrix<N2, N5> m_K0;
//    private Matrix<N2, N5> m_K1;
//
//    private ExtendedKalmanFilter<N10, N2, N3> m_observer;
//    private DifferentialDriveKinematics m_kinematics;
//
//    /**
//     * Construct a LTV Unicycle Controller.
//     *
//     * @param qElms     The maximum desired error tolerance for the robot's state, in
//     *                  the form [X, Y, Heading]^T. Units are meters and radians.
//     * @param rElms     The maximum desired control effort by the feedback controller,
//     *                  in the form [vMax, wMax]^T. Units are meters per second and
//     *                  radians per second. Note that this is not the maximum speed of
//     *                  the robot, but rather the maximum effort the feedback controller
//     *                  should apply on top of the trajectory feedforward.
//     * @param dtSeconds The nominal dt of this controller. With command based this is 0.020.
//     */
//    public LTVDiffDriveController(LinearSystem plant, Matrix<N5, N1> qElms, Matrix<N2, N1> rElms, double dtSeconds) {
//        this.m_plant = plant;
//
//        m_localY = MatrixUtils.zeros(Nat.N3(), Nat.N1());
//        m_globalY = MatrixUtils.zeros(Nat.N6(), Nat.N1());
//
//        var x0 = MatrixUtils.zeros(Nat.N10());
//        x0.set(State.kLeftVelocity.value, 0, 1e-9);
//        x0.set(State.kRightVelocity.value, 0, 1e-9);
//
//        var x1 = MatrixUtils.zeros(Nat.N10());
//        x1.set(State.kLeftVelocity.value, 0, 1);
//        x1.set(State.kRightVelocity.value, 0, 1);
//
//        var u0 = MatrixUtils.zeros(Nat.N2());
//
//        var a0 = NumericalJacobian.numericalJacobianX(
//                Nat.N10(), Nat.N10(),
//                this::getDynamics, x0, u0)
//                .getStorage().extractMatrix(0, 0, 5, 5);
//        var a1 = NumericalJacobian.numericalJacobianX(
//                Nat.N10(), Nat.N10(),
//                this::getDynamics, x1, u0)
//                .getStorage().extractMatrix(0, 0, 5, 5);
//        m_B = new Matrix<N5, N2>(
//                NumericalJacobian.numericalJacobianX(
//                        Nat.N10(), Nat.N10(),
//                        this::getDynamics, x0, u0
//                ).getStorage().extractMatrix(0, 0, 5, 2));
//
//        m_K0 = new LinearQuadraticRegulator<N5, N2, N3>(Nat.N5(), Nat.N2(),
//                new Matrix<>(a0), m_B, qElms, rElms, dtSeconds).getK();
//        m_K1 = new LinearQuadraticRegulator<N5, N2, N3>(Nat.N5(), Nat.N2(),
//                new Matrix<>(a1), m_B, qElms, rElms, dtSeconds).getK();
//    }
//
//    private Matrix<N10, N1> getDynamics(Matrix<N10, N1> x, Matrix<N2, N1> u) {
//        var B = m_plant.getB().getStorage()
//                .concatRows(new SimpleMatrix(2, 2));
//
//        var A = new Matrix<N4, N7>(new SimpleMatrix(4, 7));
//        A.getStorage().insertIntoThis(0, 0, m_plant.getA().getStorage());
//        A.getStorage().insertIntoThis(2, 0, SimpleMatrix.identity(2));
//        A.getStorage().insertIntoThis(0, 4, B);
//        A.getStorage().setColumn(6, 0, 0, 0, 1, -1);
//
//        var v = (x.get(State.kLeftVelocity.value, 0) + x.get(State.kRightVelocity.value, 0)) / 2.0;
//
//        var result = new Matrix<N10, N1>(new SimpleMatrix(10, 1));
//        result.set(0, 0, v * Math.cos(x.get(State.kHeading.value, 0)));
//        result.set(1, 0, v * Math.sin(x.get(State.kHeading.value, 0)));
//        result.set(2, 0,
//                ((x.get(State.kRightVelocity.value, 0) - x.get(State.kLeftVelocity.value, 0)) / (2.0 * rb)));
//        result.getStorage().insertIntoThis(3, 0,
//                A.getStorage().mult(x.getStorage().extractMatrix(3, 3 + 7, 0, 1))
//                        .plus(B.mult(u.getStorage()))
//        );
//        return result;
//    }
//
//    private Matrix<N3, N1> getLocalMeasurementModel(
//            Matrix<N10, N1> x,
//            Matrix<N2, N1> u) {
//        return new MatBuilder<>(Nat.N3(), Nat.N1()).fill(
//                x.get(State.kHeading.value, 0),
//                x.get(State.kLeftPosition.value, 0),
//                x.get(State.kRightPosition.value, 0)
//        );
//    }
//
//    /**
//     * Returns whether the drivetrain controller is at the goal waypoint.
//     */
//    public boolean atGoal() {
//
//    }
//
//    /**
//     * Set inputs.
//     *
//     * @param leftUVolts  Voltage applied to the left drivetrain
//     * @param rightUVolts Voltage applied to the right drivetrain
//     */
//    public void setMeasuredInputs(double leftUVolts, double rightUVolts) {
//
//    }
//
//    /**
//     * Set local measurements.
//     *
//     * @param headingRadians Angle of the robot in radians.
//     * @param leftPosition   Encoder count of left side in meters.
//     * @param rightPosition  Encoder count of right side in meters.
//     */
//    public void setMeasuredLocalOutputs(double headingRadians,
//                                        double leftPosition,
//                                        double rightPosition) {
//
//    }
//
//    /**
//     * Set global measurements.
//     *
//     * @param xMeters                  X position of the robot in meters.
//     * @param yMeters                  Y position of the robot in meters.
//     * @param headingRadians           Angle of the robot.
//     * @param leftPositionMeters       Encoder count of left side in meters.
//     * @param rightPositionMeters      Encoder count of right side in meters.
//     * @param angularVelocityRadPerSec Angular velocity of the robot in radians per
//     *                                 second.
//     */
//    public void setMeasuredGlobalOutputs(double xMeters, double yMeters,
//                                         double headingRadians,
//                                         double leftPositionMeters,
//                                         double rightPositionMeters,
//                                         double angularVelocityRadPerSec) {
//
//    }
//
//    public Matrix<N10, N1> getReference() {
//
//    }
//
//    public Matrix<N10, N1> getStates() {
//
//    }
//
//    public Matrix<N2, N1> getInputs() {
//
//    }
//
//    public Matrix<N3, N1> getOutputs() {
//
//    }
//
//    /**
//     * Returns the estimated outputs based on the current state estimate.
//     * <p>
//     * This provides only local measurements.
//     */
//    public Matrix<N3, N1> getEstimatedLocalOutputs() {
//
//    }
//
//    /**
//     * Returns the estimated outputs based on the current state estimate.
//     * <p>
//     * This provides global measurements (including pose).
//     */
//    public Matrix<N6, N1> getEstimatedGlobalOutputs() {
//
//    }
//
//    public Matrix<N2, N1> getController(SimpleMatrix x, SimpleMatrix r) {
//        // This implements the linear time-varying differential drive controller in
//        // theorem 8.6.2 of https://tavsys.net/controls-in-frc.
//        double kx = m_K0.get(0, 0);
//        double ky0 = m_K0.get(0, 1);
//        double kvPos0 = m_K0.get(1, 3);
//        double kvneg0 = m_K0.get(1, 3);
//        double ky1 = m_K1.get(0, 1);
//        double ktheta1 = m_K1.get(0, 2);
//        double kvpos1 = m_K1.get(0, 3);
//
//        double v = (x.get(State.kLeftVelocity.value) + x.get(State.kRightVelocity.value)) / 2.0;
//
//
//    }
//
//    /**
//     * Returns the next output of the controller.
//     * <p>
//     * The reference pose, linear velocity, and angular velocity should come
//     * from a {@link Trajectory}.
//     *
//     * @param currentPose                   The current position of the robot.
//     * @param poseRef                       The desired pose of the robot.
//     * @param linearVelocityRefMetersPerSec The desired linear velocity of the robot.
//     * @param angularVelocityRefRadPerSec   The desired angular velocity of the robot.
//     * @return The next calculated output.
//     */
//    public ChassisSpeeds calculate(Pose2d currentPose, Pose2d poseRef,
//                                   double linearVelocityRefMetersPerSec,
//                                   double angularVelocityRefRadPerSec) {
//
//        var wheelVelocities = m_kinematics.toWheelSpeeds(new ChassisSpeeds(linearVelocityRefMetersPerSec, 0, angularVelocityRefRadPerSec));
//
//        m_observer.correct(m_appliedU, m_localY);
//
//        m_nextR = new MatBuilder<>(Nat.N10(), Nat.N1()).fill(
//                poseRef.getTranslation().getX(),
//                poseRef.getTranslation().getY(),
//                poseRef.getRotation().getRadians(),
//                wheelVelocities.leftMetersPerSecond,
//                wheelVelocities.rightMetersPerSecond,
//                0.0, 0.0, 0.0, 0.0, 0.0 // 0s for now
//        );
//
//        // Compute feedforward
//        var rdot = (m_nextR.getStorage().extractMatrix(0, 0, 5, 1)
//                .minus(m_r.getStorage().extractMatrix(0, 0, 5, 1)))
//                .divide(m_dtSeconds);
//        var uff = StateSpaceUtils.householderQrDecompose(m_B.getStorage()).solve(
//                rdot.minus(
//                        getDynamics(m_r, new Matrix<>(new SimpleMatrix(2, 1)))
//                                .getStorage()
//                                .extractMatrix(0, 0, 5, 1)));
//
//        m_cappedU = getController(m_observer.getXhat().getStorage(), m_nextR.getStorage().extractMatrix(0, 0, 5, 1).plus(uff));
//
//    }
//
//    /**
//     * Returns the next output of the controller.
//     *
//     * <p>The reference pose and desired state should come from a {@link Trajectory}.
//     *
//     * @param currentPose  The current pose.
//     * @param desiredState The desired pose, linear velocity, and angular velocity
//     *                     from a trajectory.
//     */
//    public ChassisSpeeds calculate(Pose2d currentPose, Trajectory.State desiredState) {
//        return calculate(currentPose, desiredState.poseMeters,
//                desiredState.velocityMetersPerSecond,
//                desiredState.velocityMetersPerSecond * desiredState.curvatureRadPerMeter);
//    }
//
//    /**
//     * Resets any internal state.
//     */
//    public void reset() {
//
//    }
//
//    /**
//     * Resets any internal state.
//     *
//     * @param initialPose Initial pose for state estimate.
//     */
//    public void reset(Pose2d initialPose) {
//
//    }
//
//    private enum State {
//        kX(0),
//        kY(1),
//        kHeading(2),
//        kLeftVelocity(3),
//        kRightVelocity(4),
//        kLeftPosition(5),
//        kRightPosition(6),
//        kLeftVoltageError(7),
//        kRightVoltageError(8),
//        kAngularVelocityError(9);
//
//        private int value;
//
//        State(int i) {
//            this.value = i;
//        }
//    }
//
//    private enum Input {
//        kLeftVoltage(0), kRightVoltage(1);
//
//        private int value;
//
//        Input(int i) {
//            this.value = i;
//        }
//    }
//
//    private enum LocalOutput {
//        kHeading(0), kLeftPosition(1), kRightPosition(1);
//
//        private int value;
//
//        LocalOutput(int i) {
//            this.value = i;
//        }
//    }
//
//    private enum GlobalOutput {
//        kX(0),
//        kY(1),
//        kHeading(2),
//        kLeftPosition(3),
//        kRightPosition(4),
//        kAngularVelocity(5);
//
//        private int value;
//
//        GlobalOutput(int i) {
//            this.value = i;
//        }
//    }
//
//}
