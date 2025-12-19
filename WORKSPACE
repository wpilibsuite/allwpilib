load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive", "http_file", "http_jar")
load("//thirdparty/ceres:repositories.bzl", "ceres_repositories")

ceres_repositories()

http_archive(
    name = "bazel_features",
    sha256 = "a015f3f2ebf4f1ac3f4ca8ea371610acb63e1903514fa8725272d381948d2747",
    strip_prefix = "bazel_features-1.31.0",
    url = "https://github.com/bazel-contrib/bazel_features/releases/download/v1.31.0/bazel_features-v1.31.0.tar.gz",
)

# TODO(austin): Upgrade when the patches land.
# https://github.com/bazelbuild/rules_cc/pull/430
# https://github.com/bazelbuild/rules_cc/pull/431
# https://github.com/bazelbuild/rules_cc/pull/432
http_archive(
    name = "rules_cc",
    patch_args = ["-p1"],
    patches = ["//:shared/bazel/patches/rules_cc_windows.patch"],
    sha256 = "0d3b4f984c4c2e1acfd1378e0148d35caf2ef1d9eb95b688f8e19ce0c41bdf5b",
    strip_prefix = "rules_cc-0.1.4",
    url = "https://github.com/bazelbuild/rules_cc/releases/download/0.1.4/rules_cc-0.1.4.tar.gz",
)

http_archive(
    name = "build_bazel_apple_support",
    sha256 = "93456fae59f225693fae7e0fdbb2899433766d7e8c0797ed28e35c6f04b5f255",
    url = "https://github.com/bazelbuild/apple_support/releases/download/1.24.4/apple_support.1.24.4.tar.gz",
)

http_archive(
    name = "rules_java",
    sha256 = "d31b6c69e479ffa45460b64dc9c7792a431cac721ef8d5219fc9f603fa2ff877",
    urls = [
        "https://github.com/bazelbuild/rules_java/releases/download/8.11.0/rules_java-8.11.0.tar.gz",
    ],
)

http_archive(
    name = "rules_pkg",
    sha256 = "b7215c636f22c1849f1c3142c72f4b954bb12bb8dcf3cbe229ae6e69cc6479db",
    urls = [
        "https://github.com/bazelbuild/rules_pkg/releases/download/1.1.0/rules_pkg-1.1.0.tar.gz",
    ],
)

# Rules Python
http_archive(
    name = "rules_python",
    sha256 = "6b9185460d11d57a32139b103363fce39d81889206561ef582678273b74372ac",
    strip_prefix = "rules_python-1.7.0-rc5",
    url = "https://github.com/bazel-contrib/rules_python/releases/download/1.7.0-rc5/rules_python-1.7.0-rc5.tar.gz",
)

# Download Extra java rules
RULES_JVM_EXTERNAL_TAG = "6.8"

http_archive(
    name = "rules_jvm_external",
    sha256 = "704a0197e4e966f96993260418f2542568198490456c21814f647ae7091f56f2",
    strip_prefix = "rules_jvm_external-%s" % RULES_JVM_EXTERNAL_TAG,
    url = "https://github.com/bazel-contrib/rules_jvm_external/releases/download/{v}/rules_jvm_external-{v}.tar.gz".format(v = RULES_JVM_EXTERNAL_TAG),
)

# Setup aspect lib
http_archive(
    name = "aspect_bazel_lib",
    sha256 = "a8a92645e7298bbf538aa880131c6adb4cf6239bbd27230f077a00414d58e4ce",
    strip_prefix = "bazel-lib-2.7.2",
    url = "https://github.com/aspect-build/bazel-lib/releases/download/v2.7.2/bazel-lib-v2.7.2.tar.gz",
)

# Download toolchains
http_archive(
    name = "rules_bzlmodrio_toolchains",
    sha256 = "102b4507628e9724b0c1e441727762c344e40170f65ac60516168178ea33a89a",
    url = "https://github.com/wpilibsuite/rules_bzlmodrio_toolchains/releases/download/2025-1.bcr6/rules_bzlmodrio_toolchains-2025-1.bcr6.tar.gz",
)

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
    name = "rules_python_pytest",
    sha256 = "e2556404ef56ea3ec938597616afc51d78e1832cfe511b196e9f2b8fd7f8f149",
    strip_prefix = "rules_python_pytest-1.1.1",
    url = "https://github.com/caseyduquettesc/rules_python_pytest/releases/download/v1.1.1/rules_python_pytest-v1.1.1.tar.gz",
)

http_archive(
    name = "bazel_skylib",
    sha256 = "51b5105a760b353773f904d2bbc5e664d0987fbaf22265164de65d43e910d8ac",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.8.1/bazel-skylib-1.8.1.tar.gz",
        "https://github.com/bazelbuild/bazel-skylib/releases/download/1.8.1/bazel-skylib-1.8.1.tar.gz",
    ],
)

http_archive(
    name = "rules_doxygen",
    sha256 = "ab17caade4e4427578b545fa2890c55ee3898f8a7a5597416230227bbec8e61a",
    strip_prefix = "rules_doxygen-2.5.0",
    url = "https://github.com/TendTo/rules_doxygen/releases/download/2.5.0/rules_doxygen-2.5.0.tar.gz",
)

# This gives us a repository layout which matches what normal BCR modules expect.
# The goal here is to make it easier to depend on external projects which already
# include @eigen without introducing multiple eigen versions.
local_repository(
    name = "eigen",
    path = "wpimath/src/main/native/thirdparty/eigen/include/",
)

load("@bazel_features//:deps.bzl", "bazel_features_deps")

bazel_features_deps()

load("@build_bazel_apple_support//lib:repositories.bzl", "apple_support_dependencies")

apple_support_dependencies()

load("@rules_cc//cc:repositories.bzl", "rules_cc_toolchains")

rules_cc_toolchains()

load("@rules_java//java:rules_java_deps.bzl", "rules_java_dependencies")

rules_java_dependencies()

# note that the following line is what is minimally required from protobuf for the java rules
# consider using the protobuf_deps() public API from @com_google_protobuf//:protobuf_deps.bzl
load("@com_google_protobuf//bazel/private:proto_bazel_features.bzl", "proto_bazel_features")  # buildifier: disable=bzl-visibility

proto_bazel_features(name = "proto_bazel_features")

# register toolchains
load("@rules_java//java:repositories.bzl", "rules_java_toolchains")

rules_java_toolchains()

load("@rules_python//python:repositories.bzl", "py_repositories", "python_register_toolchains")

py_repositories()

python_register_toolchains(
    name = "python_3_10",
    ignore_root_user_error = True,
    python_version = "3.10",
)

load("@rules_python//python:pip.bzl", "pip_parse")

pip_parse(
    name = "allwpilib_pip_deps",
    python_interpreter_target = "@python_3_10_host//:python",
    requirements_lock = "//:requirements_lock.txt",
    requirements_windows = "//:requirements_windows_lock.txt",
)

load("@allwpilib_pip_deps//:requirements.bzl", "install_deps")

install_deps()

load("@rules_jvm_external//:repositories.bzl", "rules_jvm_external_deps")

rules_jvm_external_deps()

load("@rules_jvm_external//:setup.bzl", "rules_jvm_external_setup")

rules_jvm_external_setup()

load("@rules_jvm_external//:defs.bzl", "maven_install")
load("@rules_jvm_external//:specs.bzl", "maven")

maven_artifacts = [
    "org.ejml:ejml-simple:0.44.0",
    "com.fasterxml.jackson.core:jackson-annotations:2.19.2",
    "com.fasterxml.jackson.core:jackson-core:2.19.2",
    "com.fasterxml.jackson.core:jackson-databind:2.19.2",
    "us.hebi.quickbuf:quickbuf-runtime:1.4",
    "com.google.code.gson:gson:2.13.1",
    "edu.wpi.first.thirdparty.frc2025.opencv:opencv-java:4.10.0-3",
    maven.artifact(
        "org.junit.jupiter",
        "junit-jupiter",
        "5.10.1",
        testonly = True,
    ),
    maven.artifact(
        "org.junit.platform",
        "junit-platform-console",
        "1.10.1",
        testonly = True,
    ),
    maven.artifact(
        "org.junit.platform",
        "junit-platform-launcher",
        "1.10.1",
        testonly = True,
    ),
    maven.artifact(
        "org.junit.platform",
        "junit-platform-reporting",
        "1.10.1",
        testonly = True,
    ),
    maven.artifact(
        "org.hamcrest",
        "hamcrest-all",
        "1.3",
        testonly = True,
    ),
    maven.artifact(
        "com.googlecode.junit-toolbox",
        "junit-toolbox",
        "2.4",
        testonly = True,
    ),
    maven.artifact(
        "org.apache.ant",
        "ant",
        "1.10.12",
        testonly = True,
    ),
    maven.artifact(
        "org.apache.ant",
        "ant-junit",
        "1.10.12",
        testonly = True,
    ),
    maven.artifact(
        "org.mockito",
        "mockito-core",
        "4.1.0",
        testonly = True,
    ),
    maven.artifact(
        "com.google.testing.compile",
        "compile-testing",
        "0.21.0",
        testonly = True,
    ),
]

maven_install(
    name = "maven",
    artifacts = maven_artifacts,
    maven_install_json = "//:maven_install.json",
    repositories = [
        "https://repo1.maven.org/maven2",
        "https://frcmaven.wpi.edu/artifactory/release/",
    ],
)

load("@maven//:defs.bzl", "pinned_maven_install")

pinned_maven_install()

load("@aspect_bazel_lib//lib:repositories.bzl", "aspect_bazel_lib_dependencies", "aspect_bazel_lib_register_toolchains")

aspect_bazel_lib_dependencies()

aspect_bazel_lib_register_toolchains()

load("@rules_bzlmodrio_toolchains//:maven_deps.bzl", "setup_legacy_setup_toolchains_dependencies")

setup_legacy_setup_toolchains_dependencies()

load("@rules_bzlmodrio_toolchains//toolchains:load_toolchains.bzl", "load_toolchains")

load_toolchains()

register_toolchains(
    "@local_roborio//:macos",
    "@local_roborio//:linux",
    "@local_roborio//:windows",
    "@local_systemcore//:macos",
    "@local_systemcore//:linux",
    "@local_systemcore//:windows",
    "@local_raspi_bookworm_32//:macos",
    "@local_raspi_bookworm_32//:linux",
    "@local_raspi_bookworm_32//:windows",
    "@local_bookworm_32//:macos",
    "@local_bookworm_32//:linux",
    "@local_bookworm_32//:windows",
    "@local_bookworm_64//:macos",
    "@local_bookworm_64//:linux",
    "@local_bookworm_64//:windows",
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

# Setup quickbuf compiler
QUICKBUF_VERSION = "1.3.2"

http_file(
    name = "quickbuffer_protoc_linux",
    executable = True,
    sha256 = "f9a041bccaa7040db523666ef1b5fe9f6f94e70a82c88951f18f58aadd9c50b5",
    url = "https://repo1.maven.org/maven2/us/hebi/quickbuf/protoc-gen-quickbuf/" + QUICKBUF_VERSION + "/protoc-gen-quickbuf-" + QUICKBUF_VERSION + "-linux-x86_64.exe",
)

http_file(
    name = "quickbuffer_protoc_osx_x86-64",
    executable = True,
    sha256 = "ea307c2b69664ae7e7c69db4cddf5803187e5a34bceffd09a21652f0f16044f7",
    url = "https://repo1.maven.org/maven2/us/hebi/quickbuf/protoc-gen-quickbuf/" + QUICKBUF_VERSION + "/protoc-gen-quickbuf-" + QUICKBUF_VERSION + "-osx-x86_64.exe   ",
)

http_file(
    name = "quickbuffer_protoc_osx_aarch64",
    executable = True,
    sha256 = "a9abdee09d8b5ef0aa954b238536917313511deec11e1901994af26ade033e28",
    url = "https://repo1.maven.org/maven2/us/hebi/quickbuf/protoc-gen-quickbuf/" + QUICKBUF_VERSION + "/protoc-gen-quickbuf-" + QUICKBUF_VERSION + "-osx-aarch_64.exe   ",
)

http_file(
    name = "quickbuffer_protoc_windows",
    executable = True,
    sha256 = "27dc1f29764a62b5e6a813a4bcd63e81bbdc3394da760a44acae1025b4a89f1d",
    url = "https://repo1.maven.org/maven2/us/hebi/quickbuf/protoc-gen-quickbuf/" + QUICKBUF_VERSION + "/protoc-gen-quickbuf-" + QUICKBUF_VERSION + "-windows-x86_64.exe ",
)

# Setup rules_proto
http_archive(
    name = "rules_proto",
    sha256 = "0e5c64a2599a6e26c6a03d6162242d231ecc0de219534c38cb4402171def21e8",
    strip_prefix = "rules_proto-7.0.2",
    url = "https://github.com/bazelbuild/rules_proto/releases/download/7.0.2/rules_proto-7.0.2.tar.gz",
)

load("@rules_proto//proto:repositories.bzl", "rules_proto_dependencies")

rules_proto_dependencies()

load("@rules_proto//proto:setup.bzl", "rules_proto_setup")

rules_proto_setup()

load("@rules_pkg//:deps.bzl", "rules_pkg_dependencies")

rules_pkg_dependencies()

load("@rules_python_pytest//python_pytest:repositories.bzl", "rules_python_pytest_dependencies")

rules_python_pytest_dependencies()

# Capture the repository environmental variables which specify the filter list for what architectures to build in CI.
load("//shared/bazel/rules:publishing_rule.bzl", "publishing_repo")

publishing_repo(
    name = "com_wpilib_allwpilib_publishing_config",
)

load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")

bazel_skylib_workspace()

load("@rules_doxygen//:extensions.bzl", "doxygen_repository")

# Download the os specific version 1.15.0 of doxygen supporting all the indicated platforms
doxygen_repository(
    name = "doxygen",
    executables = [
        "",
        "",
        "",
    ],
    platforms = [
        "windows",
        "mac",
        "linux",
    ],
    sha256s = [
        "44658b9cc5c91749e6e3cc426ba63e2550b4a4a7619065acd77029aa234719c6",
        "b7630eaa0d97bb50b0333929ef5dc1c18f9e38faf1e22dca3166189a9718faf0",
        "0ec2e5b2c3cd82b7106d19cb42d8466450730b8cb7a9e85af712be38bf4523a1",
    ],
    versions = [
        "1.15.0",
        "1.15.0",
        "1.15.0",
    ],
)

http_jar(
    name = "com_github_google_copybara",
    integrity = "sha256-IHW6y6WXJFjX9RYD+IwVAMwAbEo36fLqonIKR+FaqpQ=",
    urls = ["https://github.com/google/copybara/releases/download/v20251027/copybara_deploy.jar"],
)
