load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

RULES_JVM_EXTERNAL_TAG = "3.3"

RULES_JVM_EXTERNAL_SHA = "d85951a92c0908c80bd8551002d66cb23c3434409c814179c0ff026b53544dab"

http_archive(
    name = "rules_jvm_external",
    sha256 = RULES_JVM_EXTERNAL_SHA,
    strip_prefix = "rules_jvm_external-%s" % RULES_JVM_EXTERNAL_TAG,
    url = "https://github.com/bazelbuild/rules_jvm_external/archive/%s.zip" % RULES_JVM_EXTERNAL_TAG,
)

http_archive(
    name = "rules_python",
    sha256 = "b6d46438523a3ec0f3cead544190ee13223a52f6a6765a29eae7b7cc24cc83a0",
    url = "https://github.com/bazelbuild/rules_python/releases/download/0.1.0/rules_python-0.1.0.tar.gz",
)

#############################################
git_repository(
    name = "wpi_bazel_deps",
    commit = "8a30cd98ea9e60f6fec1cb3c05403c32e065bd09",
    remote = "https://github.com/bazelRio/wpi-bazel-deps-rules",
    shallow_since = "1621631074 -0400",
)

git_repository(
    name = "wpi_bazel_rules",
    commit = "5891f8452d7dde2a7a214320d515574f00461af0",
    remote = "https://github.com/bazelRio/wpi-bazel-rules",
    shallow_since = "1621639589 -0400",
)

# local_repository(
#     name = "wpi_bazel_rules",
#    path = "C:/Users/PJ/Documents/GitHub/wpilib/original/bazel_rules/wpi-bazel-rules",
#     # path = "/wpi-bazel-rules",
# )

load("@wpi_bazel_rules//toolchains:load_toolchains.bzl", "load_toolchains")

load_toolchains()

load("@wpi_bazel_rules//toolchains:configure_toolchains.bzl", "configure_toolchains")

configure_toolchains()

#############################################

load("//bazelThirdParty:load_repos.bzl", "load_repos")
load_repos()

load("//bazelThirdParty:repos.bzl", "wpilib_third_party_repositories")

wpilib_third_party_repositories()
