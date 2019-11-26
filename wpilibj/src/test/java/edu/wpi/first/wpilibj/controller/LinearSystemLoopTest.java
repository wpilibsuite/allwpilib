package edu.wpi.first.wpilibj.controller;

import edu.wpi.first.wpilibj.estimator.KalmanFilter;
import edu.wpi.first.wpilibj.system.LinearSystem;
import edu.wpi.first.wpilibj.system.LinearSystemLoop;
import edu.wpi.first.wpilibj.system.plant.DCMotor;
import edu.wpi.first.wpilibj.trajectory.TrapezoidProfile;
import edu.wpi.first.wpiutil.math.*;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N2;
import org.ejml.simple.SimpleMatrix;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;

import java.util.Random;

public class LinearSystemLoopTest {

    private static final double kPositionStddev = 0.0001;
    private final LinearSystemLoop<N2, N1, N1> m_loop;
    public static final double kDt = 0.00505;
    private static final Random random = new Random();

    public LinearSystemLoopTest() {
        LinearSystem<N2, N1, N1> plant = LinearSystem.createElevatorSystem(DCMotor.getVex775Pro(2), 5, 0.0181864, 1.0, 12.0);
        KalmanFilter<N2, N1, N1> observer = new KalmanFilter<>(Nat.N2(), Nat.N1(), Nat.N1(), plant,
                new MatBuilder<>(Nat.N2(), Nat.N1()).fill(0.05, 1.0),
                new MatBuilder<>(Nat.N1(), Nat.N1()).fill(0.0001), kDt);

        var qElms = new Vector<N2>(new SimpleMatrix(2, 1));
        qElms.getStorage().setColumn(0, 0, 0.02, 0.4);
        var rElms = new Vector<N1>(new SimpleMatrix(1, 1));
        rElms.getStorage().setColumn(0, 0, 12.0);
        var dt = 0.00505;

        var controller = new LinearQuadraticRegulator<>(
                Nat.N2(), Nat.N1(),
                plant, qElms, rElms, dt);

        m_loop = new LinearSystemLoop<>(Nat.N2(), Nat.N1(), Nat.N1(), plant, controller, observer);
    }

    private static void update(LinearSystemLoop<N2, N1, N1> loop, double noise) {
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

        for(int i = 0; i < 1000; i++) {

            // trapezoidal profile gang
            var profile = new TrapezoidProfile(
                    constraints, new TrapezoidProfile.State(m_loop.getXHat(0), m_loop.getXHat(1)),
                    new TrapezoidProfile.State(references.get(0, 0), references.get(1, 0))
            );
            var state = profile.calculate(kDt);
            m_loop.setNextR(new MatBuilder<>(Nat.N2(), Nat.N1()).fill(state.position, state.velocity));

            update(m_loop, (random.nextDouble() - 0.5) * kPositionStddev);
            var u = m_loop.getU(0);

            Assertions.assertTrue(u >= -12 && u <= 12.0);
        }

        Assertions.assertEquals(2.0, m_loop.getXHat(0), 0.05);
        Assertions.assertEquals(0.0, m_loop.getXHat(1), 0.5);

    }

    @Test
    public void testStateSpaceDisabled() {
        m_loop.disable();
        Matrix<N2, N1> references = new MatBuilder<>(Nat.N2(), Nat.N1()).fill(2.0, 0.0);
        m_loop.setNextR(references);

        Assertions.assertEquals(0.0, m_loop.getXHat(0), 1e-6);
        Assertions.assertEquals(0.0, m_loop.getXHat(1), 1e-6);

        for (int i = 0; i < 100; i++) {
            update(m_loop, 0.0);
        }

        Assertions.assertEquals(0.0, m_loop.getXHat(0), 1e-6);
        Assertions.assertEquals(0.0, m_loop.getXHat(1), 1e-6);
    }

}
