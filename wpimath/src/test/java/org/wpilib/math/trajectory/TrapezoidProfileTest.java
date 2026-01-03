// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

class TrapezoidProfileTest {
  private static final double kDt = 0.01;

  /**
   * Asserts "val1" is less than or equal to "val2".
   *
   * @param val1 First operand in comparison.
   * @param val2 Second operand in comparison.
   */
  private static void assertLessThanOrEquals(double val1, double val2) {
    assertTrue(val1 <= val2, val1 + " is greater than " + val2);
  }

  /**
   * Asserts "val1" is within "eps" of "val2".
   *
   * @param val1 First operand in comparison.
   * @param val2 Second operand in comparison.
   * @param eps Tolerance for whether values are near to each other.
   */
  private static void assertNear(double val1, double val2, double eps) {
    assertTrue(
        Math.abs(val1 - val2) <= eps,
        "Difference between " + val1 + " and " + val2 + " is greater than " + eps);
  }

  /**
   * Asserts "val1" is less than or within "eps" of "val2".
   *
   * @param val1 First operand in comparison.
   * @param val2 Second operand in comparison.
   * @param eps Tolerance for whether values are near to each other.
   */
  private static void assertLessThanOrNear(double val1, double val2, double eps) {
    if (val1 <= val2) {
      assertLessThanOrEquals(val1, val2);
    } else {
      assertNear(val1, val2, eps);
    }
  }

  /**
   * Asserts that the states could feasibly be reached within a kDt seconds with a set acceleration.
   *
   * @param state The original state.
   * @param newState The new state.
   * @param maxAccel The positive valued max acceleration that could have been applied to the state.
   */
  private static void checkFeasible(
      TrapezoidProfile.State state, TrapezoidProfile.State newState, double maxAccel) {
    double deltaV = newState.velocity - state.velocity;
    double deltaX = newState.position - state.position;
    assertLessThanOrNear(Math.abs(deltaV), maxAccel * kDt, 1e-10);
    assertLessThanOrNear(
        Math.abs(deltaX), Math.abs(state.velocity) * kDt + maxAccel / 2.0 * kDt * kDt, 1e-10);
  }

  @Test
  void checkTiming() {
    TrapezoidProfile.Constraints constraints = new TrapezoidProfile.Constraints(1.75, 0.75);
    TrapezoidProfile.State goal = new TrapezoidProfile.State(12, -1);
    TrapezoidProfile.State state = new TrapezoidProfile.State(0, 1);

    TrapezoidProfile profile = new TrapezoidProfile(constraints);
    var newState = profile.calculate(kDt, state, goal);

    double profileTime = profile.totalTime();

    assertNear(profileTime, 9.952380952380953, 1e-10);
    assertEquals(profileTime, profile.timeLeftUntil(state, goal));
    profile.timeLeftUntil(goal, new TrapezoidProfile.State(goal.position, goal.velocity));
    assertEquals(profileTime, profile.totalTime());
  }

  @Test
  void reachesGoal() {
    TrapezoidProfile.Constraints constraints = new TrapezoidProfile.Constraints(1.75, 0.75);
    TrapezoidProfile.State goal = new TrapezoidProfile.State(3, 0);
    TrapezoidProfile.State state = new TrapezoidProfile.State();

    TrapezoidProfile profile = new TrapezoidProfile(constraints);
    for (int i = 0; i < 450; ++i) {
      var newState = profile.calculate(kDt, state, goal);
      checkFeasible(state, newState, constraints.maxAcceleration);
      state = newState;
    }
    assertEquals(state, goal);
  }

  @Test
  void backwards() {
    TrapezoidProfile.Constraints constraints = new TrapezoidProfile.Constraints(0.75, 0.75);
    TrapezoidProfile.State goal = new TrapezoidProfile.State(-2, 0);
    TrapezoidProfile.State state = new TrapezoidProfile.State();

    TrapezoidProfile profile = new TrapezoidProfile(constraints);
    for (int i = 0; i < 400; ++i) {
      var newState = profile.calculate(kDt, state, goal);
      checkFeasible(state, newState, constraints.maxAcceleration);
      state = newState;
    }
    assertEquals(state, goal);
  }

  // Test the forwards case for an invalid initial velocity with the profile sign.
  @Test
  void checkLargeVelocitySameSignAsPeak() {
    TrapezoidProfile.Constraints constraints = new TrapezoidProfile.Constraints(1.75, 0.75);
    TrapezoidProfile.State goal = new TrapezoidProfile.State(12, 0);
    TrapezoidProfile.State state = new TrapezoidProfile.State(0, 3);

    TrapezoidProfile profile = new TrapezoidProfile(constraints);

    int plateauCount = 0;
    // About 7.5s.
    for (int i = 0; i < 1000; ++i) {
      var newState = profile.calculate(kDt, state, goal);
      checkFeasible(state, newState, constraints.maxAcceleration);
      state = newState;
      if (state.velocity == constraints.maxVelocity) {
        plateauCount++;
      }
    }

    assertLessThanOrEquals(5, plateauCount);

    assertEquals(state, goal);
  }

  // Test the backwards case for an invalid initial velocity with the profile sign.
  @Test
  void checkLargeVelocitySameSignAsPeakBackwards() {
    TrapezoidProfile.Constraints constraints = new TrapezoidProfile.Constraints(1.75, 0.75);
    TrapezoidProfile.State goal = new TrapezoidProfile.State(-12, 0);
    TrapezoidProfile.State state = new TrapezoidProfile.State(0, -3);

    TrapezoidProfile profile = new TrapezoidProfile(constraints);

    int plateauCount = 0;
    // About 7.5s.
    for (int i = 0; i < 1000; ++i) {
      var newState = profile.calculate(kDt, state, goal);
      checkFeasible(state, newState, constraints.maxAcceleration);
      state = newState;
      if (state.velocity == -constraints.maxVelocity) {
        plateauCount++;
      }
    }

    assertLessThanOrEquals(5, plateauCount);

    assertEquals(state, goal);
  }

  // Test the forwards case for an invalid initial velocity with a sign
  // opposite the profile sign.
  @Test
  void checkLargeVelocityOppositePeak() {
    TrapezoidProfile.Constraints constraints = new TrapezoidProfile.Constraints(1.75, 0.75);
    TrapezoidProfile.State goal = new TrapezoidProfile.State(12, 0);
    TrapezoidProfile.State state = new TrapezoidProfile.State(0, -3);

    TrapezoidProfile profile = new TrapezoidProfile(constraints);

    int plateauCount = 0;
    // About 7.5s.
    for (int i = 0; i < 1700; ++i) {
      var newState = profile.calculate(kDt, state, goal);
      checkFeasible(state, newState, constraints.maxAcceleration);
      state = newState;
      if (state.velocity == constraints.maxVelocity) {
        plateauCount++;
      }
    }

    assertLessThanOrEquals(5, plateauCount);

    assertEquals(state, goal);
  }

  // Test the backwards case for an invalid initial velocity with a sign
  // opposite the profile sign.
  @Test
  void checkLargeVelocityOppositePeakBackwards() {
    TrapezoidProfile.Constraints constraints = new TrapezoidProfile.Constraints(1.75, 0.75);
    TrapezoidProfile.State goal = new TrapezoidProfile.State(-12, 0);
    TrapezoidProfile.State state = new TrapezoidProfile.State(0, 3);

    TrapezoidProfile profile = new TrapezoidProfile(constraints);

    int plateauCount = 0;
    for (int i = 0; i < 1700; ++i) {
      var newState = profile.calculate(kDt, state, goal);
      checkFeasible(state, newState, constraints.maxAcceleration);
      state = newState;
      if (state.velocity == -constraints.maxVelocity) {
        plateauCount++;
      }
    }

    assertLessThanOrEquals(5, plateauCount);

    assertEquals(state, goal);
  }

  // This is the case that generated a broken profile in the old impl.
  @Test
  void largeVelocityAndSmallPositionDelta() {
    TrapezoidProfile.Constraints constraints = new TrapezoidProfile.Constraints(1.75, 0.75);
    TrapezoidProfile.State goal = new TrapezoidProfile.State(0.01, 0);
    TrapezoidProfile.State state = new TrapezoidProfile.State(0, 1);

    TrapezoidProfile profile = new TrapezoidProfile(constraints);

    for (int i = 0; i < 450; ++i) {
      var newState = profile.calculate(kDt, state, goal);
      checkFeasible(state, newState, constraints.maxAcceleration);
      state = newState;
    }

    assertEquals(state, goal);
  }

  @Test
  void largeVelocityAndSmallPositionDeltaBackwards() {
    TrapezoidProfile.Constraints constraints = new TrapezoidProfile.Constraints(1.75, 0.75);
    TrapezoidProfile.State goal = new TrapezoidProfile.State(-0.01, 0);
    TrapezoidProfile.State state = new TrapezoidProfile.State(0, -2);

    TrapezoidProfile profile = new TrapezoidProfile(constraints);

    for (int i = 0; i < 700; ++i) {
      var newState = profile.calculate(kDt, state, goal);
      checkFeasible(state, newState, constraints.maxAcceleration);
      state = newState;
    }

    assertEquals(state, goal);
  }

  @Test
  void switchGoalInMiddle() {
    TrapezoidProfile.Constraints constraints = new TrapezoidProfile.Constraints(0.75, 0.75);
    TrapezoidProfile.State goal = new TrapezoidProfile.State(-2, 0);
    TrapezoidProfile.State state = new TrapezoidProfile.State();

    TrapezoidProfile profile = new TrapezoidProfile(constraints);
    for (int i = 0; i < 200; ++i) {
      var newState = profile.calculate(kDt, state, goal);
      checkFeasible(state, newState, constraints.maxAcceleration);
      state = newState;
    }
    assertNotEquals(state, goal);

    goal = new TrapezoidProfile.State(0.0, 0.0);
    profile = new TrapezoidProfile(constraints);
    for (int i = 0; i < 550; ++i) {
      var newState = profile.calculate(kDt, state, goal);
      checkFeasible(state, newState, constraints.maxAcceleration);
      state = newState;
    }
    assertEquals(state, goal);
  }

  // Checks to make sure that it hits top speed
  @Test
  void topSpeed() {
    TrapezoidProfile.Constraints constraints = new TrapezoidProfile.Constraints(0.75, 0.75);
    TrapezoidProfile.State goal = new TrapezoidProfile.State(4, 0);
    TrapezoidProfile.State state = new TrapezoidProfile.State();

    TrapezoidProfile profile = new TrapezoidProfile(constraints);
    for (int i = 0; i < 200; ++i) {
      var newState = profile.calculate(kDt, state, goal);
      checkFeasible(state, newState, constraints.maxAcceleration);
      state = newState;
    }
    assertNear(constraints.maxVelocity, state.velocity, 10e-5);

    profile = new TrapezoidProfile(constraints);
    for (int i = 0; i < 2000; ++i) {
      var newState = profile.calculate(kDt, state, goal);
      checkFeasible(state, newState, constraints.maxAcceleration);
      state = newState;
    }
    assertEquals(state, goal);
  }

  @Test
  void timingToCurrent() {
    TrapezoidProfile.Constraints constraints = new TrapezoidProfile.Constraints(0.75, 0.75);
    TrapezoidProfile.State goal = new TrapezoidProfile.State(2, 0);
    TrapezoidProfile.State state = new TrapezoidProfile.State();

    TrapezoidProfile profile = new TrapezoidProfile(constraints);
    for (int i = 0; i < 400; i++) {
      var newState = profile.calculate(kDt, state, goal);
      checkFeasible(state, newState, constraints.maxAcceleration);
      state = newState;
      assertNear(profile.timeLeftUntil(state, state), 0, 2e-2);
    }
  }

  @Test
  void timingToGoal() {
    TrapezoidProfile.Constraints constraints = new TrapezoidProfile.Constraints(0.75, 0.75);
    TrapezoidProfile.State goal = new TrapezoidProfile.State(2, 0);

    TrapezoidProfile profile = new TrapezoidProfile(constraints);
    TrapezoidProfile.State state = profile.calculate(kDt, goal, new TrapezoidProfile.State());

    double predictedTimeLeft = profile.timeLeftUntil(state, goal);
    boolean reachedGoal = false;
    for (int i = 0; i < 400; i++) {
      var newState = profile.calculate(kDt, state, goal);
      checkFeasible(state, newState, constraints.maxAcceleration);
      state = newState;
      if (!reachedGoal && state.equals(goal)) {
        // Expected value using for loop index is just an approximation since
        // the time left in the profile doesn't increase linearly at the
        // endpoints
        assertNear(predictedTimeLeft, i / 100.0, 0.25);
        reachedGoal = true;
      }
    }
  }

  @Test
  void timingToNegativeGoal() {
    TrapezoidProfile.Constraints constraints = new TrapezoidProfile.Constraints(0.75, 0.75);
    TrapezoidProfile.State goal = new TrapezoidProfile.State(-2, 0);

    TrapezoidProfile profile = new TrapezoidProfile(constraints);
    TrapezoidProfile.State state = profile.calculate(kDt, goal, new TrapezoidProfile.State());

    double predictedTimeLeft = profile.timeLeftUntil(state, goal);
    boolean reachedGoal = false;
    for (int i = 0; i < 400; i++) {
      var newState = profile.calculate(kDt, state, goal);
      checkFeasible(state, newState, constraints.maxAcceleration);
      state = newState;
      if (!reachedGoal && state.equals(goal)) {
        // Expected value using for loop index is just an approximation since
        // the time left in the profile doesn't increase linearly at the
        // endpoints
        assertNear(predictedTimeLeft, i / 100.0, 0.25);
        reachedGoal = true;
      }
    }
  }

  @Test
  void goalVelocityConstraints() {
    TrapezoidProfile.Constraints constraints = new TrapezoidProfile.Constraints(0.75, 0.75);
    TrapezoidProfile.State goal = new TrapezoidProfile.State(10, 5);
    TrapezoidProfile.State state = new TrapezoidProfile.State(0, 0.75);

    TrapezoidProfile profile = new TrapezoidProfile(constraints);

    for (int i = 0; i < 200; ++i) {
      var newState = profile.calculate(kDt, state, goal);
      checkFeasible(state, newState, constraints.maxAcceleration);
      state = newState;
      assertLessThanOrEquals(Math.abs(state.velocity), Math.abs(constraints.maxVelocity));
    }
  }

  @Test
  void negativeGoalVelocityConstraints() {
    TrapezoidProfile.Constraints constraints = new TrapezoidProfile.Constraints(0.75, 0.75);
    TrapezoidProfile.State goal = new TrapezoidProfile.State(10, -5);
    TrapezoidProfile.State state = new TrapezoidProfile.State(0, 0.75);

    TrapezoidProfile profile = new TrapezoidProfile(constraints);

    for (int i = 0; i < 200; ++i) {
      var newState = profile.calculate(kDt, state, goal);
      checkFeasible(state, newState, constraints.maxAcceleration);
      state = newState;
      assertLessThanOrEquals(Math.abs(state.velocity), Math.abs(constraints.maxVelocity));
    }
  }
}
