#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib
import wpilib.simulation
import wpimath
import wpimath.units

import constants


class Elevator:
    def __init__(self) -> None:
        # This gearbox represents a gearbox containing 4 Vex 775pro motors.
        self.elevatorGearbox = wpimath.DCMotor.NEO(2)

        self.profile = wpimath.ExponentialProfileMeterVolts(
            wpimath.ExponentialProfileMeterVolts.Constraints.fromCharacteristics(
                constants.kElevatorMaxV,
                constants.kElevatorkV,
                constants.kElevatorkA,
            )
        )

        self.setpoint = wpimath.ExponentialProfileMeterVolts.State(0, 0)

        # Standard classes for controlling our elevator
        self.pidController = wpimath.PIDController(
            constants.kElevatorKp, constants.kElevatorKi, constants.kElevatorKd
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
            [0.005, 0.0],
        )
        self.encoderSim = wpilib.simulation.EncoderSim(self.encoder)
        self.motorSim = wpilib.simulation.PWMMotorControllerSim(self.motor)

        # Create a Mechanism2d visualization of the elevator
        self.mech2d = wpilib.Mechanism2d(
            wpimath.units.inchesToMeters(10),
            wpimath.units.inchesToMeters(51),
        )
        self.mech2dRoot = self.mech2d.getRoot(
            "Elevator Root",
            wpimath.units.inchesToMeters(5),
            wpimath.units.inchesToMeters(0.5),
        )
        self.elevatorMech2d = self.mech2dRoot.appendLigament(
            "Elevator", self.elevatorSim.getPosition(), 90
        )

        # Subsystem constructor.
        self.encoder.setDistancePerPulse(constants.kElevatorEncoderDistPerPulse)

        # Publish Mechanism2d to SmartDashboard
        # To view the Elevator visualization, select Network Tables -> SmartDashboard -> Elevator Sim
        wpilib.SmartDashboard.putData("Elevator Sim", self.mech2d)

    def simulationPeriodic(self) -> None:
        """Advance the simulation."""
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
        goalState = wpimath.ExponentialProfileMeterVolts.State(goal, 0)

        next_state = self.profile.calculate(0.020, self.setpoint, goalState)

        # With the setpoint value we run PID control like normal
        pidOutput = self.pidController.calculate(
            self.encoder.getDistance(), self.setpoint.position
        )
        feedforwardOutput = self.feedforward.calculate(
            self.setpoint.velocity, next_state.velocity
        )

        self.motor.setVoltage(pidOutput + feedforwardOutput)

        self.setpoint = next_state

    def stop(self) -> None:
        """Stop the control loop and motor output."""
        self.motor.set(0.0)

    def reset(self) -> None:
        """Reset Exponential profile to begin from current position on enable."""
        self.setpoint = wpimath.ExponentialProfileMeterVolts.State(
            self.encoder.getDistance(), 0
        )

    def updateTelemetry(self) -> None:
        """Update telemetry, including the mechanism visualization."""
        # Update elevator visualization with position
        self.elevatorMech2d.setLength(self.encoder.getDistance())
