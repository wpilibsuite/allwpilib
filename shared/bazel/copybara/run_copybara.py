import argparse
import dataclasses
import json
import os
import pathlib
import re
import subprocess
from typing import Optional


@dataclasses.dataclass
class CopybaraConfig:
    # Needed to run the additional updates for updating the rdev file
    mostrobotpy_local_repo_path: Optional[str] = None

    # Settings for migrating to a fork that you own
    mostrobotpy_fork_repo: Optional[str] = None
    allwpilib_fork_repo: Optional[str] = None
    robotpy_commandsv2_fork_repo: Optional[str] = None

    # Settings for truth repositories
    mostrobotpy_truth_repo: str = "https://github.com/robotpy/mostrobotpy.git"
    mostrobotpy_truth_branch: str = "2027"

    allwpilib_truth_repo: str = "https://github.com/wpilibsuite/allwpilib.git"
    allwpilib_truth_branch: str = "2027"


def run_copybara(copybara_file: pathlib.Path, migration: str, destination_url: str):
    args = [
        "bazel",
        "run",
        "//:copybara",
        "--",
        "migrate",
        str(copybara_file),
        migration,
        "--force",
        "--git-destination-url",
        destination_url,
        "--git-destination-non-fast-forward",
    ]

    subprocess.check_call(args)


def checkout_branch(auto_delete_branch: bool, branch_name: str):
    """
    This will attempt run a fetch on the repository in the cwd and checkout the given branch.

    If the branch currently exists locally, it will be deleted before the fetch happens.
    """
    # Check if the magic branch exists locally, and if so attempt to delete it
    ret = subprocess.call(["git", "rev-parse", "--verify", branch_name])
    branch_exists = ret == 0
    if branch_exists:
        if not auto_delete_branch:
            ans = input(f"Delete local branch {branch_name}?")
            if ans.lower() != "y":
                raise Exception(
                    f"You must delete your local copy of {branch_name} before the script can finish"
                )

        subprocess.check_call(["git", "branch", "-D", branch_name])

    subprocess.check_call(["git", "fetch", "--all"])
    subprocess.check_call(["git", "checkout", branch_name])


def update_mostrobotpy_rdev(wpilib_bin_version: str, is_development_build: bool):
    with open("rdev.toml") as f:
        contents = f.read()

    artifactory_path = "https://frcmaven.wpi.edu/artifactory/"
    if is_development_build:
        artifactory_path += "development-2027"
    else:
        artifactory_path += "release-2027"

    contents = re.sub(
        'wpilib_bin_version = ".*"',
        f'wpilib_bin_version = "{wpilib_bin_version}"',
        contents,
    )
    contents = re.sub(
        'wpilib_bin_url = ".*"', f'wpilib_bin_url = "{artifactory_path}"', contents
    )

    with open("rdev.toml", "w") as f:
        f.write(contents)

    subprocess.check_call(["./rdev.sh", "update-pyproject", "--commit"])


def allwpilib_to_mostrobotpy(
    copybara_file: pathlib.Path,
    mostrobotpy_local_repository: str,
    mostrobotpy_fork_repo: str,
    wpilib_bin_version: str,
    is_development_build: bool,
    auto_delete_branch: bool,
):
    run_copybara(copybara_file, "allwpilib_to_mostrobotpy", mostrobotpy_fork_repo)

    os.chdir(mostrobotpy_local_repository)
    checkout_branch(auto_delete_branch, "copybara_allwpilib_to_mostrobotpy")
    update_mostrobotpy_rdev(wpilib_bin_version, is_development_build)
    subprocess.check_call(["git", "push", "-f"])


def mostrobotpy_to_allwpilib(copybara_file: pathlib.Path, allwpilib_fork):
    run_copybara(copybara_file, "mostrobotpy_to_allwpilib", allwpilib_fork)


def commandsv2_to_allwpilib(copybara_file: pathlib.Path, allwpilib_fork):
    run_copybara(copybara_file, "commandsv2_to_allwpilib", allwpilib_fork)


def allwpilib_to_commandsv2(copybara_file: pathlib.Path, allwpilib_fork):
    run_copybara(copybara_file, "allwpilib_to_commandsv2", allwpilib_fork)


def load_user_config() -> CopybaraConfig:
    script_dir = pathlib.Path(__file__).parent
    user_config_file = script_dir / ".copybara.json"
    if os.path.exists(user_config_file):
        print(f"Loading user config from '{user_config_file}'")
        with open(user_config_file, "r") as f:
            json_config = json.load(f)
    else:
        print(
            f"No user config present at '{user_config_file}', no defaults will be loaded"
        )
        json_config = {}

    return CopybaraConfig(**json_config)


def main():
    user_config = load_user_config()

    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers(
        dest="migration", required=True, help="Available commands"
    )

    def add_allwpilib_fork_arg(subparser):
        subparser.add_argument(
            "--allwpilib_fork_repo",
            default=user_config.allwpilib_fork_repo,
            help="URL to your github fork of allwpilib that you have write permissions for",
        )

    # allwpilib -> mostrobotpy
    allwpilib_to_mostrobotpy_parser = subparsers.add_parser(
        "allwpilib_to_mostrobotpy",
        help="Pushes changes from the allwpilib mirror to mostrobotpy",
    )
    allwpilib_to_mostrobotpy_parser.add_argument(
        "--wpilib_bin_version",
        required=True,
        type=str,
        help="The wpilib release version as hosted on artifactory",
    )
    allwpilib_to_mostrobotpy_parser.add_argument(
        "--development_build",
        action="store_true",
        help="True if you are upgrading to a development build instead of a release build. Affects where artifacts will be downloaded from.",
    )
    allwpilib_to_mostrobotpy_parser.add_argument(
        "--mostrobotpy_local_repo_path",
        default=user_config.mostrobotpy_local_repo_path,
        help="Path on your local computer to a mostrobotpy clone",
    )
    allwpilib_to_mostrobotpy_parser.add_argument(
        "--mostrobotpy_fork_repo",
        default=user_config.mostrobotpy_fork_repo,
        help="URL to your github fork of mostrobotpy that you have write permissions for",
    )
    allwpilib_to_mostrobotpy_parser.add_argument(
        "-y",
        "--auto_delete_branch",
        action="store_true",
        help="If present, will automatically delete the local version of the copybara branch if it exists. Otherwise you will be prompted if it is ok to delete",
    )

    # mostrobotpy -> allwpilib
    mostrobotpy_to_allwpilib_parser = subparsers.add_parser(
        "mostrobotpy_to_allwpilib",
        help="Pulls changes from the mostrobotpy source of truth to this mirror",
    )
    add_allwpilib_fork_arg(mostrobotpy_to_allwpilib_parser)

    # allwpilib -> commands-v2
    allwpilib_to_commandsv2_parser = subparsers.add_parser(
        "allwpilib_to_commandsv2",
        help="Pushes changes from the allwpilib mirror to the robotpy commands-v2 repo",
    )
    allwpilib_to_commandsv2_parser.add_argument(
        "--robotpy_commandsv2_fork_repo",
        default=user_config.robotpy_commandsv2_fork_repo,
        help="URL to your github fork of mostrobotpy that you have write permissions for",
    )

    # commands-v2 -> allwpilib
    commandsv2_to_allwpilib_parser = subparsers.add_parser(
        "commandsv2_to_allwpilib",
        help="Pulls changes from the robotpy commands-v2 source of truth into this mirror",
    )
    add_allwpilib_fork_arg(commandsv2_to_allwpilib_parser)

    script_dir = pathlib.Path(__file__).parent
    copybara_file = script_dir / "../../../copy.bara.sky"

    args = parser.parse_args()

    if args.migration == "allwpilib_to_mostrobotpy":
        if args.mostrobotpy_local_repo_path is None:
            raise Exception(
                "You mist specify mostrobotpy_local_repo_path, either on the command line or in your user config"
            )
        if args.mostrobotpy_fork_repo is None:
            raise Exception(
                "You mist specify mostrobotpy_fork_repo, either on the command line or in your user config"
            )
        allwpilib_to_mostrobotpy(
            copybara_file,
            args.mostrobotpy_local_repo_path,
            args.mostrobotpy_fork_repo,
            args.wpilib_bin_version,
            args.development_build,
            args.auto_delete_branch,
        )
    elif args.migration == "mostrobotpy_to_allwpilib":
        if args.allwpilib_fork_repo is None:
            raise Exception(
                "You mist specify allwpilib_fork_repo, either on the command line or in your user config"
            )
        mostrobotpy_to_allwpilib(copybara_file, args.allwpilib_fork_repo)
    elif args.migration == "allwpilib_to_commandsv2":
        if args.robotpy_commandsv2_fork_repo is None:
            raise Exception(
                "You mist specify robotpy_commandsv2_fork_repo, either on the command line or in your user config"
            )
        allwpilib_to_commandsv2(copybara_file, args.robotpy_commandsv2_fork_repo)
    elif args.migration == "commandsv2_to_allwpilib":
        if args.allwpilib_fork_repo is None:
            raise Exception(
                "You mist specify allwpilib_fork_repo, either on the command line or in your user config"
            )
        commandsv2_to_allwpilib(copybara_file, args.allwpilib_fork_repo)
    else:
        raise Exception(f"Unexpected migration {args.migration}")


if __name__ == "__main__":
    main()
