def _generate_fields_impl(ctx):
    """
    Custom rule used to create the fields pre-generated files. See `./README-Bazel.md` for the reasoning.
    """
    output_dir = ctx.actions.declare_directory("_gendir")

    protoc = ctx.toolchains["@protobuf//bazel/private:proto_toolchain_type"].proto.proto_compiler

    args = ctx.actions.args()
    args.add("--output_directory", output_dir.path)
    args.add("--resources_root", "fields/src/main/native/resources")
    args.add("--template_root", "fields/src/generate")
    args.add("--proto_directory", "fields/src/main/proto")
    args.add("--protoc", protoc.executable)
    args.add("--quickbuf_plugin", ctx.executable._quickbuf)
    args.add("--nanopb", ctx.executable._nanopb)

    ctx.actions.run(
        inputs = depset(transitive = [
            ctx.attr._field_jsons.files,
            ctx.attr._templates.files,
            ctx.attr.proto_files.files,
        ]),
        outputs = [output_dir],
        executable = ctx.executable._tool,
        arguments = [args],
        tools = [protoc, ctx.executable._nanopb, ctx.executable._quickbuf],
    )

    return [DefaultInfo(files = depset([output_dir]))]

generate_fields = rule(
    implementation = _generate_fields_impl,
    attrs = {
        "proto_files": attr.label(
            allow_files = True,
            default = Label("//fields:proto_files"),
        ),
        "_field_jsons": attr.label(
            default = Label("//fields:field-jsons"),
        ),
        "_nanopb": attr.label(
            default = Label("//wpiutil:nanopb_generator"),
            cfg = "exec",
            executable = True,
        ),
        "_quickbuf": attr.label(
            default = Label("//:quickbuf_protoc"),
            cfg = "exec",
            executable = True,
        ),
        "_templates": attr.label(
            default = Label("//fields:templates"),
        ),
        "_tool": attr.label(
            default = Label("//fields:generate_fields_py"),
            cfg = "exec",
            executable = True,
        ),
    },
    toolchains = ["@protobuf//bazel/private:proto_toolchain_type"],
)
