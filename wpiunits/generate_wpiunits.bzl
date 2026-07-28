def __generate_wpiunits_impl(ctx):
    """
    Custom rule used to create the wpiunits pre-generated files. See `./README-Bazel.md` for the reasoning.
    """
    output_dir = ctx.actions.declare_directory("_gendir")

    args = ctx.actions.args()
    args.add("--output_directory", output_dir.path)

    ctx.actions.run(
        outputs = [output_dir],
        executable = ctx.executable._tool,
        arguments = [args],
    )

    return [DefaultInfo(files = depset([output_dir]))]

generate_wpiunits = rule(
    implementation = __generate_wpiunits_impl,
    attrs = {
        "_tool": attr.label(
            default = Label("//wpiunits:generate_units"),
            cfg = "exec",
            executable = True,
        ),
    },
)
