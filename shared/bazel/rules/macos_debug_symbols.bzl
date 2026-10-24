"""Linked DWARF for published macOS shared libraries."""

load("@build_bazel_apple_support//lib:apple_support.bzl", "apple_support")
load("@rules_cc//cc:defs.bzl", "CcInfo")

def _macos_debug_symbols_impl(ctx):
    library = ctx.attr.shared_library[OutputGroupInfo].main_shared_library_output.to_list()[0]
    debug = ctx.actions.declare_file(ctx.label.name + ".debug")

    # Mach-O libraries contain a debug map, not linked DWARF. Declare the
    # transitive objects and archives so dsymutil can read that map in its own
    # sandbox, including when the compile/link actions came from a remote cache.
    linker_inputs = depset(transitive = [
        dep[CcInfo].linking_context.linker_inputs
        for dep in ctx.attr.deps
    ])
    inputs = [library]
    for linker_input in linker_inputs.to_list():
        for lib in linker_input.libraries:
            for archive in [lib.static_library, lib.pic_static_library]:
                if archive != None:
                    inputs.append(archive)
            inputs.extend(lib.objects or [])
            inputs.extend(lib.pic_objects or [])

    args = ctx.actions.args()
    args.add("dsymutil")
    args.add_all(["--flat", "--no-swiftmodule-timestamp", "-o", debug, library])
    apple_support.run(
        actions = ctx.actions,
        apple_fragment = ctx.fragments.apple,
        xcode_config = ctx.attr._xcode_config[apple_common.XcodeVersionConfig],
        executable = "/usr/bin/xcrun",
        arguments = [args],
        inputs = depset(inputs),
        outputs = [debug],
        mnemonic = "MacosDsym",
        progress_message = "Linking macOS debug symbols for %{label}",
    )
    return [DefaultInfo(files = depset([debug]))]

macos_debug_symbols = rule(
    implementation = _macos_debug_symbols_impl,
    attrs = apple_support.action_required_attrs() | {
        "deps": attr.label_list(providers = [CcInfo], doc = "The shared library's static dependencies."),
        "shared_library": attr.label(mandatory = True, doc = "The cc_shared_library whose debug map to read."),
    },
    fragments = ["apple"],
)
