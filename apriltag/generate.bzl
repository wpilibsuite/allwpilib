def __generate_apriltag_impl(ctx):
    """
    Custom rule used to create the apriltag pre-generated files. See `./README-Bazel.md` for the reasoning.
    """
    output_dir = ctx.actions.declare_directory("_gendir")

    args = ctx.actions.args()
    args.add("--output_directory", output_dir.path)
    args.add("--protoc", ctx.executable._protoc)
    args.add("--quickbuf_plugin", ctx.executable._quickbuf)
    args.add("--nanopb_generator", ctx.executable._nanopb_generator)

    ctx.actions.run(
        inputs = ctx.attr.proto_files.files.to_list(),
        outputs = [output_dir],
        executable = ctx.executable._tool,
        arguments = [args],
        tools = [ctx.executable._protoc, ctx.executable._nanopb_generator, ctx.executable._quickbuf],
    )

    return [DefaultInfo(files = depset([output_dir]))]

generate_apriltag = rule(
    implementation = __generate_apriltag_impl,
    attrs = {
        "proto_files": attr.label(
            allow_files = True,
            default = Label("//apriltag:proto_files"),
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
        "_quickbuf": attr.label(
            default = Label("//:quickbuf_protoc"),
            cfg = "exec",
            executable = True,
        ),
        "_tool": attr.label(
            default = Label("//apriltag:generate_apriltag_py"),
            cfg = "exec",
            executable = True,
        ),
    },
)
