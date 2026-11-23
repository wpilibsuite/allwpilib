# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import pytest

from wpimath import ExponentialProfile, ExponentialProfileSample, SimpleMotorFeedforward

DT = 0.01
feedforward = SimpleMotorFeedforward(0, 2.5629, 0.43277, DT)
constraints = ExponentialProfile.Constraints.from_characteristics(12, 2.5629, 0.43277)


def assert_near(val1, val2, eps):
    assert (
        abs(val1 - val2) <= eps
    ), f"Difference between {val1} and {val2} is greater than {eps}"


def assert_near_state(val1, val2, eps):
    assert_near(val1.position, val2.position, eps)
    assert_near(val1.position, val2.position, eps)


def check_dynamics(
    profile_constraints: ExponentialProfile.Constraints,
    current: ExponentialProfile.State,
    goal: ExponentialProfile.State,
):
    sample = ExponentialProfile.generate(profile_constraints, current, goal).sample_at(
        DT
    )
    next_state = ExponentialProfile.State(sample.position, sample.velocity)

    signal = feedforward.calculate(current.velocity, next_state.velocity)

    assert abs(signal) < constraints.max_input + 1e-9

    return next_state


@pytest.fixture
def profile_constraints():
    return constraints


def test_reaches_goal(profile_constraints):
    goal = ExponentialProfile.State(10, 0)
    state = ExponentialProfile.State(0, 0)

    for _ in range(450):
        state = check_dynamics(profile_constraints, state, goal)

    assert state == goal


def test_pos_continuous_under_vel_change(profile_constraints):
    goal = ExponentialProfile.State(10, 0)
    state = ExponentialProfile.State(0, 0)

    for i in range(300):
        if i == 150:
            profile_constraints = ExponentialProfile.Constraints.from_state_space(
                9, constraints.a, constraints.b
            )

        state = check_dynamics(profile_constraints, state, goal)

    assert state == goal


def test_pos_continuous_under_vel_change_backward(profile_constraints):
    goal = ExponentialProfile.State(-10, 0)
    state = ExponentialProfile.State(0, 0)

    for i in range(300):
        if i == 150:
            profile_constraints = ExponentialProfile.Constraints.from_state_space(
                9, constraints.a, constraints.b
            )

        state = check_dynamics(profile_constraints, state, goal)

    assert state == goal


def test_backwards(profile_constraints):
    goal = ExponentialProfile.State(-10, 0)
    state = ExponentialProfile.State(0, 0)

    for _ in range(400):
        state = check_dynamics(profile_constraints, state, goal)

    assert state == goal


def test_switch_goal_in_middle(profile_constraints):
    goal = ExponentialProfile.State(-10, 0)
    state = ExponentialProfile.State(0, 0)

    for _ in range(50):
        state = check_dynamics(profile_constraints, state, goal)

    assert state != goal

    goal = ExponentialProfile.State(0.0, 0.0)
    for _ in range(100):
        state = check_dynamics(profile_constraints, state, goal)

    assert state == goal


def test_top_velocity(profile_constraints):
    goal = ExponentialProfile.State(40, 0)
    state = ExponentialProfile.State(0, 0)

    max_velocity = 0
    for _ in range(900):
        state = check_dynamics(profile_constraints, state, goal)
        max_velocity = max(max_velocity, state.velocity)

    assert_near(constraints.max_velocity(), max_velocity, 10e-5)
    assert state == goal


def test_top_velocity_backward(profile_constraints):
    goal = ExponentialProfile.State(-40, 0)
    state = ExponentialProfile.State(0, 0)

    max_velocity = 0
    for _ in range(900):
        state = check_dynamics(profile_constraints, state, goal)
        max_velocity = min(max_velocity, state.velocity)

    assert_near(-constraints.max_velocity(), max_velocity, 10e-5)
    assert state == goal


def test_large_initial_velocity(profile_constraints):
    goal = ExponentialProfile.State(40, 0)
    state = ExponentialProfile.State(0, 8)

    for _ in range(900):
        state = check_dynamics(profile_constraints, state, goal)

    assert state == goal


def test_large_negative_initial_velocity(profile_constraints):
    goal = ExponentialProfile.State(-40, 0)
    state = ExponentialProfile.State(0, -8)

    for _ in range(900):
        state = check_dynamics(profile_constraints, state, goal)

    assert state == goal


class ETestCase:
    def __init__(self, initial, goal, inflection_point):
        self.initial = initial
        self.goal = goal
        self.inflection_point = inflection_point


def test_heuristic(profile_constraints):
    test_cases = [
        ETestCase(
            ExponentialProfile.State(0.0, -4),
            ExponentialProfile.State(0.75, -4),
            ExponentialProfile.State(1.3758, 4.4304),
        ),
        ETestCase(
            ExponentialProfile.State(0.0, -4),
            ExponentialProfile.State(1.4103, 4),
            ExponentialProfile.State(1.3758, 4.4304),
        ),
        ETestCase(
            ExponentialProfile.State(0.6603, 4),
            ExponentialProfile.State(0.75, -4),
            ExponentialProfile.State(1.3758, 4.4304),
        ),
        ETestCase(
            ExponentialProfile.State(0.6603, 4),
            ExponentialProfile.State(1.4103, 4),
            ExponentialProfile.State(1.3758, 4.4304),
        ),
        ETestCase(
            ExponentialProfile.State(0.0, -4),
            ExponentialProfile.State(0.5, -2),
            ExponentialProfile.State(0.4367, 3.7217),
        ),
        ETestCase(
            ExponentialProfile.State(0.0, -4),
            ExponentialProfile.State(0.546, 2),
            ExponentialProfile.State(0.4367, 3.7217),
        ),
        ETestCase(
            ExponentialProfile.State(0.6603, 4),
            ExponentialProfile.State(0.5, -2),
            ExponentialProfile.State(0.5560, -2.9616),
        ),
        ETestCase(
            ExponentialProfile.State(0.6603, 4),
            ExponentialProfile.State(0.546, 2),
            ExponentialProfile.State(0.5560, -2.9616),
        ),
        ETestCase(
            ExponentialProfile.State(0.0, -4),
            ExponentialProfile.State(-0.75, -4),
            ExponentialProfile.State(-0.7156, -4.4304),
        ),
        ETestCase(
            ExponentialProfile.State(0.0, -4),
            ExponentialProfile.State(-0.0897, 4),
            ExponentialProfile.State(-0.7156, -4.4304),
        ),
        ETestCase(
            ExponentialProfile.State(0.6603, 4),
            ExponentialProfile.State(-0.75, -4),
            ExponentialProfile.State(-0.7156, -4.4304),
        ),
        ETestCase(
            ExponentialProfile.State(0.6603, 4),
            ExponentialProfile.State(-0.0897, 4),
            ExponentialProfile.State(-0.7156, -4.4304),
        ),
        ETestCase(
            ExponentialProfile.State(0.0, -4),
            ExponentialProfile.State(-0.5, -4.5),
            ExponentialProfile.State(1.095, 4.314),
        ),
        ETestCase(
            ExponentialProfile.State(0.0, -4),
            ExponentialProfile.State(1.0795, 4.5),
            ExponentialProfile.State(-0.5122, -4.351),
        ),
        ETestCase(
            ExponentialProfile.State(0.6603, 4),
            ExponentialProfile.State(-0.5, -4.5),
            ExponentialProfile.State(1.095, 4.314),
        ),
        ETestCase(
            ExponentialProfile.State(0.6603, 4),
            ExponentialProfile.State(1.0795, 4.5),
            ExponentialProfile.State(-0.5122, -4.351),
        ),
        ETestCase(
            ExponentialProfile.State(0.0, -8),
            ExponentialProfile.State(0, 0),
            ExponentialProfile.State(-0.1384, 3.342),
        ),
        ETestCase(
            ExponentialProfile.State(0.0, -8),
            ExponentialProfile.State(-1, 0),
            ExponentialProfile.State(-0.562, -6.792),
        ),
        ETestCase(
            ExponentialProfile.State(0.0, 8),
            ExponentialProfile.State(1, 0),
            ExponentialProfile.State(0.562, 6.792),
        ),
        ETestCase(
            ExponentialProfile.State(0.0, 8),
            ExponentialProfile.State(-1, 0),
            ExponentialProfile.State(-0.785, -4.346),
        ),
    ]

    for test_case in test_cases:
        state = ExponentialProfile.generate(
            profile_constraints, test_case.initial, test_case.goal
        ).samples()[1]
        assert_near_state(test_case.inflection_point, state, 1e-3)


def test_timing_to_current(profile_constraints):
    goal = ExponentialProfile.State(2, 0)
    state = ExponentialProfile.State(0, 0)

    for _ in range(400):
        state = check_dynamics(profile_constraints, state, goal)
        assert_near(
            ExponentialProfile.generate(profile_constraints, state, state).duration(),
            0,
            2e-2,
        )


def test_timing_to_goal(profile_constraints):
    goal = ExponentialProfile.State(2, 0)
    state = ExponentialProfile.State(0, 0)

    predicted_time_left = ExponentialProfile.generate(
        profile_constraints, state, goal
    ).duration()
    reached_goal = False
    for _ in range(400):
        state = check_dynamics(profile_constraints, state, goal)

        if not reached_goal and state == goal:
            # Expected value using for loop index is just an approximation since
            # the time left in the profile doesn't increase linearly at the
            # endpoints
            assert_near(predicted_time_left, _ / 100.0, 0.25)
            reached_goal = True


def test_timing_to_negative_goal(profile_constraints):
    goal = ExponentialProfile.State(-2, 0)
    state = ExponentialProfile.State(0, 0)

    predicted_time_left = ExponentialProfile.generate(
        profile_constraints, state, goal
    ).duration()
    reached_goal = False
    for _ in range(400):
        state = check_dynamics(profile_constraints, state, goal)

        if not reached_goal and state == goal:
            # Expected value using for loop index is just an approximation since
            # the time left in the profile doesn't increase linearly at the
            # endpoints
            assert_near(predicted_time_left, _ / 100.0, 0.25)
            reached_goal = True


def test_samples_are_phase_transitions():
    current = ExponentialProfile.State(0, 0)
    goal = ExponentialProfile.State(2, 0)
    profile = ExponentialProfile.generate(constraints, current, goal)

    samples = profile.samples()
    assert len(samples) == 3

    # The profile applies max input until the inflection point, then max input in
    # the opposite direction until the goal
    start = samples[0]
    assert start.time == 0.0
    assert start.position == current.position
    assert start.velocity == current.velocity
    assert start.input == constraints.max_input
    assert_near(start.acceleration, constraints.b * constraints.max_input, 1e-12)

    inflection = samples[1]
    assert 0.0 < inflection.time < profile.duration()
    assert inflection.input == -constraints.max_input

    end = samples[2]
    assert end == ExponentialProfileSample(profile.duration(), 2.0, 0.0, 0.0, 0.0)

    # Each segment is continuous with the samples at its boundaries
    eps = 1e-9
    before_inflection = profile.sample_at(inflection.time - eps)
    assert_near(before_inflection.position, inflection.position, 1e-6)
    assert_near(before_inflection.velocity, inflection.velocity, 1e-6)
    before_end = profile.sample_at(profile.duration() - eps)
    assert_near(before_end.position, end.position, 1e-6)
    assert_near(before_end.velocity, end.velocity, 1e-6)

    # Interpolated samples obey the system dynamics with the segment's input
    t = 0.0
    while t < profile.duration():
        sample = profile.sample_at(t)
        u = constraints.max_input if t < inflection.time else -constraints.max_input
        assert sample.input == u
        assert_near(
            sample.acceleration,
            constraints.a * sample.velocity + constraints.b * u,
            1e-9,
        )
        t += 0.05


def test_current_equals_goal():
    state = ExponentialProfile.State(1, 2)
    profile = ExponentialProfile.generate(constraints, state, state)

    assert profile.duration() == 0.0

    sample = profile.sample_at(DT)
    assert ExponentialProfile.State(sample.position, sample.velocity) == state
    assert sample.acceleration == 0.0
    assert_near(sample.input, -constraints.a * state.velocity / constraints.b, 1e-12)
