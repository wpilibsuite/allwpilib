load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Rules Python
http_archive(
    name = "rules_python",
    sha256 = "5fa3c738d33acca3b97622a13a741129f67ef43f5fdfcec63b29374cc0574c29",
    strip_prefix = "rules_python-0.9.0",
    url = "https://github.com/bazelbuild/rules_python/archive/refs/tags/0.9.0.tar.gz",
)

# Download BazelRio <3
http_archive(
    name = "bazelrio",
    sha256 = "fc90366a9af694154cf9454eea2a9a9d0916bb094ab3650331958314b868abae",
    strip_prefix = "bazelrio-45a469c2b9b31bf2832f71e013f117d7c989bab7/bazelrio",
    url = "https://github.com/pjreiniger/bazelRio/archive/45a469c2b9b31bf2832f71e013f117d7c989bab7.tar.gz",
)

# Download Extra java rules
RULES_JVM_EXTERNAL_TAG = "4.2"

RULES_JVM_EXTERNAL_SHA = "cd1a77b7b02e8e008439ca76fd34f5b07aecb8c752961f9640dea15e9e5ba1ca"

http_archive(
    name = "rules_jvm_external",
    sha256 = RULES_JVM_EXTERNAL_SHA,
    strip_prefix = "rules_jvm_external-%s" % RULES_JVM_EXTERNAL_TAG,
    url = "https://github.com/bazelbuild/rules_jvm_external/archive/%s.zip" % RULES_JVM_EXTERNAL_TAG,
)

load("@bazelrio//:deps.bzl", "setup_bazelrio_dependencies")

setup_bazelrio_dependencies(
    navx_version = "4.0.442",
    ni_version = "2022.4.0",
    opencv_version = "4.5.2-1",
    phoenix_version = "5.20.2",
    revlib_version = "2022.1.0",
    toolchain_versions = "2022-1",
    wpilib_version = "2022.4.1",
)

load("//shared/bazel/deps:repo.bzl", "load_third_party")

load_third_party()

# Initialize repositories for all packages in requirements_lock.txt.
load("@__allwpilib_pip_deps//:requirements.bzl", "install_deps")

install_deps()

load("@maven//:defs.bzl", "pinned_maven_install")

pinned_maven_install()
