def __generate_wpiutil_impl(ctx):
    """
    Custom rule used to create the wpiutil pre-generated files. See `./README-Bazel.md` for the reasoning.
    """
    output_dir = ctx.actions.declare_directory("_gendir")

    args = ctx.actions.args()
    args.add("--output_directory", output_dir.path)
    args.add("--nanopb", ctx.executable._nanopb_generator)

    ctx.actions.run(
        inputs = ctx.attr.proto_files.files,
        outputs = [output_dir],
        executable = ctx.executable._tool,
        arguments = [args],
        tools = [ctx.executable._nanopb_generator],
    )

    return [DefaultInfo(files = depset([output_dir]))]

generate_wpiutil = rule(
    implementation = __generate_wpiutil_impl,
    attrs = {
        "proto_files": attr.label(
            allow_files = True,
            mandatory = True,
        ),
        "_nanopb_generator": attr.label(
            default = Label("//wpiutil:nanopb_generator"),
            cfg = "exec",
            executable = True,
        ),
        "_tool": attr.label(
            default = Label("//wpiutil:generate_nanopb"),
            cfg = "exec",
            executable = True,
        ),
    },
)
