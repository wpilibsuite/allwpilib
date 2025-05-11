load("@rules_python//python:defs.bzl", "py_binary")

def bundle_library_artifacts(
        name,
        maven_base,
        library_base_name = "",
        cc_hdr_pkg = None,
        cc_src_pkg = None,
        cc_static_library_pkg = None,
        java_pkg = None,
        jni_pkg = None):
    srcs = []

    cmd = "$(locations //shared/bazel/rules/publishing:generate_maven_bundle) --output_file=$(OUTS) --maven_infos "

    # TODO(pjreiniger) Make this cross platform
    platform = "linuxx86-64"

    def add_cc_arg(package_name, suffix):
        return "$(locations " + package_name + ")," + maven_base + "," + library_base_name + "-cpp," + suffix + " "

    if cc_hdr_pkg:
        srcs.append(cc_hdr_pkg)
        cmd += add_cc_arg(cc_hdr_pkg, "-headers")

    if cc_src_pkg:
        srcs.append(cc_src_pkg)
        cmd += add_cc_arg(cc_src_pkg, "-sources")

    if cc_static_library_pkg:
        srcs.append(cc_static_library_pkg)
        cmd += add_cc_arg(cc_static_library_pkg, "-" + platform + "static")

    if jni_pkg:
        srcs.append(jni_pkg)
        cmd += "$(locations " + jni_pkg + ")," + maven_base + "," + library_base_name + "-jni,-" + platform + " "

    output_file = name + "-maven-info.json"

    native.genrule(
        name = name,
        srcs = srcs,
        outs = [output_file],
        cmd = cmd,
        tools = ["//shared/bazel/rules/publishing:generate_maven_bundle"],
        visibility = ["//visibility:public"],
        tags = ["manual"],
    )

def bundle_default_jni_library(
        name,
        library_base_name,
        maven_base):
    bundle_library_artifacts(
        name = "publishing_bundle",
        maven_base = maven_base,
        library_base_name = library_base_name,
        cc_hdr_pkg = ":{}.static-hdrs-zip".format(library_base_name),
        cc_src_pkg = ":{}.static-srcs-zip".format(library_base_name),
        cc_static_library_pkg = ":{}.static-zip".format(library_base_name),
        jni_pkg = ":{}jni-zip".format(library_base_name),
    )

def wpilib_publish(
        name,
        bundles):
    py_binary(
        name = name,
        srcs = ["//shared/bazel/rules/publishing:publish.py"],
        args = ["--bundles "] + ["$(location " + x + ") " for x in bundles],
        data = bundles,
        deps = ["@rules_python//python/runfiles"],
    )
