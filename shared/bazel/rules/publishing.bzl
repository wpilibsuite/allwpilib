load("@bazel_skylib//rules:write_file.bzl", "write_file")
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

def linux_architectures_pkg_zip(
        name,
        compilation_modes = ["dbg", "opt"],
        architectures = {
            "//shared/bazel/rules:linux_x86_64": "x86-64",
            #"@rules_bzlmodrio_toolchains//platforms/roborio": "roborio",
            "@rules_bzlmodrio_toolchains//platforms/systemcore": "systemcore",
        },
        **kwargs):
    architectures_pkg_zip(
        name,
        compilation_modes,
        architectures,
        target_compatible_with = ["@platforms//os:linux"],
        **kwargs
    )

# Unfortunately, rules_jvm_external really wants each of the classifier
# artifacts to have a unique name so they can go in the runfiles together and
# not collide.
def architectures_pkg_zip(
        name,
        compilation_modes,
        architectures,
        **kwargs):
    zips = []
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
            )
            zips.append(package_name)

    native.filegroup(
        name = name,
        srcs = zips,
    )

def platform_prefix(t):
    return select({
        "//shared/bazel/rules:is_linux_x86_64": "linux/x86-64/" + t,
        "@rules_bzlmodrio_toolchains//constraints/is_roborio:roborio": "linux/roborio/" + t,
        "@rules_bzlmodrio_toolchains//constraints/is_systemcore:systemcore": "linux/systemcore/" + t,
    })
