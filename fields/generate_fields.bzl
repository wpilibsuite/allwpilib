def _generate_fields_impl(ctx):
    output_dir = ctx.actions.declare_directory("_gendir")

    args = ctx.actions.args()
    args.add("--output_directory", output_dir.path)
    args.add("--resources_root", "fields/src/main/native/resources")
    args.add("--template_root", "fields/src/generate")

    ctx.actions.run(
        inputs = depset(transitive = [
            ctx.attr._field_jsons.files,
            ctx.attr._templates.files,
        ]),
        outputs = [output_dir],
        executable = ctx.executable._tool,
        arguments = [args],
    )

    return [DefaultInfo(files = depset([output_dir]))]

generate_fields = rule(
    implementation = _generate_fields_impl,
    attrs = {
        "_field_jsons": attr.label(
            default = Label("//fields:field-jsons"),
        ),
        "_templates": attr.label(
            default = Label("//fields:templates"),
        ),
        "_tool": attr.label(
            default = Label("//fields:generate_fields_script"),
            cfg = "exec",
            executable = True,
        ),
    },
)
