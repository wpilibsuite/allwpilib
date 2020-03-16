package edu.wpi.first.wpilibj.controller;

import edu.wpi.first.wpilibj.system.LinearSystem;
import edu.wpi.first.wpilibj.system.plant.DCMotor;
import edu.wpi.first.wpiutil.math.MatBuilder;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N2;
import org.ejml.simple.SimpleMatrix;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;

public class LinearQuadraticRegulatorTest {

    public static LinearSystem<N2, N1, N1> elevatorPlant = null;
    static LinearSystem<N2, N1, N1> armPlant = null;

    static {
        createArm();
        createElevator();
    }

    public static void createArm() {
        var motors = DCMotor.getVex775Pro(2);

        var m = 4.0;
        var r = 0.4;
        var J = 1d / 3d * m * r * r;
        var G = 100.0;

        armPlant = LinearSystem.createSingleJointedArmSystem(motors, J, G, 12.0);
    }

    public static void createElevator() {
        var motors = DCMotor.getVex775Pro(2);

        var m = 5.0;
        var r = 0.0181864;
        var G = 1.0;
        elevatorPlant = LinearSystem.createElevatorSystem(motors, m, r, G, 12.0);
    }

    @Test
    public void testLQROnElevator() {

        var qElms = new Matrix<N2, N1>(new SimpleMatrix(2, 1));
        qElms.getStorage().setColumn(0, 0, 0.02, 0.4);
        var rElms = new Matrix<N1, N1>(new SimpleMatrix(1, 1));
        rElms.getStorage().setColumn(0, 0, 12.0);
        var dt = 0.00505;

        var controller = new LinearQuadraticRegulator<>(
                N2.instance, N1.instance,
                elevatorPlant, qElms, rElms, dt);

        var k = controller.getK();

        assertEquals(522.153, k.get(0, 0), 0.1);
        assertEquals(38.2, k.get(0, 1), 0.1);
    }

    @Test
    public void testFourMotorElevator() {

        var kDt = 0.020;

        var plant = LinearSystem.createElevatorSystem(
                DCMotor.getVex775Pro(4),
                8.0,
                0.75 * 25.4 / 1000.0,
                14.67,
                12.0
        );

        var regulator = new LinearQuadraticRegulator<>(
                Nat.N2(), Nat.N1(), plant,
                new MatBuilder<>(Nat.N2(), Nat.N1()).fill(0.1, 0.2),
                new MatBuilder<>(Nat.N1(), Nat.N1()).fill(12.0),
                kDt);

        assertEquals(10.381, regulator.getK().get(0, 0), 1e-2);
        assertEquals(0.6929, regulator.getK().get(0, 1), 1e-2);

    }

    @Test
    public void testLQROnArm() {

        var qElms = new Matrix<N2, N1>(new SimpleMatrix(2, 1));
        qElms.getStorage().setColumn(0, 0, 0.01745, 0.08726);
        var rElms = new Matrix<N1, N1>(new SimpleMatrix(1, 1));
        rElms.getStorage().setColumn(0, 0, 12.0);
        var dt = 0.00505;

        var controller = new LinearQuadraticRegulator<>(
                N2.instance, N1.instance,
                armPlant, qElms, rElms, dt);

        var k = controller.getK();

        assertEquals(19.16, k.get(0, 0), 0.1);
        assertEquals(3.32, k.get(0, 1), 0.1);
    }

}
