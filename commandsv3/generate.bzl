def __generate_commandsv3_impl(ctx):
    """
    Custom rule used to create the commandsv3 pre-generated files. See `./README-Bazel.md` for the reasoning.
    """
    output_dir = ctx.actions.declare_directory("_gendir")

    args = ctx.actions.args()
    args.add("--output_directory", output_dir.path)
    args.add("--template_root", "commandsv3/src/generate")
    args.add("--protoc", ctx.executable._protoc)
    args.add("--quickbuf_plugin", ctx.executable._quickbuf)

    ctx.actions.run(
        inputs = ctx.attr._templates.files,
        outputs = [output_dir],
        executable = ctx.executable._tool,
        arguments = [args],
        tools = [ctx.executable._protoc, ctx.executable._quickbuf],
    )

    return [DefaultInfo(files = depset([output_dir]))]

generate_commandsv3 = rule(
    implementation = __generate_commandsv3_impl,
    attrs = {
        "_protoc": attr.label(
            default = Label("@com_google_protobuf//:protoc"),
            cfg = "exec",
            executable = True,
        ),
        "_quickbuf": attr.label(
            default = Label("//:quickbuf_protoc"),
            cfg = "exec",
            executable = True,
        ),
        "_templates": attr.label(
            default = Label("//commandsv3:templates"),
        ),
        "_tool": attr.label(
            default = Label("//commandsv3:generate_hids"),
            cfg = "exec",
            executable = True,
        ),
    },
)
