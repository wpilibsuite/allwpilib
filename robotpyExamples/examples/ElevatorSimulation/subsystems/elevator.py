#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib
import wpilib.simulation
import wpimath

import constants


class Elevator:
    """Represents the elevator subsystem."""

    def __init__(self) -> None:
        # This gearbox represents a gearbox containing 4 Vex 775pro motors.
        self.elevator_gearbox = wpimath.DCMotor.vex775_pro(4)

        # Standard classes for controlling our elevator
        self.controller = wpimath.ProfiledPIDController(
            constants.ELEVATOR_KP,
            constants.ELEVATOR_KI,
            constants.ELEVATOR_KD,
            wpimath.TrapezoidProfile.Constraints(2.45, 2.45),
        )
        self.feedforward = wpimath.ElevatorFeedforward(
            constants.ELEVATOR_KS,
            constants.ELEVATOR_KG,
            constants.ELEVATOR_KV,
            constants.ELEVATOR_KA,
        )
        self.encoder = wpilib.Encoder(
            constants.ENCODER_A_CHANNEL, constants.ENCODER_B_CHANNEL
        )
        self.motor = wpilib.PWMSparkMax(constants.MOTOR_PORT)

        # Simulation classes help us simulate what's going on, including gravity.
        self.elevator_sim = wpilib.simulation.ElevatorSim(
            self.elevator_gearbox,
            constants.ELEVATOR_GEARING,
            constants.CARRIAGE_MASS,
            constants.ELEVATOR_DRUM_RADIUS,
            constants.MIN_ELEVATOR_HEIGHT,
            constants.MAX_ELEVATOR_HEIGHT,
            True,
            0,
            [0.01, 0.0],
        )
        self.encoder_sim = wpilib.simulation.EncoderSim(self.encoder)
        self.motor_sim = wpilib.simulation.PWMMotorControllerSim(self.motor)

        # Create a Mechanism2d visualization of the elevator
        self.mech2d = wpilib.Mechanism2d(20, 50)
        self.mech2d_root = self.mech2d.get_root("Elevator Root", 10, 0)
        self.elevator_mech2d = self.mech2d_root.append_ligament(
            "Elevator", self.elevator_sim.get_position(), 90
        )

        self.encoder.set_distance_per_pulse(constants.ELEVATOR_ENCODER_DIST_PER_PULSE)

        # Publish the Mechanism2d visualization through telemetry.
        wpilib.Telemetry.log("Elevator Sim", self.mech2d)

    def simulation_periodic(self) -> None:
        # In this method, we update our simulation of what our elevator is doing
        # First, we set our "inputs" (voltages)
        self.elevator_sim.set_input_voltage(
            self.motor_sim.get_throttle() * wpilib.RobotController.get_battery_voltage()
        )

        # Next, we update it. The standard loop time is 20ms.
        self.elevator_sim.update(0.020)

        # Finally, we set our simulated encoder's readings and simulated battery voltage
        self.encoder_sim.set_distance(self.elevator_sim.get_position())
        # SimBattery estimates loaded battery voltages
        wpilib.simulation.RoboRioSim.set_vin_voltage(
            wpilib.simulation.BatterySim.calculate(
                [self.elevator_sim.get_current_draw()]
            )
        )

    def reach_goal(self, goal: float) -> None:
        """Run control loop to reach and maintain goal.

        :param goal: the position to maintain
        """

        self.controller.set_goal(goal)

        # With the setpoint value we run PID control like normal
        pid_output = self.controller.calculate(self.encoder.get_distance())
        feedforward_output = self.feedforward.calculate(
            self.controller.get_setpoint().velocity
        )
        self.motor.set_voltage(pid_output + feedforward_output)

    def stop(self) -> None:
        """Stop the control loop and motor output."""
        self.controller.set_goal(0.0)
        self.motor.set_throttle(0.0)

    def update_telemetry(self) -> None:
        """Update telemetry, including the mechanism visualization."""
        # Update elevator visualization with position
        self.elevator_mech2d.set_length(self.encoder.get_distance())
