#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib
import wpilib.simulation
import wpimath
import wpimath.units
import wpiutil

from constants import Constants


class Arm:
    def __init__(self) -> None:
        # The P gain for the PID controller that drives this arm.
        self.armKp = Constants.kDefaultArmKp
        self.armSetpointDegrees = Constants.kDefaultArmSetpointDegrees

        # The arm gearbox represents a gearbox containing two Vex 775pro motors.
        self.armGearbox = wpimath.DCMotor.vex775Pro(2)

        # Standard classes for controlling our arm
        self.controller = wpimath.PIDController(self.armKp, 0, 0)
        self.encoder = wpilib.Encoder(
            Constants.kEncoderAChannel, Constants.kEncoderBChannel
        )
        self.motor = wpilib.PWMSparkMax(Constants.kMotorPort)

        # Simulation classes help us simulate what's going on, including gravity.
        # This arm sim represents an arm that can travel from -75 degrees (rotated down front)
        # to 255 degrees (rotated down in the back).
        self.armSim = wpilib.simulation.SingleJointedArmSim(
            self.armGearbox,
            Constants.kArmReduction,
            wpilib.simulation.SingleJointedArmSim.estimateMOI(
                Constants.kArmLength, Constants.kArmMass
            ),
            Constants.kArmLength,
            Constants.kMinAngleRads,
            Constants.kMaxAngleRads,
            True,
            0,
            [Constants.kArmEncoderDistPerPulse, 0.0],
        )
        self.encoderSim = wpilib.simulation.EncoderSim(self.encoder)

        # Create a Mechanism2d display of an Arm with a fixed ArmTower and moving Arm.
        self.mech2d = wpilib.Mechanism2d(60, 60)
        self.armPivot = self.mech2d.getRoot("ArmPivot", 30, 30)
        self.armTower = self.armPivot.appendLigament("ArmTower", 30, -90)
        self.armLigament = self.armPivot.appendLigament(
            "Arm",
            30,
            wpimath.units.radiansToDegrees(self.armSim.getAngle()),
            6,
            wpiutil.Color8Bit(wpiutil.Color.kYellow),
        )

        # Subsystem constructor.
        self.encoder.setDistancePerPulse(Constants.kArmEncoderDistPerPulse)

        # Put Mechanism 2d to SmartDashboard
        wpilib.SmartDashboard.putData("Arm Sim", self.mech2d)
        self.armTower.setColor(wpiutil.Color8Bit(wpiutil.Color.kBlue))

        # Set the Arm position setpoint and P constant to Preferences if the keys don't already exist
        wpilib.Preferences.initDouble(
            Constants.kArmPositionKey, self.armSetpointDegrees
        )
        wpilib.Preferences.initDouble(Constants.kArmPKey, self.armKp)

    def simulationPeriodic(self) -> None:
        # In this method, we update our simulation of what our arm is doing
        # First, we set our "inputs" (voltages)
        self.armSim.setInput(
            [self.motor.get() * wpilib.RobotController.getBatteryVoltage()]
        )

        # Next, we update it. The standard loop time is 20ms.
        self.armSim.update(0.020)

        # Finally, we set our simulated encoder's readings and simulated battery voltage
        self.encoderSim.setDistance(self.armSim.getAngle())
        # SimBattery estimates loaded battery voltages
        wpilib.simulation.RoboRioSim.setVInVoltage(
            wpilib.simulation.BatterySim.calculate([self.armSim.getCurrentDraw()])
        )

        # Update the Mechanism Arm angle based on the simulated arm angle
        self.armLigament.setAngle(
            wpimath.units.radiansToDegrees(self.armSim.getAngle())
        )

    def loadPreferences(self) -> None:
        # Read Preferences for Arm setpoint and kP on entering Teleop
        self.armSetpointDegrees = wpilib.Preferences.getDouble(
            Constants.kArmPositionKey, self.armSetpointDegrees
        )
        if self.armKp != wpilib.Preferences.getDouble(Constants.kArmPKey, self.armKp):
            self.armKp = wpilib.Preferences.getDouble(Constants.kArmPKey, self.armKp)
            self.controller.setP(self.armKp)

    def reachSetpoint(self) -> None:
        pidOutput = self.controller.calculate(
            self.encoder.getDistance(),
            wpimath.units.degreesToRadians(self.armSetpointDegrees),
        )
        self.motor.setVoltage(pidOutput)

    def stop(self) -> None:
        self.motor.set(0.0)
