#!/usr/bin/env python3

# Avoid needing display if plots aren't being shown
import os
import sys

if "--noninteractive" in sys.argv:
    import matplotlib as mpl

    mpl.use("svg")

import frccontrol as fct
import math
import matplotlib.pyplot as plt
import numpy as np


class SingleJointedArm(fct.System):

    def __init__(self, dt):
        """Single-jointed arm subsystem.

        Keyword arguments:
        dt -- time between model/controller updates
        """
        state_labels = [("Angle", "rad"), ("Angular velocity", "rad/s")]
        u_labels = [("Voltage", "V")]
        self.set_plot_labels(state_labels, u_labels)

        fct.System.__init__(self, np.zeros((2, 1)), np.array([[-12.0]]),
                            np.array([[12.0]]), dt)

    def create_model(self, states):
        # Number of motors
        num_motors = 1.0
        # Mass of arm in kg
        m = 2.2675
        # Length of arm in m
        l = 1.2192
        # Arm moment of inertia in kg-m^2
        J = 1 / 3 * m * l**2
        # Gear ratio
        G = 1.0 / 2.0

        return fct.models.single_jointed_arm(fct.models.MOTOR_CIM, num_motors,
                                             J, G)

    def design_controller_observer(self):
        q_pos = 0.01745
        q_vel = 0.08726
        self.design_lqr([q_pos, q_vel], [12.0])
        self.design_two_state_feedforward([q_pos, q_vel], [12.0])

        q_pos = 0.01745
        q_vel = 0.1745329
        r_pos = 0.01
        r_vel = 0.01
        self.design_kalman_filter([q_pos, q_vel], [r_pos])


def main():
    dt = 0.00505
    single_jointed_arm = SingleJointedArm(dt)
    single_jointed_arm.export_java_coeffs("SingleJointedArm", True)
    os.rename("SingleJointedArmCoeffs.java", "subsystems/SingleJointedArmCoeffs.java")

    if "--save-plots" in sys.argv or "--noninteractive" not in sys.argv:
        try:
            import slycot

            plt.figure(1)
            single_jointed_arm.plot_pzmaps()
        except ImportError:  # Slycot unavailable. Can't show pzmaps.
            pass
    if "--save-plots" in sys.argv:
        plt.savefig("single_jointed_arm_pzmaps.svg")

    t, xprof, vprof, aprof = fct.generate_s_curve_profile(max_v=0.5,
                                                          max_a=1,
                                                          time_to_max_a=0.5,
                                                          dt=dt,
                                                          goal=1.04)

    # Generate references for simulation
    refs = []
    for i in range(len(t)):
        r = np.array([[xprof[i]], [vprof[i]]])
        refs.append(r)

    if "--save-plots" in sys.argv or "--noninteractive" not in sys.argv:
        plt.figure(2)
        x_rec, ref_rec, u_rec = single_jointed_arm.generate_time_responses(
            t, refs)
        single_jointed_arm.plot_time_responses(t, x_rec, ref_rec, u_rec)
    if "--save-plots" in sys.argv:
        plt.savefig("single_jointed_arm_response.svg")
    if "--noninteractive" not in sys.argv:
        plt.show()


if __name__ == "__main__":
    main()
