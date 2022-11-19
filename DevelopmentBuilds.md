# Installing Development Builds

This article contains instructions on building projects using a development build and a local WPILib build.

**Note:** This only applies to Java/C++ teams.

## Development Build

Development builds are the per-commit build hosted every time a commit is pushed to the [allwpilib](https://github.com/wpilibsuite/allwpilib/) repository. These builds are then hosted on [artifactory](https://frcmaven.wpi.edu/artifactory/webapp/#/home).

To build a project using a development build, find the build.gradle file and open it. Then, add the following code below the plugin section and replace YEAR with the year of the development version. It is also necessary to use a 2023 GradleRIO version, ie `2023.0.0-alpha-1`

```groovy
wpi.maven.useLocal = false
wpi.maven.useDevelopment = true
wpi.versions.wpilibVersion = 'YEAR.+'
wpi.versions.wpimathVersion = 'YEAR.+
```

The top of your ``build.gradle`` file should now look similar to the code below. Ignore any differences in versions.

Java
```groovy
plugins {
  id "java"
  id "edu.wpi.first.GradleRIO" version "2023.0.0-alpha-1"
}

wpi.maven.useLocal = false
wpi.maven.useDevelopment = true
wpi.versions.wpilibVersion = '2023.+'
wpi.versions.wpimathVersion = '2023.+'
```

C++
```groovy
plugins {
  id "cpp"
  id "google-test-test-suite"
  id "edu.wpi.first.GradleRIO" version "2023.0.0-alpha-1"
}

wpi.maven.useLocal = false
wpi.maven.useDevelopment = true
wpi.versions.wpilibVersion = '2023.+'
wpi.versions.wpimathVersion = '2023.+'
```

### Development Build Documentation

* C++: https://github.wpilib.org/allwpilib/docs/development/cpp/
* Java: https://github.wpilib.org/allwpilib/docs/development/java/

## Local Build

Building with a local build is very similar to building with a development build. Ensure you have built and published WPILib by following the instructions attached [here](https://github.com/wpilibsuite/allwpilib#building-wpilib). Next, find the ``build.gradle`` file in your robot project and open it. Then, add the following code below the plugin section and replace ``YEAR`` with the year of the local version.

Java
```groovy
plugins {
  id "java"
  id "edu.wpi.first.GradleRIO" version "2023.0.0-alpha-1"
}

wpi.maven.useLocal = false
wpi.maven.useFrcMavenLocalDevelopment = true
wpi.versions.wpilibVersion = 'YEAR.424242.+'
wpi.versions.wpimathVersion = 'YEAR.424242.+'
```

C++
```groovy
plugins {
  id "cpp"
  id "google-test-test-suite"
  id "edu.wpi.first.GradleRIO" version "2023.0.0-alpha-1"
}

wpi.maven.useLocal = false
wpi.maven.useFrcMavenLocalDevelopment = true
wpi.versions.wpilibVersion = 'YEAR.424242.+'
wpi.versions.wpimathVersion = 'YEAR.424242.+'
```

# roboRIO Development

This repo contains a myRobot project built in way to do full project development without needing to do a full publish into GradleRIO. These also only require building the minimum amount of binaries for the roboRIO, so the builds are much faster as well.

The setup only works if the roboRIO is USB connected. If an alternate IP address is preferred, the `address` block in myRobot\build.gradle can be changed to point to another address.

The following 3 tasks can be used for deployment:
* `:myRobot:deployShared` deploys the C++ project using shared dependencies. Prefer this one for most C++ development.
* `:myRobot:deployStatic` deploys the C++ project with all dependencies statically linked.
* `:myRobot:deployJava` deploys the Java project and all required dependencies. Also installs the JRE if not currently installed.

Deploying any of these to the roboRIO will disable the current startup project until it is redeployed.

From here, ssh into the roboRIO using the `lvuser` account and run `frcRunRobot.sh` (It's in path).
