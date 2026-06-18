from ._wpilib import RobotBase
import json
import typing


def getDeployData() -> typing.Optional[typing.Dict[str, str]]:
    """
    Utility function useful for retrieving deploy-related information
    that pyfrc stores with your robot code. The dictionary has the
    following keys:

    * deploy-host
    * deploy-user
    * deploy-date
    * code-path

    If the code is deployed from a git repo, and the git program is in
    your path, the following keys will also be present:

    * git-hash
    * git-desc
    * git-branch

    :returns: None in simulation, or a dictionary
    """
    if not RobotBase.isReal():
        return None

    try:
        with open("/home/systemcore/py/deploy.json") as fp:
            return json.load(fp)
    except FileNotFoundError:
        return {}
