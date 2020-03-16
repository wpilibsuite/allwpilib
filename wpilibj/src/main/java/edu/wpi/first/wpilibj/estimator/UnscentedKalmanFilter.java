package edu.wpi.first.wpilibj.estimator;

import edu.wpi.first.wpilibj.math.StateSpaceUtils;
import edu.wpi.first.wpilibj.util.Pair;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.Num;
import edu.wpi.first.wpiutil.math.numbers.N1;
import org.ejml.dense.row.CommonOps_DDRM;
import org.ejml.simple.SimpleMatrix;

import java.util.function.BiFunction;


public class UnscentedKalmanFilter<States extends Num, Inputs extends Num,
        Outputs extends Num> {

    private final BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<States, N1>> m_f;
    private final BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<Outputs, N1>> m_h;
    private final Matrix<States, States> m_Q;
    private final Matrix<Outputs, Outputs> m_R;
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
    private Matrix<States, N1> m_xHat;
    private Matrix<States, States> m_P;
    private Matrix m_sigmasF;
    // TODO where is this initilized?
    private MerweScaledSigmaPoints<States> m_pts;

    public UnscentedKalmanFilter(Nat<States> states, Nat<Inputs> inputs, Nat<Outputs> outputs,
                                 BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<States, N1>> f,
                                 BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<Outputs, N1>> h,
                                 Matrix<States, N1> stateStdDevs,
                                 Matrix<Outputs, N1> measurementStdDevs) {
        this.states = states;
        this.inputs = inputs;
        this.outputs = outputs;

        m_f = f;
        m_h = h;
        m_Q = StateSpaceUtils.makeCovMatrix(states, stateStdDevs);
        m_R = StateSpaceUtils.makeCovMatrix(outputs, measurementStdDevs);

        reset();
    }

    public static <S extends Num, CovDim extends Num> Pair<Matrix<N1, CovDim>, Matrix<CovDim, CovDim>> unscentedTransform(
            S s, CovDim dim, Matrix sigmas, Matrix Wm, Matrix Wc, Matrix noiseCov
    ) {
        if (sigmas.getNumCols() != 2 * s.getNum() + 1 || sigmas.getNumCols() != dim.getNum())
            throw new IllegalArgumentException("Sigmas must be 2 * states + 1 by covDim! Got "
                    + sigmas.getNumRows() + " by " + sigmas.getNumCols());

        if (Wm.getNumRows() != 1 || Wm.getNumCols() != 2 * s.getNum() + 1)
            throw new IllegalArgumentException("Wm must be 1 by 2 * states + 1! Got "
                    + Wm.getNumRows() + " by " + Wm.getNumCols());

        if (Wc.getNumRows() != 1 || Wc.getNumCols() != 2 * s.getNum() + 1)
            throw new IllegalArgumentException("Wc must be 1 by 2 * states + 1! Got "
                    + Wc.getNumRows() + " by " + Wc.getNumCols());

        if (noiseCov.getNumRows() != dim.getNum() || noiseCov.getNumCols() != dim.getNum())
            throw new IllegalArgumentException("noiseCov must be covDim by covDim! Got "
                    + noiseCov.getNumRows() + " by " + noiseCov.getNumCols());

//        // new mean is just the sum of the sigmas * weight
//        // dot = \Sigma^n_1 (W[k]*Xi[k])
//        Matrix x = Wm.times(sigmas);
//
//        // new covariance is the sum of the outer product of the residuals
//        // times the weights
//        Matrix y = new Matrix(new SimpleMatrix(2 * s.getNum() + 1, dim.getNum()));
//        for (int i = 0; i < 2 * s.getNum() + 1; ++i) {
//            // set the block from i, 0 to i+1, covDim
//
//            y.template block<1, CovDim>(i, 0) =
//            sigmas.template block<1, CovDim>(i, 0) - x;
//        }
//        Matrix<CovDim, CovDim> P =
//                y.transpose() * Eigen::DiagonalMatrix<double, 2 * States + 1>(Wc) * y;
//
//        P += noiseCov;
//
//        return std::make_tuple(x, P);
        return null;
    }

    /**
     * Resets the observer.
     */
    public void reset() {
        m_xHat = new Matrix<>(new SimpleMatrix(states.getNum(), 1));
        m_P = new Matrix<>(new SimpleMatrix(states.getNum(), states.getNum()));
    }

    /**
     * Project the model into the future with a new control input u.
     *
     * @param u         New control input from controller.
     * @param dtSeconds Timestep for prediction.
     */
    public void predict(Matrix<Inputs, N1> u, double dtSeconds) {
        var sigmas = m_pts.sigmaPoints(m_xHat, m_P);

        for (int i = 0; i < m_pts.getNumSigns(); ++i) {

            var temp = new SimpleMatrix(1, states.getNum());
            CommonOps_DDRM.extract(sigmas.getStorage().getDDRM(), i, 0, temp.getDDRM());
            var x = new Matrix<States, N1>(temp.transpose());

            // set the block from i, 0 to i+1, States to the result of RungeKutta
//            var rungeKutta = RungeKuttaHelper.RungeKutta(m_f, x, u, dtSeconds).transpose();

//            Eigen::Matrix<double, States, 1> x =
//                    sigmas.template block<1, States>(i, 0).transpose();
//            m_sigmasF.template block<1, States>(i, 0) =
//            RungeKutta(m_f, x, u, dt).transpose();

        }

    }

    /**
     * Correct the state estimate x-hat using the measurements in y.
     * <p>
     * This is useful for when the measurements available during a timestep's
     * Correct() call vary. The h(x, u) passed to the constructor is used if one
     * is not provided (the two-argument version of this function).
     *
     * @param u Same control input used in the predict step.
     * @param y Measurement vector.
     * @param h A vector-valued function of x and u that returns
     *          the measurement vector.
     * @param R Measurement noise covariance matrix.
     */
    public void Correct(
            Matrix<Inputs, N1> u,
            Matrix<Outputs, N1> y,
            BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<Outputs, N1>> h,
            Matrix<Outputs, Outputs> R) {

//        // Transform sigma points into measurement space
//        Eigen::Matrix<double, 2 * States + 1, Rows> sigmasH;
//        for (int i = 0; i < m_pts.NumSigmas(); ++i) {
//            sigmasH.template block<1, Rows>(i, 0) =
//            h(m_sigmasF.template block<1, States>(i, 0).transpose(), u);
//        }
//
//        // Mean and covariance of prediction passed through UT
//        var ret = UnscentedTransform<States, Rows>(sigmasH, m_pts.Wm(), m_pts.Wc(), R);
//        Eigen::Matrix<double, Rows, 1> yHat = std::get<0>(ret);
//        Eigen::Matrix<double, Rows, Rows> Py = std::get<1>(ret);
//
//        // Compute cross covariance of the state and the measurements
//        Eigen::Matrix<double, States, Rows> Pxy;
//        Pxy.setZero();
//        for (int i = 0; i < m_pts.NumSigmas(); ++i) {
//            Pxy += m_pts.Wc(i) *
//                    (m_sigmasF.template block<1, States>(i, 0) - m_xHat.transpose())
//                 .transpose() *
//                    (sigmasH.template block<1, Rows>(i, 0) - yHat.transpose());
//        }

//        // K = P_{xy} Py^-1
//        // K^T = P_y^T^-1 P_{xy}^T
//        // P_y^T K^T = P_{xy}^T
//        // K^T = P_y^T.solve(P_{xy}^T)
//        // K = (P_y^T.solve(P_{xy}^T)^T
//        auto K = Py.transpose().ldlt().solve(Pxy.transpose()).transpose();
//
//        m_xHat += K * (y - yHat);
//        m_P -= K * Py * K.transpose();
    }

    public static class Tuple<A, B, C> {
        private final A m_first;
        private final B m_second;
        private final C m_third;

        public Tuple(A first, B second, C third) {
            m_first = first;
            m_second = second;
            m_third = third;
        }

        public A getFirst() {
            return m_first;
        }

        public B getSecond() {
            return m_second;
        }

        public C getThird() {
            return m_third;
        }
    }


}
