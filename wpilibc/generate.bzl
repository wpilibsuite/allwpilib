def __generate_wpilibc_impl(ctx):
    """
    Custom rule used to create the wpilibc pre-generated files. See `./README-Bazel.md` for the reasoning.
    """
    output_dir = ctx.actions.declare_directory("_gendir")

    args = ctx.actions.args()
    args.add("--output_directory", output_dir.path)
    args.add("--schema_root", "wpilibj/src/generate")
    args.add("--template_root", "wpilibc/src/generate")

    ctx.actions.run(
        inputs = ctx.attr._templates.files,
        outputs = [output_dir],
        executable = ctx.executable._tool,
        arguments = [args],
    )

    return [DefaultInfo(files = depset([output_dir]))]

generate_wpilibc = rule(
    implementation = __generate_wpilibc_impl,
    attrs = {
        "_templates": attr.label(
            default = Label("//wpilibc:templates"),
        ),
        "_tool": attr.label(
            default = Label("//wpilibc:generate_wpilibc_py"),
            cfg = "exec",
            executable = True,
        ),
    },
)
