def __generate_datalog_impl(ctx):
    """Creates the datalog pre-generated files."""
    output_dir = ctx.actions.declare_directory("_gendir")

    args = ctx.actions.args()
    args.add("--output_directory", output_dir.path)
    args.add("--template_root", "datalog/src/generate")

    ctx.actions.run(
        inputs = ctx.attr._templates.files,
        outputs = [output_dir],
        executable = ctx.executable._tool,
        arguments = [args],
    )

    return [DefaultInfo(files = depset([output_dir]))]

generate_datalog = rule(
    implementation = __generate_datalog_impl,
    attrs = {
        "_templates": attr.label(
            default = Label("//datalog:templates"),
        ),
        "_tool": attr.label(
            default = Label("//datalog:generate_datalog_entries"),
            cfg = "exec",
            executable = True,
        ),
    },
)
