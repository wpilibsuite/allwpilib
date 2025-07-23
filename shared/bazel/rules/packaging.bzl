load("@rules_pkg//:mappings.bzl", "pkg_filegroup", "pkg_files")
load("@rules_pkg//pkg:zip.bzl", "pkg_zip")
load("//shared/bazel/rules:publishing.bzl", "architectures_pkg_zip", "platform_prefix", "wpilib_maven_export")

def pkg_java_src_files(name):
    pkg_files(
        name = name + "-java-srcs",
        srcs = native.glob(["src/main/java/**"]),
        strip_prefix = "src/main/java",
    )

    pkg_files(
        name = name + "-generated-java-srcs",
        srcs = native.glob(["src/generated/main/java/**/*.java"], allow_empty = True),
        strip_prefix = "src/generated/main/java",
    )

    pkg_files(
        name = name + "-proto-srcs",
        srcs = native.glob(["src/main/proto/**"], allow_empty = True),
        strip_prefix = "src/main/proto",
    )

    pkg_filegroup(
        name = name,
        srcs = [
            name + "-java-srcs",
            name + "-proto-srcs",
            name + "-generated-java-srcs",
            "//shared/bazel/rules:src_jar_dummy_manifest",
        ],
    )

def zip_java_srcs(name, extra_pkgs = []):
    pkg_java_src_files("{}-sources.pkg".format(name))

    pkg_zip(
        name = "lib{}-java-sources".format(name),
        srcs = ["{}-sources.pkg".format(name)] + extra_pkgs,
        out = "lib{}-sources.jar".format(name),
        tags = ["manual"],
    )

def package_default_jni_project(
        name,
        maven_group_id,
        maven_artifact_name):
    """
    Packages a the C++ libraries for a project that has a JNI component.

    This assumes that static and shared libraries exist for the project, and 
    that it is built for all native platforms plus systemcore. It runs the
    per-platform transitions and bundles them all into a single maven_export target.
    """
    pkg_files(
        name = "{}-static-files".format(name),
        srcs = [
            ":static/{}".format(name),
        ],
        prefix = platform_prefix("static"),
        strip_prefix = "static",
    )

    pkg_filegroup(
        name = "{}-shared-files".format(name),
        srcs = [
            ":shared/lib{}-shared-files".format(name),
            ":shared/lib{}jni-shared-files".format(name),
        ],
        prefix = platform_prefix("shared"),
    )

    architectures_pkg_zip(
        name = "{}_static_zip".format(name),
        srcs = [
            ":{}-static-files".format(name),
            "//:license_pkg_files",
        ],
    )

    architectures_pkg_zip(
        name = "{}_shared_zip".format(name),
        srcs = [
            ":{}-shared-files".format(name),
            "//:license_pkg_files",
        ],
    )

    wpilib_maven_export(
        name = "{}-cpp_publish".format(name),
        classifier_artifacts = {
            "headers": ":{}-hdrs-zip".format(name),
            "linuxsystemcore": ":{}_shared_zip-opt-systemcore".format(name),
            "linuxsystemcoredebug": ":{}_shared_zip-dbg-systemcore".format(name),
            "linuxsystemcorestatic": ":{}_static_zip-opt-systemcore".format(name),
            "linuxsystemcorestaticdebug": ":{}_static_zip-dbg-systemcore".format(name),
            "sources": ":{}-srcs-zip".format(name),
        },
        linux_artifacts = {
            "linuxx86-64": ":{}_shared_zip-opt-linux-x86-64".format(name),
            "linuxx86-64debug": ":{}_shared_zip-dbg-linux-x86-64".format(name),
            "linuxx86-64static": ":{}_static_zip-opt-linux-x86-64".format(name),
            "linuxx86-64staticdebug": ":{}_static_zip-dbg-linux-x86-64".format(name),
        },
        maven_coordinates = "{}:{}:$(WPILIB_VERSION)".format(maven_group_id, maven_artifact_name),
        osx_artifacts = {
            "osxuniversal": ":{}_shared_zip-opt-osxuniversal".format(name),
            "osxuniversaldebug": ":{}_shared_zip-dbg-osxuniversal".format(name),
            "osxuniversalstatic": ":{}_static_zip-opt-osxuniversal".format(name),
            "osxuniversalstaticdebug": ":{}_static_zip-dbg-osxuniversal".format(name),
        },
        visibility = ["//visibility:public"],
        windows_artifacts = {
            "windowsarm64": ":{}_shared_zip-opt-windows-arm64".format(name),
            "windowsarm64debug": ":{}_shared_zip-dbg-windows-arm64".format(name),
            "windowsarm64static": ":{}_static_zip-opt-windows-arm64".format(name),
            "windowsarm64staticdebug": ":{}_static_zip-dbg-windows-arm64".format(name),
            "windowsx86-64": ":{}_shared_zip-opt-windows-x86-64".format(name),
            "windowsx86-64debug": ":{}_shared_zip-dbg-windows-x86-64".format(name),
            "windowsx86-64static": ":{}_static_zip-opt-windows-x86-64".format(name),
            "windowsx86-64staticdebug": ":{}_static_zip-dbg-windows-x86-64".format(name),
        },
    )

def package_minimal_jni_project(
        name,
        maven_group_id,
        maven_artifact_name):
    wpilib_maven_export(
        name = "{}-cpp_publish".format(name),
        classifier_artifacts = {
            "headers": ":{}-hdrs-zip".format(name),
            "sources": ":{}-srcs-zip".format(name),
        },
        linux_artifacts = {},
        maven_coordinates = "{}:{}:$(WPILIB_VERSION)".format(maven_group_id, maven_artifact_name),
        osx_artifacts = {},
        visibility = ["//visibility:public"],
        windows_artifacts = {},
    )

def package_minimal_cc_project(
        name,
        maven_group_id,
        maven_artifact_name):
    wpilib_maven_export(
        name = "{}-cpp_publish".format(name),
        classifier_artifacts = {
            "headers": ":{}-hdrs-zip".format(name),
            "sources": ":{}-srcs-zip".format(name),
        },
        linux_artifacts = {},
        maven_coordinates = "{}:{}:$(WPILIB_VERSION)".format(maven_group_id, maven_artifact_name),
        osx_artifacts = {},
        visibility = ["//visibility:public"],
        windows_artifacts = {},
    )
