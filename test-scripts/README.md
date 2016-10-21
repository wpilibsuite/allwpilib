# WPILIB TEST SCRIPTS
## Overview
These test scripts are designed to allow the user of the WPILib test framework to quickly and easily deploy and run their tests on the WPI roboRIO.

In order for the automated test system to work there is a driverstation onboard the roboRIO that handles a queue of users waiting to use the driver station. All of the interaction with this queue is handled internally by test scripts contained within this folder.

If you deploy code to the test stand using the Eclipse plugins, you _must_ remove the build artifacts in `/home/lvuser`, or you will break tests.

## roboRIO Setup
The roboRIO on the test bench must be updated everytime NI releases a new image.

1. [Use the roboRIO Imaging Tool to format the roboRIO with the lastest image.](https://wpilib.screenstepslive.com/s/4485/m/13503/l/144984-imaging-your-roborio)
2. [Install Java on the roboRIO.](https://wpilib.screenstepslive.com/s/4485/m/13503/l/599747-installing-java-8-on-the-roborio-using-the-frc-roborio-java-installer-java-only)
3. SFTP the [teststand, netconsole, and libstdc++ ipk files](https://users.wpi.edu/~phplenefisch/ipk/) on to the roboRIO.
4. ssh on to the roboRIO as the admin user (ex: `ssh admin@roboRIO-190-FRC.local`)
5. Use opkg to install the ipk files (ex: `opkg install teststand_1.2-1_armv7a-vfp.ipk`)
6. Reboot the roboRIO
