load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive", "http_file", "http_jar")
load("//thirdparty/ceres:repositories.bzl", "ceres_repositories")

ceres_repositories()

http_archive(
    name = "pybind11_bazel",
    integrity = "sha256-iwRj1wuX2pDS6t6DqiCfhIXisv4y+7CvxSJtZoSAzGw=",
    strip_prefix = "pybind11_bazel-2b6082a4d9d163a52299718113fa41e4b7978db5",
    urls = ["https://github.com/pybind/pybind11_bazel/archive/2b6082a4d9d163a52299718113fa41e4b7978db5.tar.gz"],
)

http_archive(
    name = "pybind11",
    build_file = "@pybind11_bazel//:pybind11-BUILD.bazel",
    integrity = "sha256-wkxQme/TKRS6e0rFedV3IH00zgb8ZsZktnc3HRi9DyU=",
    strip_prefix = "pybind11-dfe7e65b4527eeb11036402aac3a394130960bb2",
    urls = ["https://github.com/pybind/pybind11/archive/dfe7e65b4527eeb11036402aac3a394130960bb2.zip"],
)

http_archive(
    name = "bzlmodrio-opencv",
    sha256 = "867ec3e90b7efc30ff6eb68d14050e7f1e800656d390505b135069f080c5cd91",
    url = "https://github.com/wpilibsuite/bzlmodRio-opencv/releases/download/2025.4.10.0-3.bcr5/bzlmodRio-opencv-2025.4.10.0-3.bcr5.tar.gz",
)

load("@bzlmodrio-opencv//:maven_cpp_deps.bzl", "setup_legacy_bzlmodrio_opencv_cpp_dependencies")

setup_legacy_bzlmodrio_opencv_cpp_dependencies()

http_archive(
    name = "bzlmodrio-libssh",
    sha256 = "f8fef627c7b393f7f6ed638e12b80ff90b2cfea11488b15214f25ce1e470723a",
    url = "https://github.com/wpilibsuite/bzlmodRio-libssh/releases/download/2024.0.105-1.bcr1/bzlmodrio-libssh-2024.0.105-1.bcr1.tar.gz",
)

load("@bzlmodrio-libssh//:maven_cpp_deps.bzl", "setup_legacy_bzlmodrio_libssh_cpp_dependencies")

setup_legacy_bzlmodrio_libssh_cpp_dependencies()

# Capture the repository environmental variables which specify the filter list for what architectures to build in CI.
load("//shared/bazel/rules:publishing_rule.bzl", "publishing_repo")

publishing_repo(
    name = "com_wpilib_allwpilib_publishing_config",
)

http_jar(
    name = "com_github_google_copybara",
    integrity = "sha256-IHW6y6WXJFjX9RYD+IwVAMwAbEo36fLqonIKR+FaqpQ=",
    urls = ["https://github.com/google/copybara/releases/download/v20251027/copybara_deploy.jar"],
)
