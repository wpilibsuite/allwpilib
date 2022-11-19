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

The first types are Java artifacts. These are usually published as `jar` files. Usually, the actual jar file is published with no classifier. The sources are published with the `-sources` classifier, and the javadocs are published with the `-javadoc` classifier.

The second types are native artifacts. These are usually published as `zip` files (except for the `JNI` artifact types, which are `jar` files. See below for information on this). The `-sources` and `-headers` classifiers contain the sources and headers respecively for the library. Each artifact also contains a classifier for each platform we publish. This platform is in the format `{os}{arch}`. The platform artifact only contains the binaries for a specific platform. In addition, we provide a `-all` classifier. This classifer combines all of the platform artifacts into a single artifact. This is useful for tools that cannot determine what version to use during builds. However, we recommend using the platform specific classifier when possible. Note that the binary artifacts never contain the headers, you always need the `-headers` classifier to get those.

## Artifact Names

WPILib builds four different types of artifacts.

##### C++ Only Libraries
When we publish C++ only libraries, they are published with the base artifact name as their artifact name, with a `-cpp` extension. All dependencies for the library are linked as shared libraries to the binary.


Example:
```
edu.wpi.first.wpilibc:wpilibc-cpp:version:classifier@zip
```

#### Java Only Libraries
When we publish Java only libraries, they are published with the base artifact name as their artifact name, with a `-java` extension.

Example:
```
edu.wpi.first.wpilibj:wpilibj-java:version
```

#### C++/Java Libraries without JNI
For libraries that are both C++ and Java, but without a JNI component, the C++ component is published with the `basename-cpp` artifact name, and the Java component is published with the `basename-java` artifact name.

Example:
```
edu.wpi.first.wpiutil:wpiutil-cpp:version:classifier@zip (C++)
edu.wpi.first.wpiutil:wpiutil-java:version (Java)
```

#### C++/Java Libraries with JNI
For libraries that are both C++ and Java with a JNI component there are three different artifact names. For Java, the component is published as `basename-java`. For C++, the `basename-cpp` artifact contains the C++ artifacts with all dependencies linked as shared libraries to the binary. These binaries DO contain the JNI entry points. The `basename-jni` artifact contains identical C++ binaries to the `-cpp` artifact, however all of its dependencies are statically linked, and only the JNI and C entry points are exported.

The `-jni` artifact should only be used in cases where you want to create a self contained Java application where the native artifacts are embedded in the jar. Note in an extraction scenario, extending off of the library is never supported, which is why the C++ entry points are not exposed. The name of the library is randomly generated during extraction. For pretty much all cases, and if you ever want to extend from a native library, you should use the `-cpp` artifacts. GradleRIO uses the `-cpp` artifacts for all platforms, even desktop, for this reason.

Example:
```
edu.wpi.first.ntcore:ntcore-cpp:version:classifier@zip (C++)
edu.wpi.first.ntcore:ntcore-jni:version:classifier (JNI jar library)
edu.wpi.first.ntcore:ntcore-java:version (Java)
```

## Provided Artifacts
This repository provides the following artifacts. Below each artifact is its dependencies. Note if ever using the `-jni` artifacts, no dependencies are needed for native binaries.

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


### Third Party Artifacts

This repository provides the builds of the following third party software.

All artifacts are based at `edu.wpi.first.thirdparty.frcYEAR` in the repository.

* googletest
* imgui
* opencv
* libssh
