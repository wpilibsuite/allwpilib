#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

from commands2 import Command
from commands2.button import CommandNiDsXboxController
from commands2.sysid import SysIdRoutine

from subsystems.drive import Drive
from subsystems.shooter import Shooter

from constants import OIConstants


class SysIdRoutineBot:
    """This class is where the bulk of the robot should be declared. Since Command-based is a
    "declarative" paradigm, very little robot logic should actually be handled in the :class:`.Robot`
    periodic methods (other than the scheduler calls). Instead, the structure of the robot (including
    subsystems, commands, and button mappings) should be declared here.
    """

    def __init__(self) -> None:
        # The robot's subsystems
        self.drive = Drive()
        self.shooter = Shooter()

        # The driver's controller
        self.controller = CommandNiDsXboxController(OIConstants.DRIVER_CONTROLLER_PORT)

    def configure_bindings(self) -> None:
        """Use this method to define bindings between conditions and commands. These are useful for
        automating robot behaviors based on button and sensor input.

        Should be called in the robot class constructor.

        Event binding methods are available on the :class:`.Trigger` class.
        """

        # Control the drive with split-stick arcade controls
        self.drive.set_default_command(
            self.drive.arcade_drive_command(
                lambda: -self.controller.get_left_y(),
                lambda: -self.controller.get_right_x(),
            )
        )

        # Bind full set of SysId routine tests to buttons; a complete routine should run each of these
        # once.
        # Using bumpers as a modifier and combining it with the buttons so that we can have both sets
        # of bindings at once
        self.controller.a().and_(self.controller.right_bumper()).while_true(
            self.drive.sys_id_quasistatic(SysIdRoutine.Direction.FORWARD)
        )
        self.controller.b().and_(self.controller.right_bumper()).while_true(
            self.drive.sys_id_quasistatic(SysIdRoutine.Direction.REVERSE)
        )
        self.controller.x().and_(self.controller.right_bumper()).while_true(
            self.drive.sys_id_dynamic(SysIdRoutine.Direction.FORWARD)
        )
        self.controller.y().and_(self.controller.right_bumper()).while_true(
            self.drive.sys_id_dynamic(SysIdRoutine.Direction.REVERSE)
        )

        # Control the shooter wheel with the left trigger
        self.shooter.set_default_command(
            self.shooter.run_shooter(self.controller.get_left_trigger_axis)
        )

        self.controller.a().and_(self.controller.left_bumper()).while_true(
            self.shooter.sys_id_quasistatic(SysIdRoutine.Direction.FORWARD)
        )
        self.controller.b().and_(self.controller.left_bumper()).while_true(
            self.shooter.sys_id_quasistatic(SysIdRoutine.Direction.REVERSE)
        )
        self.controller.x().and_(self.controller.left_bumper()).while_true(
            self.shooter.sys_id_dynamic(SysIdRoutine.Direction.FORWARD)
        )
        self.controller.y().and_(self.controller.left_bumper()).while_true(
            self.shooter.sys_id_dynamic(SysIdRoutine.Direction.REVERSE)
        )

    def get_autonomous_command(self) -> Command:
        """Use this to define the command that runs during autonomous.

        Scheduled during :meth:`.Robot.autonomous_init`.
        """

        # Do nothing
        return self.drive.run(lambda: None)
