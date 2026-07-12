def _generate_version_file_impl(ctx):
    out = ctx.actions.declare_file(ctx.attr.output_file)
    wpilib_version = ctx.var.get("WPILIB_VERSION")
    ctx.actions.expand_template(
        output = out,
        template = ctx.file.template,
        substitutions = {"${wpilib_version}": wpilib_version},
    )
    return [DefaultInfo(files = depset([out]))]

generate_version_file = rule(
    implementation = _generate_version_file_impl,
    attrs = {
        "output_file": attr.string(mandatory = True),
        "template": attr.label(
            allow_single_file = True,
            mandatory = True,
        ),
    },
)
