"""Bazel rules for publishing maven artifacts."""

load("@com_wpilib_allwpilib_publishing_config//:publishing_config.bzl", "CLASSIFIER_FILTER")
load("@platforms//host:constraints.bzl", _host_constraints = "HOST_CONSTRAINTS")
load("@rules_jvm_external//:defs.bzl", "maven_export")
load("@rules_pkg//pkg:zip.bzl", "pkg_zip")
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

def _publish_all_impl(ctx):
    # This is a rule, not a macro, to allow the `targets` attribute to be
    # configurable with `select()`. Macros are expanded during the loading phase,
    # before `select()` is resolved, which makes this impossible with a macro.
    #
    # This rule works by generating a shell script that executes all of the publisher
    # scripts from the `targets` attribute. This runs at execution time (`bazel run`)
    # rather than build time.

    script = ctx.actions.declare_file(ctx.label.name + "_runner.sh")
    executables = [target.files_to_run.executable for target in ctx.attr.targets]

    script_content = """#!/bin/bash
set -e

for arg in {publish_scripts}; do
    "$arg" "$@"
done
""".format(
        publish_scripts = " ".join(["'" + e.short_path + "'" for e in executables]),
    )

    ctx.actions.write(
        output = script,
        content = script_content,
        is_executable = True,
    )

    # The publisher scripts have their own dependencies (runfiles), which must be
    # collected and merged so they are available when our runner script executes.
    all_runfiles = [ctx.runfiles(files = executables)]
    for target in ctx.attr.targets:
        all_runfiles.append(target[DefaultInfo].default_runfiles)

    return [
        DefaultInfo(
            runfiles = ctx.runfiles().merge_all(all_runfiles),
            executable = script,
        ),
    ]

publish_all = rule(
    doc = "Rule to publish multiple maven artifacts with a single `bazel run` command.",
    implementation = _publish_all_impl,
    attrs = {
        "targets": attr.label_list(
            doc = "A list of targets to publish. These are typically the `.publish` targets from `maven_export`.",
            providers = [DefaultInfo],
        ),
    },
    executable = True,
)

host_architectures = {
    "@allwpilib//shared/bazel:linux_x86_64": "linux-x86-64",
    "@allwpilib//shared/bazel:osx": "osxuniversal",
    "@allwpilib//shared/bazel:windows_arm64": "windows-arm64",
    "@allwpilib//shared/bazel:windows_x86_64": "windows-x86-64",
}

# Unfortunately, rules_jvm_external really wants each of the classifier
# artifacts to have a unique name so they can go in the runfiles together and
# not collide.
def architectures_pkg_zip(
        name,
        compilation_modes = ["dbg", "opt"],
        architectures = None,
        **kwargs):
    if architectures == None:
        architectures = {
            "@allwpilib//shared/bazel:linux_x86_64": "linux-x86-64",
            "@allwpilib//shared/bazel:osx": "osxuniversal",
            "@allwpilib//shared/bazel:systemcore": "systemcore",
            "@allwpilib//shared/bazel:windows_arm64": "windows-arm64",
            "@allwpilib//shared/bazel:windows_x86_64": "windows-x86-64",
        }

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
                tags = ["manual"],
                **kwargs
            )

            package_name = name + "-" + compilation_mode + "-" + shortname
            platform_transition_filegroup(
                name = package_name,
                srcs = [":" + zip_name],
                target_platform = platform,
                compilation_mode = compilation_mode,
                target_compatible_with = architectures_target_compatible_with[shortname],
                tags = ["manual"],
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
