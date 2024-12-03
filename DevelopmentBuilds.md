# Installing Development Builds

This article contains instructions on building projects using a development build and a local WPILib build.

**Note:** This only applies to Java/C++ teams.

> [!WARNING]
> **There are no stability or compatibility guarantees for builds outside of [tagged releases](https://github.com/wpilibsuite/allwpilib/releases). Changes may not be fully documented. Use them at your own risk!**
>
> Development builds may be non-functional between the end of the season and the start of beta testing. Development builds are also likely to be incompatible with vendor libraries during this time.

## Development Build

Development builds are the per-commit build hosted every time a commit is pushed to the [allwpilib](https://github.com/wpilibsuite/allwpilib/) repository. These builds are then hosted on [artifactory](https://frcmaven.wpi.edu/artifactory/webapp/#/home).

To build a project using a development build, find the build.gradle file and open it. Then, add the following code below the plugin section and replace YEAR with the year of the development version. It is also necessary to use a 2025 GradleRIO version, ie `2025.1.1-beta-1`

```groovy
wpi.maven.useLocal = false
wpi.maven.useDevelopment = true
wpi.versions.wpilibVersion = 'YEAR.+'
wpi.versions.wpimathVersion = 'YEAR.+'
```

The top of your ``build.gradle`` file should now look similar to the code below. Ignore any differences in versions.

Java
```groovy
plugins {
  id "java"
  id "edu.wpi.first.GradleRIO" version "2025.1.1-beta-1"
}

wpi.maven.useLocal = false
wpi.maven.useDevelopment = true
wpi.versions.wpilibVersion = '2025.+'
wpi.versions.wpimathVersion = '2025.+'
```

C++
```groovy
plugins {
  id "cpp"
  id "google-test-test-suite"
  id "edu.wpi.first.GradleRIO" version "2025.1.1-beta-1"
}

wpi.maven.useLocal = false
wpi.maven.useDevelopment = true
wpi.versions.wpilibVersion = '2025.+'
wpi.versions.wpimathVersion = '2025.+'
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
  id "edu.wpi.first.GradleRIO" version "2025.1.1-beta-1"
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
  id "edu.wpi.first.GradleRIO" version "2025.1.1-beta-1"
}

wpi.maven.useLocal = false
wpi.maven.useFrcMavenLocalDevelopment = true
wpi.versions.wpilibVersion = 'YEAR.424242.+'
wpi.versions.wpimathVersion = 'YEAR.424242.+'
```

# roboRIO Development

See the [developerRobot](developerRobot/README.md) subproject.
