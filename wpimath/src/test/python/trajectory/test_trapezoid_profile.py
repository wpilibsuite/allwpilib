# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.


import math

from wpimath import (
    TrapezoidProfile,
    TrapezoidProfileRadiansConstraints,
    TrapezoidProfileSample,
)

DT = 0.01  # 10 ms


def assert_less_than_or_close(val_1, val_2, eps):
    if val_1 <= val_2:
        assert val_1 <= val_2
    else:
        assert math.isclose(val_1, val_2, abs_tol=eps)


def step(constraints, state, goal):
    profile = TrapezoidProfile.generate(constraints, state, goal)
    sample = profile.sample_at(DT)
    return TrapezoidProfile.State(sample.position, sample.velocity)


def assert_feasible(initial, final, max_accel):
    delta_x = final.position - initial.position
    delta_v = final.velocity - initial.velocity
    max_pos_change = abs(initial.velocity) * DT + max_accel / 2.0 * DT * DT
    max_vel_change = max_accel * DT
    assert_less_than_or_close(abs(delta_x), max_pos_change, 1e-10)
    assert_less_than_or_close(abs(delta_v), max_vel_change, 1e-10)


def test_constraints_repr():
    assert repr(TrapezoidProfile.Constraints(1.75, 0.75)) == (
        "TrapezoidProfileConstraints(max_velocity=1.75, max_acceleration=0.75)"
    )
    assert repr(TrapezoidProfileRadiansConstraints(1.75, 0.75)) == (
        "TrapezoidProfileRadiansConstraints(max_velocity=1.75, max_acceleration=0.75)"
    )


def test_timing():
    constraints = TrapezoidProfile.Constraints(1.75, 0.75)
    goal = TrapezoidProfile.State(12.0, -1.0)
    state = TrapezoidProfile.State(0.0, 1.0)

    profile = TrapezoidProfile.generate(constraints, state, goal)
    profile.sample_at(DT)
    profile_time = profile.duration()

    assert math.isclose(profile_time, 9.952380952380953, abs_tol=1e-10)


def test_samples_are_phase_transitions():
    # Accelerate for 1 s over 0.5 m, cruise for 2 s over 2 m, then decelerate
    # for 1 s over 0.5 m
    profile = TrapezoidProfile.generate(
        TrapezoidProfile.Constraints(1.0, 1.0),
        TrapezoidProfile.State(0.0, 0.0),
        TrapezoidProfile.State(3.0, 0.0),
    )

    samples = profile.samples()
    assert len(samples) == 4

    assert samples[0] == TrapezoidProfileSample(0.0, 0.0, 0.0, 1.0)
    assert samples[1] == TrapezoidProfileSample(1.0, 0.5, 1.0, 0.0)
    assert samples[2] == TrapezoidProfileSample(3.0, 2.5, 1.0, -1.0)
    assert samples[3] == TrapezoidProfileSample(4.0, 3.0, 0.0, 0.0)

    # Interpolation within each phase follows that phase's acceleration
    sample = profile.sample_at(0.5)
    assert math.isclose(sample.position, 0.125, abs_tol=1e-12)
    assert math.isclose(sample.velocity, 0.5, abs_tol=1e-12)
    assert sample.acceleration == 1.0

    sample = profile.sample_at(2.0)
    assert math.isclose(sample.position, 1.5, abs_tol=1e-12)
    assert math.isclose(sample.velocity, 1.0, abs_tol=1e-12)
    assert sample.acceleration == 0.0

    sample = profile.sample_at(3.5)
    assert math.isclose(sample.position, 2.875, abs_tol=1e-12)
    assert math.isclose(sample.velocity, 0.5, abs_tol=1e-12)
    assert sample.acceleration == -1.0


def test_reaches_goal():
    constraints = TrapezoidProfile.Constraints(1.75, 0.75)
    goal = TrapezoidProfile.State(3.0, 0.0)
    state = TrapezoidProfile.State()

    for i in range(450):
        new_state = step(constraints, state, goal)
        assert_feasible(state, new_state, constraints.max_acceleration)
        state = new_state
    assert state == goal


def test_backwards():
    constraints = TrapezoidProfile.Constraints(0.75, 0.75)
    goal = TrapezoidProfile.State(-2.0, 0.0)
    state = TrapezoidProfile.State()

    for i in range(400):
        new_state = step(constraints, state, goal)
        assert_feasible(state, new_state, constraints.max_acceleration)
        state = new_state
    assert state == goal


# Test the forwards case for an invalid initial velocity with the profile sign.
def test_large_velocity_same_sign_as_peak():
    constraints = TrapezoidProfile.Constraints(1.75, 0.75)
    goal = TrapezoidProfile.State(12.0, 0.0)
    state = TrapezoidProfile.State(0.0, 3.0)

    plateau_count = 0
    # Profile lasts about 7.5s.
    for i in range(1000):
        new_state = step(constraints, state, goal)
        assert_feasible(state, new_state, constraints.max_acceleration)
        if new_state.velocity == constraints.max_velocity:
            plateau_count += 1
        state = new_state

    # Make sure it plateaued at the correct velocity, not just passed it.
    assert plateau_count > 5

    assert state == goal


# Test the backwards case for an invalid initial velocity with the profile sign.
def test_large_velocity_same_sign_as_peak_backwards():
    constraints = TrapezoidProfile.Constraints(1.75, 0.75)
    goal = TrapezoidProfile.State(-12.0, 0.0)
    state = TrapezoidProfile.State(0.0, -3.0)

    plateau_count = 0
    # Profile lasts about 7.5s.
    for i in range(1000):
        new_state = step(constraints, state, goal)
        assert_feasible(state, new_state, constraints.max_acceleration)
        if new_state.velocity == -constraints.max_velocity:
            plateau_count += 1
        state = new_state

    # Make sure it plateaued at the correct velocity, not just passed it.
    assert plateau_count > 5

    assert state == goal


# Test the forwards case for an invalid initial velocity with the profile sign.
def test_large_velocity_opposite_peak():
    constraints = TrapezoidProfile.Constraints(1.75, 0.75)
    goal = TrapezoidProfile.State(12.0, 0.0)
    state = TrapezoidProfile.State(0.0, -3.0)

    plateau_count = 0
    for i in range(1700):
        new_state = step(constraints, state, goal)
        assert_feasible(state, new_state, constraints.max_acceleration)
        if new_state.velocity == constraints.max_velocity:
            plateau_count += 1
        state = new_state

    # Make sure it plateaued at the correct velocity, not just passed it.
    assert plateau_count > 5

    assert state == goal


# Test the backwards case for an invalid initial velocity with the profile sign.
def test_large_velocity_opposite_peak_backwards():
    constraints = TrapezoidProfile.Constraints(1.75, 0.75)
    goal = TrapezoidProfile.State(-12.0, 0.0)
    state = TrapezoidProfile.State(0.0, 3.0)

    plateau_count = 0
    for i in range(1700):
        new_state = step(constraints, state, goal)
        assert_feasible(state, new_state, constraints.max_acceleration)
        if new_state.velocity == -constraints.max_velocity:
            plateau_count += 1
        state = new_state

    # Make sure it plateaued at the correct velocity, not just passed it.
    assert plateau_count > 5

    assert state == goal


def test_sign_at_threshold():
    constraints = TrapezoidProfile.Constraints(4.0, 4.0)
    goal = TrapezoidProfile.State(1, 1.0)
    state = TrapezoidProfile.State(0.0, 3.0)

    # Normal profile is 0.5s, and an incorrect implementation might repeat.
    for i in range(52):
        new_state = step(constraints, state, goal)
        assert_feasible(state, new_state, constraints.max_acceleration)
        state = new_state

    # The "chattering" failure mode won't reach the goal.
    assert state == goal


def test_sign_at_threshold_backwards():
    constraints = TrapezoidProfile.Constraints(4.0, 4.0)
    goal = TrapezoidProfile.State(-1, -1.0)
    state = TrapezoidProfile.State(0.0, -3.0)

    # Normal profile is 0.5s, and an incorrect implementation might repeat.
    for i in range(52):
        new_state = step(constraints, state, goal)
        assert_feasible(state, new_state, constraints.max_acceleration)
        state = new_state

    # The "chattering" failure mode won't reach the goal.
    assert state == goal


# This is primary case that is broken in the old impl.
def test_large_velocity_and_small_position_delta():
    constraints = TrapezoidProfile.Constraints(1.75, 0.75)
    goal = TrapezoidProfile.State(0.01, 0.0)
    state = TrapezoidProfile.State(0.0, 1.0)

    for i in range(450):
        new_state = step(constraints, state, goal)
        assert_feasible(state, new_state, constraints.max_acceleration)
        state = new_state

    assert state == goal


def test_large_velocity_and_small_position_delta_backwards():
    constraints = TrapezoidProfile.Constraints(1.75, 0.75)
    goal = TrapezoidProfile.State(-0.01, 0.0)
    state = TrapezoidProfile.State(0.0, -2.0)

    for i in range(700):
        new_state = step(constraints, state, goal)
        assert_feasible(state, new_state, constraints.max_acceleration)
        state = new_state

    assert state == goal


def test_switch_goal_in_middle():
    constraints = TrapezoidProfile.Constraints(0.75, 0.75)
    goal = TrapezoidProfile.State(-2.0, 0.0)
    state = TrapezoidProfile.State()

    for _ in range(200):
        new_state = step(constraints, state, goal)
        assert_feasible(state, new_state, constraints.max_acceleration)
        state = new_state
    assert state != goal

    goal = TrapezoidProfile.State(0.0, 0.0)
    for _ in range(550):
        new_state = step(constraints, state, goal)
        assert_feasible(state, new_state, constraints.max_acceleration)
        state = new_state
    assert state == goal


def test_timing_to_current():
    constraints = TrapezoidProfile.Constraints(0.75, 0.75)
    goal = TrapezoidProfile.State(2.0, 0.0)
    state = TrapezoidProfile.State()

    for _ in range(400):
        new_state = step(constraints, state, goal)
        assert_feasible(state, new_state, constraints.max_acceleration)
        state = new_state


def test_timing_to_goal():
    constraints = TrapezoidProfile.Constraints(0.75, 0.75)
    goal = TrapezoidProfile.State(2.0, 0.0)
    state = TrapezoidProfile.State(0.0, 0.0)

    predicted_time_left = TrapezoidProfile.generate(constraints, state, goal).duration()

    reached_goal = False
    for i in range(400):
        new_state = step(constraints, state, goal)
        assert_feasible(state, new_state, constraints.max_acceleration)
        state = new_state
        if not reached_goal and state == goal:
            assert math.isclose(predicted_time_left, i * DT, abs_tol=0.25)
            reached_goal = True


def test_timing_to_negative_goal():
    constraints = TrapezoidProfile.Constraints(0.75, 0.75)
    goal = TrapezoidProfile.State(-2.0, 0.0)
    state = TrapezoidProfile.State(0.0, 0.0)

    predicted_time_left = TrapezoidProfile.generate(constraints, state, goal).duration()

    reached_goal = False
    for i in range(400):
        new_state = step(constraints, state, goal)
        assert_feasible(state, new_state, constraints.max_acceleration)
        state = new_state
        if not reached_goal and state == goal:
            assert math.isclose(predicted_time_left, i * DT, abs_tol=0.25)
            reached_goal = True


def test_goal_velocity_constraints():
    constraints = TrapezoidProfile.Constraints(0.75, 0.75)
    goal = TrapezoidProfile.State(10.0, 5.0)
    state = TrapezoidProfile.State(0.0, 0.75)

    for i in range(1400):
        new_state = step(constraints, state, goal)
        assert_feasible(state, new_state, constraints.max_acceleration)
        state = new_state
        assert abs(state.velocity) <= constraints.max_velocity


def test_negative_goal_velocity_constraints():
    constraints = TrapezoidProfile.Constraints(0.75, 0.75)
    goal = TrapezoidProfile.State(10.0, -5.0)
    state = TrapezoidProfile.State(0.0, 0.75)

    for i in range(1600):
        new_state = step(constraints, state, goal)
        assert_feasible(state, new_state, constraints.max_acceleration)
        state = new_state
        assert abs(state.velocity) <= constraints.max_velocity
