# WPILib Maven Artifacts

WPILib publishes its built artifacts to our Maven server for use by downstream projects. This document explains these locations, and the meanings of artifact names, classifiers, and versions.

## Repositories
We provide two repositories. These repositories are:

* (Release)     https://frcmaven.wpi.edu/artifactory/release/
* (Development) https://frcmaven.wpi.edu/artifactory/development/

The release repository is where official WPILib releases are pushed.
The development repository is where development releases of every commit to [main](https://github.com/wpilibsuite/allwpilib/tree/main) is pushed.

## Artifact classifiers
We provide two base types of artifacts.

The first types are Java artifacts. These are usually published as `jar` files. Usually, the actual jar file is published with no classifier. The sources are published with the `-sources` classifier, and the javadocs are published with the `-javadoc` classifier. These artifacts are published with the base artifact name as their artifact ID, with a `-java` extension.

Example:
```
edu.wpi.first.wpilibj:wpilibj-java:version
```

The second types are native artifacts. These are usually published as `zip` files. The `-sources` and `-headers` classifiers contain the sources and headers respectively for the library. Each artifact also contains a classifier for each platform we publish. This platform is in the format `{os}{arch}`. The full list of supported platforms can be found in [native-utils](https://github.com/wpilibsuite/native-utils/blob/main/src/main/java/edu/wpi/first/nativeutils/WPINativeUtilsExtension.java#L94). If the library is built statically, it will have `static` appended to the classifier. Additionally, if the library was built in debug mode, `debug` will be appended to the classifier. The platform artifact only contains the binaries for a specific platform. Note that the binary artifacts never contain the headers, you always need the `-headers` classifier to get those.

If the library is Java and C++ and has a JNI component, the native artifact will have a shared library containing JNI entrypoints alongside the C++ shared library. This JNI shared library will have a `jni` suffix in the file name.

Native artifacts are published with the base artifact name as their artifact ID, with a `-cpp` extension.

Example:
```
edu.wpi.first.wpimath:wpimath-cpp:version:classifier@zip
edu.wpi.first.wpimath:wpimath-cpp:version:windowsx86-64staticdebug@zip
```

## Provided Artifacts
This repository provides the following artifacts. Below each artifact is its dependencies.

For C++, if building with static dependencies, the listed order should be the link order in your linker.

All artifacts are based at `edu.wpi.first.artifactname` in the repository.

* wpiutil

* wpigui
  * imgui

* wpimath
  * wpiutil

* wpinet
  * wpiutil

* ntcore
  * wpiutil
  * wpinet

* glass/libglass
  * wpiutil
  * wpimath
  * wpigui

* glass/libglassnt
  * wpiutil
  * wpinet
  * ntcore
  * wpimath
  * wpigui

* hal
  * wpiutil

* halsim
  * wpiutil
  * wpinet
  * ntcore
  * wpimath
  * wpigui
  * libglass
  * libglassnt

* cscore
  * opencv
  * wpinet
  * wpiutil

* cameraserver
  * ntcore
  * cscore
  * opencv
  * wpinet
  * wpiutil

* wpilibj
  * hal
  * cameraserver
  * ntcore
  * cscore
  * wpinet
  * wpiutil

* wpilibc
  * hal
  * cameraserver
  * ntcore
  * cscore
  * wpimath
  * wpinet
  * wpiutil

* wpilibNewCommands
  * wpilibc
  * hal
  * cameraserver
  * ntcore
  * cscore
  * wpimath
  * wpinet
  * wpiutil

* wpiunits

* apriltag
  * wpiutil
  * wpimath

### Third Party Artifacts

This repository provides the builds of the following third party software.

All artifacts are based at `edu.wpi.first.thirdparty.frcYEAR` in the repository.

* apriltaglib
* googletest
* imgui
* opencv
* libssh
