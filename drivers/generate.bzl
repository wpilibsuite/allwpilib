def __generate_drivers_impl(ctx):
    """Creates the drivers pre-generated files."""
    output_dir = ctx.actions.declare_directory("_gendir")

    args = ctx.actions.args()
    args.add("--output_directory", output_dir.path)
    args.add("--template_root", "drivers/src/generate")

    ctx.actions.run(
        inputs = ctx.attr._templates.files,
        outputs = [output_dir],
        executable = ctx.executable._tool,
        arguments = [args],
    )

    return [DefaultInfo(files = depset([output_dir]))]

generate_drivers = rule(
    implementation = __generate_drivers_impl,
    attrs = {
        "_templates": attr.label(
            default = Label("//drivers:templates"),
        ),
        "_tool": attr.label(
            default = Label("//drivers:generate_pwm_motor_controllers"),
            cfg = "exec",
            executable = True,
        ),
    },
)
