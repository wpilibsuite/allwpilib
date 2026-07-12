# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import math
import random

from wpimath import SimulatedAnnealing


def clamp(v, minval, maxval):
    return max(min(v, maxval), minval)


def test_double_function_optimization_heartbeat():
    def function(x):
        return -(x + math.sin(x)) * math.exp(-x * x) + 1

    step_size = 10.0

    simulated_annealing = SimulatedAnnealing(
        2.0, lambda x: clamp(x + (random.random() - 0.5) * step_size, -3, 3), function
    )

    solution = simulated_annealing.solve(-1.0, 5000)

    assert math.isclose(solution, 0.68, abs_tol=1e-1)


def test_double_function_optimization_multimodal():
    def function(x):
        return math.sin(x) + math.sin((10.0 / 3.0) * x)

    step_size = 10.0

    simulated_annealing = SimulatedAnnealing(
        2.0, lambda x: clamp(x + (random.random() - 0.5) * step_size, 0, 7), function
    )

    solution = simulated_annealing.solve(-1.0, 5000)

    assert math.isclose(solution, 5.146, abs_tol=1e-1)
