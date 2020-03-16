package edu.wpi.first.wpilibj.estimator;

import edu.wpi.first.wpilibj.math.Drake;
import edu.wpi.first.wpilibj.math.StateSpaceUtils;
import edu.wpi.first.wpilibj.system.LinearSystem;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.MatrixUtils;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.Num;
import edu.wpi.first.wpiutil.math.numbers.N1;
import org.ejml.simple.SimpleMatrix;

/**
 * Luenberger observers combine predictions from a model and measurements to
 * give an estimate of the true system state.
 * <p>
 * Luenberger observers use an L gain matrix to determine whether to trust the
 * model or measurements more. Kalman filter theory uses statistics to compute
 * an optimal L gain (alternatively called the Kalman gain, K) which minimizes
 * the sum of squares error in the state estimate.
 * <p>
 * Luenberger observers run the prediction and correction steps simultaneously
 * while Kalman filters run them sequentially. To implement a discrete-time
 * Kalman filter as a Luenberger observer, use the following mapping:
 * <pre>C = H, L = A * K</pre>
 * (H is the measurement matrix).
 * <p>
 * For more on the underlying math, read
 * https://file.tavsys.net/control/controls-engineering-in-frc.pdf.
 */
public class KalmanFilter<States extends Num, Inputs extends Num,
        Outputs extends Num> {

    /**
     * The states of the system.
     */
    private final Nat<States> states;

    /**
     * The inputs of the system.
     */
    private final Nat<Inputs> inputs;

    /**
     * The outputs of the system.
     */
    private final Nat<Outputs> outputs;

    private final LinearSystem<States, Inputs, Outputs> m_plant;

    /**
     * Error covariance matrix.
     */
    private Matrix<States, States> m_P;

    /**
     * Continuous process noise covariance matrix.
     */
    private Matrix<States, States> m_contQ;

    /**
     * Continuous measurement noise covariance matrix.
     */
    private Matrix<Outputs, Outputs> m_contR;

    /**
     * Discrete process noise covariance matrix.
     */
    private Matrix<States, States> m_discQ;

    /**
     * Discrete measurement noise covariance matrix.
     */
    private Matrix<Outputs, Outputs> m_discR;

    /**
     * Constructs a state-space observer with the given plant.
     *
     * @param states             A Nat representing the states of the system.
     * @param inputs             A Nat representing the inputs to the system.
     * @param outputs            A Nat representing the outputs of the system.
     * @param plant              The plant used for the prediction step.
     * @param stateStdDevs       Standard deviations of model states.
     * @param measurementStdDevs Standard deviations of measurements.
     * @param dtSeconds          Nominal discretization timestep.
     */
    public KalmanFilter(
            Nat<States> states, Nat<Inputs> inputs, Nat<Outputs> outputs,
            LinearSystem<States, Inputs, Outputs> plant,
            Matrix<States, N1> stateStdDevs,
            Matrix<Outputs, N1> measurementStdDevs,
            double dtSeconds
    ) {
        this.states = states;
        this.inputs = inputs;
        this.outputs = outputs;

        this.m_plant = plant;

        this.m_contQ = StateSpaceUtils.makeCovMatrix(states, stateStdDevs);
        this.m_contR = StateSpaceUtils.makeCovMatrix(outputs, measurementStdDevs);

        var pair = StateSpaceUtils.discretizeAQTaylor(plant.getA(), m_contQ, dtSeconds);
        var discA = pair.getFirst();
        var discQ = pair.getSecond();

        m_discR = StateSpaceUtils.discretizeR(m_contR, dtSeconds);

        if (StateSpaceUtils.isStabilizable(discA.transpose(),
                plant.getC().transpose()) && outputs.getNum() <= states.getNum()) {
            m_P = new Matrix<>(Drake.discreteAlgebraicRiccatiEquation(
                    discA.transpose(), plant.getC().transpose(), discQ, m_discR));
        } else {
            m_P = new Matrix<>(new SimpleMatrix(states.getNum(), states.getNum()));
        }

    }

    /**
     * Returns the error covariance matrix P.
     *
     * @return the error covariance matrix P.
     */
    public Matrix<States, States> getP() {
        return m_P;
    }

    /**
     * Returns an element of the error covariance matrix P.
     *
     * @param i Row of P.
     * @param j Column of P.
     * @return the element (i, j) of the error covariance matrix P.
     */
    public double getP(int i, int j) {
        return m_P.get(i, j);
    }


    /**
     * Returns the state estimate x-hat.
     *
     * @return the state estimate x-hat.
     */
    public Matrix<States, N1> getXhat() {
        return m_plant.getX();
    }

    /**
     * Set initial state estimate x-hat.
     *
     * @param xHat The state estimate x-hat.
     */
    public void setXhat(Matrix<States, N1> xHat) {
        m_plant.setX(xHat);
    }

    /**
     * Returns an element of the state estimate x-hat.
     *
     * @param i Row of x-hat.
     * @return the state estimate x-hat at i.
     */
    public double getXhat(int i) {
        return m_plant.getX(i);
    }

    /**
     * Set an element of the initial state estimate x-hat.
     *
     * @param i     Row of x-hat.
     * @param value Value for element of x-hat.
     */
    public void setXhat(int i, double value) {
        m_plant.setX(i, value);
    }

    /**
     * Resets the observer.
     */
    public void reset() {
        m_plant.reset();
    }

    /**
     * Project the model into the future with a new control input u.
     *
     * @param u         New control input from controller.
     * @param dtSeconds Timestep for prediction.
     */
    public void predict(Matrix<Inputs, N1> u, double dtSeconds) {
        m_plant.setX(m_plant.calculateX(m_plant.getX(), u, dtSeconds));

        var pair = StateSpaceUtils.discretizeAQTaylor(m_plant.getA(), m_contQ, dtSeconds);
        var discA = pair.getFirst();
        var discQ = pair.getSecond();

        m_P = discA.times(m_P).times(discA.transpose()).plus(discQ);
        m_discR = StateSpaceUtils.discretizeR(m_contR, dtSeconds);
    }

    /**
     * Correct the state estimate x-hat using the measurements in y.
     *
     * @param u Same control input used in the last predict step.
     * @param y Measurement vector.
     */
    public void correct(Matrix<Inputs, N1> u, Matrix<Outputs, N1> y) {
        correct(u, y, m_plant.getC(), m_discR);
    }

    /**
     * Correct the state estimate x-hat using the measurements in y.
     * <p>
     * This is useful for when the measurements available during a timestep's
     * Correct() call vary. The C matrix passed to the constructor is used if one
     * is not provided (the two-argument version of this function).
     *
     * @param <Rows> Number of rows in the result of f(x, u).
     * @param u      Same control input used in the predict step.
     * @param y      Measurement vector.
     * @param C      Output matrix.
     * @param R      Measurement noise covariance matrix.
     */
    public <Rows extends Num> void correct(
            Matrix<Inputs, N1> u,
            Matrix<Rows, N1> y,
            Matrix<Rows, States> C,
            Matrix<Rows, Rows> R) {
        var x = m_plant.getX();
        var S = C.times(m_P).times(C.transpose()).plus(R);

        // We want to put K = PC^T S^-1 into Ax = b form so we can solve it more
        // efficiently.
        //
        // K = PC^T S^-1
        // KS = PC^T
        // (KS)^T = (PC^T)^T
        // S^T K^T = CP^T
        //
        // The solution of Ax = b can be found via x = A.solve(b).
        //
        // K^T = S^T.solve(CP^T)
        // K = (S^T.solve(CP^T))^T

        SimpleMatrix K = StateSpaceUtils.lltDecompose(S.transpose().getStorage())
                .solve((C.times(m_P.transpose())).getStorage()).transpose();

        m_plant.setX(
                x.plus(new Matrix<>(K.mult((y.minus(new Matrix<>(C.times(x).getStorage().plus(m_plant.getD().times(u).getStorage())))).getStorage())))
        );

        m_P = (MatrixUtils.eye(states).minus(new Matrix<>(K.mult(C.getStorage())))).times(m_P);

    }

}
