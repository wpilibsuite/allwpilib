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
        self.arm_kp = Constants.DEFAULT_ARM_KP
        self.arm_setpoint_degrees = Constants.DEFAULT_ARM_SETPOINT_DEGREES

        # The arm gearbox represents a gearbox containing two Vex 775pro motors.
        self.arm_gearbox = wpimath.DCMotor.vex775_pro(2)

        # Standard classes for controlling our arm
        self.controller = wpimath.PIDController(self.arm_kp, 0, 0)
        self.encoder = wpilib.Encoder(
            Constants.ENCODER_A_CHANNEL, Constants.ENCODER_B_CHANNEL
        )
        self.motor = wpilib.PWMSparkMax(Constants.MOTOR_PORT)

        # Simulation classes help us simulate what's going on, including gravity.
        # This arm sim represents an arm that can travel from -75 degrees (rotated down front)
        # to 255 degrees (rotated down in the back).
        self.arm_sim = wpilib.simulation.SingleJointedArmSim(
            self.arm_gearbox,
            Constants.ARM_REDUCTION,
            wpilib.simulation.SingleJointedArmSim.estimate_moi(
                Constants.ARM_LENGTH, Constants.ARM_MASS
            ),
            Constants.ARM_LENGTH,
            Constants.MIN_ANGLE_RADS,
            Constants.MAX_ANGLE_RADS,
            True,
            0,
            [Constants.ARM_ENCODER_DIST_PER_PULSE, 0.0],
        )
        self.encoder_sim = wpilib.simulation.EncoderSim(self.encoder)

        # Create a Mechanism2d display of an Arm with a fixed ArmTower and moving Arm.
        self.mech2d = wpilib.Mechanism2d(60, 60)
        self.arm_pivot = self.mech2d.get_root("ArmPivot", 30, 30)
        self.arm_tower = self.arm_pivot.append_ligament("ArmTower", 30, -90)
        self.arm_ligament = self.arm_pivot.append_ligament(
            "Arm",
            30,
            wpimath.units.radians_to_degrees(self.arm_sim.get_angle()),
            6,
            wpiutil.Color8Bit(wpiutil.Color.YELLOW),
        )

        # Subsystem constructor.
        self.encoder.set_distance_per_pulse(Constants.ARM_ENCODER_DIST_PER_PULSE)

        # Publish the Mechanism2d visualization through telemetry.
        wpilib.Telemetry.log("Arm Sim", self.mech2d)
        self.arm_tower.set_color(wpiutil.Color8Bit(wpiutil.Color.BLUE))

        # Set the Arm position setpoint and P constant to Preferences if the keys don't already exist
        wpilib.Preferences.init_double(
            Constants.ARM_POSITION_KEY, self.arm_setpoint_degrees
        )
        wpilib.Preferences.init_double(Constants.ARM_P_KEY, self.arm_kp)

    def simulation_periodic(self) -> None:
        # In this method, we update our simulation of what our arm is doing
        # First, we set our "inputs" (voltages)
        self.arm_sim.set_input(
            [self.motor.get_throttle() * wpilib.RobotController.get_battery_voltage()]
        )

        # Next, we update it. The standard loop time is 20ms.
        self.arm_sim.update(0.020)

        # Finally, we set our simulated encoder's readings and simulated battery voltage
        self.encoder_sim.set_distance(self.arm_sim.get_angle())
        # SimBattery estimates loaded battery voltages
        wpilib.simulation.RoboRioSim.set_vin_voltage(
            wpilib.simulation.BatterySim.calculate([self.arm_sim.get_current_draw()])
        )

        # Update the Mechanism Arm angle based on the simulated arm angle
        self.arm_ligament.set_angle(
            wpimath.units.radians_to_degrees(self.arm_sim.get_angle())
        )

    def load_preferences(self) -> None:
        # Read Preferences for Arm setpoint and kP on entering Teleop
        self.arm_setpoint_degrees = wpilib.Preferences.get_double(
            Constants.ARM_POSITION_KEY, self.arm_setpoint_degrees
        )
        if self.arm_kp != wpilib.Preferences.get_double(
            Constants.ARM_P_KEY, self.arm_kp
        ):
            self.arm_kp = wpilib.Preferences.get_double(
                Constants.ARM_P_KEY, self.arm_kp
            )
            self.controller.set_p(self.arm_kp)

    def reach_setpoint(self) -> None:
        pid_output = self.controller.calculate(
            self.encoder.get_distance(),
            wpimath.units.degrees_to_radians(self.arm_setpoint_degrees),
        )
        self.motor.set_voltage(pid_output)

    def stop(self) -> None:
        self.motor.set_throttle(0.0)
