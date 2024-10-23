def generate_resources(name, resource_files, prefix, namespace, visibility = None):
    cmd = "$(locations //shared/bazel/rules/gen:gen_resources) --prefix={} --namespace={} --resources $(SRCS) --output_files $(OUTS)".format(prefix, namespace, resource_files)
    native.genrule(
        name = name,
        srcs = resource_files,
        outs = ["{}.cpp".format(input_file) for input_file in resource_files],
        cmd = cmd,
        tools = ["//shared/bazel/rules/gen:gen_resources"],
        visibility = visibility,
    )
