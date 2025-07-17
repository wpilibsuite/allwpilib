def __generate_wpilib_new_commands_impl(ctx):
    """
    Custom rule used to create the wpilibNewCommands pre-generated files. See `./README-Bazel.md` for the reasoning.
    """
    output_dir = ctx.actions.declare_directory("_gendir")

    args = ctx.actions.args()
    args.add("--output_directory", output_dir.path)
    args.add("--template_root", "wpilibNewCommands/src/generate")

    ctx.actions.run(
        inputs = ctx.attr._templates.files,
        outputs = [output_dir],
        executable = ctx.executable._tool,
        arguments = [args],
    )

    return [DefaultInfo(files = depset([output_dir]))]

generate_wpilib_new_commands = rule(
    implementation = __generate_wpilib_new_commands_impl,
    attrs = {
        "_templates": attr.label(
            default = Label("//wpilibNewCommands:templates"),
        ),
        "_tool": attr.label(
            default = Label("//wpilibNewCommands:generate_hids"),
            cfg = "exec",
            executable = True,
        ),
    },
)
