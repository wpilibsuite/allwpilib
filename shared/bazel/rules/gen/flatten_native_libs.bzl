"""Flattens the transitively-collected native shared libraries of a set of
java targets into a single directory, so OS dynamic loaders (particularly
Windows' implicit DLL search, which prefers the loading DLL's own directory)
can resolve every native dependency uniformly."""

def _is_shared_lib_name(basename):
    if basename.endswith((".dylib", ".dll")):
        return True

    # Match ".so" as well as versioned sonames like "libfoo.so.4.13".
    so_index = basename.find(".so")
    if so_index == -1:
        return False
    remainder = basename[so_index + len(".so"):]
    return remainder == "" or remainder.startswith(".")

def _is_native_lib(f):
    return _is_shared_lib_name(f.basename)

def _is_solib_path(path):
    # Bazel's internal "_solib_*" symlink farm re-exposes cc_shared_library
    # (and cc_import-wrapped prebuilt library) outputs under a mangled path,
    # used for C++-level dynamic linking. For libraries built in this repo, a
    # canonical (non-mangled) copy also exists and is preferred; for some
    # prebuilt third-party libraries (e.g. OpenCV), the "_solib_*" copy is the
    # only one available, so it can't just be excluded outright.
    return "/_solib_" in path.replace("\\", "/")

# Name of an always-present file inside the flattened directory. The
# directory itself isn't a single Bazel output (each library is its own
# symlink output), so it has no runfiles-manifest entry of its own; callers
# look up this file instead and take its parent directory.
NATIVE_LIBS_MARKER = "native-libs.marker"

def _native_lib_files(deps):
    file_sets = []
    for dep in deps:
        if DefaultInfo not in dep:
            continue
        default_info = dep[DefaultInfo]
        if default_info.default_runfiles:
            file_sets.append(default_info.default_runfiles.files)
        file_sets.append(default_info.files)

    return [
        f
        for f in depset(transitive = file_sets).to_list()
        if _is_native_lib(f)
    ]

def _own_native_lib_files(deps):
    # Like _native_lib_files, but only each dep's own DefaultInfo.files (its
    # own output), not default_runfiles (which for a cc_shared_library also
    # transitively includes its dynamic_deps' .so files). Used for halsim_deps
    # basenames: HALSIM_EXTENSIONS should list only the extension's own entry
    # point (e.g. libhalsim_ws_client.so), not libraries like libwpiHal.so
    # that it happens to link against - those get pulled in by the OS loader
    # automatically once the extension itself is dlopen'd, and don't export a
    # HALSIM_InitExtension symbol themselves.
    file_sets = []
    for dep in deps:
        if DefaultInfo not in dep:
            continue
        file_sets.append(dep[DefaultInfo].files)

    return [
        f
        for f in depset(transitive = file_sets).to_list()
        if _is_native_lib(f)
    ]

def _wpilib_flatten_native_libs_impl(ctx):
    native_libs = _native_lib_files(ctx.attr.deps)

    # halsim_deps are HAL simulation extensions (e.g. halsim_ws_client): like
    # any other dep, their .so ends up in the flattened directory, but their
    # basenames are also recorded separately (below) so the caller (the
    # java_executable_wrapper.sh launcher) knows which specific files in the
    # flattened directory to auto-load via HALSIM_EXTENSIONS, as opposed to
    # the rest of its contents, which are only ever passively dlopen'd on demand.
    halsim_libs = _native_lib_files(ctx.attr.halsim_deps)

    # Dedupe by basename: prefer the canonical (non-solib) copy over one
    # found only in Bazel's "_solib_*" symlink farm.
    chosen = {}
    for f in native_libs + halsim_libs:
        basename = f.basename
        existing = chosen.get(basename)
        if existing == None or (_is_solib_path(existing.path) and not _is_solib_path(f.path)):
            chosen[basename] = f

    # One symlink per library rather than a copy: on Linux/macOS each is a
    # real filesystem symlink to the library's existing build output, so a
    # flattened directory costs a few KB instead of the libraries' full size.
    # On Windows, ctx.actions.symlink falls back to copying the file unless
    # --windows_enable_symlinks is set.
    outputs = []
    for basename, f in chosen.items():
        link = ctx.actions.declare_file(ctx.label.name + "/" + basename)
        ctx.actions.symlink(output = link, target_file = f)
        outputs.append(link)

    marker = ctx.actions.declare_file(ctx.label.name + "/" + NATIVE_LIBS_MARKER)
    ctx.actions.write(output = marker, content = "")
    outputs.append(marker)

    halsim_basenames = sorted({f.basename: None for f in _own_native_lib_files(ctx.attr.halsim_deps)}.keys())
    halsim_manifest = ctx.actions.declare_file(ctx.label.name + ".halsim-extensions.txt")
    ctx.actions.write(
        output = halsim_manifest,
        content = "".join([b + "\n" for b in halsim_basenames]),
    )

    return [DefaultInfo(files = depset(outputs + [halsim_manifest]))]

wpilib_flatten_native_libs = rule(
    implementation = _wpilib_flatten_native_libs_impl,
    attrs = {
        "deps": attr.label_list(mandatory = True),
        "halsim_deps": attr.label_list(default = []),
    },
)
