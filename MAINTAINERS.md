## Publishing Third Party Dependencies
Currently the 3rd party deps are imgui, opencv, google test, libssh, and apriltaglib

For publishing these dependencies, the version needs to be manually updated in the publish.gradle file of their respective repository.
Then, in the azure build for the dependency you want to build for, manually start a pipeline build (As of current, this is the `Run Pipeline` button).
A variable needs to be added called `RUN_AZURE_ARTIFACTORY_RELEASE`, with a value of `true`. Then when the pipeline gets started, the final build outputs will be updated to artifactory.

To use newer versions of C++ dependencies, in `shared/config.gradle`, update the version related to the specific dependency.
For Java dependencies, there is likely a file related to the specific dependency in the shared folder. Update the version in there.

Note, changing artifact locations (This includes changing the artifact year currently, I have an issue open to change this) requires updating the `native-utils` plugin

## Publishing allwpilib
allwpilib publishes to the development repo on every push to main. To publish a release build, upload a new tag, and a release will automatically be built and published.

## Publishing desktop tools
Desktop tools publish to the development repo on every push to main. To publish a release build, upload a new tag, and a release will automatically be built and published.

## Publishing VS Code
Before publishing, make sure to update the gradlerio version in `vscode-wpilib/resources/gradle/version.txt` Also make sure the gradle wrapper version matches the wrapper required by gradlerio.
Upon pushing a tag, a release will be built, and the files will be uploaded to the releases on GitHub.

## Publishing GradleRIO
Before publishing, make sure to update the version in build.gradle. Publishing must happen locally, using the command `./gradlew publishPlugin`. This does require your API key for publishing to be set.

## Building the installer
Update the GradleRIO version in gradle.properties, and in the scripts folder in vscode, update the vscode extension. To publish a release build, upload a new tag, and a release will automatically be built and published to artifactory and cloudflare.
