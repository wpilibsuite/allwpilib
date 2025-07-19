"""Bazel rules for publishing maven artifacts."""

load("@bazel_skylib//rules:write_file.bzl", "write_file")
load("@com_wpilib_allwpilib_publishing_config//:publishing_config.bzl", "CLASSIFIER_FILTER")
load("@platforms//host:constraints.bzl", _host_constraints = "HOST_CONSTRAINTS")
load("@rules_jvm_external//:defs.bzl", "maven_export")
load("@rules_pkg//pkg:zip.bzl", "pkg_zip")
load("@rules_shell//shell:sh_binary.bzl", "sh_binary")
load("//shared/bazel/rules:transitions.bzl", "platform_transition_filegroup")

def _get_host_os():
    if "@platforms//os:linux" in _host_constraints:
        return "linux"
    if "@platforms//os:osx" in _host_constraints:
        return "osx"
    if "@platforms//os:windows" in _host_constraints:
        return "windows"
    fail("Unsupported host OS. Must be one of linux, osx, or windows.")

HOST_OS = _get_host_os()

def publish_all(name, targets):
    """Macro to publish multiple maven artifacts with 1 call."""
    publish_name = name + "_publish_all"
    write_file(
        name = publish_name,
        out = publish_name + ".sh",
        content = [
            "#!/bin/bash",
            "set -e",
            "",
            'for arg in "$@";',
            "do",
            '  "$arg"',
            "done",
        ],
    )
    sh_binary(
        name = name,
        srcs = [publish_name + ".sh"],
        args = ["$(location " + x + ".publish)" for x in targets],
        data = [x + ".publish" for x in targets],
    )

# Unfortunately, rules_jvm_external really wants each of the classifier
# artifacts to have a unique name so they can go in the runfiles together and
# not collide.
def architectures_pkg_zip(
        name,
        compilation_modes = ["dbg", "opt"],
        architectures = {
            "@rules_bzlmodrio_toolchains//platforms/linux_x86_64": "linux-x86-64",
            "@rules_bzlmodrio_toolchains//platforms/osx": "osxuniversal",
            "@rules_bzlmodrio_toolchains//platforms/systemcore": "systemcore",
            "@rules_bzlmodrio_toolchains//platforms/windows_arm64": "windows-arm64",
            "@rules_bzlmodrio_toolchains//platforms/windows_x86_64": "windows-x86-64",
        },
        **kwargs):
    architectures_target_compatible_with = {
        "linux-x86-64": ["@platforms//os:linux"],
        "osxuniversal": ["@platforms//os:osx"],
        "systemcore": None,
        "windows-arm64": ["@platforms//os:windows"],
        "windows-x86-64": ["@platforms//os:windows"],
    }
    for compilation_mode in compilation_modes:
        for platform, shortname in architectures.items():
            zip_name = name + "-" + compilation_mode + "-arch-" + shortname
            pkg_zip(
                name = zip_name,
                **kwargs
            )

            package_name = name + "-" + compilation_mode + "-" + shortname
            platform_transition_filegroup(
                name = package_name,
                srcs = [":" + zip_name],
                target_platform = platform,
                compilation_mode = compilation_mode,
                target_compatible_with = architectures_target_compatible_with[shortname],
            )

def platform_prefix(t):
    return select({
        "@rules_bzlmodrio_toolchains//conditions:linux_x86_64": "linux/x86-64/" + t,
        "@rules_bzlmodrio_toolchains//conditions:osx": "osx/universal/" + t,
        "@rules_bzlmodrio_toolchains//conditions:windows_arm64": "windows/arm64/" + t,
        "@rules_bzlmodrio_toolchains//conditions:windows_x86_64": "windows/x86-64/" + t,
        "@rules_bzlmodrio_toolchains//constraints/is_roborio:roborio": "linux/roborio/" + t,
        "@rules_bzlmodrio_toolchains//constraints/is_systemcore:systemcore": "linux/systemcore/" + t,
    })

def _wpilib_maven_export_impl(
        name,
        maven_coordinates,
        classifier_artifacts,
        linux_artifacts,
        osx_artifacts,
        windows_artifacts,
        lib_name = None,
        visibility = None):
    """Implementation of wpilib_maven_export."""
    all_artifacts = {}
    all_artifacts.update(classifier_artifacts)

    if HOST_OS == "linux":
        all_artifacts.update(linux_artifacts)
    elif HOST_OS == "osx":
        all_artifacts.update(osx_artifacts)
    elif HOST_OS == "windows":
        all_artifacts.update(windows_artifacts)

    if CLASSIFIER_FILTER:
        allowed_classifiers = CLASSIFIER_FILTER.split(",")
        filtered_artifacts = {
            key: value
            for key, value in all_artifacts.items()
            if key in allowed_classifiers
        }
    else:
        filtered_artifacts = all_artifacts

    maven_export(
        name = name,
        maven_coordinates = maven_coordinates,
        classifier_artifacts = filtered_artifacts,
        lib_name = lib_name,
        visibility = visibility,
    )

wpilib_maven_export = macro(
    implementation = _wpilib_maven_export_impl,
    attrs = {
        "classifier_artifacts": attr.string_keyed_label_dict(default = {}, configurable = False),
        "lib_name": attr.string(configurable = False),
        "linux_artifacts": attr.string_keyed_label_dict(default = {}, configurable = False),
        "maven_coordinates": attr.string(mandatory = True, configurable = False),
        "osx_artifacts": attr.string_keyed_label_dict(default = {}, configurable = False),
        "windows_artifacts": attr.string_keyed_label_dict(default = {}, configurable = False),
    },
    doc = "A symbolic macro that wraps maven_export for wpilib.",
)
