# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.


import math

from wpimath import TrapezoidProfile

kDt = 0.01  # 10 ms

def assert_less_than_or_close(val1, val2, eps):
    if val1 <= val2:
        assert val1 <= val2
    else:
        assert math.isclose(val1, val2, abs_tol=eps)


def assert_feasible(initial, final, maxAccel):
    deltaX = final.position - initial.position
    deltaV = final.velocity - initial.velocity
    maxPosChange = abs(initial.velocity) * kDt + maxAccel / 2.0 * kDt * kDt
    maxVelChange = maxAccel * kDt
    assert_less_than_or_close(abs(deltaX), maxPosChange, 1e-10)
    assert_less_than_or_close(abs(deltaV), maxVelChange, 1e-10)

def test_timing():
    constraints = TrapezoidProfile.Constraints(1.75, 0.75)
    goal = TrapezoidProfile.State(12.0, -1.0)
    state = TrapezoidProfile.State(0.0, 1.0)

    profile = TrapezoidProfile(constraints)
    profile.calculate(kDt, state, goal)
    profileTime = profile.totalTime()

    assert math.isclose(profileTime, 9.952380952380953, abs_tol=1e-10)
    assert math.isclose(profileTime, profile.timeLeftUntil(state, goal), abs_tol=1e-10)
    profile.timeLeftUntil(goal, goal)
    assert math.isclose(profileTime, profile.totalTime(), abs_tol=1e-10)

def test_reaches_goal():
    constraints = TrapezoidProfile.Constraints(1.75, 0.75)
    goal = TrapezoidProfile.State(3.0, 0.0)
    state = TrapezoidProfile.State()

    profile = TrapezoidProfile(constraints)
    for _ in range(450):
        newState = profile.calculate(kDt, state, goal)
        assert_feasible(state, newState, constraints.maxAcceleration)
        state = newState
    assert state == goal

def test_backwards():
    constraints = TrapezoidProfile.Constraints(0.75, 0.75)
    goal = TrapezoidProfile.State(-2.0, 0.0)
    state = TrapezoidProfile.State()
    profile = TrapezoidProfile(constraints)

    for _ in range(400):
        newState = profile.calculate(kDt, state, goal)
        assert_feasible(state, newState, constraints.maxAcceleration)
        state = newState
    assert state == goal

# Test the forwards case for an invalid initial velocity with the profile sign.
def test_large_velocity_same_sign_as_peak():
    constraints = TrapezoidProfile.Constraints(1.75, 0.75)
    goal = TrapezoidProfile.State(12.0, 0.0)
    state = TrapezoidProfile.State(0.0, 3.0)
    profile = TrapezoidProfile(constraints)

    plateauCount = 0
    # Profile lasts about 7.5s.
    for i in range(1000):
        newState = profile.calculate(kDt, state, goal)
        assert_feasible(state, newState, constraints.maxAcceleration)
        if newState.velocity == constraints.maxVelocity:
            plateauCount += 1
        state = newState

    # Make sure it plateaued at the correct velocity, not just passed it.
    assert plateauCount > 5

    assert state == goal

# Test the backwards case for an invalid initial velocity with the profile sign.
def test_large_velocity_same_sign_as_peak_backwards():
    constraints = TrapezoidProfile.Constraints(1.75, 0.75)
    goal = TrapezoidProfile.State(-12.0, 0.0)
    state = TrapezoidProfile.State(0.0, -3.0)
    profile = TrapezoidProfile(constraints)

    plateauCount = 0
    # Profile lasts about 7.5s.
    for i in range(1000):
        newState = profile.calculate(kDt, state, goal)
        assert_feasible(state, newState, constraints.maxAcceleration)
        if newState.velocity == -constraints.maxVelocity:
            plateauCount += 1
        state = newState

    # Make sure it plateaued at the correct velocity, not just passed it.
    assert plateauCount > 5

    assert state == goal

# Test the forwards case for an invalid initial velocity with the profile sign.
def test_large_velocity_opposite_peak():
    constraints = TrapezoidProfile.Constraints(1.75, 0.75)
    goal = TrapezoidProfile.State(12.0, 0.0)
    state = TrapezoidProfile.State(0.0, -3.0)
    profile = TrapezoidProfile(constraints)

    plateauCount = 0
    for i in range(1700):
        newState = profile.calculate(kDt, state, goal)
        assert_feasible(state, newState, constraints.maxAcceleration)
        if newState.velocity == constraints.maxVelocity:
            plateauCount += 1
        state = newState

    # Make sure it plateaued at the correct velocity, not just passed it.
    assert plateauCount > 5

    assert state == goal

# Test the backwards case for an invalid initial velocity with the profile sign.
def test_large_velocity_opposite_peak_backwards():
    constraints = TrapezoidProfile.Constraints(1.75, 0.75)
    goal = TrapezoidProfile.State(-12.0, 0.0)
    state = TrapezoidProfile.State(0.0, 3.0)
    profile = TrapezoidProfile(constraints)

    plateauCount = 0
    for i in range(1700):
        newState = profile.calculate(kDt, state, goal)
        assert_feasible(state, newState, constraints.maxAcceleration)
        if newState.velocity == -constraints.maxVelocity:
            plateauCount += 1
        state = newState

    # Make sure it plateaued at the correct velocity, not just passed it.
    assert plateauCount > 5

    assert state == goal

def test_sign_at_threshold():
    constraints = TrapezoidProfile.Constraints(4.0, 4.0)
    goal = TrapezoidProfile.State(0.375, 1.0)
    state = TrapezoidProfile.State(0.0, 2.0)
    profile = TrapezoidProfile(constraints)

    # Normal profile is 0.25s long. Some failure modes are multiples of that.
    for i in range(90):
        newState = profile.calculate(kDt, state, goal)
        assert_feasible(state, newState, constraints.maxAcceleration)
        state = newState

    # The "chattering" failure mode won't reach the goal.
    assert state == goal

def test_sign_at_threshold_backwards():
    constraints = TrapezoidProfile.Constraints(4.0, 4.0)
    goal = TrapezoidProfile.State(-0.375, -1.0)
    state = TrapezoidProfile.State(0.0, -2.0)
    profile = TrapezoidProfile(constraints)

    # Normal profile is 0.25s long. Some failure modes are multiples of that.
    for i in range(90):
        newState = profile.calculate(kDt, state, goal)
        assert_feasible(state, newState, constraints.maxAcceleration)
        state = newState

    # The "chattering" failure mode won't reach the goal.
    assert state == goal

# This is primary case that is broken in the old impl.
def test_large_velocity_and_small_position_delta():
    constraints = TrapezoidProfile.Constraints(1.75, 0.75)
    goal = TrapezoidProfile.State(0.01, 0.0)
    state = TrapezoidProfile.State(0.0, 1.0)
    profile = TrapezoidProfile(constraints)

    for i in range(450):
        newState = profile.calculate(kDt, state, goal)
        assert_feasible(state, newState, constraints.maxAcceleration)
        state = newState

    assert state == goal

def test_large_velocity_and_small_position_delta_backwards():
    constraints = TrapezoidProfile.Constraints(1.75, 0.75)
    goal = TrapezoidProfile.State(-0.01, 0.0)
    state = TrapezoidProfile.State(0.0, -2.0)
    profile = TrapezoidProfile(constraints)

    for i in range(700):
        newState = profile.calculate(kDt, state, goal)
        assert_feasible(state, newState, constraints.maxAcceleration)
        state = newState

    assert state == goal

def test_switch_goal_in_middle():
    constraints = TrapezoidProfile.Constraints(0.75, 0.75)
    goal = TrapezoidProfile.State(-2.0, 0.0)
    state = TrapezoidProfile.State()
    profile = TrapezoidProfile(constraints)

    for _ in range(200):
        newState = profile.calculate(kDt, state, goal)
        assert_feasible(state, newState, constraints.maxAcceleration)
        state = newState
    assert state != goal

    goal = TrapezoidProfile.State(0.0, 0.0)
    profile = TrapezoidProfile(constraints)
    for _ in range(550):
        newState = profile.calculate(kDt, state, goal)
        assert_feasible(state, newState, constraints.maxAcceleration)
        state = newState
    assert state == goal


def test_top_speed():
    constraints = TrapezoidProfile.Constraints(0.75, 0.75)
    goal = TrapezoidProfile.State(4.0, 0.0)
    state = TrapezoidProfile.State()
    profile = TrapezoidProfile(constraints)

    for _ in range(200):
        newState = profile.calculate(kDt, state, goal)
        assert_feasible(state, newState, constraints.maxAcceleration)
        state = newState
    assert math.isclose(constraints.maxVelocity, state.velocity, abs_tol=1e-4)

    profile = TrapezoidProfile(constraints)
    for _ in range(2000):
        newState = profile.calculate(kDt, state, goal)
        assert_feasible(state, newState, constraints.maxAcceleration)
        state = newState
    assert state == goal


def test_timing_to_current():
    constraints = TrapezoidProfile.Constraints(0.75, 0.75)
    goal = TrapezoidProfile.State(2.0, 0.0)
    state = TrapezoidProfile.State()
    profile = TrapezoidProfile(constraints)

    for _ in range(400):
        newState = profile.calculate(kDt, state, goal)
        assert_feasible(state, newState, constraints.maxAcceleration)
        state = newState
        assert math.isclose(profile.timeLeftUntil(state, state), 0.0, abs_tol=0.02)

def test_timing_to_goal():
    constraints = TrapezoidProfile.Constraints(0.75, 0.75)
    goal = TrapezoidProfile.State(2.0, 0.0)
    state = TrapezoidProfile.State(0.0, 0.0)
    profile = TrapezoidProfile(constraints)

    predicted_time_left = profile.timeLeftUntil(state, goal)

    reached_goal = False
    for i in range(400):
        newState = profile.calculate(kDt, state, goal)
        assert_feasible(state, newState, constraints.maxAcceleration)
        state = newState
        if not reached_goal and state == goal:
            assert math.isclose(predicted_time_left, i * kDt, abs_tol=0.25)
            reached_goal = True

def test_timing_to_negative_goal():
    constraints = TrapezoidProfile.Constraints(0.75, 0.75)
    goal = TrapezoidProfile.State(-2.0, 0.0)
    state = TrapezoidProfile.State(0.0, 0.0)
    profile = TrapezoidProfile(constraints)

    predicted_time_left = profile.timeLeftUntil(state, goal)

    reached_goal = False
    for i in range(400):
        newState = profile.calculate(kDt, state, goal)
        assert_feasible(state, newState, constraints.maxAcceleration)
        state = newState
        if not reached_goal and state == goal:
            assert math.isclose(predicted_time_left, i * kDt, abs_tol=0.25)
            reached_goal = True

def test_goal_velocity_constraints():
    constraints = TrapezoidProfile.Constraints(0.75, 0.75)
    goal = TrapezoidProfile.State(10.0, 5.0)
    state = TrapezoidProfile.State(0.0, 0.75)
    profile = TrapezoidProfile(constraints)

    for i in range(1400):
        newState = profile.calculate(kDt, state, goal)
        assert_feasible(state, newState, constraints.maxAcceleration)
        state = newState
        assert abs(state.velocity) <= constraints.maxVelocity

def test_negative_goal_velocity_constraints():
    constraints = TrapezoidProfile.Constraints(0.75, 0.75)
    goal = TrapezoidProfile.State(10.0, -5.0)
    state = TrapezoidProfile.State(0.0, 0.75)
    profile = TrapezoidProfile(constraints)

    for i in range(1600):
        newState = profile.calculate(kDt, state, goal)
        assert_feasible(state, newState, constraints.maxAcceleration)
        state = newState
        assert abs(state.velocity) <= constraints.maxVelocity
