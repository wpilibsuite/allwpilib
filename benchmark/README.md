# Benchmark

This is a benchmark project built directly against this repo's sources.

## Desktop benchmarking

This command runs the Java benchmarks on desktop.

```bash
./gradlew benchmark:run
```

This command runs the C++ benchmarks on desktop.

```
./gradlew benchmark:runCpp
```

## Deploy to a roboRIO

This project can only deploy over USB. If an alternate IP address is preferred, the `address` block in benchmark/build.gradle can be changed to point to another address.

This command deploys the C++ project using shared dependencies. Prefer this one for most C++ development.
```bash
./gradlew benchmark:deployShared
```

This command deploys the C++ project with all dependencies statically linked.
```bash
./gradlew benchmark:deployStatic
```

This command deploys the Java project and all required dependencies. It also installs the JRE if it's not currently installed.
```bash
./gradlew benchmark:deployJava
```

Those commands won't start the robot executable, so you have to manually ssh in and start it. The following command will do that.
```bash
ssh lvuser@172.22.11.2 frcRunRobot.sh
```

Console log prints will appear in the terminal.

Deploying any of these to the roboRIO will disable the current startup project until it is redeployed.

## Faster builds

If your benchmarks only need some projects, you can comment out or delete unnecessary subprojects from the dependencies, benchmarkCpp, and benchmarkCppStatic blocks in benchmark/build.gradle (Java or C++) and from `target_link_libraries()` in benchmark/CMakeLists.txt (C++ only).
