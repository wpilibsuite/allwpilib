// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.system.Discretization;
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.math.system.plant.LinearSystemId;
import edu.wpi.first.wpilibj.simulation.ElevatorSim;

/** Represents a simulated elevator mechanism. */
public class ElevatorSimSysId extends ElevatorSim {

    // The min allowable height for the elevator.
    private final double m_minHeight;

    // The max allowable height for the elevator.
    private final double m_maxHeight;

    private final Matrix<N2, N1> gravityMatrix;

    /**
     * Creates a state-space model of a simulated elevator mechanism.
     *
     * @param kG The gravity gain , volts
     * @param kV The velocity gain, in volts/(unit/sec)
     * @param kA The acceleration gain, in volts/(unit/sec^2)     
     * @param gearbox          The type of and number of motors in the elevator
     *                         gearbox.
     * @param gearing          The gearing of the elevator (numbers greater than 1
     *                         represent reductions).
     * @param drumRadiusMeters The radius of the drum that the elevator spool is
     *                         wrapped around.
     * @param minHeightMeters  The min allowable height of the elevator.
     * @param maxHeightMeters  The max allowable height of the elevator.
     */
    public ElevatorSimSysId(
            double kG,
            double kV,
            double kA,
            DCMotor gearbox,
            double gearing,
            double drumRadiusMeters,
            double minHeightMeters,
            double maxHeightMeters) {
        this(
                kG,
                kV,
                kA,
                gearbox,
                gearing,
                drumRadiusMeters,
                minHeightMeters,
                maxHeightMeters,
                null);
    }

    /**
     * Creates a state-space model of a simulated elevator mechanism.
     *
     * @param kG The gravity gain , volts
     * @param kV The velocity gain, in volts/(unit/sec)
     * @param kA The acceleration gain, in volts/(unit/sec^2)  
     * @param gearbox            The type of and number of motors in the elevator
     *                           gearbox.
     * @param gearing            The gearing of the elevator (numbers greater than 1
     *                           represent reductions).
     * @param drumRadiusMeters   The radius of the drum that the elevator spool is
     *                           wrapped around.
     * @param minHeightMeters    The min allowable height of the elevator.
     * @param maxHeightMeters    The max allowable height of the elevator.
     * @param measurementStdDevs The standard deviations of the measurements.
     */
    public ElevatorSimSysId(
            double kG,
            double kV,
            double kA,
            DCMotor gearbox,
            double gearing,
            double drumRadiusMeters,
            double minHeightMeters,
            double maxHeightMeters,
            Matrix<N1, N1> measurementStdDevs) {
        super(
                LinearSystemId.identifyPositionSystem(kV, kA),
                gearbox,
                gearing,
                drumRadiusMeters,
                minHeightMeters,
                maxHeightMeters,
                true,
                measurementStdDevs);

        m_minHeight = minHeightMeters;
        m_maxHeight = maxHeightMeters;

        this.gravityMatrix = Matrix.mat(Nat.N2(), Nat.N1())
                .fill(0, -kG / kA);
    }

    /**
     * Updates the state of the elevator.
     *
     * @param currentXhat The current state estimate.
     * @param u           The system inputs (voltage).
     * @param dtSeconds   The time difference between controller updates.
     */
    @Override
    protected Matrix<N2, N1> updateX(Matrix<N2, N1> currentXhat, Matrix<N1, N1> u, double dtSeconds) {

        var discABpair = Discretization.discretizeAB(m_plant.getA(), m_plant.getB(), dtSeconds);
        var discA = discABpair.getFirst();
        var discB = discABpair.getSecond();
        Matrix<N1, N1> bplusc = m_plant.getB().solve(gravityMatrix);
        Matrix<N2, N1> discc = discB.times(bplusc);
        var updatedXhat = discA.times(m_x).plus(discB.times(m_u)).plus(discc);

        // We check for collisions after updating x-hat.
        if (wouldHitLowerLimit(updatedXhat.get(0, 0))) {
            return VecBuilder.fill(m_minHeight, 0);
        }
        if (wouldHitUpperLimit(updatedXhat.get(0, 0))) {
            return VecBuilder.fill(m_maxHeight, 0);
        }
        return updatedXhat;
    }
}
