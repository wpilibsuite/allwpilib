# WPILib Bazel Support

WPILib is normally built with Gradle, but [Bazel](https://www.bazel.build/) can also be used to increase development speed due to the superior caching ability and the ability to use remote caching and remote execution (on select platforms)


## Prerequisites
- Install [Bazelisk](https://github.com/bazelbuild/bazelisk/releases) and add it to your path. Bazelisk is a wrapper that will download the correct version of bazel specified in the repository. Note: You can alias/rename the binary to `bazel` if you want to keep the familiar `bazel build` vs `bazelisk build` syntax.

## Building
To build the entire repository, simply run `bazel build //...`. To run all of the unit tests, run `bazel test //...`
Other examples:
- `bazel build //wpimath/...` - Builds every target in the wpimath folder
- `bazel test //wpiutil:wpiutil-cpp-test` - Runs only the cpp test target in the wpiutil folder
- `bazel coverage //wpiutil/...` - (*Nix only) - Runs a code coverage report for both C++ and Java on all the targets under wpiutil

## User settings
When invoking bazel, it will check if `user.bazelrc` exists for additional, user specified flags. You can use these settings to do things like always ignore buildin a specific folder, or limiting the CPU/RAM usage during a build.
Examples:
- `build --build_tag_filters=-wpi-example` - Do not build any targets tagged with `wpi-example` (Currently all of the targets in wpilibcExamples and wpilibjExamples contain this tag)
- `build -c opt` - Always build optimized targets. The default compiler flags were chosen to build as fast as possible, and thus don't contain many optimizations
- `build -k` - `-k` is analogous to the MAKE flag `--keep-going`, so the build will not stop on the first error.
- ```
  build --local_resources=memory=HOST_RAM*.5 # Don't use more than half my RAM when building
  build --local_resources=cpu=HOST_CPUS-1 # Leave one core alone
  ```
Bazel's RAM usage estimation is simplistic and hardcoded, so limiting the allowed number of CPU cores is the best way to reduce memory usage if it becomes a problem.

The default settings build all the release artifact variants relevant for your platform.  The overall list of options ends up being, essentially, all the variants of (linux, osx, windows) x (debug, release) x (static, shared) x (aarch64, x86).  OSX and Windows are hard to compile for from any other OS, so we by default build for your local OS and the system core, with all the variants.

This can be a bit expensive.  If you would like to build a subset, you can specify the repo environmental variable, `WPI_PUBLISH_CLASSIFIER_FILTER`, and pick what you build for.  The default is, in the .bazelrc file,
```
common --repo_env="WPI_PUBLISH_CLASSIFIER_FILTER=headers,sources,linuxsystemcore,linuxsystemcoredebug,linuxsystemcorestatic,linuxsystemcorestaticdebug,linuxx86-64,linuxx86-64debug,linuxx86-64static,linuxx86-64staticdebug,osxuniversal,osxuniversaldebug,osxuniversalstatic,osxuniversalstaticdebug,windowsarm64,windowsarm64debug,windowsarm64static,windowsarm64staticdebug,windowsx86-64,windowsx86-64debug,windowsx86-64static,windowsx86-64staticdebug"
```

Modify this to your likings if you want to build less.

## Pregenerating Files
allwpilib uses extensive use of pre-generating files that are later used to build C++ / Java libraries that are tracked by version control. Quite often,
these pre-generation scripts use some configuration file to create multipile files inside of an output directory. While this process could be accomplished
with a `genrule` that would require an explicit listing of every output file, which would be tedious to maintain as well as potentially confusing to people
adding new features those libraries. Therefor, we use `@aspect_bazel_lib` and their `write_source_files` feature to generate these directories. In the event that the generation process creates more than a small handful of predictable files, a custom rule is written to generate the directory.

## Remote Caching
One of the huge benefits of bazel is its remote caching ability. However, due to bazels strict build definitions it is hard to share remote cache artifacts between different computers unless our toolchains are fully hermetic, which means you are unlikely to be able to reuse the cache artifacts published from the `main` branch on your local machine like you might be able to with the `gradle` or `cmake` caches. Luckily the github actions CI machines are generally stable between runs and can reuse cache artifacts, and your local machine should remain stable, so if you set up a free buildbuddy account you can have your forks CI actions be able to use a personalized cache, as well as your local machine.

For the main `allwpilib` upstream, the cache is only updated on the main branch; pull requests from forks will not be able to modify the cache. However, you can set up your fork to enable its own cache by following the steps below.

### Setting Up API keys
Follow the [buildbuddy authentication](https://www.buildbuddy.io/docs/guide-auth) guide to create keys. For your local machine, it is recommended that you place the following configuration line in either a `user.bazelrc` or `bazel_auth.rc` file in the repositories root directory.

```
build --remote_header=<your api key>
```

To get your forks CI actions using your own buildbuddy cache, follow [GitHub's](https://docs.github.com/en/actions/how-tos/security-for-github-actions/security-guides/using-secrets-in-github-actions) documentation for setting up a repository secret. The secrets key should be `BUILDBUDDY_API_KEY`, and the value should be your buildbuddy API key.
