package edu.wpi.first.wpilibj.system;

import edu.wpi.first.wpilibj.system.plant.DCMotor;
import edu.wpi.first.wpiutil.math.*;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N2;
import org.ejml.dense.row.CommonOps_DDRM;
import org.ejml.simple.SimpleMatrix;

import java.util.Objects;

public class LinearSystem<States extends Num, Inputs extends Num,
        Outputs extends Num> {

    /**
     * Continuous system matrix.
     */
    private final Matrix<States, States> m_A;

    /**
     * Continuous input matrix.
     */
    private final Matrix<States, Inputs> m_B;

    /**
     * Output matrix.
     */
    private final Matrix<Outputs, States> m_C;

    /**
     * Feedthrough matrix.
     */
    private final Matrix<Outputs, Inputs> m_D;

    /**
     * Minimum allowable input vector.
     */
    private final Matrix<Inputs, N1> m_uMin;

    /**
     * Maximum allowable input vector.
     */
    private final Matrix<Inputs, N1> m_uMax;

    /**
     * State vector.
     */
    private Matrix<States, N1> m_x;

    /**
     * Output vector.
     */
    private Matrix<Outputs, N1> m_y;

    /**
     * Delayed u since predict and correct steps are run in reverse.
     */
    private Matrix<Inputs, N1> m_delayedU;

    /**
     * The states of the system represented as a Nat.
     */
    private final Nat<States> states;

    /**
     * The inputs of the system represented as a Nat.
     */
    private final Nat<Inputs> inputs;

    /**
     * The outputs of the system represented as a Nat.
     */
    private final Nat<Outputs> outputs;

    public LinearSystem(Nat<States> states, Nat<Inputs> inputs, Nat<Outputs> outputs,
                        Matrix<States, States> A, Matrix<States, Inputs> B,
                        Matrix<Outputs, States> C, Matrix<Outputs, Inputs> D,
                        Matrix<Inputs, N1> uMin, Matrix<Inputs, N1> uMax) {

        this.states = states;
        this.inputs = inputs;
        this.outputs = outputs;

        this.m_A = A;
        this.m_B = B;
        this.m_C = C;
        this.m_D = D;
        this.m_uMin = uMin;
        this.m_uMax = uMax;

        this.m_x = MatrixUtils.zeros(states);
        this.m_y = MatrixUtils.zeros(outputs);
        this.m_delayedU = MatrixUtils.zeros(inputs);

        reset();
    }

    /**
     * Resets the plant.
     */
    public void reset() {
        m_x = MatrixUtils.zeros(states);
        m_y  = MatrixUtils.zeros(outputs);
        m_delayedU = MatrixUtils.zeros(inputs);
    }

    /**
     * Returns the system matrix A.
     */
    public Matrix<States, States> getA() {
        return m_A;
    }

    /**
     * Returns an element of the system matrix A.
     *
     * @param i Row of A.
     * @param j Column of A.
     */
    public double getA(int i, int j) {
        return m_A.get(i, j);
    }

    /**
     * Returns the input matrix B.
     */
    public Matrix<States, Inputs> getB() {
        return m_B;
    }

    /**
     * Returns an element of the input matrix B.
     *
     * @param i Row of B.
     * @param j Column of B.
     */
    public double getB(int i, int j) {
        return m_B.get(i, j);
    }

    /**
     * Returns the output matrix C.
     */
    public Matrix<Outputs, States> getC() {
        return m_C;
    }

    /**
     * Returns an element of the output matrix C.
     *
     * @param i Row of C.
     * @param j Column of C.
     */
    public double getC(int i, int j) {
        return m_C.get(i, j);
    }

    /**
     * Returns the feedthrough matrix D.
     */
    public Matrix<Outputs, Inputs> getD() {
        return m_D;
    }

    /**
     * Returns an element of the feedthrough matrix D.
     *
     * @param i Row of D.
     * @param j Column of D.
     */
    public double getD(int i, int j) {
        return m_D.get(i, j);
    }

    /**
     * Returns the minimum control input vector u.
     */
    public Matrix<Inputs, N1> getUMin() {
        return m_uMin;
    }

    /**
     * Returns an element of the minimum control input vector u.
     *
     * @param i Row of u.
     */
    public double getUMin(int i) {
        return m_uMin.get(i, 1);
    }

    /**
     * Returns the maximum control input vector u.
     */
    public Matrix<Inputs, N1> getUMax() {
        return m_uMax;
    }

    /**
     * Returns an element of the maximum control input vector u.
     *
     * @param i Row of u.
     */
    public double getUMax(int i) {
        return m_uMax.get(i, 1);
    }

    /**
     * Returns the current state x.
     */
    public Matrix<States, N1> getX() {
        return m_x;
    }

    /**
     * Returns an element of the current state x.
     *
     * @param i Row of x.
     */
    public double getX(int i) {
        return m_x.get(i, 0);
    }

    /**
     * Returns the current measurement vector y.
     */
    public Matrix<Outputs, N1> getY() {
        return m_y;
    }

    /**
     * Returns an element of the current measurement vector y.
     *
     * @param i Row of y.
     */
    public double getY(int i) {
        return m_y.get(i, 0);
    }

    /**
     * Returns the control input vector u.
     */
    public Matrix<Inputs, N1> getU() {
        return m_delayedU;
    }

    /**
     * Returns an element of the control input vector u.
     *
     * @param i Row of u.
     */
    public double getU(int i) {
        return m_delayedU.get(i, 0);
    }

    /**
     * Set the initial state x.
     *
     * @param x The initial state.
     */
    public void setX(Matrix<States, N1> x) {
        m_x = x;
    }

    /**
     * Set an element of the initial state x.
     *
     * @param i     Row of x.
     * @param value Value of element of x.
     */
    public void setX(int i, double value) { m_x.set(i, 0, value); }

    /**
     * Set the current measurement y.
     *
     * @param y The current measurement.
     */
    public void setY(Matrix<Outputs, N1> y) { m_y = y; }

    /**
     * Set an element of the current measurement y.
     *
     * @param i     Row of y.
     * @param value Value of element of y.
     */
    public void setY(int i, double value) { m_y.set(i, 0, value); }

    /**
     * Computes the new x and y given the control input.
     *
     * @param x The current state.
     * @param u The control input.
     * @param dtSeconds Timestep for model update.
     */
    public void update(Matrix<States, N1> x, Matrix<Inputs, N1> u, double dtSeconds) {
        m_x = calculateX(x, m_delayedU, dtSeconds);
        m_y = calculateY(m_x, m_delayedU);
        m_delayedU = u;
    }

    /**
     * Computes the new x given the old x and the control input.
     *
     * This is used by state observers directly to run updates based on state
     * estimate.
     *
     * @param x  The current state.
     * @param u  The control input.
     * @param dtSeconds Timestep for model update.
     */
    public Matrix<States, N1> calculateX(Matrix<States, N1> x, Matrix<Inputs, N1> u, double dtSeconds) {
        Matrix<States, States> discA = new Matrix<>(new SimpleMatrix(states.getNum(), states.getNum()));
        Matrix<States, Inputs> discB = new Matrix<>(new SimpleMatrix(states.getNum(), inputs.getNum()));

        discretizeAB(m_A, m_B, dtSeconds, discA, discB);

        return (discA.times(x)).plus(discB.times(clampInput(u)));
    }

    private void discretizeAB(Matrix<States, States> contA,
                  Matrix<States, Inputs> contB,
                      double dtSeconds,
                      Matrix<States, States> discA,
                      Matrix<States, Inputs> discB) {

        SimpleMatrix Mcont = new SimpleMatrix(0, 0);
        var scaledA = contA.times(dtSeconds);
        var scaledB = contB.times(dtSeconds);
        Mcont = Mcont.concatColumns(scaledA.getStorage());
        Mcont = Mcont.concatColumns(scaledB.getStorage());
        // so our Mcont is now states x (states + inputs)
        // and we want (states + inputs) x (states + inputs)
        // so we want to add (inputs) many rows onto the bottom
        Mcont = Mcont.concatRows(new SimpleMatrix(inputs.getNum(), states.getNum() + inputs.getNum()));

//        System.out.println(Mcont);

        // Discretize A and B with the given timestep
        var Mdisc = SimpleMatrixUtils.scipyExpm(Mcont);

//        System.out.printf("Mdisc: \n%s", Mdisc);

//        discA.getStorage().set(Mdisc.extractMatrix(0, 0, states.getNum(), states.getNum()));
//        discB.getStorage().set(Mdisc.extractMatrix(0, states.getNum(), states.getNum(), inputs.getNum()));
        CommonOps_DDRM.extract(Mdisc.getDDRM(), 0, 0, discA.getStorage().getDDRM());
        CommonOps_DDRM.extract(Mdisc.getDDRM(), 0, states.getNum(), discB.getStorage().getDDRM());
    }

    /**
     * Computes the new y given the control input.
     *
     * This is used by state observers directly to run updates based on state
     * estimate.
     *
     * @param x The current state.
     * @param u The control input.
     */
    public Matrix<Outputs, N1> calculateY(
            Matrix<States, N1> x,
            Matrix<Inputs, N1> u) {
        return m_C.times(x).plus(m_D.times(clampInput(u)));
    }

    public Matrix<Inputs, N1> clampInput(Matrix<Inputs, N1> u) {
        var result = new Matrix<Inputs, N1>(new SimpleMatrix(inputs.getNum(), 1));
        for (int i = 0; i < inputs.getNum(); i++) {
            result.set(i, 0,
                    MathUtil.clamp(
                            u.get(i, 0),
                            m_uMin.get(i, 0),
                            m_uMax.get(i, 0)
                    ));
        }
        return result;
    }

    public static LinearSystem<N2, N1, N1> createElevatorSystem(DCMotor motor, double massKg, double radiusMeters, double G, double maxVoltage) {
        var A_ = new SimpleMatrix(2, 2);
        var B_ = new SimpleMatrix(2, 1);
        var C_ = new SimpleMatrix(1, 2);
        var D_ = new SimpleMatrix(1, 1);
        A_.setRow(0, 0, 0, 1);
        A_.setRow(1, 0, 0,

                -Math.pow(G, 2) * motor.KtNMPerAmp /
                        (motor.Rohms * radiusMeters * radiusMeters * massKg * motor.KvRadPerSecPerVolt)

        );

//        System.out.printf("elevator A is %s", A_);

        var A = new Matrix<N2, N2>(A_);

        B_.setColumn(0, 0,
                0, (G * motor.KtNMPerAmp / (motor.Rohms * radiusMeters * massKg)));
        var B = new Matrix<N2, N1>(B_);

        C_.setRow(0, 0, 1, 0);
        var C = new Matrix<N1, N2>(C_);

        D_.set(0, 0, 0);
        var D = new Matrix<N1, N1>(D_);

        var uMax = new Matrix<N1, N1>(new SimpleMatrix(1, 1));
        uMax.set(0, 0, maxVoltage);
        var uMin = uMax.times(-1);

//        System.out.printf("Returning system with matricies: A: \n%s\nB:\n%s\nC:\n%s\nD:\n%s\n", A, B, C, D);

        return new LinearSystem<>(Nat.N2(), Nat.N1(), Nat.N1(), A, B, C, D, uMin, uMax);
    }

    public static LinearSystem<N1, N1, N1> createFlywheelSystem(DCMotor motor, double jKgSquaredMeters, double G, double maxVoltage) {
        var A = new Matrix<N1, N1>(new SimpleMatrix(1, 1));
        var B = new Matrix<N1, N1>(new SimpleMatrix(1, 1));
        var C = new Matrix<N1, N1>(new SimpleMatrix(1, 1));
        var D = new Matrix<N1, N1>(new SimpleMatrix(1, 1));

        A.set(0, 0, -G * G * motor.KtNMPerAmp /
                (motor.KvRadPerSecPerVolt * motor.Rohms * jKgSquaredMeters));
        B.set(0, 0, G * motor.KtNMPerAmp / (motor.Rohms * jKgSquaredMeters));
        C.set(0, 0, 1);
        D.set(0, 0, 0);

        var uMax = new Matrix<N1, N1>(new SimpleMatrix(1, 1));
        uMax.set(0, 0, maxVoltage);
        var uMin = uMax.times(-1);

        return new LinearSystem<>(Nat.N1(), Nat.N1(), Nat.N1(), A, B, C, D, uMax, uMax);
    }

    public static LinearSystem<N2, N1, N1> createSingleJointedArmSystem(DCMotor motor, double jKgSquaredMeters, double G, double maxVoltage) {
        var A_ = new SimpleMatrix(2, 2);
        var B_ = new SimpleMatrix(2, 1);
        var C_ = new SimpleMatrix(1, 2);
        var D_ = new SimpleMatrix(1, 1);
        A_.setRow(0, 0, 0, 1);
        A_.setRow(1, 0, 0,
                -Math.pow(G, 2) * motor.KtNMPerAmp /
                        (motor.KvRadPerSecPerVolt * motor.Rohms * jKgSquaredMeters)
        );
        var A = new Matrix<N2, N2>(A_);

        B_.setColumn(0, 0,
                0, (G * motor.KtNMPerAmp / (motor.Rohms * jKgSquaredMeters)));
        var B = new Matrix<N2, N1>(B_);

        C_.setRow(0, 0, 1, 0);
        var C = new Matrix<N1, N2>(C_);

        D_.set(0, 0, 0);
        var D = new Matrix<N1, N1>(D_);

        var uMax = new Matrix<N1, N1>(new SimpleMatrix(1, 1));
        uMax.set(0, 0, maxVoltage);
        var uMin = uMax.times(-1);

        return new LinearSystem<>(Nat.N2(), Nat.N1(), Nat.N1(), A, B, C, D, uMin, uMax);
    }

    @Override
    public String toString() {
        return String.format("Linear System: A\n%s\n\nB:\n%s\n\nC:\n%s\n\nD:\n%s\n", m_A, m_B, m_C, m_D);
    }
}
