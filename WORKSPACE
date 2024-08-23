load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Download Extra java rules
http_archive(
    name = "rules_jvm_external",
    sha256 = "08ea921df02ffe9924123b0686dc04fd0ff875710bfadb7ad42badb931b0fd50",
    strip_prefix = "rules_jvm_external-6.1",
    url = "https://github.com/bazelbuild/rules_jvm_external/releases/download/6.1/rules_jvm_external-6.1.tar.gz",
)

load("@rules_jvm_external//:repositories.bzl", "rules_jvm_external_deps")

rules_jvm_external_deps()

load("@rules_jvm_external//:defs.bzl", "maven_install")

maven_artifacts = [
    "org.ejml:ejml-simple:0.43.1",
    "com.fasterxml.jackson.core:jackson-annotations:2.15.2",
    "com.fasterxml.jackson.core:jackson-core:2.15.2",
    "com.fasterxml.jackson.core:jackson-databind:2.15.2",
    "us.hebi.quickbuf:quickbuf-runtime:1.3.3",
]

maven_install(
    name = "maven",
    artifacts = maven_artifacts,
    repositories = [
        "https://repo1.maven.org/maven2",
        "https://frcmaven.wpi.edu/artifactory/release/",
    ],
)

# Download toolchains
http_archive(
    name = "rules_bzlmodrio_toolchains",
    sha256 = "cd3ff046427e9c6dbc0c86a458c8cf081b8045fc3fb4265d08c0ebfc17f9cb30",
    url = "https://github.com/bzlmodRio/rules_bzlmodRio_toolchains/releases/download/2024-1/rules_bzlmodRio_toolchains-2024-1.tar.gz",
)

load("@rules_bzlmodrio_toolchains//:maven_deps.bzl", "setup_legacy_setup_toolchains_dependencies")

setup_legacy_setup_toolchains_dependencies()

load("@rules_bzlmodrio_toolchains//toolchains:load_toolchains.bzl", "load_toolchains")

load_toolchains()

#
http_archive(
    name = "rules_bzlmodrio_jdk",
    sha256 = "a00d5fa971fbcad8a17b1968cdc5350688397035e90b0cb94e040d375ecd97b4",
    url = "https://github.com/bzlmodRio/rules_bzlmodRio_jdk/releases/download/17.0.8.1-1/rules_bzlmodRio_jdk-17.0.8.1-1.tar.gz",
)

load("@rules_bzlmodrio_jdk//:maven_deps.bzl", "setup_legacy_setup_jdk_dependencies")

setup_legacy_setup_jdk_dependencies()

register_toolchains(
    "@local_roborio//:macos",
    "@local_roborio//:linux",
    "@local_roborio//:windows",
    "@local_raspi_32//:macos",
    "@local_raspi_32//:linux",
    "@local_raspi_32//:windows",
    "@local_bullseye_32//:macos",
    "@local_bullseye_32//:linux",
    "@local_bullseye_32//:windows",
    "@local_bullseye_64//:macos",
    "@local_bullseye_64//:linux",
    "@local_bullseye_64//:windows",
)

setup_legacy_setup_jdk_dependencies()

# Download other dependencies
http_archive(
    name = "rules_bazelrio",
    sha256 = "0c5a98476ac5b606689863b7b9ef3f7d685c47ce2681e448ca977e8e95de31c1",
    url = "https://github.com/bzlmodRio/rules_bazelrio/releases/download/0.0.14/rules_bazelrio-0.0.14.tar.gz",
)

http_archive(
    name = "bzlmodrio-ni",
    sha256 = "02a9b1d9722ad3cc7d55ee31a709938884d981f69634dfe93f92e3986bb7a43f",
    url = "https://github.com/bzlmodRio/bzlmodRio-ni/releases/download/2024.2.1/bzlmodRio-ni-2024.2.1.tar.gz",
)

load("@bzlmodrio-ni//:maven_cpp_deps.bzl", "setup_legacy_bzlmodrio_ni_cpp_dependencies")

setup_legacy_bzlmodrio_ni_cpp_dependencies()

http_archive(
    name = "bzlmodrio-opencv",
    sha256 = "f61f21220bf3d01d9585af30d23714b774235fe0f5334446745f6eee682a9b14",
    url = "https://github.com/bzlmodRio/bzlmodRio-opencv/releases/download/2024.4.8.0-4/bzlmodRio-opencv-2024.4.8.0-4.tar.gz",
)

load("@bzlmodrio-opencv//:maven_cpp_deps.bzl", "setup_legacy_bzlmodrio_opencv_cpp_dependencies")

setup_legacy_bzlmodrio_opencv_cpp_dependencies()

load("@bzlmodrio-opencv//:maven_java_deps.bzl", "setup_legacy_bzlmodrio_opencv_java_dependencies")

setup_legacy_bzlmodrio_opencv_java_dependencies()
