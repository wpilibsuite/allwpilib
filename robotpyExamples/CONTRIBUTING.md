Guidelines for porting WPILib examples to Python
================================================

To ensure that our examples are helpful and accurate for those learning how to
use RobotPy, we have a set of guidelines for adding new examples to the project.
These guidelines are not strictly enforced, but we do ask that you follow them
when submitting pull requests with new examples. This will make the review
process easier for everyone involved.

In general, our examples are based on the Java examples from allwpilib, as Java
is often easier to translate to Python. However, not all of our existing
examples adhere to all of these guidelines. If you see an opportunity to improve
an existing example, feel free to make the necessary changes.

Shorter thoughts
----------------

Testing:

* New examples must run! You *must* test your code on either a robot or in
  simulation. If there's something broken in RobotPy, file an issue to get it
  fixed
  * You can find instructions on how to test a vision file [here](https://robotpy.readthedocs.io/en/stable/vision/other.html#vision-other-runcustom)!
* Format your code with black

General:

* We always try to stay as close to the original examples as possible
* `Main.java` is never needed
* Don't ever check in files for your IDE (.vscode, .idea, etc)
* Copy over the copyright statement from the original file

Naming:

* Filenames should always be all lowercase
* Function names are camelCase
* Class names start with a capital letter
* Class method names are camelCase
* Class member variables such as `m_name` should be `self.name` in Python
* Protected/private methods/members can optionally be prefixed with `_`

Misc conversion thoughts

* Comparisons to null such as `foo == null` become `foo is None`
* Single-line lambdas can be converted to python lambda statements. Anything
  longer needs to be a separate function somewhere
* Never modify `sys.path` directly!

Longer thoughts
---------------

Never initialize anything other than constants at class/global scope. Here are
a few examples:

```python

# OK: just a constant
MY_CONSTANT = 42

# BAD: at global scope
motor = wpilib.Talon(1)

class MyRobot:
    # BAD: at class scope
    motor = wpilib.Talon(1)

    def __init__(self):
        # OK: variable assigned to `self`
        self.motor = wpilib.Talon(1)
```

---

Import order doesn't really matter, but we prefer the following convention:

```python

# Import things from the python standard library first
import os
import typing

# Import things from robotpy in a second group
import wpilib
import commands2

# Import things from the other files in the example last
import constants
import subsystems.drivetrain

```

---

The `pass` statement is only required for empty functions:


```python
# OK
def empty_function():
    pass

def has_docstring():
    """Some docstring"""
    pass # NOT NEEDED

class C:
    def __init__(self):
        super().__init__()
        pass # NOT NEEDED
```


Include all the comments
------------------------

**IMPORTANT**: Include all the comments from the existing examples. These
comments provide helpful explanations and context for the code.

Converting Java comments to Python docstrings can be tedious and error prone. We
have a tool at https://github.com/robotpy/devtools/blob/main/sphinxify_server.py
that launches an HTML page that you can just paste doxygen or javadoc comments
into and it will convert it to a mostly usable docstring.

```python
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

"""
Some docstring describing what this file does
"""

class SomeClass:
    """
    This describes what this class does
    """

    def __init__(self):
        """
        This describes what the constructor does
        """ 

    def myFunction(self, a: int) -> int:
        """
        This function is great.

        :param a: Input parameter a
        """

```

Command-based robot specific things
-----------------------------------

We use `commands2.TimedCommandRobot` instead of TimedRobot. It provides a 
`robotPeriodic` method for you, so it doesn't need to be included from
the java code unless robotPeriodic function does something other than
run the command scheduler.

Java examples will often have a `Constants.java` file with a bunch of constants
in it. RobotPy examples will put those constants in a `constants.py` as globals.
To group constants sometimes it makes sense to put each group in its own class,
but a single `Constants` class should be avoided.

Final thoughts
--------------

Before translating WPILib Java code to RobotPy's WPILib, first take some time
and read through the existing RobotPy code to get a feel for the style of the
code. Try to keep it Pythonic and yet true to the original spirit of the code.
Style *does* matter, as students will be reading through this code and it will
potentially influence their decisions in the future.

Remember, all contributions are welcome, no matter how big or small!
