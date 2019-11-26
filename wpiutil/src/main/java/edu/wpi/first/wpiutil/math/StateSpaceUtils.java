package edu.wpi.first.wpiutil.math;

import edu.wpi.first.wpiutil.math.numbers.N1;
import org.ejml.dense.row.CommonOps_DDRM;
import org.ejml.simple.SimpleMatrix;

public class StateSpaceUtils {

    public static <States extends Num> Matrix<States, States> makeCovMatrix(
            Nat<States> states, Matrix<States, N1> stdDevs
    ) {
        var result = new Matrix<States, States>(new SimpleMatrix(states.getNum(), states.getNum()));
        for(int i = 0; i < states.getNum(); i++) {
            result.set(i, i, Math.pow(stdDevs.get(i, 0), 2));
        }
        return result;
    }

    /**
     * Returns a discretized version of the provided continuous process noise
     * covariance matrix.
     *
     * @param Q  Continuous process noise covariance matrix.
     * @param dtSeconds Discretization timestep.
     */
    public static <States extends Num> Matrix<States, States> discretizeProcessNoiseCov(
            Nat<States> states, Matrix<States, States> A, Matrix<States, States> Q, double dtSeconds) {

        var Mgain = new SimpleMatrix(0, 0);

        // Set up the matrix M = [[-A, Q], [0, A.T]]
        Mgain = Mgain.concatColumns(
                (A.times(-1)).getStorage().concatRows(new SimpleMatrix(states.getNum(), states.getNum())),
                Q.getStorage().concatRows(A.transpose().getStorage())
        );

        var phi = SimpleMatrixUtils.expm(Mgain.scale(dtSeconds));

        // Phi12 = phi[0:States,        States:2*States]
        // Phi22 = phi[States:2*States, States:2*States]
        Matrix<States, States> phi12 = new Matrix<>(new SimpleMatrix(states.getNum(), states.getNum()));
        Matrix<States, States> phi22 = new Matrix<>(new SimpleMatrix(states.getNum(), states.getNum()));
        CommonOps_DDRM.extract(
                phi.getDDRM(), 0, states.getNum(), states.getNum(), states.getNum(), phi12.getStorage().getDDRM()
        );
        CommonOps_DDRM.extract(
                phi.getDDRM(), states.getNum(), states.getNum(), states.getNum(), states.getNum(), phi22.getStorage().getDDRM()
        );

        return phi22.transpose().times(phi12);
    }

    /**
     * Discretizes the given continuous A matrix.
     *  @param contA Continuous system matrix.
     * @param dtSeconds    Discretization timestep.
     * @return the discrete matrix system.
     */
    public static <States extends Num> Matrix<States, States> discretizeA(Matrix<States, States> contA, double dtSeconds) {
        return Drake.exp((contA.times(dtSeconds)));
    }

    /**
     * Discretizes the given continuous A and Q matrices.
     *
     * Rather than solving a 2N x 2N matrix exponential like in DiscretizeQ() (which
     * is expensive), we take advantage of the structure of the block matrix of A
     * and Q.
     *
     * The exponential of A*t, which is only N x N, is relatively cheap.
     * 2) The upper-right quarter of the 2N x 2N matrix, which we can approximate
     *    using a taylor series to several terms and still be substantially cheaper
     *    than taking the big exponential.
     *  @param contA Continuous system matrix.
     * @param contQ Continuous process noise covariance matrix.
     * @param dtSeconds    Discretization timestep.
     * @return a pair representing the discrete system matrix and process noise covariance matrix.
     */
    public static <States extends Num> SimpleMatrixUtils.Pair<Matrix<States, States>, Matrix<States, States>> discretizeAQTaylor(
            Matrix<States, States> contA, Matrix<States, States> contQ, double dtSeconds
    ) {

        Matrix<States, States> Q = (contQ.plus(contQ.transpose())).div(2.0);

        Matrix<States, States> lastTerm = Q.copy();
        double lastCoeff = dtSeconds;

        // A^T^n
        Matrix<States, States> Atn = contA.transpose();
        Matrix<States, States> phi12 = lastTerm.times(lastCoeff);

        // i = 6 i.e. 6th order should be enough precision
        for (int i = 2; i < 6; ++i) {
            lastTerm = contA.times(-1).times(lastTerm).plus(Q.times(Atn));
            lastCoeff *= dtSeconds / ((double) i);

            phi12 = phi12.plus(lastTerm.times(lastCoeff));

            Atn = Atn.times(contA.transpose());
        }

        var discA = discretizeA(contA, dtSeconds);
        Q = discA.times(phi12);

        // Make Q symmetric if it isn't already
        var discQ = (Q.plus(Q.transpose()).div(2.0));

        return new SimpleMatrixUtils.Pair<>(discA, discQ);
    }

    /**
     * Returns a discretized version of the provided continuous measurement noise
     * covariance matrix.
     *
     * @param R  Continuous measurement noise covariance matrix.
     * @param dtSeconds Discretization timestep.
     */
    public static <Outputs extends Num> Matrix<Outputs, Outputs> discretizeR(Matrix<Outputs, Outputs> R, double dtSeconds) {
        return R.div(dtSeconds);
    }

    /**
     * Returns a discretized version of the provided continuous measurement noise
     * covariance matrix.
     *
     * @param R  Continuous measurement noise covariance matrix.
     * @param dtSeconds Discretization timestep.
     */
    public static <Outputs extends Num> Matrix<Outputs, Outputs> discretizeMeasurementNoiseCov(
            Matrix<Outputs, Outputs> R, double dtSeconds) {
        return R.div(dtSeconds);
    }


    public static <States extends Num> Matrix<States, States> makeCostMatrix(Nat<States> states, Matrix<States, N1> costs) {
        var result = new SimpleMatrix(states.getNum(), states.getNum());
        result.fill(0.0);

        for(int i = 0; i < states.getNum(); i++) {
            result.set(i, i, 1.0 / (Math.pow(costs.get(i, 0), 2)));
        }

        return new Matrix<>(result);
    }

    public static <States extends Num, Inputs extends Num> void discretizeAB(Nat<States> states, Nat<Inputs> inputs,
                                                                             Matrix<States,States> a, Matrix<States,Inputs> b,
                                                                             double dtSeconds, Matrix<States, States> discA,
                                                                             Matrix<States, Inputs> discB) {

        SimpleMatrix Mcont = new SimpleMatrix(states.getNum() + inputs.getNum(), states.getNum() + inputs.getNum());
        var scaledA = a.times(dtSeconds).getStorage();
        var scaledB = b.times(dtSeconds).getStorage();

        // i is row
        for(int i = 0; i < states.getNum(); i++) {
            for(int j = 0; j < states.getNum(); j++) {
                Mcont.set(i, j, scaledA.get(i, j));
            }
            for(int j = 0; j < inputs.getNum(); j++) {
                Mcont.set(i, j + states.getNum(), scaledA.get(i, j));
            }
        }

        // Discretize A and B with the given timestep
        var Mdisc = SimpleMatrixUtils.scipyExpm(Mcont);

        var discA_ = new SimpleMatrix(states.getNum(), states.getNum());
        var discB_ = new SimpleMatrix(states.getNum(), inputs.getNum());
        CommonOps_DDRM.extract(Mdisc.getDDRM(), 0, 0, discA_.getDDRM());
        CommonOps_DDRM.extract(Mdisc.getDDRM(), 0, states.getNum(), discB_.getDDRM());

        discA = new Matrix<>(discA_);
        discB = new Matrix<>(discB_);

    }
}
