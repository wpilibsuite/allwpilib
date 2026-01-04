import os
import argparse
import importlib.metadata
import logging
import sys
import typing

import wpilib


logger = logging.getLogger("robot.sim")


class RobotSim:
    """
    Runs the robot in simulation mode
    """

    def __init__(self, parser: argparse.ArgumentParser):
        parser.add_argument(
            "--nogui",
            default=False,
            action="store_true",
            help="Don't use the WPIlib simulation gui",
        )

        self.simexts = {}

        for entry_point in importlib.metadata.entry_points(group="robotpy_sim.2027"):
            try:
                sim_ext_module = entry_point.load()
            except ImportError:
                print(f"WARNING: Error detected in {entry_point}", file=sys.stderr)
                continue

            self.simexts[entry_point.name] = sim_ext_module

            try:
                cmd_help = importlib.metadata.metadata(entry_point.dist.name)["summary"]
            except AttributeError:
                cmd_help = "Load specified simulation extension"
            parser.add_argument(
                f"--{entry_point.name}",
                default=False,
                action="store_true",
                help=cmd_help,
            )

    def run(
        self,
        options: argparse.Namespace,
        nogui: bool,
        robot_class: typing.Type[wpilib.RobotBase],
    ):
        if not nogui:
            try:
                import halsim_gui
            except ImportError:
                print("robotpy-halsim-gui is not installed!", file=sys.stderr)
                exit(1)
            else:
                halsim_gui.loadExtension()

        # Some extensions (gui) changes the current directory
        cwd = os.getcwd()

        for name, module in self.simexts.items():
            if getattr(options, name.replace("-", "_"), False):
                try:
                    module.loadExtension()
                except:
                    print(f"Error loading {name}!", file=sys.stderr)
                    raise

        os.chdir(cwd)

        # run the robot
        return robot_class.main(robot_class)
