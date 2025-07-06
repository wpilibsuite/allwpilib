load("@bazel_skylib//rules:write_file.bzl", "write_file")
load("@rules_jvm_external//:defs.bzl", "maven_export")
load("@rules_pkg//pkg:zip.bzl", "pkg_zip")
load("@rules_shell//shell:sh_binary.bzl", "sh_binary")
load("//shared/bazel/rules:transitions.bzl", "platform_transition_filegroup")

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
            "@rules_bzlmodrio_toolchains//platforms/windows_arm64": "windows-arm64",
            "@rules_bzlmodrio_toolchains//platforms/windows_x86_64": "windows-x86-64",
            "@rules_bzlmodrio_toolchains//platforms/systemcore": "systemcore",
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
    is_platform_specific = linux_artifacts or osx_artifacts or windows_artifacts

    if not is_platform_specific:
        maven_export(
            name = name,
            maven_coordinates = maven_coordinates,
            classifier_artifacts = classifier_artifacts,
            lib_name = lib_name,
            visibility = visibility,
        )
        return

    common_args = {"visibility": visibility}
    if lib_name:
        common_args["lib_name"] = lib_name

    all_linux_artifacts = {}
    all_linux_artifacts.update(classifier_artifacts)
    all_linux_artifacts.update(linux_artifacts)

    all_osx_artifacts = {}
    all_osx_artifacts.update(classifier_artifacts)
    all_osx_artifacts.update(osx_artifacts)

    all_windows_artifacts = {}
    all_windows_artifacts.update(classifier_artifacts)
    all_windows_artifacts.update(windows_artifacts)

    maven_export(
        name = name + "-linux",
        maven_coordinates = maven_coordinates,
        classifier_artifacts = all_linux_artifacts,
        target_compatible_with = ["@platforms//os:linux"],
        **common_args
    )

    maven_export(
        name = name + "-osx",
        maven_coordinates = maven_coordinates,
        classifier_artifacts = all_osx_artifacts,
        target_compatible_with = ["@platforms//os:osx"],
        **common_args
    )

    maven_export(
        name = name + "-windows",
        maven_coordinates = maven_coordinates,
        classifier_artifacts = all_windows_artifacts,
        target_compatible_with = ["@platforms//os:windows"],
        **common_args
    )

    native.alias(
        name = name + ".publish",
        actual = select({
            "@platforms//os:linux": name + "-linux.publish",
            "@platforms//os:osx": name + "-osx.publish",
            "@platforms//os:windows": name + "-windows.publish",
        }),
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
