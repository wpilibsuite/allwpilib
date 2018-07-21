Simulation Work in Progress
===========================

This branch has been put in place by Jeremy White to faciliate
testing and development of WPILib simulation plugins.

It is not meant as the final design or production use.
More than likely, we will be using a GradleRIO focused strategy
to deploy simulation plugins.

It has two major features:  an extension that allows a simulated
Robot to talk directly to a driver station, and the re-addition of
the plugins to allow operation with the Gazebo physics simulator.

IMPORTANT
=========
This is all meant for developers, it is not really suitable for
even power users.  If at any point you find yourself frustrated
or stumped, you would probably be wisest to stop.

On Linux, if you're not adept at figuring out what package you
need to install to satisfy a compile failure, then you are probably
not going to have any fun on Linux.  If you can't open up a bash
script and puzzle out what it'd doing wrong, then you won't have any fun.
If you didn't understand this paragraph, you will not have any fun.

On Windows, if you don't know how to open PowerShell and use git
from the command line, you are not going to have any fun.


Building on Windows:
====================

On Windows, using power shell, a simple:

 `.\gradlew build`

 should build and configure everything for operation.

Building on Linux:
==================

On Linux, it's fun to use Gazebo, so then you need to install the Gazebo
libraries.  On Debian stretch, that package is libgazebo7-dev.

You will also need a driver station.  You can use a Windows box with the
real driver station, or you can obtatin and run QDriverstation.

You'll know you're okay when:

   `pkg-config --modversion gazebo`

works.

The build process is similar:

  `./gradlew build -PmakeSim`

The -PmakeSim is not strictly necessary, but it forces the build of
Gazebo, so you can find if you have a simulation error.


Running on Windows:
===================

On Windows, we will run a simulated robot and the 'lowfi' simulator.
The 'lowfi' simulator simply exposes values on Network Tables.

* Open the driver station and your favorite Network Tables viewer
  I use Shuffleboard, but it doesn't really matter.

* Configure the driver station to connect to the PC.
  In the settings tab, where you have a team number, simply type
  in 127.0.0.1.  That will cause the driver station to connect to
  your PC.

* Start the simulated program.

  For C++ examples, you do that by running (from PowerShell):

    `wpilibcExamples\build\bin\runcppexample.ps1 pacgoat`

  You can specify any example name instead of pacgoat, but pacgoat
  has been the sample I've been testing with.

  For Java examples, you do that by running from PowerShell:

    `wpilibjExamples\build\bin\runjavaexample.ps1 pacgoat`

  *Note*: for Java, the special keyword 'delete' is needed to
  remove a cached .jar file.  You will need to delete your cached
  jar file any time you update your simulator or wpilib.

  The robot should start; you should see the Driver Station and
  Lowfi extensions initialize correctly, and your driver station
  lights should turn green.

* Operate the robot

  Now you can use a joystick to 'drive' the robot.  If you use
  the PacGoat example with a Logitech F300 joystick, you should
  be able to see the simulated PWM values in Network Tables
  (under the 'sim' section) change as you push the joysticks.


Running on Linux:
=================

On Linux, we will run a simulated robot and the Gazebo simulator.
This should be more gratifying because you can see something
that looks like a robot.

* Open your favorite driver station
  I prefer to use QDriverstation.

* Configure the driver station to connect to the PC.
  This is left as an exercise for the reader; you want to connect to 127.0.0.1.

* Start the Gazebo simulator

  This is done by invoking:

      `simulation/frc_gazebo_plugins/build/bin/frcgazebo PacGoat2014`

  where PacGoat2014 is the name of a Gazebo world file to use.  You can
  use any world file, however that is one that I have been experimenting
  with, so you should certainly start with it.

* Start the simulated program.

  For C++ examples, you do that by running:

    `wpilibcExamples/build/bin/runcppexample pacgoat`

  You can specify any example name instead of pacgoat, but pacgoat
  has been the sample I've been testing with.

  For Java examples, you do that by running from PowerShell:

    `wpilibjExamples/build/bin/runjavaexample pacgoat`

  *Note*: for Java, the special keyword 'delete' is needed to
  remove a cached .jar file.  You will need to delete your cached
  jar file any time you update your simulator or wpilib.

  The robot should start; you should see the Driver Station and
  Gazebo extensions initialize correctly, and your driver station
  lights should turn green.

* Operate the robot

  Now you can use a joystick to 'drive' the robot.  If you use
  the PacGoat example with a Logitech F300 joystick, you should
  be able to see the robot move around.  If you push the bumpers
  (left and right), you should see the robot pivot the ball around.
