package edu.wpi.first.wpilibj.controller;

import edu.wpi.first.wpilibj.system.LinearSystem;
import edu.wpi.first.wpiutil.math.*;
import edu.wpi.first.wpiutil.math.numbers.N1;
import org.ejml.dense.row.CommonOps_DDRM;
import org.ejml.simple.SimpleMatrix;

public class LinearQuadraticRegulator<States extends Num, Inputs extends Num,
        Outputs extends Num> {

    private final Matrix<States, States> m_A;
    private final Matrix<States, Inputs> m_B;

    private boolean m_enabled = false;

    /**
     * The current reference state
     */
    private Matrix<States, N1> m_r;

    /**
     * The computed and capped controller output
     */
    private Matrix<Inputs, N1> m_u;

    // Controller gain.
    private Matrix<Inputs, States> m_K;

    public Matrix<Inputs, N1> getU() {
        return m_u;
    }

    public Matrix<States, N1> getR() {
        return m_r;
    }

    /**
     * Constructs a controller with the given coefficients and plant.
     *
     * @param plant The plant being controlled.
     * @param qElms The maximum desired error tolerance for each state.
     * @param rElms The maximum desired control effort for each input.
     * @param dtSeconds     Discretization timestep.
     */
    public LinearQuadraticRegulator(
            Nat<States> states, Nat<Inputs> inputs,
            LinearSystem<States, Inputs, Outputs> plant,
            Matrix<States, N1> qElms,
            Matrix<Inputs, N1> rElms,
            double dtSeconds
    ) {
        this(states, inputs, plant.getA(), plant.getB(), qElms, rElms, dtSeconds);
    }

    /**
     * Constructs a controller with the given coefficients and plant.
     *
     * @param a      Continuous system matrix of the plant being controlled.
     * @param b      Continuous input matrix of the plant being controlled.
     * @param qElems The maximum desired error tolerance for each state.
     * @param rElems The maximum desired control effort for each input.
     * @param dtSeconds     Discretization timestep.
     */
    public LinearQuadraticRegulator(
            Nat<States> states, Nat<Inputs> inputs,
            Matrix<States, States> a, Matrix<States, Inputs> b,
            Matrix<States, N1> qElems, Matrix<Inputs, N1> rElems,
            double dtSeconds
    ) {
        this.m_A = a;
        this.m_B = b;

        var size = states.getNum() + inputs.getNum();
        var Mcont = new SimpleMatrix(0, 0);
        var scaledA = m_A.times(dtSeconds);
        var scaledB = m_B.times(dtSeconds);
        Mcont = Mcont.concatColumns(scaledA.getStorage());
        Mcont = Mcont.concatColumns(scaledB.getStorage());
        // so our Mcont is now states x (states + inputs)
        // and we want (states + inputs) x (states + inputs)
        // so we want to add (inputs) many rows onto the bottom
        Mcont = Mcont.concatRows(new SimpleMatrix(inputs.getNum(), size));

        // calculate discrete A and B matrices
        SimpleMatrix Mstate = Drake.exp(Mcont);

        var discA = new SimpleMatrix(states.getNum(), states.getNum());
        var discB = new SimpleMatrix(states.getNum(), inputs.getNum());
        CommonOps_DDRM.extract(Mstate.getDDRM(), 0, 0, discA.getDDRM());
        CommonOps_DDRM.extract(Mstate.getDDRM(), 0, states.getNum(), discB.getDDRM());

        // make the cost matrices
        var Q = StateSpaceUtils.makeCostMatrix(states, qElems);
        var R = StateSpaceUtils.makeCostMatrix(inputs, rElems);

        var S = Drake.discreteAlgebraicRiccatiEquation(discA, discB, Q.getStorage(), R.getStorage());
        m_K = new Matrix<>((discB.transpose().mult(S).mult(discB).plus(R.getStorage())).invert().mult(discB.transpose()).mult(S).mult(discA)); // TODO (HIGH) SWITCH ALGORITHMS

        this.m_r = new MatBuilder<>(states, Nat.N1()).fill(0.0, 0.0);
        this.m_u = new MatBuilder<>(inputs, Nat.N1()).fill(0.0);
    }

    /**
     * Enables the controller
     */
    public void enable() {
        m_enabled = true;
    }

    /**
     * Disables controller and zeros controller output U.
     */
    public void disable() {
        m_enabled = false;
        m_u.getStorage().fill(0.0);
    }

    public boolean isEnabled() {
        return m_enabled;
    }

    /**
     * Returns the controller matrix K.
     */
    public Matrix<Inputs, States> getK() {
        return m_K;
    }

    /**
     * Resets the controller.
     */
    public void reset() {
        m_r.getStorage().fill(0.0);
        m_u.getStorage().fill(0.0);
    }

    /**
     * Update controller without setting a new reference.
     *
     * @param x The current state x.
     */
    public void update(Matrix<States, N1> x) {
        if(m_enabled) {
            m_u = m_K.times(m_r.minus(x)).plus(new Matrix<>(SimpleMatrixUtils.householderQrDecompose(m_B.getStorage()).solve((m_r.minus(m_A.times(m_r))).getStorage())));
        }
    }

    /**
     * Set a new reference and update the controller.
     *
     * @param x The current state x.
     * @param nextR the next reference vector r.
     */
    public void update(Matrix<States, N1> x, Matrix<States, N1> nextR) {
        if(m_enabled) {
            Matrix<States, N1> error = m_r.minus(x);
            Matrix<Inputs, N1> feedBack = m_K.times(error);
            Matrix<Inputs, N1> feedForward = new Matrix<>(SimpleMatrixUtils.householderQrDecompose(m_B.getStorage()).solve((nextR.minus(m_A.times(m_r))).getStorage()));

            m_u = feedBack.plus(feedForward);
            m_r = nextR;
        }
    }

}
