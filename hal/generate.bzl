def __generate_hal_impl(ctx):
    """
    Custom rule used to create the hal pre-generated files. See `./README-Bazel.md` for the reasoning.
    """
    output_dir = ctx.actions.declare_directory("_gendir")

    args = ctx.actions.args()
    args.add("--output_directory", output_dir.path + "/main/native/cpp/mrc/protobuf")
    args.add("--nanopb", ctx.executable._nanopb_generator)

    ctx.actions.run(
        inputs = ctx.attr._templates.files.to_list() + ctx.attr.proto_files.files.to_list(),
        outputs = [output_dir],
        executable = ctx.executable._tool,
        arguments = [args],
        tools = [ctx.executable._protoc, ctx.executable._nanopb_generator],
    )

    return [DefaultInfo(files = depset([output_dir]))]

generate_hal = rule(
    implementation = __generate_hal_impl,
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
        "_protoc": attr.label(
            default = Label("@com_google_protobuf//:protoc"),
            cfg = "exec",
            executable = True,
        ),
        "_templates": attr.label(
            default = Label("//hal:templates"),
        ),
        "_tool": attr.label(
            default = Label("//hal:generate_nanopb"),
            cfg = "exec",
            executable = True,
        ),
    },
)
