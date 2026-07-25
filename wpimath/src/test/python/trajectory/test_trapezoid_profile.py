# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.


import math

from wpimath import TrapezoidProfile

DT = 0.01  # 10 ms


def assert_less_than_or_close(val_1, val_2, eps):
    if val_1 <= val_2:
        assert val_1 <= val_2
    else:
        assert math.isclose(val_1, val_2, abs_tol=eps)


def assert_feasible(initial, final, max_accel):
    delta_x = final.position - initial.position
    delta_v = final.velocity - initial.velocity
    max_pos_change = abs(initial.velocity) * DT + max_accel / 2.0 * DT * DT
    max_vel_change = max_accel * DT
    assert_less_than_or_close(abs(delta_x), max_pos_change, 1e-10)
    assert_less_than_or_close(abs(delta_v), max_vel_change, 1e-10)


def test_timing():
    constraints = TrapezoidProfile.Constraints(1.75, 0.75)
    goal = TrapezoidProfile.State(12.0, -1.0)
    state = TrapezoidProfile.State(0.0, 1.0)

    profile = TrapezoidProfile(constraints)
    profile.calculate(DT, state, goal)
    profile_time = profile.duration()

    assert math.isclose(profile_time, 9.952380952380953, abs_tol=1e-10)
    assert math.isclose(
        profile_time, profile.time_left_until(state, goal), abs_tol=1e-10
    )
    profile.time_left_until(goal, goal)
    assert math.isclose(profile_time, profile.duration(), abs_tol=1e-10)


def test_reaches_goal():
    constraints = TrapezoidProfile.Constraints(1.75, 0.75)
    goal = TrapezoidProfile.State(3.0, 0.0)
    state = TrapezoidProfile.State()

    profile = TrapezoidProfile(constraints)
    for i in range(450):
        new_state = profile.calculate(DT, state, goal)
        assert_feasible(state, new_state, constraints.max_acceleration)
        state = new_state
    assert state == goal


def test_backwards():
    constraints = TrapezoidProfile.Constraints(0.75, 0.75)
    goal = TrapezoidProfile.State(-2.0, 0.0)
    state = TrapezoidProfile.State()
    profile = TrapezoidProfile(constraints)

    for i in range(400):
        new_state = profile.calculate(DT, state, goal)
        assert_feasible(state, new_state, constraints.max_acceleration)
        state = new_state
    assert state == goal


# Test the forwards case for an invalid initial velocity with the profile sign.
def test_large_velocity_same_sign_as_peak():
    constraints = TrapezoidProfile.Constraints(1.75, 0.75)
    goal = TrapezoidProfile.State(12.0, 0.0)
    state = TrapezoidProfile.State(0.0, 3.0)
    profile = TrapezoidProfile(constraints)

    plateau_count = 0
    # Profile lasts about 7.5s.
    for i in range(1000):
        new_state = profile.calculate(DT, state, goal)
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
    profile = TrapezoidProfile(constraints)

    plateau_count = 0
    # Profile lasts about 7.5s.
    for i in range(1000):
        new_state = profile.calculate(DT, state, goal)
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
    profile = TrapezoidProfile(constraints)

    plateau_count = 0
    for i in range(1700):
        new_state = profile.calculate(DT, state, goal)
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
    profile = TrapezoidProfile(constraints)

    plateau_count = 0
    for i in range(1700):
        new_state = profile.calculate(DT, state, goal)
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
    profile = TrapezoidProfile(constraints)

    # Normal profile is 0.5s, and an incorrect implementation might repeat.
    for i in range(52):
        new_state = profile.calculate(DT, state, goal)
        assert_feasible(state, new_state, constraints.max_acceleration)
        state = new_state

    # The "chattering" failure mode won't reach the goal.
    assert state == goal


def test_sign_at_threshold_backwards():
    constraints = TrapezoidProfile.Constraints(4.0, 4.0)
    goal = TrapezoidProfile.State(-1, -1.0)
    state = TrapezoidProfile.State(0.0, -3.0)
    profile = TrapezoidProfile(constraints)

    # Normal profile is 0.5s, and an incorrect implementation might repeat.
    for i in range(52):
        new_state = profile.calculate(DT, state, goal)
        assert_feasible(state, new_state, constraints.max_acceleration)
        state = new_state

    # The "chattering" failure mode won't reach the goal.
    assert state == goal


# This is primary case that is broken in the old impl.
def test_large_velocity_and_small_position_delta():
    constraints = TrapezoidProfile.Constraints(1.75, 0.75)
    goal = TrapezoidProfile.State(0.01, 0.0)
    state = TrapezoidProfile.State(0.0, 1.0)
    profile = TrapezoidProfile(constraints)

    for i in range(450):
        new_state = profile.calculate(DT, state, goal)
        assert_feasible(state, new_state, constraints.max_acceleration)
        state = new_state

    assert state == goal


def test_large_velocity_and_small_position_delta_backwards():
    constraints = TrapezoidProfile.Constraints(1.75, 0.75)
    goal = TrapezoidProfile.State(-0.01, 0.0)
    state = TrapezoidProfile.State(0.0, -2.0)
    profile = TrapezoidProfile(constraints)

    for i in range(700):
        new_state = profile.calculate(DT, state, goal)
        assert_feasible(state, new_state, constraints.max_acceleration)
        state = new_state

    assert state == goal


def test_switch_goal_in_middle():
    constraints = TrapezoidProfile.Constraints(0.75, 0.75)
    goal = TrapezoidProfile.State(-2.0, 0.0)
    state = TrapezoidProfile.State()
    profile = TrapezoidProfile(constraints)

    for _ in range(200):
        new_state = profile.calculate(DT, state, goal)
        assert_feasible(state, new_state, constraints.max_acceleration)
        state = new_state
    assert state != goal

    goal = TrapezoidProfile.State(0.0, 0.0)
    profile = TrapezoidProfile(constraints)
    for _ in range(550):
        new_state = profile.calculate(DT, state, goal)
        assert_feasible(state, new_state, constraints.max_acceleration)
        state = new_state
    assert state == goal


def test_timing_to_current():
    constraints = TrapezoidProfile.Constraints(0.75, 0.75)
    goal = TrapezoidProfile.State(2.0, 0.0)
    state = TrapezoidProfile.State()
    profile = TrapezoidProfile(constraints)

    for _ in range(400):
        new_state = profile.calculate(DT, state, goal)
        assert_feasible(state, new_state, constraints.max_acceleration)
        state = new_state
        assert math.isclose(profile.time_left_until(state, state), 0.0, abs_tol=0.02)


def test_timing_to_goal():
    constraints = TrapezoidProfile.Constraints(0.75, 0.75)
    goal = TrapezoidProfile.State(2.0, 0.0)
    state = TrapezoidProfile.State(0.0, 0.0)
    profile = TrapezoidProfile(constraints)

    predicted_time_left = profile.time_left_until(state, goal)

    reached_goal = False
    for i in range(400):
        new_state = profile.calculate(DT, state, goal)
        assert_feasible(state, new_state, constraints.max_acceleration)
        state = new_state
        if not reached_goal and state == goal:
            assert math.isclose(predicted_time_left, i * DT, abs_tol=0.25)
            reached_goal = True


def test_timing_to_negative_goal():
    constraints = TrapezoidProfile.Constraints(0.75, 0.75)
    goal = TrapezoidProfile.State(-2.0, 0.0)
    state = TrapezoidProfile.State(0.0, 0.0)
    profile = TrapezoidProfile(constraints)

    predicted_time_left = profile.time_left_until(state, goal)

    reached_goal = False
    for i in range(400):
        new_state = profile.calculate(DT, state, goal)
        assert_feasible(state, new_state, constraints.max_acceleration)
        state = new_state
        if not reached_goal and state == goal:
            assert math.isclose(predicted_time_left, i * DT, abs_tol=0.25)
            reached_goal = True


def test_goal_velocity_constraints():
    constraints = TrapezoidProfile.Constraints(0.75, 0.75)
    goal = TrapezoidProfile.State(10.0, 5.0)
    state = TrapezoidProfile.State(0.0, 0.75)
    profile = TrapezoidProfile(constraints)

    for i in range(1400):
        new_state = profile.calculate(DT, state, goal)
        assert_feasible(state, new_state, constraints.max_acceleration)
        state = new_state
        assert abs(state.velocity) <= constraints.max_velocity


def test_negative_goal_velocity_constraints():
    constraints = TrapezoidProfile.Constraints(0.75, 0.75)
    goal = TrapezoidProfile.State(10.0, -5.0)
    state = TrapezoidProfile.State(0.0, 0.75)
    profile = TrapezoidProfile(constraints)

    for i in range(1600):
        new_state = profile.calculate(DT, state, goal)
        assert_feasible(state, new_state, constraints.max_acceleration)
        state = new_state
        assert abs(state.velocity) <= constraints.max_velocity
