import argparse
import importlib.metadata
import os
import sys
import typing

import wpilib


if sys.version_info < (3, 10):

    def entry_points(group):
        eps = importlib.metadata.entry_points()
        return eps.get(group, [])

else:
    entry_points = importlib.metadata.entry_points


def _int_env_default(name: str, fallback: int) -> int:
    value = os.environ.get(name)
    if value is None:
        return fallback
    try:
        return int(value)
    except ValueError:
        return fallback


class RunXrp:
    """
    Runs the robot using the HAL simulator connected to an XRP platform
    """

    def __init__(self, parser: argparse.ArgumentParser):
        parser.add_argument(
            "--nogui",
            default=False,
            action="store_true",
            help="Don't use the WPILib simulation gui",
        )

        sim_group = parser.add_argument_group("Additional simulation extensions")
        self.simexts = {}

        for entry_point in entry_points(group="robotpy_sim.2027"):
            try:
                sim_ext_module = entry_point.load()
            except ImportError:
                print(f"WARNING: Error detected in {entry_point}")
                continue

            self.simexts[entry_point.name] = sim_ext_module

            try:
                if entry_point.name == "xrp":
                    cmd_help = argparse.SUPPRESS
                else:
                    cmd_help = importlib.metadata.metadata(entry_point.dist.name)[
                        "summary"
                    ]
            except AttributeError:
                cmd_help = "Load specified simulation extension"
            sim_group.add_argument(
                f"--{entry_point.name}",
                default=False,
                action="store_true",
                help=cmd_help,
            )

        parser.add_argument(
            "--host",
            default=os.environ.get("HALSIMXRP_HOST", "192.168.42.1"),
            help="XRP host (default: HALSIMXRP_HOST or 192.168.42.1)",
        )
        parser.add_argument(
            "--port",
            type=int,
            default=_int_env_default("HALSIMXRP_PORT", 3540),
            help="XRP port (default: HALSIMXRP_PORT or 3540)",
        )

    def run(
        self,
        options: argparse.Namespace,
        project_path: "os.PathLike[str]",
        robot_class: typing.Type[wpilib.RobotBase],
    ):
        if "xrp" not in self.simexts:
            print(
                "robotpy-xrp HALSim extension is missing. Reinstall robotpy-xrp to use run-xrp.",
                file=sys.stderr,
            )
            return False

        os.environ["HALSIMXRP_HOST"] = options.host
        os.environ["HALSIMXRP_PORT"] = str(options.port)

        options.xrp = True

        if not options.nogui:
            try:
                import halsim_gui
            except ImportError:
                print("robotpy-halsim-gui is not installed!", file=sys.stderr)
                return False
            else:
                halsim_gui.loadExtension()

        cwd = os.getcwd()

        for name, module in self.simexts.items():
            if getattr(options, name.replace("-", "_"), False):
                try:
                    module.loadExtension()
                except Exception:
                    print(f"Error loading {name}!", file=sys.stderr)
                    raise

        os.chdir(cwd)
        return robot_class.main(robot_class)
