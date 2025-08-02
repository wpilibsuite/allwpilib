"""Starlark rule for generating a version script from a symbols file."""

def _gen_versionscript_impl(ctx):
    ext = ".txt"
    if ctx.attr.format == "windows":
        ext = ".def"
    elif ctx.attr.format == "osx":
        ext = ".list"

    output_file = ctx.actions.declare_file(ctx.label.name + ext)

    ctx.actions.run(
        outputs=[output_file],
        inputs=[ctx.file.src],
        executable=ctx.executable._tool,
        arguments=[
            "--input",
            ctx.file.src.path,
            "--output",
            output_file.path,
            "--lib_name",
            ctx.attr.lib_name,
            "--format",
            ctx.attr.format,
        ],
        mnemonic="GenVersionScript",
        progress_message="Generating version script for %{label}",
    )

    return [DefaultInfo(files=depset([output_file]), runfiles=ctx.runfiles(files=[output_file])), OutputGroupInfo(version_script_file = depset([output_file]))]

gen_versionscript = rule(
    implementation=_gen_versionscript_impl,
    attrs={
        "src": attr.label(
            mandatory=True,
            allow_single_file=True,
            doc="The input symbols file.",
        ),
        "lib_name": attr.string(
            mandatory=True,
            doc="The name of the library.",
        ),
        "format": attr.string(
            mandatory=True,
            values=["linux", "windows", "osx"],
            doc="The output format.",
        ),
        "_tool": attr.label(
            default=Label("//shared/bazel/rules/gen:gen_versionscript"),
            executable=True,
            cfg="exec",
        ),
    },
    doc="Generates a version script from a symbols file.",
)
