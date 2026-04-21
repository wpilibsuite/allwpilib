load("@rules_cc//cc:defs.bzl", "cc_library")
load("//shared/bazel/rules/robotpy:compatibility_select.bzl", "robotpy_compatibility_select")

RESOLVE_CASTERS_DIR = "generated/resolve_casters/"
HEADER_DAT_DIR = "generated/header_to_dat/"
DAT_TO_CC_DIR = "generated/dat_to_cc/"
DAT_TO_TMPL_CC_DIR = "generated/dat_to_tmpl_cc/"
DAT_TO_TMPL_HDR_DIR = "generated/dat_to_tmpl_hdr/"
GEN_MODINIT_HDR_DIR = "generated/gen_modinit_hdr/"

def _location_helper(filename):
    return " $(locations " + filename + ")"

def _wrapper():
    return "$(locations //shared/bazel/rules/robotpy:wrapper) "

def _wrapper_dep():
    return ["//shared/bazel/rules/robotpy:wrapper"]

def _semiwrap_caster():
    return "//shared/bazel/rules/robotpy:semiwrap_casters_files"

def publish_casters(
        name,
        project_config,
        caster_name,
        output_json,
        output_pc,
        typecasters_srcs,
        package_root):
    """
    Sugar wrapper for the semiwrap.cmd.publish_casters tool
    """
    cmd = _wrapper() + " semiwrap.cmd.publish_casters"
    cmd += " $(SRCS) " + caster_name + " $(OUTS)"

    native.genrule(
        name = name,
        srcs = [project_config],
        outs = [output_json, output_pc],
        cmd = cmd,
        tools = _wrapper_dep() + typecasters_srcs + [package_root],
        target_compatible_with = robotpy_compatibility_select(),
        visibility = ["//visibility:public"],
        tags = ["robotpy"],
    )

def resolve_casters(
        name,
        casters_pkl_file,
        dep_file,
        caster_files = [],
        caster_deps = []):
    """
    Sugar wrapper for the semiwrap.cmd.resolve_casters tool
    """
    cmd = _wrapper() + " semiwrap.cmd.resolve_casters "
    cmd += " $(OUTS)"

    cmd += _location_helper(_semiwrap_caster()) + "/semiwrap/semiwrap.pybind11.json"

    resolved_caster_files = []

    deps = []
    for dep in caster_deps:
        deps.append(dep)
        cmd += _location_helper(dep)

    for cfd in caster_files:
        if cfd.startswith(":"):
            resolved_caster_files.append(cfd)
            cmd += _location_helper(cfd)
        else:
            cmd += " " + cfd

    native.genrule(
        name = name,
        srcs = resolved_caster_files + deps,
        outs = [RESOLVE_CASTERS_DIR + casters_pkl_file, RESOLVE_CASTERS_DIR + dep_file],
        cmd = cmd,
        tools = _wrapper_dep() + [_semiwrap_caster()],
        target_compatible_with = robotpy_compatibility_select(),
        tags = ["robotpy"],
    )

def gen_libinit(
        name,
        output_file,
        modules):
    """
    Sugar wrapper for the semiwrap.cmd.gen_libinit tool
    """
    cmd = _wrapper() + " semiwrap.cmd.gen_libinit "
    cmd += " $(OUTS) "
    cmd += " ".join(modules)

    native.genrule(
        name = name,
        outs = [output_file],
        cmd = cmd,
        tools = _wrapper_dep(),
        target_compatible_with = robotpy_compatibility_select(),
        tags = ["robotpy"],
    )

def gen_pkgconf(
        name,
        project_file,
        module_pkg_name,
        pkg_name,
        output_file,
        libinit_py,
        install_path,
        package_root):
    """
    Sugar wrapper for the semiwrap.cmd.gen_pkgconf tool
    """
    cmd = _wrapper() + " semiwrap.cmd.gen_pkgconf "
    cmd += " " + module_pkg_name + " " + pkg_name
    cmd += _location_helper(project_file)
    cmd += " $(OUTS)"
    if libinit_py:
        cmd += " --libinit-py " + libinit_py

    OUT_FILE = install_path + "/" + output_file
    native.genrule(
        name = name,
        srcs = [package_root],
        outs = [OUT_FILE],
        cmd = cmd,
        tools = _wrapper_dep() + [project_file],
        target_compatible_with = robotpy_compatibility_select(),
        visibility = ["//visibility:public"],
        tags = ["robotpy"],
    )

def header_to_dat(
        name,
        casters_pickle,
        include_root,
        class_name,
        yml_file,
        header_location,
        generation_includes = [],
        header_to_dat_deps = [],
        extra_defines = [],
        deps = []):
    """
    Sugar wrapper for the semiwrap.cmd.header2dat tool
    """
    cmd = _wrapper() + " semiwrap.cmd.header2dat "
    cmd += "--cpp 202002L "  # TODO(pj) - This is the option when I ran on linux. Does its value really matter?
    cmd += class_name
    cmd += _location_helper(yml_file)

    cmd += " -I " + include_root

    for inc in generation_includes:
        cmd += " -I " + inc
    for d in extra_defines:
        cmd += " -D '" + d + "'"
    cmd += " " + header_location

    cmd += " " + include_root
    cmd += _location_helper(RESOLVE_CASTERS_DIR + casters_pickle)
    cmd += " $(OUTS)"
    cmd += " bogus c++20 ccache c++ -- -std=c++20"  # TODO(pj) Does it matter what these values are?

    native.genrule(
        name = name + "." + class_name,
        srcs = [RESOLVE_CASTERS_DIR + casters_pickle] + deps + header_to_dat_deps,
        outs = [HEADER_DAT_DIR + class_name + ".dat", HEADER_DAT_DIR + class_name + ".d"],
        cmd = cmd,
        tools = _wrapper_dep() + [yml_file],
        target_compatible_with = robotpy_compatibility_select(),
        tags = ["robotpy"],
    )

def dat_to_cc(
        name,
        class_name):
    dat_file = HEADER_DAT_DIR + class_name + ".dat"
    cmd = _wrapper() + " semiwrap.cmd.dat2cpp "
    cmd += _location_helper(dat_file)
    cmd += " $(OUTS)"
    native.genrule(
        name = name + "." + class_name,
        outs = [DAT_TO_CC_DIR + class_name + ".cpp"],
        cmd = cmd,
        tools = _wrapper_dep() + [dat_file],
        target_compatible_with = robotpy_compatibility_select(),
        tags = ["robotpy"],
    )

def dat_to_tmpl_cpp(name, base_class_name, specialization, tmp_class_name):
    cmd = _wrapper() + " semiwrap.cmd.dat2tmplcpp "
    cmd += _location_helper(HEADER_DAT_DIR + base_class_name + ".dat")
    cmd += " " + specialization
    cmd += " $(OUTS)"
    native.genrule(
        name = name + "." + tmp_class_name,
        outs = [DAT_TO_TMPL_CC_DIR + tmp_class_name + ".cpp"],
        cmd = cmd,
        tools = _wrapper_dep() + [HEADER_DAT_DIR + base_class_name + ".dat"],
        target_compatible_with = robotpy_compatibility_select(),
        tags = ["robotpy"],
    )

def dat_to_tmpl_hpp(name, class_name):
    dat_file = HEADER_DAT_DIR + class_name + ".dat"

    cmd = _wrapper() + " semiwrap.cmd.dat2tmplhpp "
    cmd += _location_helper(dat_file)
    cmd += " $(OUTS)"
    native.genrule(
        name = name + "." + class_name,
        outs = [DAT_TO_TMPL_HDR_DIR + class_name + "_tmpl.hpp"],
        cmd = cmd,
        tools = _wrapper_dep() + [dat_file],
        target_compatible_with = robotpy_compatibility_select(),
        tags = ["robotpy"],
    )

def dat_to_trampoline(name, dat_file, class_name, output_file):
    cmd = _wrapper() + " semiwrap.cmd.dat2trampoline "

    cmd += _location_helper(HEADER_DAT_DIR + dat_file)
    cmd += "  " + class_name
    cmd += " $(OUTS)"

    native.genrule(
        name = name + "." + output_file,
        outs = [output_file],
        cmd = cmd,
        tools = _wrapper_dep() + [HEADER_DAT_DIR + dat_file],
        target_compatible_with = robotpy_compatibility_select(),
        tags = ["robotpy"],
    )

def gen_modinit_hpp(
        name,
        libname,
        input_dats,
        output_file):
    input_dats = [HEADER_DAT_DIR + x + ".dat" for x in input_dats]

    cmd = _wrapper() + " semiwrap.cmd.gen_modinit_hpp "
    cmd += " " + libname
    cmd += " $(OUTS)"
    for input_dat in input_dats:
        cmd += _location_helper(input_dat)

    native.genrule(
        name = name + ".gen",
        outs = [GEN_MODINIT_HDR_DIR + output_file],
        cmd = cmd,
        tools = _wrapper_dep() + input_dats,
        target_compatible_with = robotpy_compatibility_select(),
        tags = ["robotpy"],
    )
    cc_library(
        name = name,
        hdrs = [GEN_MODINIT_HDR_DIR + output_file],
        strip_include_prefix = GEN_MODINIT_HDR_DIR,
        tags = ["robotpy"],
        target_compatible_with = robotpy_compatibility_select(),
    )

def run_header_gen(name, casters_pickle, trampoline_subpath, header_gen_config, deps = [], generation_defines = [], local_native_libraries = [], header_to_dat_deps = [], yml_prefix = "src/main/python/"):
    generation_includes = []
    header_to_dat_deps = list(header_to_dat_deps)

    for dep in local_native_libraries:
        header_to_dat_deps.append(dep)
        generation_includes.append("$(execpath " + dep + ")")

    for header_gen in header_gen_config:
        header_to_dat(
            name = name + ".header_to_dat",
            casters_pickle = casters_pickle,
            include_root = header_gen.header_root,
            class_name = header_gen.class_name,
            yml_file = yml_prefix + header_gen.yml_file,
            header_location = header_gen.header_file,
            deps = deps,
            generation_includes = generation_includes,
            extra_defines = generation_defines,
            header_to_dat_deps = header_to_dat_deps,
        )

    generated_cc_files = []
    for header_gen in header_gen_config:
        dat_to_cc(
            name = name + ".dat_to_cc",
            class_name = header_gen.class_name,
        )
        generated_cc_files.append(DAT_TO_CC_DIR + header_gen.class_name + ".cpp")

    tmpl_hdrs = []
    for header_gen in header_gen_config:
        if header_gen.tmpl_class_names:
            dat_to_tmpl_hpp(
                name = name + ".dat_to_tmpl_hpp",
                class_name = header_gen.class_name,
            )
            tmpl_hdrs.append(DAT_TO_TMPL_HDR_DIR + header_gen.class_name + "_tmpl.hpp")

        for tmpl_class_name, specialization in header_gen.tmpl_class_names:
            dat_to_tmpl_cpp(
                name = name + ".dat_to_tmpl_cpp",
                base_class_name = header_gen.class_name,
                specialization = specialization,
                tmp_class_name = tmpl_class_name,
            )
            generated_cc_files.append(DAT_TO_TMPL_CC_DIR + tmpl_class_name + ".cpp")

    trampoline_hdrs = []
    for header_gen in header_gen_config:
        for trampoline_symbol, trampoline_header in header_gen.trampolines:
            output_path = trampoline_subpath + "/trampolines/" + trampoline_header
            dat_to_trampoline(
                name = name + ".dat2trampoline",
                dat_file = header_gen.class_name + ".dat",
                class_name = trampoline_symbol,
                output_file = output_path,
            )
            trampoline_hdrs.append(output_path)
    cc_library(
        name = name + ".tmpl_hdrs",
        hdrs = tmpl_hdrs,
        strip_include_prefix = DAT_TO_TMPL_HDR_DIR,
        tags = ["robotpy"],
    )
    cc_library(
        name = name + ".trampoline_hdrs",
        hdrs = trampoline_hdrs,
        strip_include_prefix = trampoline_subpath,
        tags = ["robotpy"],
    )

    native.filegroup(
        name = name + ".generated_srcs",
        srcs = generated_cc_files,
        tags = ["manual", "robotpy"],
    )

    native.filegroup(
        name = name + ".trampoline_hdr_files",
        srcs = trampoline_hdrs,
        tags = ["manual", "robotpy"],
    )

    native.filegroup(
        name = name + ".header_gen_files",
        srcs = tmpl_hdrs + trampoline_hdrs + generated_cc_files,
        tags = ["manual", "robotpy"],
    )
