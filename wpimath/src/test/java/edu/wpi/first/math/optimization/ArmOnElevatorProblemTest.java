// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.optimization;

import static edu.wpi.first.math.autodiff.Variable.pow;
import static edu.wpi.first.math.optimization.Constraints.eq;
import static edu.wpi.first.math.optimization.Constraints.le;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.autodiff.ExpressionType;
import edu.wpi.first.math.autodiff.Variable;
import edu.wpi.first.math.autodiff.VariableMatrix;
import edu.wpi.first.math.optimization.solver.ExitStatus;
import org.junit.jupiter.api.Test;

// This problem tests the case where regularization fails
class ArmOnElevatorProblemTest {
  @Test
  void testArmOnElevatorProblem() {
    final int N = 800;

    final double ELEVATOR_START_HEIGHT = 1.0; // m
    final double ELEVATOR_END_HEIGHT = 1.25; // m
    final double ELEVATOR_MAX_VELOCITY = 1.0; // m/s
    final double ELEVATOR_MAX_ACCELERATION = 2.0; // m/s²

    // final double ARM_LENGTH = 1.0; // m
    final double ARM_START_ANGLE = 0.0; // rad
    final double ARM_END_ANGLE = Math.PI; // rad
    final double ARM_MAX_VELOCITY = 2.0 * Math.PI; // rad/s
    final double ARM_MAX_ACCELERATION = 4.0 * Math.PI; // rad/s²

    // final double END_EFFECTOR_MAX_HEIGHT = 1.8; // m

    final double TOTAL_TIME = 4.0;
    final double dt = TOTAL_TIME / N;

    var problem = new Problem();

    var elevator = problem.decisionVariable(2, N + 1);
    var elevator_accel = problem.decisionVariable(1, N);

    var arm = problem.decisionVariable(2, N + 1);
    var arm_accel = problem.decisionVariable(1, N);

    for (int k = 0; k < N; ++k) {
      // Elevator dynamics constraints
      problem.subjectTo(
          eq(
              elevator.get(0, k + 1),
              elevator
                  .get(0, k)
                  .plus(elevator.get(1, k).times(dt))
                  .plus(elevator_accel.get(0, k).times(0.5 * dt * dt))));
      problem.subjectTo(
          eq(elevator.get(1, k + 1), elevator.get(1, k).plus(elevator_accel.get(0, k).times(dt))));

      // Arm dynamics constraints
      problem.subjectTo(
          eq(
              arm.get(0, k + 1),
              arm.get(0, k)
                  .plus(arm.get(1, k).times(dt))
                  .plus(arm_accel.get(0, k).times(0.5 * dt * dt))));
      problem.subjectTo(eq(arm.get(1, k + 1), arm.get(1, k).plus(arm_accel.get(0, k).times(dt))));
    }

    // Elevator start and end conditions
    problem.subjectTo(
        eq(elevator.col(0), new VariableMatrix(new double[][] {{ELEVATOR_START_HEIGHT}, {0.0}})));
    problem.subjectTo(
        eq(elevator.col(N), new VariableMatrix(new double[][] {{ELEVATOR_END_HEIGHT}, {0.0}})));

    // Arm start and end conditions
    problem.subjectTo(
        eq(arm.col(0), new VariableMatrix(new double[][] {{ARM_START_ANGLE}, {0.0}})));
    problem.subjectTo(eq(arm.col(N), new VariableMatrix(new double[][] {{ARM_END_ANGLE}, {0.0}})));

    // Elevator velocity limits
    problem.subjectTo(le(-ELEVATOR_MAX_VELOCITY, elevator.row(1)));
    problem.subjectTo(le(elevator.row(1), ELEVATOR_MAX_VELOCITY));

    // Elevator acceleration limits
    problem.subjectTo(le(-ELEVATOR_MAX_ACCELERATION, elevator_accel));
    problem.subjectTo(le(elevator_accel, ELEVATOR_MAX_ACCELERATION));

    // Arm velocity limits
    problem.subjectTo(le(-ARM_MAX_VELOCITY, arm.row(1)));
    problem.subjectTo(le(arm.row(1), ARM_MAX_VELOCITY));

    // Arm acceleration limits
    problem.subjectTo(le(-ARM_MAX_ACCELERATION, arm_accel));
    problem.subjectTo(le(arm_accel, ARM_MAX_ACCELERATION));

    // Height limit
    // var heights = elevator.row(0).plus(arm.row(0).cwiseMap(Variable::sin).times(ARM_LENGTH));
    // problem.subjectTo(le(heights, END_EFFECTOR_MAX_HEIGHT));

    // Cost function
    var J = new Variable(0.0);
    for (int k = 0; k < N + 1; ++k) {
      J =
          J.plus(
              pow(new Variable(ELEVATOR_END_HEIGHT).minus(elevator.get(0, k)), 2)
                  .plus(pow(new Variable(ARM_END_ANGLE).minus(arm.get(0, k)), 2)));
    }
    problem.minimize(J);

    assertEquals(ExpressionType.QUADRATIC, problem.costFunctionType());
    assertEquals(ExpressionType.LINEAR, problem.equalityConstraintType());
    assertEquals(ExpressionType.LINEAR, problem.inequalityConstraintType());

    assertEquals(ExitStatus.SUCCESS, problem.solve());
  }
}
