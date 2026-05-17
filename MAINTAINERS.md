## Publishing Third Party Dependencies
Currently the 3rd party external deps are opencv, libssh, ceres, and gtsam.

For publishing these dependencies, the version needs to be manually updated in the publish.gradle file of their respective repository.
Then, upload a new tag for the dependency you want to build for, which will automatically start a build.
The CI workflow should set `RUN_AZURE_ARTIFACTORY_RELEASE` to `true` on tagged runs. Then when the pipeline gets started, the final build outputs will be uploaded to artifactory.

To use newer versions of C++ dependencies, in `shared/config.gradle`, update the version related to the specific dependency.
For Java dependencies, there is likely a file related to the specific dependency in the shared folder. Update the version in there.

Note, changing artifact locations requires updating the `native-utils` plugin; specifically, the `configureDependencies` method in the `WPINativeUtilsExtension` class.

## Publishing allwpilib
allwpilib publishes to the development repo on every push to main. To publish a release build, upload a new tag, and a release will automatically be built and published.

### Adding a new robot code dependency/subproject
If a new subproject has been added that is meant for use from robot code, both GradleRIO and the `native-utils` plugin need to be updated. `native-utils` is updated in the same way as 3rd party dependencies. For GradleRIO, update the `WPIJavaDepsExtension` to contain your new subproject's artifacts.

## Publishing VS Code
Before publishing, make sure to update the GradleRIO version in `vscode-wpilib/resources/gradle/version.txt`. Also make sure the Gradle Wrapper version matches the wrapper required by GradleRIO.
Upon pushing a tag, a release will be built, and the files will be uploaded to the releases on GitHub.

## Publishing GradleRIO
Before publishing, make sure to update the version in build.gradle. Publishing must happen locally, using the command `./gradlew publishPlugin`. This does require your API key for publishing to be set.

## Building the installer
Update the GradleRIO version in gradle.properties, and in the scripts folder in vscode, update the vscode extension. To publish a release build, upload a new tag, and a release will automatically be built and published to artifactory and cloudflare.
