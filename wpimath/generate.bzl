def __generate_wpimath_impl(ctx):
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

generate_wpimath = rule(
    implementation = __generate_wpimath_impl,
    attrs = {
        "_templates": attr.label(
            default = Label("//wpimath:templates"),
        ),
        "_tool": attr.label(
            default = Label("//wpimath:generate_numbers"),
            cfg = "exec",
            executable = True,
        ),
    },
)
