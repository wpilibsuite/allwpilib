// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.List;
import org.junit.jupiter.api.Test;
import org.wpilib.math.controller.SimpleMotorFeedforward;

class ExponentialProfileTest {
  private static final double DT = 0.01;
  private static final SimpleMotorFeedforward feedforward =
      new SimpleMotorFeedforward(0, 2.5629, 0.43277, DT);
  private static final ExponentialProfile.Constraints constraints =
      ExponentialProfile.Constraints.fromCharacteristics(12, 2.5629, 0.43277);

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

  private static void assertNear(
      ExponentialProfile.State val1, ExponentialProfile.State val2, double eps) {
    assertAll(
        () -> assertNear(val1.position, val2.position, eps),
        () -> assertNear(val1.position, val2.position, eps));
  }

  private static ExponentialProfile.State checkDynamics(
      ExponentialProfile.Constraints profileConstraints,
      ExponentialProfile.State current,
      ExponentialProfile.State goal) {
    var sample = ExponentialProfile.generate(profileConstraints, current, goal).sampleAt(DT);
    var next = new ExponentialProfile.State(sample.position, sample.velocity);
    var signal = feedforward.calculate(current.velocity, next.velocity);

    assertTrue(Math.abs(signal) < constraints.maxInput + 1e-9);

    return next;
  }

  @Test
  void reachesGoal() {
    ExponentialProfile.Constraints profileConstraints = constraints;

    ExponentialProfile.State goal = new ExponentialProfile.State(10, 0);
    ExponentialProfile.State state = new ExponentialProfile.State(0, 0);

    for (int i = 0; i < 450; ++i) {
      state = checkDynamics(profileConstraints, state, goal);
    }
    assertEquals(state, goal);
  }

  // Tests that decreasing the maximum velocity in the middle when it is already
  // moving faster than the new max is handled correctly
  @Test
  void posContinuousUnderVelChange() {
    ExponentialProfile.Constraints profileConstraints = constraints;

    ExponentialProfile.State goal = new ExponentialProfile.State(10, 0);
    ExponentialProfile.State state = new ExponentialProfile.State(0, 0);

    for (int i = 0; i < 300; ++i) {
      if (i == 150) {
        profileConstraints =
            ExponentialProfile.Constraints.fromStateSpace(9, constraints.A, constraints.B);
      }

      state = checkDynamics(profileConstraints, state, goal);
    }
    assertEquals(state, goal);
  }

  // Tests that decreasing the maximum velocity in the middle when it is already
  // moving faster than the new max is handled correctly
  @Test
  void posContinuousUnderVelChangeBackward() {
    ExponentialProfile.Constraints profileConstraints = constraints;

    ExponentialProfile.State goal = new ExponentialProfile.State(-10, 0);
    ExponentialProfile.State state = new ExponentialProfile.State(0, 0);

    for (int i = 0; i < 300; ++i) {
      if (i == 150) {
        profileConstraints =
            ExponentialProfile.Constraints.fromStateSpace(9, constraints.A, constraints.B);
      }

      state = checkDynamics(profileConstraints, state, goal);
    }
    assertEquals(state, goal);
  }

  // There is some somewhat tricky code for dealing with going backwards
  @Test
  void backwards() {
    ExponentialProfile.State goal = new ExponentialProfile.State(-10, 0);
    ExponentialProfile.State state = new ExponentialProfile.State(0, 0);

    ExponentialProfile.Constraints profileConstraints = constraints;

    for (int i = 0; i < 400; ++i) {
      state = checkDynamics(profileConstraints, state, goal);
    }
    assertEquals(state, goal);
  }

  @Test
  void switchGoalInMiddle() {
    ExponentialProfile.State goal = new ExponentialProfile.State(-10, 0);
    ExponentialProfile.State state = new ExponentialProfile.State(0, 0);

    ExponentialProfile.Constraints profileConstraints = constraints;
    for (int i = 0; i < 50; ++i) {
      state = checkDynamics(profileConstraints, state, goal);
    }
    assertNotEquals(state, goal);

    goal = new ExponentialProfile.State(0.0, 0.0);
    for (int i = 0; i < 100; ++i) {
      state = checkDynamics(profileConstraints, state, goal);
    }
    assertEquals(state, goal);
  }

  // Checks to make sure that it hits top velocity
  @Test
  void topVelocity() {
    ExponentialProfile.State goal = new ExponentialProfile.State(40, 0);
    ExponentialProfile.State state = new ExponentialProfile.State(0, 0);

    ExponentialProfile.Constraints profileConstraints = constraints;
    double maxVelocity = 0;
    for (int i = 0; i < 900; ++i) {
      state = checkDynamics(profileConstraints, state, goal);
      maxVelocity = Math.max(maxVelocity, state.velocity);
    }

    assertNear(constraints.maxVelocity(), maxVelocity, 10e-5);
    assertEquals(state, goal);
  }

  @Test
  void topVelocityBackward() {
    ExponentialProfile.State goal = new ExponentialProfile.State(-40, 0);
    ExponentialProfile.State state = new ExponentialProfile.State(0, 0);

    ExponentialProfile.Constraints profileConstraints = constraints;
    double maxVelocity = 0;
    for (int i = 0; i < 900; ++i) {
      state = checkDynamics(profileConstraints, state, goal);
      maxVelocity = Math.min(maxVelocity, state.velocity);
    }

    assertNear(-constraints.maxVelocity(), maxVelocity, 10e-5);
    assertEquals(state, goal);
  }

  @Test
  void largeInitialVelocity() {
    ExponentialProfile.State goal = new ExponentialProfile.State(40, 0);
    ExponentialProfile.State state = new ExponentialProfile.State(0, 8);

    ExponentialProfile.Constraints profileConstraints = constraints;
    for (int i = 0; i < 900; ++i) {
      state = checkDynamics(profileConstraints, state, goal);
    }

    assertEquals(state, goal);
  }

  @Test
  void largeNegativeInitialVelocity() {
    ExponentialProfile.State goal = new ExponentialProfile.State(-40, 0);
    ExponentialProfile.State state = new ExponentialProfile.State(0, -8);

    ExponentialProfile.Constraints profileConstraints = constraints;
    for (int i = 0; i < 900; ++i) {
      state = checkDynamics(profileConstraints, state, goal);
    }

    assertEquals(state, goal);
  }

  @Test
  void samplesArePhaseTransitions() {
    var current = new ExponentialProfile.State(0, 0);
    var goal = new ExponentialProfile.State(2, 0);
    var profile = ExponentialProfile.generate(constraints, current, goal);

    var samples = profile.getSamples();
    assertEquals(3, samples.size());

    // The profile applies max input until the inflection point, then max input in the opposite
    // direction until the goal
    var start = samples.get(0);
    assertEquals(0.0, start.time);
    assertEquals(current.position, start.position);
    assertEquals(current.velocity, start.velocity);
    assertEquals(constraints.maxInput, start.input);
    assertNear(start.acceleration, constraints.B * constraints.maxInput, 1e-12);

    var inflection = samples.get(1);
    assertTrue(0.0 < inflection.time && inflection.time < profile.duration());
    assertEquals(-constraints.maxInput, inflection.input);

    var end = samples.get(2);
    assertEquals(new ExponentialProfileSample(profile.duration(), 2, 0, 0, 0), end);

    // Each segment is continuous with the samples at its boundaries
    double eps = 1e-9;
    var beforeInflection = profile.sampleAt(inflection.time - eps);
    assertNear(beforeInflection.position, inflection.position, 1e-6);
    assertNear(beforeInflection.velocity, inflection.velocity, 1e-6);
    var beforeEnd = profile.sampleAt(profile.duration() - eps);
    assertNear(beforeEnd.position, end.position, 1e-6);
    assertNear(beforeEnd.velocity, end.velocity, 1e-6);

    // Interpolated samples obey the system dynamics with the segment's input
    for (double t = 0.0; t < profile.duration(); t += 0.05) {
      var sample = profile.sampleAt(t);
      double u = t < inflection.time ? constraints.maxInput : -constraints.maxInput;
      assertEquals(u, sample.input);
      assertNear(sample.acceleration, constraints.A * sample.velocity + constraints.B * u, 1e-9);
    }
  }

  @Test
  void currentEqualsGoal() {
    var state = new ExponentialProfile.State(1, 2);
    var profile = ExponentialProfile.generate(constraints, state, state);

    assertEquals(0.0, profile.duration());

    var sample = profile.sampleAt(DT);
    assertEquals(state, new ExponentialProfile.State(sample.position, sample.velocity));
    assertEquals(0.0, sample.acceleration);
    assertNear(sample.input, -constraints.A * state.velocity / constraints.B, 1e-12);
  }

  @SuppressWarnings("PMD.TestClassWithoutTestCases")
  static class TestCase {
    public final ExponentialProfile.State initial;
    public final ExponentialProfile.State goal;
    public final ExponentialProfile.State inflectionPoint;

    TestCase(
        ExponentialProfile.State initial,
        ExponentialProfile.State goal,
        ExponentialProfile.State inflectionPoint) {
      this.initial = initial;
      this.goal = goal;
      this.inflectionPoint = inflectionPoint;
    }
  }

  @Test
  void testHeuristic() {
    List<TestCase> testCases =
        List.of(
            new TestCase(
                new ExponentialProfile.State(0.0, -4),
                new ExponentialProfile.State(0.75, -4),
                new ExponentialProfile.State(1.3758, 4.4304)),
            new TestCase(
                new ExponentialProfile.State(0.0, -4),
                new ExponentialProfile.State(1.4103, 4),
                new ExponentialProfile.State(1.3758, 4.4304)),
            new TestCase(
                new ExponentialProfile.State(0.6603, 4),
                new ExponentialProfile.State(0.75, -4),
                new ExponentialProfile.State(1.3758, 4.4304)),
            new TestCase(
                new ExponentialProfile.State(0.6603, 4),
                new ExponentialProfile.State(1.4103, 4),
                new ExponentialProfile.State(1.3758, 4.4304)),
            new TestCase(
                new ExponentialProfile.State(0.0, -4),
                new ExponentialProfile.State(0.5, -2),
                new ExponentialProfile.State(0.4367, 3.7217)),
            new TestCase(
                new ExponentialProfile.State(0.0, -4),
                new ExponentialProfile.State(0.546, 2),
                new ExponentialProfile.State(0.4367, 3.7217)),
            new TestCase(
                new ExponentialProfile.State(0.6603, 4),
                new ExponentialProfile.State(0.5, -2),
                new ExponentialProfile.State(0.5560, -2.9616)),
            new TestCase(
                new ExponentialProfile.State(0.6603, 4),
                new ExponentialProfile.State(0.546, 2),
                new ExponentialProfile.State(0.5560, -2.9616)),
            new TestCase(
                new ExponentialProfile.State(0.0, -4),
                new ExponentialProfile.State(-0.75, -4),
                new ExponentialProfile.State(-0.7156, -4.4304)),
            new TestCase(
                new ExponentialProfile.State(0.0, -4),
                new ExponentialProfile.State(-0.0897, 4),
                new ExponentialProfile.State(-0.7156, -4.4304)),
            new TestCase(
                new ExponentialProfile.State(0.6603, 4),
                new ExponentialProfile.State(-0.75, -4),
                new ExponentialProfile.State(-0.7156, -4.4304)),
            new TestCase(
                new ExponentialProfile.State(0.6603, 4),
                new ExponentialProfile.State(-0.0897, 4),
                new ExponentialProfile.State(-0.7156, -4.4304)),
            new TestCase(
                new ExponentialProfile.State(0.0, -4),
                new ExponentialProfile.State(-0.5, -4.5),
                new ExponentialProfile.State(1.095, 4.314)),
            new TestCase(
                new ExponentialProfile.State(0.0, -4),
                new ExponentialProfile.State(1.0795, 4.5),
                new ExponentialProfile.State(-0.5122, -4.351)),
            new TestCase(
                new ExponentialProfile.State(0.6603, 4),
                new ExponentialProfile.State(-0.5, -4.5),
                new ExponentialProfile.State(1.095, 4.314)),
            new TestCase(
                new ExponentialProfile.State(0.6603, 4),
                new ExponentialProfile.State(1.0795, 4.5),
                new ExponentialProfile.State(-0.5122, -4.351)),
            new TestCase(
                new ExponentialProfile.State(0.0, -8),
                new ExponentialProfile.State(0, 0),
                new ExponentialProfile.State(-0.1384, 3.342)),
            new TestCase(
                new ExponentialProfile.State(0.0, -8),
                new ExponentialProfile.State(-1, 0),
                new ExponentialProfile.State(-0.562, -6.792)),
            new TestCase(
                new ExponentialProfile.State(0.0, 8),
                new ExponentialProfile.State(1, 0),
                new ExponentialProfile.State(0.562, 6.792)),
            new TestCase(
                new ExponentialProfile.State(0.0, 8),
                new ExponentialProfile.State(-1, 0),
                new ExponentialProfile.State(-0.785, -4.346)));

    for (var testCase : testCases) {
      var sample =
          ExponentialProfile.generate(constraints, testCase.initial, testCase.goal)
              .getSamples()
              .get(1);
      var state = new ExponentialProfile.State(sample.position, sample.velocity);
      assertNear(testCase.inflectionPoint, state, 1e-3);
    }
  }

  @Test
  void timingToCurrent() {
    ExponentialProfile.State goal = new ExponentialProfile.State(2, 0);
    ExponentialProfile.State state = new ExponentialProfile.State(0, 0);

    ExponentialProfile.Constraints profileConstraints = constraints;
    for (int i = 0; i < 400; i++) {
      state = checkDynamics(profileConstraints, state, goal);
      assertNear(ExponentialProfile.generate(profileConstraints, state, state).duration(), 0, 2e-2);
    }
  }

  @Test
  void timingToGoal() {
    ExponentialProfile.Constraints profileConstraints = constraints;

    ExponentialProfile.State goal = new ExponentialProfile.State(2, 0);
    ExponentialProfile.State state = new ExponentialProfile.State(0, 0);

    double predictedTimeLeft =
        ExponentialProfile.generate(profileConstraints, state, goal).duration();
    boolean reachedGoal = false;
    for (int i = 0; i < 400; i++) {
      state = checkDynamics(profileConstraints, state, goal);

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
    ExponentialProfile.Constraints profileConstraints = constraints;

    ExponentialProfile.State goal = new ExponentialProfile.State(-2, 0);
    ExponentialProfile.State state = new ExponentialProfile.State(0, 0);

    double predictedTimeLeft =
        ExponentialProfile.generate(profileConstraints, state, goal).duration();
    boolean reachedGoal = false;
    for (int i = 0; i < 400; i++) {
      state = checkDynamics(profileConstraints, state, goal);

      if (!reachedGoal && state.equals(goal)) {
        // Expected value using for loop index is just an approximation since
        // the time left in the profile doesn't increase linearly at the
        // endpoints
        assertNear(predictedTimeLeft, i / 100.0, 0.25);
        reachedGoal = true;
      }
    }
  }
}
