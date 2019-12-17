# Faster Builds for Developers

When you run `./gradlew build`, it builds EVERYTHING. This means debug and release builds for desktop and all installed cross compilers. For many developers, this is way too much, and causes much developer pain.

To help with some of these things, common tasks have shortcuts to only build necessary things for common development and testing tasks.

## Development (Desktop)

For projects `wpiutil`, `ntcore`, `cscore`, `hal` `wpilibOldCommands`, `wpilibNewCommands` and `cameraserver`, a `testDesktopJava` and a `testDesktopCpp` task exists. These can be ran with `./gradlew :projectName:task`, and will only build the minimum things required to run those tests.

For `wpilibc`, a `testDesktopCpp` task exists. For `wpilibj`, a `testDesktopJava` task exists.

For `wpilibcExamples`, a `buildDesktopCpp` task exists (These can't be ran, but they can compile).

For `wpilibjExamples`, a `buildDesktopJava` task exists.