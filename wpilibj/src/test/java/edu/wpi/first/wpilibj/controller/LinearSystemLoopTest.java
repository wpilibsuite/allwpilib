package edu.wpi.first.wpilibj.controller;

import edu.wpi.first.wpilibj.estimator.KalmanFilter;
import edu.wpi.first.wpilibj.system.LinearSystem;
import edu.wpi.first.wpilibj.system.LinearSystemLoop;
import edu.wpi.first.wpilibj.system.plant.DCMotor;
import edu.wpi.first.wpilibj.trajectory.TrapezoidProfile;
import edu.wpi.first.wpiutil.math.MatBuilder;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N2;
import org.ejml.simple.SimpleMatrix;
import static org.junit.jupiter.api.Assertions.*;
import org.junit.jupiter.api.Test;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

public class LinearSystemLoopTest {

    public static final double kDt = 0.00505;
    private static final double kPositionStddev = 0.0001;
    private static final Random random = new Random();
    private final LinearSystemLoop<N2, N1, N1> m_loop;

    public LinearSystemLoopTest() {
        LinearSystem<N2, N1, N1> plant = LinearSystem.createElevatorSystem(DCMotor.getVex775Pro(2), 5, 0.0181864, 1.0, 12.0);
        KalmanFilter<N2, N1, N1> observer = new KalmanFilter<>(Nat.N2(), Nat.N1(), Nat.N1(), plant,
                new MatBuilder<>(Nat.N2(), Nat.N1()).fill(0.05, 1.0),
                new MatBuilder<>(Nat.N1(), Nat.N1()).fill(0.0001), kDt);

        var qElms = new Matrix<N2, N1>(new SimpleMatrix(2, 1));
        qElms.getStorage().setColumn(0, 0, 0.02, 0.4);
        var rElms = new Matrix<N1, N1>(new SimpleMatrix(1, 1));
        rElms.getStorage().setColumn(0, 0, 12.0);
        var dt = 0.00505;

        var controller = new LinearQuadraticRegulator<>(
                Nat.N2(), Nat.N1(),
                plant, qElms, rElms, dt);

        m_loop = new LinearSystemLoop<>(Nat.N2(), plant, controller, observer);
    }

    private static void updateTwoState(LinearSystemLoop<N2, N1, N1> loop, double noise) {
        Matrix<N1, N1> y = loop.getPlant().calculateY(loop.getXHat(), loop.getU()).plus(
                new MatBuilder<>(Nat.N1(), Nat.N1()).fill(noise)
        );

        loop.correct(y);
        loop.predict(kDt);
    }

    private static void updateOneState(LinearSystemLoop<N1, N1, N1> loop, double noise) {
        Matrix<N1, N1> y = loop.getPlant().calculateY(loop.getXHat(), loop.getU()).plus(
                new MatBuilder<>(Nat.N1(), Nat.N1()).fill(noise)
        );

        loop.correct(y);
        loop.predict(kDt);
    }

    @Test
    public void testStateSpaceEnabled() {

        m_loop.reset();
        m_loop.enable();
        Matrix<N2, N1> references = new MatBuilder<>(Nat.N2(), Nat.N1()).fill(2.0, 0.0);
        var constraints = new TrapezoidProfile.Constraints(4, 3);
        m_loop.setNextR(references);

        for (int i = 0; i < 1000; i++) {

            // trapezoidal profile gang
            var profile = new TrapezoidProfile(
                    constraints, new TrapezoidProfile.State(m_loop.getXHat(0), m_loop.getXHat(1)),
                    new TrapezoidProfile.State(references.get(0, 0), references.get(1, 0))
            );
            var state = profile.calculate(kDt);
            m_loop.setNextR(new MatBuilder<>(Nat.N2(), Nat.N1()).fill(state.position, state.velocity));

            updateTwoState(m_loop, (random.nextGaussian()) * kPositionStddev);
            var u = m_loop.getU(0);

            assertTrue(u >= -12 && u <= 12.0);
        }

        assertEquals(2.0, m_loop.getXHat(0), 0.05);
        assertEquals(0.0, m_loop.getXHat(1), 0.5);

    }

    @Test
    public void testFlywheelEnabled() {

        LinearSystem<N1, N1, N1> plant = LinearSystem.createFlywheelSystem(DCMotor.getNEO(2),
                0.00289, 1.0, 12.0);
        KalmanFilter<N1, N1, N1> observer = new KalmanFilter<>(Nat.N1(), Nat.N1(), Nat.N1(), plant,
                new MatBuilder<>(Nat.N1(), Nat.N1()).fill(1.0),
                new MatBuilder<>(Nat.N1(), Nat.N1()).fill(0.01), kDt);

        var qElms = new MatBuilder<>(Nat.N1(), Nat.N1()).fill(9.0);
        var rElms = new MatBuilder<>(Nat.N1(), Nat.N1()).fill(12.0);

        var controller = new LinearQuadraticRegulator<>(
                Nat.N1(), Nat.N1(),
                plant, qElms, rElms, kDt);

        var loop = new LinearSystemLoop<>(Nat.N1(), plant, controller, observer);

        loop.reset();
        loop.enable();
        var references = new MatBuilder<>(Nat.N1(), Nat.N1()).fill(3000 / 60d * 2 * Math.PI);

        loop.setNextR(references);

        List<Double> timeData = new ArrayList<>();
        List<Double> xHat = new ArrayList<>();
        List<Double> volt = new ArrayList<>();
        List<Double> reference = new ArrayList<>();
        List<Double> error = new ArrayList<>();

        var time = 0.0;
        while (time < 10 || Math.abs(loop.getError(0)) < 3) {

            if (time > 10) {
                break;
            }

            loop.setNextR(references);

            updateOneState(loop, (random.nextGaussian()) * kPositionStddev);
            var u = loop.getU(0);

            assertTrue(u >= -12 && u <= 12.0);

            xHat.add(loop.getXHat(0) / 2d / Math.PI * 60);
            volt.add(u);
            time += kDt;
            timeData.add(time);
            reference.add(references.get(0, 0) / 2d / Math.PI * 60);
            error.add(loop.getError(0) / 2d / Math.PI * 60);
        }

//        XYChart bigChart = new XYChartBuilder().build();
//        bigChart.addSeries("Xhat, RPM", timeData, xHat);
//        bigChart.addSeries("Reference, RPM", timeData, reference);
//        bigChart.addSeries("Error, RPM", timeData, error);
//
//        XYChart smolChart = new XYChartBuilder().build();
//        smolChart.addSeries("Volts, V", timeData, volt);
//
//        try {
//            new SwingWrapper<>(bigChart).displayChart();
//            new SwingWrapper<>(smolChart).displayChart();
//            Thread.sleep(10000000);
//        } catch (InterruptedException e) {
//            e.printStackTrace();
//        }

        assertEquals(loop.getError(0), 0.0, 0.1);
    }

    @Test
    public void testStateSpaceDisabled() {
        m_loop.disable();
        Matrix<N2, N1> references = new MatBuilder<>(Nat.N2(), Nat.N1()).fill(2.0, 0.0);
        m_loop.setNextR(references);

        assertEquals(0.0, m_loop.getXHat(0), 1e-6);
        assertEquals(0.0, m_loop.getXHat(1), 1e-6);

        for (int i = 0; i < 100; i++) {
            updateTwoState(m_loop, 0.0);
        }

        assertEquals(0.0, m_loop.getXHat(0), 1e-6);
        assertEquals(0.0, m_loop.getXHat(1), 1e-6);
    }

}
