"""Toolchain resolution for actions that import a target-built extension."""

STUBGEN_TOOLCHAIN_TYPE = "//shared/bazel/rules/robotpy:stubgen_toolchain_type"

def _stubgen_toolchain_impl(_ctx):
    return [platform_common.ToolchainInfo()]

stubgen_toolchain = rule(implementation = _stubgen_toolchain_impl)

def stubgen_toolchains():
    native.toolchain_type(name = "stubgen_toolchain_type")
    stubgen_toolchain(name = "stubgen_toolchain")

    # The toolchain supplies no tools: it lets Bazel prefer an executor that
    # can run the target's Python runtime and load its compiled extension.
    for os_name in ["linux", "osx"]:
        for cpu in ["x86_64", "aarch64"]:
            constraints = ["@platforms//os:" + os_name, "@platforms//cpu:" + cpu]
            native.toolchain(
                name = "stubgen_{}_{}".format(os_name, cpu),
                exec_compatible_with = constraints,
                target_compatible_with = constraints,
                toolchain = ":stubgen_toolchain",
                toolchain_type = STUBGEN_TOOLCHAIN_TYPE,
            )
