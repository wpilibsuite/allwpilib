# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import pytest

from wpimath import ExponentialProfileMeterVolts, SimpleMotorFeedforwardMeters

kDt = 0.01
feedforward = SimpleMotorFeedforwardMeters(0, 2.5629, 0.43277, kDt)
constraints = ExponentialProfileMeterVolts.Constraints.fromCharacteristics(
    12, 2.5629, 0.43277
)


def assert_near(val1, val2, eps):
    assert (
        abs(val1 - val2) <= eps
    ), f"Difference between {val1} and {val2} is greater than {eps}"


def assert_near_state(val1, val2, eps):
    assert_near(val1.position, val2.position, eps)
    assert_near(val1.position, val2.position, eps)


def check_dynamics(
    profile: ExponentialProfileMeterVolts,
    current: ExponentialProfileMeterVolts.State,
    goal: ExponentialProfileMeterVolts.State,
):
    next_state = profile.calculate(kDt, current, goal)

    signal = feedforward.calculate(current.velocity, next_state.velocity)

    assert abs(signal) < constraints.maxInput + 1e-9

    return next_state


@pytest.fixture
def profile():
    return ExponentialProfileMeterVolts(constraints)


def test_reaches_goal(profile):
    goal = ExponentialProfileMeterVolts.State(10, 0)
    state = ExponentialProfileMeterVolts.State(0, 0)

    for _ in range(450):
        state = check_dynamics(profile, state, goal)

    assert state == goal


def test_pos_continuous_under_vel_change(profile):
    goal = ExponentialProfileMeterVolts.State(10, 0)
    state = ExponentialProfileMeterVolts.State(0, 0)

    for i in range(300):
        if i == 150:
            profile = ExponentialProfileMeterVolts(
                ExponentialProfileMeterVolts.Constraints.fromStateSpace(
                    9, constraints.A, constraints.B
                )
            )

        state = check_dynamics(profile, state, goal)

    assert state == goal


def test_pos_continuous_under_vel_change_backward(profile):
    goal = ExponentialProfileMeterVolts.State(-10, 0)
    state = ExponentialProfileMeterVolts.State(0, 0)

    for i in range(300):
        if i == 150:
            profile = ExponentialProfileMeterVolts(
                ExponentialProfileMeterVolts.Constraints.fromStateSpace(
                    9, constraints.A, constraints.B
                )
            )

        state = check_dynamics(profile, state, goal)

    assert state == goal


def test_backwards(profile):
    goal = ExponentialProfileMeterVolts.State(-10, 0)
    state = ExponentialProfileMeterVolts.State(0, 0)

    for _ in range(400):
        state = check_dynamics(profile, state, goal)

    assert state == goal


def test_switch_goal_in_middle(profile):
    goal = ExponentialProfileMeterVolts.State(-10, 0)
    state = ExponentialProfileMeterVolts.State(0, 0)

    for _ in range(50):
        state = check_dynamics(profile, state, goal)

    assert state != goal

    goal = ExponentialProfileMeterVolts.State(0.0, 0.0)
    for _ in range(100):
        state = check_dynamics(profile, state, goal)

    assert state == goal


def test_top_velocity(profile):
    goal = ExponentialProfileMeterVolts.State(40, 0)
    state = ExponentialProfileMeterVolts.State(0, 0)

    max_velocity = 0
    for _ in range(900):
        state = check_dynamics(profile, state, goal)
        max_velocity = max(max_velocity, state.velocity)

    assert_near(constraints.maxVelocity(), max_velocity, 10e-5)
    assert state == goal


def test_top_velocity_backward(profile):
    goal = ExponentialProfileMeterVolts.State(-40, 0)
    state = ExponentialProfileMeterVolts.State(0, 0)

    max_velocity = 0
    for _ in range(900):
        state = check_dynamics(profile, state, goal)
        max_velocity = min(max_velocity, state.velocity)

    assert_near(-constraints.maxVelocity(), max_velocity, 10e-5)
    assert state == goal


def test_large_initial_velocity(profile):
    goal = ExponentialProfileMeterVolts.State(40, 0)
    state = ExponentialProfileMeterVolts.State(0, 8)

    for _ in range(900):
        state = check_dynamics(profile, state, goal)

    assert state == goal


def test_large_negative_initial_velocity(profile):
    goal = ExponentialProfileMeterVolts.State(-40, 0)
    state = ExponentialProfileMeterVolts.State(0, -8)

    for _ in range(900):
        state = check_dynamics(profile, state, goal)

    assert state == goal


class ETestCase:
    def __init__(self, initial, goal, inflection_point):
        self.initial = initial
        self.goal = goal
        self.inflection_point = inflection_point


def test_heuristic(profile):
    test_cases = [
        ETestCase(
            ExponentialProfileMeterVolts.State(0.0, -4),
            ExponentialProfileMeterVolts.State(0.75, -4),
            ExponentialProfileMeterVolts.State(1.3758, 4.4304),
        ),
        ETestCase(
            ExponentialProfileMeterVolts.State(0.0, -4),
            ExponentialProfileMeterVolts.State(1.4103, 4),
            ExponentialProfileMeterVolts.State(1.3758, 4.4304),
        ),
        ETestCase(
            ExponentialProfileMeterVolts.State(0.6603, 4),
            ExponentialProfileMeterVolts.State(0.75, -4),
            ExponentialProfileMeterVolts.State(1.3758, 4.4304),
        ),
        ETestCase(
            ExponentialProfileMeterVolts.State(0.6603, 4),
            ExponentialProfileMeterVolts.State(1.4103, 4),
            ExponentialProfileMeterVolts.State(1.3758, 4.4304),
        ),
        ETestCase(
            ExponentialProfileMeterVolts.State(0.0, -4),
            ExponentialProfileMeterVolts.State(0.5, -2),
            ExponentialProfileMeterVolts.State(0.4367, 3.7217),
        ),
        ETestCase(
            ExponentialProfileMeterVolts.State(0.0, -4),
            ExponentialProfileMeterVolts.State(0.546, 2),
            ExponentialProfileMeterVolts.State(0.4367, 3.7217),
        ),
        ETestCase(
            ExponentialProfileMeterVolts.State(0.6603, 4),
            ExponentialProfileMeterVolts.State(0.5, -2),
            ExponentialProfileMeterVolts.State(0.5560, -2.9616),
        ),
        ETestCase(
            ExponentialProfileMeterVolts.State(0.6603, 4),
            ExponentialProfileMeterVolts.State(0.546, 2),
            ExponentialProfileMeterVolts.State(0.5560, -2.9616),
        ),
        ETestCase(
            ExponentialProfileMeterVolts.State(0.0, -4),
            ExponentialProfileMeterVolts.State(-0.75, -4),
            ExponentialProfileMeterVolts.State(-0.7156, -4.4304),
        ),
        ETestCase(
            ExponentialProfileMeterVolts.State(0.0, -4),
            ExponentialProfileMeterVolts.State(-0.0897, 4),
            ExponentialProfileMeterVolts.State(-0.7156, -4.4304),
        ),
        ETestCase(
            ExponentialProfileMeterVolts.State(0.6603, 4),
            ExponentialProfileMeterVolts.State(-0.75, -4),
            ExponentialProfileMeterVolts.State(-0.7156, -4.4304),
        ),
        ETestCase(
            ExponentialProfileMeterVolts.State(0.6603, 4),
            ExponentialProfileMeterVolts.State(-0.0897, 4),
            ExponentialProfileMeterVolts.State(-0.7156, -4.4304),
        ),
        ETestCase(
            ExponentialProfileMeterVolts.State(0.0, -4),
            ExponentialProfileMeterVolts.State(-0.5, -4.5),
            ExponentialProfileMeterVolts.State(1.095, 4.314),
        ),
        ETestCase(
            ExponentialProfileMeterVolts.State(0.0, -4),
            ExponentialProfileMeterVolts.State(1.0795, 4.5),
            ExponentialProfileMeterVolts.State(-0.5122, -4.351),
        ),
        ETestCase(
            ExponentialProfileMeterVolts.State(0.6603, 4),
            ExponentialProfileMeterVolts.State(-0.5, -4.5),
            ExponentialProfileMeterVolts.State(1.095, 4.314),
        ),
        ETestCase(
            ExponentialProfileMeterVolts.State(0.6603, 4),
            ExponentialProfileMeterVolts.State(1.0795, 4.5),
            ExponentialProfileMeterVolts.State(-0.5122, -4.351),
        ),
        ETestCase(
            ExponentialProfileMeterVolts.State(0.0, -8),
            ExponentialProfileMeterVolts.State(0, 0),
            ExponentialProfileMeterVolts.State(-0.1384, 3.342),
        ),
        ETestCase(
            ExponentialProfileMeterVolts.State(0.0, -8),
            ExponentialProfileMeterVolts.State(-1, 0),
            ExponentialProfileMeterVolts.State(-0.562, -6.792),
        ),
        ETestCase(
            ExponentialProfileMeterVolts.State(0.0, 8),
            ExponentialProfileMeterVolts.State(1, 0),
            ExponentialProfileMeterVolts.State(0.562, 6.792),
        ),
        ETestCase(
            ExponentialProfileMeterVolts.State(0.0, 8),
            ExponentialProfileMeterVolts.State(-1, 0),
            ExponentialProfileMeterVolts.State(-0.785, -4.346),
        ),
    ]

    for test_case in test_cases:
        state = profile.calculateInflectionPoint(test_case.initial, test_case.goal)
        assert_near_state(test_case.inflection_point, state, 1e-3)


def test_timing_to_current(profile):
    goal = ExponentialProfileMeterVolts.State(2, 0)
    state = ExponentialProfileMeterVolts.State(0, 0)

    for _ in range(400):
        state = check_dynamics(profile, state, goal)
        assert_near(profile.timeLeftUntil(state, state), 0, 2e-2)


def test_timing_to_goal(profile):
    goal = ExponentialProfileMeterVolts.State(2, 0)
    state = ExponentialProfileMeterVolts.State(0, 0)

    predicted_time_left = profile.timeLeftUntil(state, goal)
    reached_goal = False
    for _ in range(400):
        state = check_dynamics(profile, state, goal)

        if not reached_goal and state == goal:
            # Expected value using for loop index is just an approximation since
            # the time left in the profile doesn't increase linearly at the
            # endpoints
            assert_near(predicted_time_left, _ / 100.0, 0.25)
            reached_goal = True


def test_timing_to_negative_goal(profile):
    goal = ExponentialProfileMeterVolts.State(-2, 0)
    state = ExponentialProfileMeterVolts.State(0, 0)

    predicted_time_left = profile.timeLeftUntil(state, goal)
    reached_goal = False
    for _ in range(400):
        state = check_dynamics(profile, state, goal)

        if not reached_goal and state == goal:
            # Expected value using for loop index is just an approximation since
            # the time left in the profile doesn't increase linearly at the
            # endpoints
            assert_near(predicted_time_left, _ / 100.0, 0.25)
            reached_goal = True
