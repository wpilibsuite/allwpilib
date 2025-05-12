def __generate_ntcore_impl(ctx):
    """
    Custom rule used to create the ntcore pre-generated files. See `./README-Bazel.md` for the reasoning.
    """
    output_dir = ctx.actions.declare_directory("_gendir")

    args = ctx.actions.args()
    args.add("--output_directory", output_dir.path)

    ctx.actions.run(
        inputs = ctx.attr._templates.files,
        outputs = [output_dir],
        executable = ctx.executable._tool,
        arguments = [args],
    )

    return [DefaultInfo(files = depset([output_dir]))]

generate_ntcore = rule(
    implementation = __generate_ntcore_impl,
    attrs = {
        "_templates": attr.label(
            default = Label("//ntcore:templates"),
        ),
        "_tool": attr.label(
            default = Label("//ntcore:generate_topics"),
            cfg = "exec",
            executable = True,
        ),
    },
)
