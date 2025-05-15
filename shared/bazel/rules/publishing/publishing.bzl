load("@rules_python//python:defs.bzl", "py_binary")

def generate_maven_info_cmd(
        artifact,
        group_id,
        artifact_name,
        suffix = ""):
    """
    Helper function used to format the maven info into a CSV that can be parsed by the python tool.
    """
    return "$(locations {}),{},{},{} ".format(artifact, group_id, artifact_name, suffix)

def bundle_library_artifacts(
        name,
        group_id,
        library_base_name = "",
        cc_hdr_pkg = None,
        cc_src_pkg = None,
        cc_static_library_pkg = None,
        java_pkg = None,
        jni_pkg = None,
        add_cc_suffix = True):
    """
    Helper function for creating a file containing the info used to later publish maven libraries

    This generates a json file (name + "-maven-info.json") containing the relevant information to laster publish this library
    to maven coordinates. These can be gathered together to be published en masse with the wpilib_publish rule

    Params
        group_id: The groupd id used for determining the publihsing location
        library_base_name: The root name for all of the artifacts. Will be appended based on the artifact being published
        cc_hdr_pkg: Optional. A pkg_files target used to indcate there is are C++ headers to be published
        cc_src_pkg: Optional. A pkg_files target used to indicate there are C++ sources to be published
        cc_static_library_pkg: Optional. A pkg_files target used to indicate there is a compiled C++ library to be published
        jni_pkg: Optional. A pkg_files target used to indicate there is a compiled C++ JNI library to be published
    """
    srcs = []

    cmd = "$(locations //shared/bazel/rules/publishing:generate_maven_bundle) --output_file=$(OUTS) --maven_infos "
    maybe_cc_suffix = "-cpp" if add_cc_suffix else ""

    # TODO(pjreiniger) Make this cross platform
    platform = "linuxx86-64"

    if cc_hdr_pkg:
        srcs.append(cc_hdr_pkg)
        cmd += generate_maven_info_cmd(cc_hdr_pkg, group_id, library_base_name + maybe_cc_suffix, "-headers")

    if cc_src_pkg:
        srcs.append(cc_src_pkg)
        cmd += generate_maven_info_cmd(cc_src_pkg, group_id, library_base_name + maybe_cc_suffix, "-sources")

    if cc_static_library_pkg:
        srcs.append(cc_static_library_pkg)
        cmd += generate_maven_info_cmd(cc_static_library_pkg, group_id, library_base_name + maybe_cc_suffix, "-" + platform + "static")

    if jni_pkg:
        srcs.append(jni_pkg)
        cmd += generate_maven_info_cmd(jni_pkg, group_id, library_base_name + "-jni", "-" + platform)

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
        group_id):
    """
    Helper function to help bundle files for a standard allwpilib JNI library.

    Due to the standard layout and naming convention of the JNI libraries, this function provides
    syntactic sugar to bundle all of the pieces relevant to a C++ / Java / JNI library to maven.
    """
    bundle_library_artifacts(
        name = "publishing_bundle",
        group_id = group_id,
        library_base_name = library_base_name,
        cc_hdr_pkg = ":{}.static-hdrs-zip".format(library_base_name),
        cc_src_pkg = ":{}.static-srcs-zip".format(library_base_name),
        cc_static_library_pkg = ":{}.static-zip".format(library_base_name),
        jni_pkg = ":{}jni-zip".format(library_base_name),
    )

def wpilib_publish(
        name,
        bundles):
    """
    Helper function that takes a list of bundled maven information and publishes it to an external location.

    This will take a list of json files containing maven publishing infomration, created by the
    //shared/bazel/rules/publishing:generate_maven_bundle target, and publish to a directory.

    See the "publish.py" library for a description of the available options

    Params
        bundles: A list of json files

    """
    py_binary(
        name = name,
        srcs = ["//shared/bazel/rules/publishing:publish.py"],
        args = ["--bundles "] + ["$(location " + x + ") " for x in bundles],
        data = bundles,
        deps = ["@rules_python//python/runfiles"],
    )
