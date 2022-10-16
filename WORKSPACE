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
    sha256 = "dc88fafc8e1236831e0a9214a72d73f69d8c6aebf376dc76c23bc902d0925323",
    strip_prefix = "bazelrio-6c8de3b2199141e5c0eb19319b81c192ee61dedf/bazelrio",
    url = "https://github.com/pjreiniger/bazelRio/archive/6c8de3b2199141e5c0eb19319b81c192ee61dedf.tar.gz",
)
# local_repository(
#     name = "bazelrio",
#     path = "C:/Users/PJ/Documents/GitHub/bazelRio/bazelrio/bazelrio"
# )

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
    colorsensor_version = None,
    imgui_version = "1.88-5",
    libssh_version = "0.95-3",
    navx_version = None,
    ni_version = None,
    opencv_version = "4.6.0-2",
    phoenix_version = None,
    revlib_version = None,
    sparkmax_version = None,
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
