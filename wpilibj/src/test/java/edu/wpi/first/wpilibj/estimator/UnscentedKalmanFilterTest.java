package edu.wpi.first.wpilibj.estimator;

import edu.wpi.first.wpilibj.system.plant.DCMotor;
import edu.wpi.first.wpiutil.math.MatBuilder;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N2;
import edu.wpi.first.wpiutil.math.numbers.N3;
import edu.wpi.first.wpiutil.math.numbers.N5;
import org.ejml.simple.SimpleMatrix;
import org.junit.jupiter.api.Test;

public class UnscentedKalmanFilterTest {

  @SuppressWarnings({"LocalVariableName", "ParameterName"})
  public static Matrix<N5, N1> getDynamics(Matrix<N5, N1> x, Matrix<N2, N1> u) {
    var motors = DCMotor.getCIM(2);

    var gHigh = 7.08;
    var rb = 0.8382 / 2.0;
    var r = 0.0746125;
    var m = 63.503;
    var J = 5.6;

    var C1 = -Math.pow(gHigh, 2) * motors.m_KtNMPerAmp
            / (motors.m_KvRadPerSecPerVolt * motors.m_rOhms * r * r);
    var C2 = gHigh * motors.m_KtNMPerAmp / (motors.m_rOhms * r);
    var k1 = 1.0 / m + rb * rb / J;
    var k2 = 1.0 / m - rb * rb / J;

    var vl = x.get(3, 0);
    var vr = x.get(4, 0);
    var Vl = u.get(0, 0);
    var Vr = u.get(1, 0);

    Matrix<N5, N1> result = new Matrix<>(new SimpleMatrix(5, 1));
    var v = 0.5 * (vl + vr);
    result.set(0, 0, v * Math.cos(x.get(2, 0)));
    result.set(1, 0, v * Math.sin(x.get(2, 0)));
    result.set(2, 0, ((vr - vl) / (2.0 * rb)));
    result.set(3, 0,
            k1 * ((C1 * vl) + (C2 * Vl)) +
                    k2 * ((C1 * vr) + (C2 * Vr)));
    result.set(4, 0,
            k2 * ((C1 * vl) + (C2 * Vl)) +
                    k1 * ((C1 * vr) + (C2 * vr)));
    return result;
  }

  public static Matrix<N3, N1> getLocalMeasurementModel(Matrix<N5, N1> x, Matrix<N2, N1> u) {
    return new MatBuilder<>(Nat.N3(), Nat.N1()).fill(x.get(2, 0), x.get(3, 0), x.get(4, 0));
  }

  public static Matrix<N5, N1> getGlobalMeasurementModel(Matrix<N5, N1> x, Matrix<N2, N1> u) {
    return new MatBuilder<>(Nat.N5(), Nat.N1()).fill(x.get(0, 0), x.get(1, 0), x.get(2, 0), x.get(3, 0), x.get(4, 0));
  }

  @Test
  @SuppressWarnings("LocalVariableName")
  public void testInit() {
    UnscentedKalmanFilter<N5, N2, N3> observer = new UnscentedKalmanFilter<>(
            Nat.N5(), Nat.N2(), Nat.N3(),
            UnscentedKalmanFilterTest::getDynamics,
            UnscentedKalmanFilterTest::getLocalMeasurementModel,
            new MatBuilder<>(Nat.N5(), Nat.N1()).fill(0.5, 0.5, 10.0, 1.0, 1.0),
            new MatBuilder<>(Nat.N3(), Nat.N1()).fill(0.0001, 0.01, 0.01),
            0.02);

    var u = new MatBuilder<>(Nat.N2(), Nat.N1()).fill(12.0, 12.0);
    observer.predict(u, 0.00505);

    var localY = getLocalMeasurementModel(observer.getXhat(), u);
    observer.correct(u, localY);
  }
}
