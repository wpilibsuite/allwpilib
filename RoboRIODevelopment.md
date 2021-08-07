# RoboRIO Development

The repo contains a myRobot project built in way to do full project development without needing to do a full publish into GradleRIO. These also only require building the minimum amount of binaries for the RoboRIO, so the builds are much faster as well.

The setup only works if the RoboRIO is USB connected. If an alternate IP address is preferred, the `address` block in myRobot\build.gradle can be changed to point to another address.

The following 3 tasks can be used for deployment:
* `:myRobot:deployShared` deploys the C++ project using shared dependencies. Prefer this one for most C++ development.
* `:myRobot:deployStatic` deploys the C++ project with all dependencies statically linked.
* `:myRobot:deployJava` deploys the Java project and all required dependencies. Also installs the JRE if not currently installed.

Deploying any of these to the RoboRIO will disable the current startup project until it is redeployed.

From here, ssh into the RoboRIO using the `admin` account (`lvuser` will fail to run in many cases). In the admin home directory, a file for each deploy type will exist (`myRobotCpp`, `myRobotCppStatic` and `myRobotJavaRun`). These can be ran to start up the corresponding project.
