import hal
import wpilib
import logging
import os.path
import sys
import threading
import time
import typing

import importlib.metadata

if sys.version_info < (3, 10):

    def entry_points(group):
        eps = importlib.metadata.entry_points()
        return eps.get(group, [])

else:
    entry_points = importlib.metadata.entry_points


from .report_error import reportError, reportErrorInternal


def _log_versions(robotpy_version: typing.Optional[str]):
    import wpilib
    import wpilib.deployinfo

    import logging

    data = wpilib.deployinfo.getDeployData()
    if data:
        logger = logging.getLogger("deploy-info")
        logger.info(
            "%s@%s at %s",
            data.get("deploy-user", "<unknown>"),
            data.get("deploy-host", "<unknown>"),
            data.get("deploy-date", "<unknown>"),
        )
        if "git-hash" in data:
            logger.info(
                "- git info: %s (branch=%s)",
                data.get("git-desc", "<unknown>"),
                data.get("git-branch", "<unknown>"),
            )

    logger = logging.getLogger("wpilib")

    if robotpy_version:
        logger.info("RobotPy version %s", robotpy_version)

    logger.info("WPILib version %s", wpilib.__version__)

    if wpilib.RobotBase.isSimulation():
        logger.info("Running with simulated HAL.")

        # check to see if we're on a RoboRIO
        # NOTE: may have false positives, but it should work well enough
        if os.path.exists("/etc/natinst/share/scs_imagemetadata.ini"):
            logger.warning(
                "Running simulation HAL on actual roboRIO! This probably isn't what you want, and will probably cause difficult-to-debug issues!"
            )

    if logger.isEnabledFor(logging.DEBUG):
        versions = {}

        # Log third party versions
        for group in ("robotpylib", "robotpybuild"):
            for entry_point in entry_points(group=group):
                # Don't actually load the entry points -- just print the
                # packages unless we need to load them
                dist = entry_point.dist
                versions[dist.name] = dist.version

        for k, v in versions.items():
            if k != "wpilib":
                logger.debug("%s version %s", k, v)


class Main:
    """
    Executes the robot code using the currently installed HAL (this is probably not what you want unless you're on the roboRIO)
    """

    def __init__(self, parser):
        pass

    def run(self, options, robot_class, **static_options):
        return robot_class.main(robot_class)


class RobotStarter:
    def __init__(self):
        self.logger = logging.getLogger("robotpy")
        self.robot = None
        self.suppressExitWarning = False
        self._robotpy_version = None

    @property
    def robotpy_version(self) -> typing.Optional[str]:
        if not self._robotpy_version:
            try:
                pkg = importlib.metadata.metadata("robotpy")
            except importlib.metadata.PackageNotFoundError:
                pass
            else:
                self._robotpy_version = pkg.get("Version", None)

        return self._robotpy_version

    def run(self, robot_cls: wpilib.RobotBase) -> bool:
        _log_versions(self.robotpy_version)

        retval = False
        if hal.hasMain():
            rval = [False]

            def _start():
                try:
                    rval[0] = self.start(robot_cls)
                finally:
                    hal.exitMain()

            th = threading.Thread(target=_start, name="RobotThread", daemon=True)
            th.start()
            try:
                hal.runMain()
            except KeyboardInterrupt:
                self.logger.exception(
                    "THIS IS NOT AN ERROR: The user hit CTRL-C to kill the robot"
                )
                self.logger.info("Exiting because of keyboard interrupt")

            self.suppressExitWarning = True
            robot = self.robot
            if robot:
                try:
                    robot.endCompetition()
                except:
                    self.logger.warning("endCompetition raised an exception")

            th.join(1)
            if th.is_alive():
                self.logger.warning("robot thread didn't die, crash may occur next!")
            retval = rval[0]
        else:
            retval = self.start(robot_cls)

        from wpilib import RobotBase

        if RobotBase.isSimulation():
            import wpilib.simulation

            wpilib.simulation._simulation._resetMotorSafety()

        return retval

    def start(self, robot_cls: wpilib.RobotBase) -> bool:
        try:
            return self._start(robot_cls)
        except:
            reportErrorInternal(
                "The robot program quit unexpectedly. This is usually due to a code error.\n"
                "The above stacktrace can help determine where the error occurred.\n",
                True,
            )
            return False

    def _start(self, robot_cls: wpilib.RobotBase) -> bool:
        hal.reportUsage("Language", "Python")

        if not wpilib.Notifier.setHALThreadPriority(True, 40):
            reportErrorInternal(
                "Setting HAL Notifier RT priority to 40 failed", isWarning=True
            )

        isSimulation = wpilib.RobotBase.isSimulation()

        # hack: initialize networktables before creating the robot
        #       class, otherwise our logger doesn't get created
        import ntcore

        inst = ntcore.NetworkTableInstance.getDefault()

        # subscribe to "" to force persistent values to progagate to local
        msub = ntcore.MultiSubscriber(inst, [""])

        if not isSimulation:
            inst.startServer("/home/systemcore/networktables.ini")
        else:
            inst.startServer()

        # wait for the NT server to actually start
        for i in range(100):
            if (
                inst.getNetworkMode()
                & ntcore.NetworkTableInstance.NetworkMode.kNetModeStarting
            ) == 0:
                break
            # real sleep since we're waiting for the server, not simulated sleep
            time.sleep(0.010)
        else:
            reportErrorInternal(
                "timed out while waiting for NT server to start", isWarning=True
            )

        wpilib.SmartDashboard.init()

        # Call DriverStation.refreshData() to kick things off
        wpilib.DriverStation.refreshData()

        try:
            self.robot = robot_cls()
        except:
            reportError(
                f"Unhandled exception instantiating robot {robot_cls.__name__}", True
            )
            reportErrorInternal(f"Could not instantiate robot {robot_cls.__name__}!")
            raise

        # TODO: Add a check to see if the user forgot to call super().__init__()
        # if not hasattr(robot, "_RobotBase__initialized"):
        #     logger.error(
        #         "If your robot class has an __init__ function, it must call super().__init__()!"
        #     )
        #     return False

        try:
            self.robot.startCompetition()
        except KeyboardInterrupt:
            self.robot = None
            self.logger.exception(
                "THIS IS NOT AN ERROR: The user hit CTRL-C to kill the robot"
            )
            self.logger.info("Exiting because of keyboard interrupt")
            return True
        except:
            self.robot = None

            reportError("Unhandled exception", True)
            raise
        else:
            self.robot = None
            if self.suppressExitWarning:
                self.logger.info("Robot code exited")
                return True
            else:
                # startCompetition never returns unless exception occurs....
                reportError("Unexpected return from startCompetition() method.", False)
                return False
