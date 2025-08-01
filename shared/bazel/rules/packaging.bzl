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

_platform_mapping = {
    "linux-x86-64": "linuxx86-64",
    "osxuniversal": "osxuniversal",
    "systemcore": "linuxsystemcore",
    "windows-arm64": "windowsarm64",
    "windows-x86-64": "windowsx86-64",
}

def _filter_artifacts(architectures, artifacts):
    if architectures == None:
        return artifacts

    result = {}
    for ak, av in artifacts.items():
        if ak == "sources" or ak == "headers":
            result[ak] = av
            continue

        for k, v in architectures.items():
            if ak.startswith(_platform_mapping[v]):
                result[ak] = av
                break

    return result

def package_default_jni_project(
        name,
        maven_group_id,
        maven_artifact_name,
        architectures = None):
    """Packages the C++ shared and static libraries for a project which has a JNI component.

    This assumes that shared and static libraries exist for the project, and
    that they are compatible with the relevant architectures.  This triggers the
    transitions, packages them up, and deploys them for all native platforms
    plus systemcore.
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
        architectures = architectures,
    )

    architectures_pkg_zip(
        name = "{}_shared_zip".format(name),
        srcs = [
            ":{}-shared-files".format(name),
            "//:license_pkg_files",
        ],
        architectures = architectures,
    )

    _wpilib_maven_export_shared_static(
        name = "{}-cpp_publish".format(name),
        target_name = name,
        maven_group_id = maven_group_id,
        maven_artifact_name = maven_artifact_name,
        architectures = architectures,
    )

def package_default_cc_project(
        name,
        maven_group_id,
        maven_artifact_name,
        architectures = None):
    """Packages the C++ shared and static libraries for a project.

    This assumes that shared and static libraries exist for the project, and
    that they are compatible with the relevant architectures.  This triggers the
    transitions, packages them up, and deploys them for all native platforms
    plus systemcore.
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
        ],
        prefix = platform_prefix("shared"),
    )

    architectures_pkg_zip(
        name = "{}_static_zip".format(name),
        srcs = [
            ":{}-static-files".format(name),
            "//:license_pkg_files",
        ],
        architectures = architectures,
    )

    architectures_pkg_zip(
        name = "{}_shared_zip".format(name),
        srcs = [
            ":{}-shared-files".format(name),
            "//:license_pkg_files",
        ],
        architectures = architectures,
    )
    _wpilib_maven_export_shared_static(
        name = "{}-cpp_publish".format(name),
        target_name = name,
        maven_group_id = maven_group_id,
        maven_artifact_name = maven_artifact_name,
        architectures = architectures,
    )

def _wpilib_maven_export_shared_static(
        name,
        target_name,
        maven_group_id,
        maven_artifact_name,
        architectures = None):
    wpilib_maven_export(
        name = name,
        maven_coordinates = "{}:{}:$(WPILIB_VERSION)".format(maven_group_id, maven_artifact_name),
        classifier_artifacts = _filter_artifacts(architectures, {
            "headers": ":{}-hdrs-zip".format(target_name),
            "linuxsystemcore": ":{}_shared_zip-opt-systemcore".format(target_name),
            "linuxsystemcoredebug": ":{}_shared_zip-dbg-systemcore".format(target_name),
            "linuxsystemcorestatic": ":{}_static_zip-opt-systemcore".format(target_name),
            "linuxsystemcorestaticdebug": ":{}_static_zip-dbg-systemcore".format(target_name),
            "sources": ":{}-srcs-zip".format(target_name),
        }),
        linux_artifacts = _filter_artifacts(architectures, {
            "linuxx86-64": ":{}_shared_zip-opt-linux-x86-64".format(target_name),
            "linuxx86-64debug": ":{}_shared_zip-dbg-linux-x86-64".format(target_name),
            "linuxx86-64static": ":{}_static_zip-opt-linux-x86-64".format(target_name),
            "linuxx86-64staticdebug": ":{}_static_zip-dbg-linux-x86-64".format(target_name),
        }),
        osx_artifacts = _filter_artifacts(architectures, {
            "osxuniversal": ":{}_shared_zip-opt-osxuniversal".format(target_name),
            "osxuniversaldebug": ":{}_shared_zip-dbg-osxuniversal".format(target_name),
            "osxuniversalstatic": ":{}_static_zip-opt-osxuniversal".format(target_name),
            "osxuniversalstaticdebug": ":{}_static_zip-dbg-osxuniversal".format(target_name),
        }),
        visibility = ["//visibility:public"],
        windows_artifacts = _filter_artifacts(architectures, {
            "windowsarm64": ":{}_shared_zip-opt-windows-arm64".format(target_name),
            "windowsarm64debug": ":{}_shared_zip-dbg-windows-arm64".format(target_name),
            "windowsarm64static": ":{}_static_zip-opt-windows-arm64".format(target_name),
            "windowsarm64staticdebug": ":{}_static_zip-dbg-windows-arm64".format(target_name),
            "windowsx86-64": ":{}_shared_zip-opt-windows-x86-64".format(target_name),
            "windowsx86-64debug": ":{}_shared_zip-dbg-windows-x86-64".format(target_name),
            "windowsx86-64static": ":{}_static_zip-opt-windows-x86-64".format(target_name),
            "windowsx86-64staticdebug": ":{}_static_zip-dbg-windows-x86-64".format(target_name),
        }),
    )

def package_minimal_cc_project(
        name,
        maven_group_id,
        maven_artifact_name,
        architectures = None):
    wpilib_maven_export(
        name = "{}-cpp_publish".format(name),
        classifier_artifacts = _filter_artifacts(architectures, {
            "headers": ":{}-hdrs-zip".format(name),
            "sources": ":{}-srcs-zip".format(name),
        }),
        linux_artifacts = {},
        maven_coordinates = "{}:{}:$(WPILIB_VERSION)".format(maven_group_id, maven_artifact_name),
        osx_artifacts = {},
        visibility = ["//visibility:public"],
        windows_artifacts = {},
    )

package_minimal_jni_project = package_minimal_cc_project

def package_shared_cc_project(
        name,
        maven_group_id,
        maven_artifact_name,
        architectures = None):
    """Packages the C++ shared libraries for a project.

    This assumes that shared libraries exist for the project, and that they
    are compatible with the relevant architectures.  This triggers the
    transitions, packages them up, and deploys them for all native platforms
    plus systemcore.
    """
    pkg_filegroup(
        name = "{}-shared-files".format(name),
        srcs = [
            ":shared/lib{}-shared-files".format(name),
        ],
        prefix = platform_prefix("shared"),
    )

    architectures_pkg_zip(
        name = "{}_shared_zip".format(name),
        srcs = [
            ":{}-shared-files".format(name),
            "//:license_pkg_files",
        ],
        architectures = architectures,
    )

    wpilib_maven_export(
        name = "{}-cpp_publish".format(name),
        classifier_artifacts = _filter_artifacts(architectures, {
            "headers": ":{}-hdrs-zip".format(name),
            "linuxsystemcore": ":{}_shared_zip-opt-systemcore".format(name),
            "linuxsystemcoredebug": ":{}_shared_zip-dbg-systemcore".format(name),
            "sources": ":{}-srcs-zip".format(name),
        }),
        linux_artifacts = _filter_artifacts(architectures, {
            "linuxx86-64": ":{}_shared_zip-opt-linux-x86-64".format(name),
            "linuxx86-64debug": ":{}_shared_zip-dbg-linux-x86-64".format(name),
        }),
        maven_coordinates = "{}:{}:$(WPILIB_VERSION)".format(maven_group_id, maven_artifact_name),
        osx_artifacts = _filter_artifacts(architectures, {
            "osxuniversal": ":{}_shared_zip-opt-osxuniversal".format(name),
            "osxuniversaldebug": ":{}_shared_zip-dbg-osxuniversal".format(name),
        }),
        visibility = ["//visibility:public"],
        windows_artifacts = _filter_artifacts(architectures, {
            "windowsarm64": ":{}_shared_zip-opt-windows-arm64".format(name),
            "windowsarm64debug": ":{}_shared_zip-dbg-windows-arm64".format(name),
            "windowsx86-64": ":{}_shared_zip-opt-windows-x86-64".format(name),
            "windowsx86-64debug": ":{}_shared_zip-dbg-windows-x86-64".format(name),
        }),
    )

def package_static_cc_project(
        name,
        maven_group_id,
        maven_artifact_name,
        architectures = None):
    """Packages the C++ static libraries for a project.

    This assumes that static libraries exist for the project, and that they
    are compatible with the relevant architectures.  This triggers the
    transitions, packages them up, and deploys them for all native platforms
    plus systemcore.
    """
    pkg_files(
        name = "{}-static-files".format(name),
        srcs = [
            ":static/{}".format(name),
        ],
        prefix = platform_prefix("static"),
        strip_prefix = "static",
    )

    architectures_pkg_zip(
        name = "{}_static_zip".format(name),
        srcs = [
            ":{}-static-files".format(name),
            "//:license_pkg_files",
        ],
        architectures = architectures,
    )

    wpilib_maven_export(
        name = "{}-cpp_publish".format(name),
        maven_coordinates = "{}:{}:$(WPILIB_VERSION)".format(maven_group_id, maven_artifact_name),
        classifier_artifacts = _filter_artifacts(architectures, {
            "headers": ":{}-hdrs-zip".format(name),
            "linuxsystemcorestatic": ":{}_static_zip-opt-systemcore".format(name),
            "linuxsystemcorestaticdebug": ":{}_static_zip-dbg-systemcore".format(name),
            "sources": ":{}-srcs-zip".format(name),
        }),
        linux_artifacts = _filter_artifacts(architectures, {
            "linuxx86-64static": ":{}_static_zip-opt-linux-x86-64".format(name),
            "linuxx86-64staticdebug": ":{}_static_zip-dbg-linux-x86-64".format(name),
        }),
        osx_artifacts = _filter_artifacts(architectures, {
            "osxuniversalstatic": ":{}_static_zip-opt-osxuniversal".format(name),
            "osxuniversalstaticdebug": ":{}_static_zip-dbg-osxuniversal".format(name),
        }),
        visibility = ["//visibility:public"],
        windows_artifacts = _filter_artifacts(architectures, {
            "windowsarm64static": ":{}_static_zip-opt-windows-arm64".format(name),
            "windowsarm64staticdebug": ":{}_static_zip-dbg-windows-arm64".format(name),
            "windowsx86-64static": ":{}_static_zip-opt-windows-x86-64".format(name),
            "windowsx86-64staticdebug": ":{}_static_zip-dbg-windows-x86-64".format(name),
        }),
    )

def package_binary_cc_project(
        name,
        maven_group_id,
        maven_artifact_name,
        architectures = None,
        renames = None):
    """Packages the C++ binary targets for a project.

    This assumes that static libraries exist for the project, and that they
    are compatible with the relevant architectures.  This triggers the
    transitions, packages them up, and deploys them for just the native
    platforms.
    """
    pkg_files(
        name = "{}-files".format(name),
        srcs = [name],
        prefix = platform_prefix(""),
        renames = renames,
    )

    architectures_pkg_zip(
        name = "{}_zip".format(name),
        srcs = [
            ":{}-files".format(name),
            "//:license_pkg_files",
        ],
        architectures = architectures,
    )

    wpilib_maven_export(
        name = "{}_publish".format(name),
        maven_coordinates = "{}:{}:$(WPILIB_VERSION)".format(maven_group_id, maven_artifact_name),
        classifier_artifacts = {},
        linux_artifacts = _filter_artifacts(architectures, {
            "linuxx86-64": ":{}_zip-opt-linux-x86-64".format(name),
        }),
        osx_artifacts = _filter_artifacts(architectures, {
            "osxuniversalstatic": ":{}_zip-opt-osxuniversal".format(name),
        }),
        visibility = ["//visibility:public"],
        windows_artifacts = _filter_artifacts(architectures, {
            "windowsarm64static": ":{}_zip-opt-windows-arm64".format(name),
            "windowsx86-64static": ":{}_zip-opt-windows-x86-64".format(name),
        }),
    )
