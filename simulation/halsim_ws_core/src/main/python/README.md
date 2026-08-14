robotpy-halsim-ws
==================

Installing this package will allow you to utilize the 2020+ WPILib websim from a RobotPy program.

Usage
-----

First, install pyfrc. Then run your robot with the 'sim' argument and --ws-server or --ws-client flag:

    # Windows
    py -3 -m robotpy sim --ws-server
    py -3 -m robotpy sim --ws-client

    # Linux/OSX
    python3 -m robotpy sim --ws-server
    python3 -m robotpy sim --ws-client

WPILib's documentation for using the simulator can be found at http://docs.wpilib.org/en/latest/docs/software/wpilib-tools/robot-simulation/
