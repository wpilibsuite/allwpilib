# Installing Development Builds

This article contains instructions on building projects using a development build and a local WPILib build.

**Note:** This only applies to Java/C++ teams.

## Development Build

Development builds are the per-commit build hosted everytime a commit is pushed to the [allwpilib](https://github.com/wpilibsuite/allwpilib/) repository. These builds are then hosted on [artifactory](https://frcmaven.wpi.edu/artifactory/webapp/#/home).

In order to build a project using a development build, find the build.gradle file and open it. Then, add the following code below the plugin section and replace YEAR with the year of the development version.

```groovy
wpi.maven.useDevelopment = true
wpi.wpilibVersion = 'YEAR.+'
```

The top of your ``build.gradle`` file should now look similar to the code below. Ignore any differences in versions.

Java
```groovy
plugins {
  id "java"
  id "edu.wpi.first.GradleRIO" version "2020.3.2"
}

wpi.maven.useDevelopment = true
wpi.wpilibVersion = '2020.+'
```

C++
```groovy
plugins {
  id "cpp"
  id "google-test-test-suite"
  id "edu.wpi.first.GradleRIO" version "2020.3.2"
}

wpi.maven.useDevelopment = true
wpi.wpilibVersion = '2020.+'
```

## Local Build

Building with a local build is very similar to building with a development build. Ensure you have built and published WPILib by following the instructions attached [here](https://github.com/wpilibsuite/allwpilib#building-wpilib). Next, find the ``build.gradle`` file in your robot project and open it. Then, add the following code below the plugin section and replace ``YEAR`` with the year of the local version.

Java
```groovy
plugins {
  id "java"
  id "edu.wpi.first.GradleRIO" version "2020.3.2"
}

wpi.maven.useFrcMavenLocalDevelopment = true
wpi.wpilibVersion = 'YEAR.424242.+'
```

C++
```groovy
plugins {
  id "cpp"
  id "google-test-test-suite"
  id "edu.wpi.first.GradleRIO" version "2020.3.2"
}

wpi.maven.useFrcMavenLocalDevelopment = true
wpi.wpilibVersion = 'YEAR.424242.+'
```
