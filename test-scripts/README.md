# WPILib Test Scripts

## Overview

These test scripts are designed to allow the user of the WPILib test framework to quickly and easily deploy and run their tests on the WPI roboRIO.

If you deploy code to the test stand using GradleRIO, you _must_ remove the build artifacts in `/home/lvuser`, or you will break the test stand.

## roboRIO Setup
The roboRIO on the test bench must be updated every time NI releases a new image.

1. [Use the roboRIO Imaging Tool to format the roboRIO with the latest image.](https://frcdocs.wpi.edu/en/stable/docs/zero-to-robot/step-3/imaging-your-roborio.html)
2. Set a static ip on the roboRIO web dashboard to `10.1.90.2`
2. Install Java on the roboRIO
    1. [Download the JRE from Maven.](https://frcmaven.wpi.edu/artifactory/list/release/edu/wpi/first/jdk/)
    2. Transfer the JRE ipk to the roboRIO with scp: `scp <local path> admin@roboRIO-190-FRC.local:/tmp/frcjre.ipk`
    3. Install the JRE: `opkg install /tmp/frcjre.ipk`
    4. Remove the ipk file: `rm /tmp/frcjre.ipk`
3. Reboot the roboRIO
