import inspect
import sys


def run(robot_class, **kwargs):
    """
    ``wpilib.run`` is no longer used. You should run your robot code via one of
    the following methods instead:

    * Windows: ``py -m robotpy [arguments]``
    * Linux/macOS: ``python -m robotpy [arguments]``

    In a virtualenv the ``robotpy`` command can be used directly.
    """

    msg = inspect.cleandoc(inspect.getdoc(run) or "`wpilib.run` is no longer used")
    print(msg, file=sys.stderr)
    sys.exit(1)
