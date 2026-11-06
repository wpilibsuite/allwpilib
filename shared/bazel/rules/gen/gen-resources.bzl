def __generate_resources_impl(ctx):
    """
    Custom rule used to embed resource files into C++ source files.
    """
    output_files = [ctx.actions.declare_file("{}.cpp".format(input_file.path)) for input_file in ctx.files.resource_files]

    args = ctx.actions.args()
    args.add("--prefix", ctx.attr.prefix)
    args.add("--namespace", ctx.attr.namespace)
    args.add_all("--resources", ctx.files.resource_files)
    args.add_all("--output_files", output_files)

    ctx.actions.run(
        inputs = ctx.files.resource_files,
        outputs = output_files,
        executable = ctx.executable._tool,
        arguments = [args],
    )

    return [DefaultInfo(files = depset(output_files))]

generate_resources = rule(
    implementation = __generate_resources_impl,
    attrs = {
        "namespace": attr.string(mandatory = True),
        "prefix": attr.string(mandatory = True),
        "resource_files": attr.label_list(allow_files = True),
        "_tool": attr.label(
            default = Label("//shared/bazel/rules/gen:gen_resources"),
            cfg = "exec",
            executable = True,
        ),
    },
)
