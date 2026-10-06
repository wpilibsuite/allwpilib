// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

class TrapezoidProfileTest {
  private static final double DT = 0.01;

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
   * Asserts that the states could feasibly be reached within a DT seconds with a set acceleration.
   *
   * @param state The original state.
   * @param newState The new state.
   * @param maxAccel The positive valued max acceleration that could have been applied to the state.
   */
  private static void checkFeasible(
      TrapezoidProfile.State state, TrapezoidProfile.State newState, double maxAccel) {
    double deltaV = newState.velocity - state.velocity;
    double deltaX = newState.position - state.position;
    assertLessThanOrNear(Math.abs(deltaV), maxAccel * DT, 1e-10);
    assertLessThanOrNear(
        Math.abs(deltaX), Math.abs(state.velocity) * DT + maxAccel / 2.0 * DT * DT, 1e-10);
  }

  /**
   * Generates a profile from the given state to the goal and returns its state after DT.
   *
   * @param constraints The constraints on the profile.
   * @param state The current state.
   * @param goal The goal state.
   * @return The state of the profile after DT.
   */
  private static TrapezoidProfile.State step(
      TrapezoidProfile.Constraints constraints,
      TrapezoidProfile.State state,
      TrapezoidProfile.State goal) {
    var sample = TrapezoidProfile.generate(constraints, state, goal).sampleAt(DT);
    return new TrapezoidProfile.State(sample.position, sample.velocity);
  }

  @Test
  void constraintsRequirePositiveValues() {
    assertThrows(IllegalArgumentException.class, () -> new TrapezoidProfile.Constraints(0.0, 1.0));
    assertThrows(IllegalArgumentException.class, () -> new TrapezoidProfile.Constraints(1.0, 0.0));
    assertThrows(IllegalArgumentException.class, () -> new TrapezoidProfile.Constraints(-1.0, 1.0));
    assertThrows(IllegalArgumentException.class, () -> new TrapezoidProfile.Constraints(1.0, -1.0));
  }

  @Test
  void checkTiming() {
    TrapezoidProfile.Constraints constraints = new TrapezoidProfile.Constraints(1.75, 0.75);
    TrapezoidProfile.State goal = new TrapezoidProfile.State(12, -1);
    TrapezoidProfile.State state = new TrapezoidProfile.State(0, 1);

    var profile = TrapezoidProfile.generate(constraints, state, goal);

    assertNear(profile.duration(), 9.952380952380953, 1e-10);
  }

  @Test
  void samplesArePhaseTransitions() {
    // Accelerate for 1 s over 0.5 m, cruise for 2 s over 2 m, then decelerate for 1 s over 0.5 m
    var profile =
        TrapezoidProfile.generate(
            new TrapezoidProfile.Constraints(1, 1),
            new TrapezoidProfile.State(0, 0),
            new TrapezoidProfile.State(3, 0));

    var samples = profile.getSamples();
    assertEquals(4, samples.size());

    assertEquals(new TrapezoidProfileSample(0, 0, 0, 1), samples.get(0));
    assertEquals(new TrapezoidProfileSample(1, 0.5, 1, 0), samples.get(1));
    assertEquals(new TrapezoidProfileSample(3, 2.5, 1, -1), samples.get(2));
    assertEquals(new TrapezoidProfileSample(4, 3, 0, 0), samples.get(3));

    // Interpolation within each phase follows that phase's acceleration
    var sample = profile.sampleAt(0.5);
    assertNear(sample.position, 0.125, 1e-12);
    assertNear(sample.velocity, 0.5, 1e-12);
    assertEquals(1.0, sample.acceleration);

    sample = profile.sampleAt(2.0);
    assertNear(sample.position, 1.5, 1e-12);
    assertNear(sample.velocity, 1.0, 1e-12);
    assertEquals(0.0, sample.acceleration);

    sample = profile.sampleAt(3.5);
    assertNear(sample.position, 2.875, 1e-12);
    assertNear(sample.velocity, 0.5, 1e-12);
    assertEquals(-1.0, sample.acceleration);
  }

  @Test
  void reachesGoal() {
    TrapezoidProfile.Constraints constraints = new TrapezoidProfile.Constraints(1.75, 0.75);
    TrapezoidProfile.State goal = new TrapezoidProfile.State(3, 0);
    TrapezoidProfile.State state = new TrapezoidProfile.State();

    for (int i = 0; i < 450; ++i) {
      var newState = step(constraints, state, goal);
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

    for (int i = 0; i < 400; ++i) {
      var newState = step(constraints, state, goal);
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

    int plateauCount = 0;
    // About 7.5s.
    for (int i = 0; i < 1000; ++i) {
      var newState = step(constraints, state, goal);
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

    int plateauCount = 0;
    // About 7.5s.
    for (int i = 0; i < 1000; ++i) {
      var newState = step(constraints, state, goal);
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

    int plateauCount = 0;
    // About 7.5s.
    for (int i = 0; i < 1700; ++i) {
      var newState = step(constraints, state, goal);
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

    int plateauCount = 0;
    for (int i = 0; i < 1700; ++i) {
      var newState = step(constraints, state, goal);
      checkFeasible(state, newState, constraints.maxAcceleration);
      state = newState;
      if (state.velocity == -constraints.maxVelocity) {
        plateauCount++;
      }
    }

    assertLessThanOrEquals(5, plateauCount);

    assertEquals(state, goal);
  }

  // Test the forwards case for displacement equal to the threshold displacement.
  @Test
  void checkSignAtThreshold() {
    TrapezoidProfile.Constraints constraints = new TrapezoidProfile.Constraints(4, 4);
    TrapezoidProfile.State goal = new TrapezoidProfile.State(1, 1);
    TrapezoidProfile.State state = new TrapezoidProfile.State(0, 3);

    // Normal profile is 0.5s, and an incorrect implementation might repeat.
    for (int i = 0; i < 52; ++i) {
      var newState = step(constraints, state, goal);
      checkFeasible(state, newState, constraints.maxAcceleration);
      state = newState;
    }

    // The "chattering" failure mode won't reach the goal.
    assertEquals(state, goal);
  }

  // Test the backwards case for displacement equal to the threshold displacement.
  @Test
  void checkSignAtThresholdBackwards() {
    TrapezoidProfile.Constraints constraints = new TrapezoidProfile.Constraints(4, 4);
    TrapezoidProfile.State goal = new TrapezoidProfile.State(-1, -1);
    TrapezoidProfile.State state = new TrapezoidProfile.State(0, -3);

    // Normal profile is 0.5s, and an incorrect implementation might repeat.
    for (int i = 0; i < 52; ++i) {
      var newState = step(constraints, state, goal);
      checkFeasible(state, newState, constraints.maxAcceleration);
      state = newState;
    }

    // The "chattering" failure mode won't reach the goal.
    assertEquals(state, goal);
  }

  // This is the case that generated a broken profile in the old impl.
  @Test
  void largeVelocityAndSmallPositionDelta() {
    TrapezoidProfile.Constraints constraints = new TrapezoidProfile.Constraints(1.75, 0.75);
    TrapezoidProfile.State goal = new TrapezoidProfile.State(0.01, 0);
    TrapezoidProfile.State state = new TrapezoidProfile.State(0, 1);

    for (int i = 0; i < 450; ++i) {
      var newState = step(constraints, state, goal);
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

    for (int i = 0; i < 700; ++i) {
      var newState = step(constraints, state, goal);
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

    for (int i = 0; i < 200; ++i) {
      var newState = step(constraints, state, goal);
      checkFeasible(state, newState, constraints.maxAcceleration);
      state = newState;
    }
    assertNotEquals(state, goal);

    goal = new TrapezoidProfile.State(0.0, 0.0);
    for (int i = 0; i < 550; ++i) {
      var newState = step(constraints, state, goal);
      checkFeasible(state, newState, constraints.maxAcceleration);
      state = newState;
    }
    assertEquals(state, goal);
  }

  // Checks to make sure that it hits top velocity
  @Test
  void topVelocity() {
    TrapezoidProfile.Constraints constraints = new TrapezoidProfile.Constraints(0.75, 0.75);
    TrapezoidProfile.State goal = new TrapezoidProfile.State(4, 0);
    TrapezoidProfile.State state = new TrapezoidProfile.State();

    for (int i = 0; i < 200; ++i) {
      var newState = step(constraints, state, goal);
      checkFeasible(state, newState, constraints.maxAcceleration);
      state = newState;
    }
    assertNear(constraints.maxVelocity, state.velocity, 10e-5);

    for (int i = 0; i < 2000; ++i) {
      var newState = step(constraints, state, goal);
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

    for (int i = 0; i < 400; i++) {
      var newState = step(constraints, state, goal);
      checkFeasible(state, newState, constraints.maxAcceleration);
      state = newState;
      assertNear(TrapezoidProfile.generate(constraints, state, state).duration(), 0, 2e-2);
    }
  }

  @Test
  void timingToGoal() {
    TrapezoidProfile.Constraints constraints = new TrapezoidProfile.Constraints(0.75, 0.75);
    TrapezoidProfile.State goal = new TrapezoidProfile.State(2, 0);

    TrapezoidProfile.State state = step(constraints, goal, new TrapezoidProfile.State());

    double predictedTimeLeft = TrapezoidProfile.generate(constraints, state, goal).duration();
    boolean reachedGoal = false;
    for (int i = 0; i < 400; i++) {
      var newState = step(constraints, state, goal);
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

    TrapezoidProfile.State state = step(constraints, goal, new TrapezoidProfile.State());

    double predictedTimeLeft = TrapezoidProfile.generate(constraints, state, goal).duration();
    boolean reachedGoal = false;
    for (int i = 0; i < 400; i++) {
      var newState = step(constraints, state, goal);
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

    for (int i = 0; i < 200; ++i) {
      var newState = step(constraints, state, goal);
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

    for (int i = 0; i < 200; ++i) {
      var newState = step(constraints, state, goal);
      checkFeasible(state, newState, constraints.maxAcceleration);
      state = newState;
      assertLessThanOrEquals(Math.abs(state.velocity), Math.abs(constraints.maxVelocity));
    }
  }
}
