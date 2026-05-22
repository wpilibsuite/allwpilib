# Developer Robot

This is a robot project built directly against this repo's sources. It builds the minimum required for deploying to a Systemcore, so it's faster than publishing the whole library to Maven first.

This command builds everything.
```bash
./gradlew developerRobot:build
```

## Simulation

This command runs the Java project on desktop.
```bash
./gradlew developerRobot:run
```

This command runs the C++ project on desktop.
```bash
./gradlew developerRobot:runCpp
```

## Deploy to a Systemcore

This project can only deploy over USB. If an alternate IP address is preferred, the `address` block in developerRobot\build.gradle can be changed to point to another address.

This command deploys the C++ project using shared dependencies. Prefer this one for most C++ development.
```bash
./gradlew developerRobot:deployShared
```

This command deploys the C++ project with all dependencies statically linked.
```bash
./gradlew developerRobot:deployStatic
```

This command deploys the Java project and all required dependencies.
```bash
./gradlew developerRobot:deployJava
```

Those commands won't start the robot executable, so you have to manually ssh in and start it. The following command will do that.
```bash
ssh systemcore@robot.local sudo systemctl stop robot
ssh systemcore@robot.local sudo ~/robotCommand
```

Console log prints will appear in the terminal.

Deploying any of these to a Systemcore will disable the current startup project until it is redeployed.
