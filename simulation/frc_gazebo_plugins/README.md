Gazebo plugins for FRC
======================

These plugins enable the use of the Gazebo physics simulator to model robots,
and then use WPILIB programming libraries to control those simulated robots.

At the present time, this only builds on Linux, and has a fairly complex
installation process.

You must have the gazebo development packages available on Linux.
The gradle build system will automatically skip building this directory
if those requirements are not met.

This command:

    `./gradlew build -PmakeSim`

will force it to attempt to build.

Once you have built it, then a command like this:

  `simulation/frc_gazebo_plugins/build/bin/frcgazebo PacGoat2014.world`

should run the Gazebo simulation using our plugins against the
2014 model, with the PacGoat robot on the field.

The halsim_gazebo simulation library will provide a method for
running robot code to control a simulated robot.


TODO:  Package the plugins more nicely, and then put instructions here.

TODO:  Build and package the plugins for Windows.  The key there is to get gazebo
working on Windows.  This link was a useful trove of information on Windows/Gazebo
issues as of September, 2017:
  https://bitbucket.org/osrf/gazebo/issues/2129/visual-studio-2015-compatibility

Note that the sense remains that this will be difficult at best.
