load("@build_bazel_apple_support//rules:universal_binary.bzl", "universal_binary")
load("@rules_cc//cc:action_names.bzl", "CPP_LINK_STATIC_LIBRARY_ACTION_NAME", "OBJ_COPY_ACTION_NAME", "STRIP_ACTION_NAME")
load("@rules_cc//cc:cc_shared_library.bzl", "cc_shared_library")
load("@rules_cc//cc:defs.bzl", "CcInfo", "cc_library")
load("@rules_cc//cc:find_cc_toolchain.bzl", "CC_TOOLCHAIN_ATTRS", "find_cpp_toolchain", "use_cc_toolchain")
load("@rules_cc//cc/common:cc_common.bzl", "cc_common")
load("@rules_pkg//:mappings.bzl", "pkg_files")
load("@rules_pkg//:pkg.bzl", "pkg_tar", "pkg_zip")

def _split_debug_symbols_impl(ctx):
    label = ctx.attr.shared_library.label
    target_name = label.name
    if label.package:
        target_name = label.package + "/" + label.name
    folder, lib_name = _folder_prefix(target_name)

    debug_suffix = "d" if ctx.attr.use_debug_name else ""

    lib = ctx.actions.declare_file(folder + "/split/lib" + lib_name + debug_suffix + ".so")

    if ctx.attr.copy:
        ctx.actions.run_shell(
            command = " ".join([
                "cp",
                ctx.file.shared_library.path,
                lib.path,
            ]),
            inputs = depset(
                direct = [ctx.file.shared_library],
            ),
            outputs = [lib],
        )

        return [
            DefaultInfo(files = depset([lib])),
            OutputGroupInfo(
                default = depset([lib]),
                stripped_file = depset([lib]),
            ),
        ]
    else:
        debug = ctx.actions.declare_file(folder + "/split/lib" + lib_name + debug_suffix + ".so.debug")
        cc_toolchain = find_cpp_toolchain(ctx)

        feature_configuration = cc_common.configure_features(
            ctx = ctx,
            cc_toolchain = cc_toolchain,
            requested_features = ctx.features,
            unsupported_features = ctx.disabled_features,
        )

        objcopy = cc_common.get_tool_for_action(
            feature_configuration = feature_configuration,
            action_name = OBJ_COPY_ACTION_NAME,
        )

        strip = cc_common.get_tool_for_action(
            feature_configuration = feature_configuration,
            action_name = STRIP_ACTION_NAME,
        )

        # This is the set of commands we want to implement to strip debug symbols out and link them back together:
        #   objcopy --only-keep-debug libmy-library.so libmy-library.so.debug
        #   strip --strip-debug libmy-library.so
        #   objcopy --strip-debug libmy-library.so

        ctx.actions.run_shell(
            command = " ".join([
                "cp",
                ctx.file.shared_library.path,
                lib.path,
                "&& chmod u+w",
                lib.path,
                "&&",
                objcopy,
                "--only-keep-debug",
                lib.path,
                debug.path,
                "&&",
                strip,
                "--strip-debug",
                lib.path,
                "&&",
                objcopy,
                "--strip-debug",
                lib.path,
            ]),
            inputs = depset(
                direct = [ctx.file.shared_library],
                transitive = [
                    cc_toolchain.all_files,
                ],
            ),
            outputs = [lib, debug],
        )

        return [
            DefaultInfo(files = depset([lib, debug])),
            OutputGroupInfo(
                default = depset([lib, debug]),
                stripped_file = depset([lib]),
                debug_file = depset([debug]),
            ),
        ]

_split_debug_symbols = rule(
    implementation = _split_debug_symbols_impl,
    attrs = {
        "copy": attr.bool(mandatory = True),
        "shared_library": attr.label(mandatory = True, allow_single_file = True),
        "use_debug_name": attr.bool(mandatory = True),
    } | CC_TOOLCHAIN_ATTRS,
    fragments = ["cpp"],
    toolchains = use_cc_toolchain(),
)

def _folder_prefix(name):
    if "/" in name:
        last_slash = name.rfind("/")
        return (name[0:last_slash], name[last_slash + 1:])
    else:
        return ("", name)

def third_party_cc_lib_helper(
        name,
        include_root,
        src_root = None,
        src_excludes = [],
        visibility = None):
    """
    Helper for src / headers pairs that aren't directly compiled, but rather pulled into a bigger library.

    Due to allwpilibs directory structure of includes and sources living next to eachother, it often is required
    to make a header shim to deal with the include path, and a filegroup of the sources. This pattern is extermely
    common for the thirdparty libraries that live beneath certain libraries.

    This will produce a library shim with the include path stripped, a filegroup of sources, and packages that can be
    used to downstream to zip headers / sources with their "parent" library.

    Params
        include_root: The package relative path to the header files. This will be used to glob the files and strip the include prefix
        src_root: Optional. The package relative path to the source files.
        src_excludes: Optional. Used to exclude files from the src_root glob
        visibilty: The visibility of header shim / source files / package files
    """
    cc_library(
        name = name + "-headers",
        hdrs = native.glob([
            include_root + "/**",
        ]),
        includes = [include_root],
        strip_include_prefix = include_root,
        visibility = visibility,
    )

    pkg_files(
        name = name + "-hdrs-pkg",
        srcs = native.glob([include_root + "/**"]),
        strip_prefix = include_root,
    )

    if src_root:
        native.filegroup(
            name = name + "-srcs",
            srcs = native.glob([src_root + "/**"], exclude = src_excludes),
            visibility = visibility,
        )

        pkg_files(
            name = name + "-srcs-pkg",
            srcs = native.glob([src_root + "/**"]),
            strip_prefix = src_root,
        )

def wpilib_cc_library(
        name,
        srcs = [],
        hdrs = [],
        deps = [],
        copts = [],
        third_party_libraries = [],
        third_party_header_only_libraries = [],
        extra_src_pkg_files = [],
        extra_hdr_pkg_files = [],
        include_license_files = False,
        srcs_pkg_root = "src/main/native/cpp",
        hdrs_pkg_root = "src/main/native/include",
        strip_include_prefix = None,
        linkopts = None,
        **kwargs):
    """
    This function is used to ease the creation of a cc_library with publishing given the standard allwpilib directory structure.

    This will create a cc_library as well as automatically create header, source, and library artifacts that can be used for publishing.  This
    also provides some syntactic sugar for third party library shims declared by third_party_cc_lib_helper.

    Important outputs:
        ":name" - The cc_library
        name + "-srcs-zip" - A zip file containing all the exported sources
        name + "-hdrs-zip" - A zip file containing all the exported headers
        name + "-zip" - A zip file that contains the compiled library

    Params:
        srcs: The sources used to compile the library. Note: This may be platform dependent and not include all the sources of the library for packaging
        hdrs: The headers used to compile the library. Note: This may be platform dependent and not include all the sources of the library for packaging
        third_party_libraries: These are helper dependencies, created by third_party_cc_lib_helper. Header shims will be added as deps and src filegroups will be added to srcs
        third_party_header_only_libraries: Similar to third_party_libraries, but for shims that contain no sources
        extra_src_pkg_files: Extra pkg_files to add to the source bundle. This is useful in the event that a library is complicated and requires
                extra, customized sources to be added to the published zip file
        extra_hdr_pkg_files: Extra pkg_files to add to the headers bundle. This is useful in the event that a library is complicated and requires
                extra, customized headers to be added to the published zip file
        include_license_files: If the header / source / library zip files should automatically includes the license files. This is used to maintain
                consistency with the gradle publishing, as not all of them export the license files.
    """
    maybe_license_pkg = ["//:license_pkg_files"] if include_license_files else []

    cc_library(
        name = name + "-headers",
        hdrs = hdrs,
        deps = [lib + "-headers" for lib in third_party_libraries + third_party_header_only_libraries],
        strip_include_prefix = strip_include_prefix,
        **kwargs
    )

    cc_library(
        name = name,
        hdrs = hdrs,
        copts = copts,
        srcs = srcs + [lib + "-srcs" for lib in third_party_libraries],
        deps = deps + [lib + "-headers" for lib in third_party_libraries + third_party_header_only_libraries],
        strip_include_prefix = strip_include_prefix,
        **kwargs
    )

    if srcs_pkg_root:
        pkg_files(
            name = name + "-srcs-pkg",
            srcs = native.glob([srcs_pkg_root + "/**"]),
            strip_prefix = srcs_pkg_root,
        )

        pkg_zip(
            name = name + "-srcs-zip",
            srcs = maybe_license_pkg + extra_src_pkg_files + [name + "-srcs-pkg"] + [lib + "-srcs-pkg" for lib in third_party_libraries],
            tags = ["no-remote"],
        )

    if hdrs_pkg_root:
        pkg_files(
            name = name + "-hdrs-pkg",
            srcs = native.glob([hdrs_pkg_root + "/**"]),
            strip_prefix = hdrs_pkg_root,
        )

        pkg_zip(
            name = name + "-hdrs-zip",
            srcs = extra_hdr_pkg_files + maybe_license_pkg + [name + "-hdrs-pkg"] + [lib + "-hdrs-pkg" for lib in third_party_libraries + third_party_header_only_libraries],
            tags = ["no-remote"],
        )

def wpilib_cc_shared_library(
        name,
        auto_export_windows_symbols = True,
        user_link_flags = None,
        visibility = None,
        use_debug_name = True,
        additional_linker_inputs = None,
        features = None,
        **kwargs):
    folder, lib = _folder_prefix(name)

    if not features:
        features = []

    features.append("windows_export_all_symbols")

    if use_debug_name:
        user_link_flags = (user_link_flags or []) + select({
            "//shared/bazel/rules:linux_compilation_mode_dbg": ["-Wl,-soname,lib" + lib + "d.so"],
            "//shared/bazel/rules:osx_compilation_mode_dbg": ["-Wl,-install_name,lib" + lib + "d.so"],
            "@platforms//os:linux": ["-Wl,-soname,lib" + lib + ".so"],
            "@platforms//os:osx": ["-Wl,-install_name,lib" + lib + ".so"],
            "//conditions:default": [],
        })
    else:
        user_link_flags = (user_link_flags or []) + select({
            "@platforms//os:linux": ["-Wl,-soname,lib" + lib + ".so"],
            "@platforms//os:osx": ["-Wl,-install_name,lib" + lib + ".so"],
            "//conditions:default": [],
        })

    cc_shared_library(
        name = name,
        user_link_flags = user_link_flags,
        features = features,
        visibility = visibility,
        **kwargs
    )

    _split_debug_symbols(
        name = name + "-symbolsplit",
        copy = select({
            "//shared/bazel/rules:is_linux_x86_64": False,
            "//conditions:default": True,
        }),
        use_debug_name = select({
            "//shared/bazel/rules:compilation_mode_dbg": True,
            "//conditions:default": False,
        }) if use_debug_name else False,
        shared_library = name,
    )

    pkg_files(
        name = folder + "/lib" + lib + "-shared-files",
        srcs = [name + "-symbolsplit"],
        visibility = visibility,
    )

CcStaticLibraryInfo = provider(
    "Information about a cc static library.",
    fields = {
        "linker_input": "the resulting linker input artifact for the static library",
        "used_objects": "the object files already accounted for",
    },
)

def _accumulate_used_objects(ctx):
    transitive_used_objects = []
    for dep in ctx.attr.static_deps:
        transitive_used_objects.append(dep[CcStaticLibraryInfo].used_objects)

    return transitive_used_objects

def _filter_inputs(
        ctx,
        feature_configuration,
        cc_toolchain,
        deps,
        used_objects):
    dependency_linker_inputs_sets = []
    for dep in deps:
        dependency_linker_inputs_sets.append(dep[CcInfo].linking_context.linker_inputs)

    dependency_linker_inputs = depset(transitive = dependency_linker_inputs_sets, order = "topological").to_list()

    used_objects_depset = depset(transitive = used_objects, order = "topological").to_list()

    linker_inputs = []
    for linker_input in dependency_linker_inputs:
        for lib in linker_input.libraries:
            if lib.pic_objects:
                for o in lib.pic_objects:
                    if o not in used_objects_depset:
                        linker_inputs.append(o)
            elif lib.objects:
                for o in lib.objects:
                    if o not in used_objects_depset:
                        linker_inputs.append(o)

    return sorted(linker_inputs)

def _cc_static_library_impl(ctx):
    """
    This is a modified version of built in cc_static_library implementation
    https://github.com/bazelbuild/bazel/blob/8.2.1/src/main/starlark/builtins_bzl/common/cc/experimental_cc_static_library.bzl

    The built in version amalgamates all of the transative dependency objects into a single shared library. However, we do not want our
    static libraries to only have the symbols related to the objects for this library, and not anything transative. In order to do this,
    we add the option to specify transative static_libraries. The rule then filters out the objects that are defines in the other static
    libraries.
    """
    deps = ctx.attr.deps

    cc_toolchain = find_cpp_toolchain(ctx)
    feature_configuration = cc_common.configure_features(
        ctx = ctx,
        cc_toolchain = cc_toolchain,
        requested_features = ctx.features + ["force_no_whole_archive"],
        unsupported_features = ctx.disabled_features,
    )

    # Find all the objects which are already in another static library.
    used_objects = _accumulate_used_objects(ctx)

    # Now, find the ones we depend on which aren't.
    libs = _filter_inputs(
        ctx,
        feature_configuration,
        cc_toolchain,
        deps,
        used_objects,
    )

    used_objects_depset = depset(direct = libs, transitive = used_objects, order = "topological")

    # Generate the output library name if one isn't provided.
    output_file = ctx.actions.declare_file(ctx.attr.static_lib_name)

    # And, now do it.
    linker_input = cc_common.create_linker_input(
        owner = ctx.label,
        libraries = depset(direct = [
            cc_common.create_library_to_link(
                actions = ctx.actions,
                feature_configuration = feature_configuration,
                cc_toolchain = cc_toolchain,
                static_library = output_file,
            ),
        ]),
    )
    compilation_context = cc_common.create_compilation_context()
    linking_context = cc_common.create_linking_context(linker_inputs = depset(direct = [linker_input], order = "topological"))

    archiver_path = cc_common.get_tool_for_action(
        feature_configuration = feature_configuration,
        action_name = CPP_LINK_STATIC_LIBRARY_ACTION_NAME,
    )
    archiver_variables = cc_common.create_link_variables(
        feature_configuration = feature_configuration,
        cc_toolchain = cc_toolchain,
        output_file = output_file.path,
        is_using_linker = False,
    )
    command_line = cc_common.get_memory_inefficient_command_line(
        feature_configuration = feature_configuration,
        action_name = CPP_LINK_STATIC_LIBRARY_ACTION_NAME,
        variables = archiver_variables,
    )

    args = ctx.actions.args()
    args.add_all(command_line)
    args.add_all(libs)

    if cc_common.is_enabled(
        feature_configuration = feature_configuration,
        feature_name = "archive_param_file",
    ):
        # TODO: The flag file arg should come from the toolchain instead.
        args.use_param_file("@%s", use_always = True)

    env = cc_common.get_environment_variables(
        feature_configuration = feature_configuration,
        action_name = CPP_LINK_STATIC_LIBRARY_ACTION_NAME,
        variables = archiver_variables,
    )

    ctx.actions.run(
        executable = archiver_path,
        arguments = [args],
        env = env,
        inputs = depset(
            direct = libs,
            transitive = [
                cc_toolchain.all_files,
            ],
        ),
        outputs = [output_file],
    )

    cc_info = cc_common.merge_cc_infos(cc_infos = [
        CcInfo(compilation_context = compilation_context, linking_context = linking_context),
    ] + [dep[CcInfo] for dep in ctx.attr.deps])

    # TODO(austin): Do we want this to be able to link into a binary?  Probably...  Need to figure out what the right result needs to be for that.

    return [
        cc_info,
        DefaultInfo(
            files = depset([output_file]),
        ),
        CcStaticLibraryInfo(
            used_objects = used_objects_depset,
            linker_input = linker_input,
        ),
    ]

_wpilib_cc_static_library = rule(
    implementation = _cc_static_library_impl,
    attrs = {
        "deps": attr.label_list(
            providers = [CcInfo],
            doc = """
List of all the dependencies to accumulate objects from to link into this static library.
""",
        ),
        "static_deps": attr.label_list(
            providers = [CcStaticLibraryInfo],
            doc = """
List of all static libraries to not duplicate .o files from.
""",
        ),
        "static_lib_name": attr.string(doc = """
By default cc_static_library will use a name for the static library output file based on
the target's name and the platform. This includes an extension and sometimes a prefix.
Sometimes you may not want the default name, in which case you can use this
attribute to choose a custom name."""),
    } | CC_TOOLCHAIN_ATTRS,
    toolchains = use_cc_toolchain(),
    fragments = ["cpp"],
)

def wpilib_cc_static_library(
        name,
        static_lib_name = None,
        **kwargs):
    if not static_lib_name:
        folder, lib = _folder_prefix(name)
        static_lib_name = select({
            "//shared/bazel/rules:compilation_mode_dbg": folder + "/lib" + lib + "d.a",
            "@bazel_tools//src/conditions:windows": folder + "/" + lib + ".lib",
            "//conditions:default": folder + "/lib" + lib + ".a",
        })

    _wpilib_cc_static_library(
        name = name,
        static_lib_name = static_lib_name,
        **kwargs
    )

    pkg_files(
        name = name + "-static.pkg",
        srcs = [":" + name],
        tags = ["manual"],
    )

    pkg_zip(
        name = name + "-static-zip",
        srcs = ["//:license_pkg_files", name + "-static.pkg"],
        tags = ["no-remote", "manual"],
    )
