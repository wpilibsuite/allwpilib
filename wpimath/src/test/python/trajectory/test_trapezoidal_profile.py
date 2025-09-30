# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.


import math

from wpimath.trajectory import TrapezoidProfile

kDt = 0.01  # 10 ms


def test_reaches_goal():
    constraints = TrapezoidProfile.Constraints(1.75, 0.75)
    goal = TrapezoidProfile.State(3.0, 0.0)
    state = TrapezoidProfile.State()

    profile = TrapezoidProfile(constraints)
    for _ in range(450):
        state = profile.calculate(kDt, state, goal)
    assert state == goal


def test_pos_continuous_under_vel_change():
    constraints = TrapezoidProfile.Constraints(1.75, 0.75)
    goal = TrapezoidProfile.State(12.0, 0.0)
    profile = TrapezoidProfile(constraints)
    state = profile.calculate(kDt, TrapezoidProfile.State(), goal)

    last_pos = state.position
    for i in range(1600):
        if i == 400:
            constraints = TrapezoidProfile.Constraints(0.75, 0.75)
            profile = TrapezoidProfile(constraints)

        state = profile.calculate(kDt, state, goal)
        estimated_vel = (state.position - last_pos) / kDt

        if i >= 400:
            if estimated_vel <= constraints.maxVelocity:
                assert estimated_vel <= constraints.maxVelocity
            else:
                assert math.isclose(
                    estimated_vel, constraints.maxVelocity, abs_tol=1e-4
                )
            assert state.velocity <= constraints.maxVelocity

        last_pos = state.position

    assert state == goal


def test_backwards():
    constraints = TrapezoidProfile.Constraints(0.75, 0.75)
    goal = TrapezoidProfile.State(-2.0, 0.0)
    state = TrapezoidProfile.State()
    profile = TrapezoidProfile(constraints)

    for _ in range(400):
        state = profile.calculate(kDt, state, goal)
    assert state == goal


def test_switch_goal_in_middle():
    constraints = TrapezoidProfile.Constraints(0.75, 0.75)
    goal = TrapezoidProfile.State(-2.0, 0.0)
    state = TrapezoidProfile.State()
    profile = TrapezoidProfile(constraints)

    for _ in range(200):
        state = profile.calculate(kDt, state, goal)
    assert state != goal

    goal = TrapezoidProfile.State(0.0, 0.0)
    profile = TrapezoidProfile(constraints)
    for _ in range(550):
        state = profile.calculate(kDt, state, goal)
    assert state == goal


def test_top_speed():
    constraints = TrapezoidProfile.Constraints(0.75, 0.75)
    goal = TrapezoidProfile.State(4.0, 0.0)
    state = TrapezoidProfile.State()
    profile = TrapezoidProfile(constraints)

    for _ in range(200):
        state = profile.calculate(kDt, state, goal)
    assert math.isclose(constraints.maxVelocity, state.velocity, abs_tol=1e-4)

    profile = TrapezoidProfile(constraints)
    for _ in range(2000):
        state = profile.calculate(kDt, state, goal)
    assert state == goal


def test_timing_to_current():
    constraints = TrapezoidProfile.Constraints(0.75, 0.75)
    goal = TrapezoidProfile.State(2.0, 0.0)
    state = TrapezoidProfile.State()
    profile = TrapezoidProfile(constraints)

    for _ in range(400):
        state = profile.calculate(kDt, state, goal)
        assert math.isclose(profile.timeLeftUntil(state.position), 0.0, abs_tol=0.02)


def test_timing_to_goal():
    constraints = TrapezoidProfile.Constraints(0.75, 0.75)
    goal = TrapezoidProfile.State(2.0, 0.0)
    profile = TrapezoidProfile(constraints)

    state = profile.calculate(kDt, goal, TrapezoidProfile.State())
    predicted_time_left = profile.timeLeftUntil(goal.position)

    reached_goal = False
    for i in range(400):
        state = profile.calculate(kDt, state, goal)
        if not reached_goal and state == goal:
            assert math.isclose(predicted_time_left, i * kDt, abs_tol=0.25)
            reached_goal = True


def test_timing_before_goal():
    constraints = TrapezoidProfile.Constraints(0.75, 0.75)
    goal = TrapezoidProfile.State(2.0, 0.0)
    profile = TrapezoidProfile(constraints)

    state = profile.calculate(kDt, goal, TrapezoidProfile.State())
    predicted_time_left = profile.timeLeftUntil(1.0)

    reached_goal = False
    for i in range(400):
        state = profile.calculate(kDt, state, goal)
        if not reached_goal and abs(state.velocity - 1.0) < 1e-4:
            assert math.isclose(predicted_time_left, i * kDt, abs_tol=0.02)
            reached_goal = True


def test_timing_to_negative_goal():
    constraints = TrapezoidProfile.Constraints(0.75, 0.75)
    goal = TrapezoidProfile.State(-2.0, 0.0)
    profile = TrapezoidProfile(constraints)

    state = profile.calculate(kDt, goal, TrapezoidProfile.State())
    predicted_time_left = profile.timeLeftUntil(goal.position)

    reached_goal = False
    for i in range(400):
        state = profile.calculate(kDt, state, goal)
        if not reached_goal and state == goal:
            assert math.isclose(predicted_time_left, i * kDt, abs_tol=0.25)
            reached_goal = True


def test_timing_before_negative_goal():
    constraints = TrapezoidProfile.Constraints(0.75, 0.75)
    goal = TrapezoidProfile.State(-2.0, 0.0)
    profile = TrapezoidProfile(constraints)

    state = profile.calculate(kDt, goal, TrapezoidProfile.State())
    predicted_time_left = profile.timeLeftUntil(-1.0)

    reached_goal = False
    for i in range(400):
        state = profile.calculate(kDt, state, goal)
        if not reached_goal and abs(state.velocity + 1.0) < 1e-4:
            assert math.isclose(predicted_time_left, i * kDt, abs_tol=0.02)
            reached_goal = True


def test_initialization_of_current_state():
    constraints = TrapezoidProfile.Constraints(1.0, 1.0)
    profile = TrapezoidProfile(constraints)
    assert math.isclose(profile.timeLeftUntil(0.0), 0.0, abs_tol=1e-10)
    assert math.isclose(profile.totalTime(), 0.0, abs_tol=1e-10)
