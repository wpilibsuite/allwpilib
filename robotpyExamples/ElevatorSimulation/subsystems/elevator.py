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
        self.elevatorGearbox = wpimath.DCMotor.vex775Pro(4)

        # Standard classes for controlling our elevator
        self.controller = wpimath.ProfiledPIDController(
            constants.kElevatorKp,
            constants.kElevatorKi,
            constants.kElevatorKd,
            wpimath.TrapezoidProfile.Constraints(2.45, 2.45),
        )
        self.feedforward = wpimath.ElevatorFeedforward(
            constants.kElevatorkS,
            constants.kElevatorkG,
            constants.kElevatorkV,
            constants.kElevatorkA,
        )
        self.encoder = wpilib.Encoder(
            constants.kEncoderAChannel, constants.kEncoderBChannel
        )
        self.motor = wpilib.PWMSparkMax(constants.kMotorPort)

        # Simulation classes help us simulate what's going on, including gravity.
        self.elevatorSim = wpilib.simulation.ElevatorSim(
            self.elevatorGearbox,
            constants.kElevatorGearing,
            constants.kCarriageMass,
            constants.kElevatorDrumRadius,
            constants.kMinElevatorHeight,
            constants.kMaxElevatorHeight,
            True,
            0,
            [0.01, 0.0],
        )
        self.encoderSim = wpilib.simulation.EncoderSim(self.encoder)
        self.motorSim = wpilib.simulation.PWMMotorControllerSim(self.motor)

        # Create a Mechanism2d visualization of the elevator
        self.mech2d = wpilib.Mechanism2d(20, 50)
        self.mech2dRoot = self.mech2d.getRoot("Elevator Root", 10, 0)
        self.elevatorMech2d = self.mech2dRoot.appendLigament(
            "Elevator", self.elevatorSim.getPosition(), 90
        )

        self.encoder.setDistancePerPulse(constants.kElevatorEncoderDistPerPulse)

        # Publish Mechanism2d to SmartDashboard
        # To view the Elevator visualization, select Network Tables -> SmartDashboard -> Elevator Sim
        wpilib.SmartDashboard.putData("Elevator Sim", self.mech2d)

    def simulationPeriodic(self) -> None:
        # In this method, we update our simulation of what our elevator is doing
        # First, we set our "inputs" (voltages)
        self.elevatorSim.setInputVoltage(
            self.motorSim.getVelocity() * wpilib.RobotController.getBatteryVoltage()
        )

        # Next, we update it. The standard loop time is 20ms.
        self.elevatorSim.update(0.020)

        # Finally, we set our simulated encoder's readings and simulated battery voltage
        self.encoderSim.setDistance(self.elevatorSim.getPosition())
        # SimBattery estimates loaded battery voltages
        wpilib.simulation.RoboRioSim.setVInVoltage(
            wpilib.simulation.BatterySim.calculate([self.elevatorSim.getCurrentDraw()])
        )

    def reachGoal(self, goal: float) -> None:
        """Run control loop to reach and maintain goal.

        :param goal: the position to maintain
        """

        self.controller.setGoal(goal)

        # With the setpoint value we run PID control like normal
        pidOutput = self.controller.calculate(self.encoder.getDistance())
        feedforwardOutput = self.feedforward.calculate(
            self.controller.getSetpoint().velocity
        )
        self.motor.setVoltage(pidOutput + feedforwardOutput)

    def stop(self) -> None:
        """Stop the control loop and motor output."""
        self.controller.setGoal(0.0)
        self.motor.set(0.0)

    def updateTelemetry(self) -> None:
        """Update telemetry, including the mechanism visualization."""
        # Update elevator visualization with position
        self.elevatorMech2d.setLength(self.encoder.getDistance())
