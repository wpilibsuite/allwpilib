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

# Flattens the (possibly empty) set of chosen native libs into out_dir via a
# single inline shell action: TreeArtifact outputs must be produced by one
# action, so there's no way to populate out_dir with per-file
# ctx.actions.symlink/declare_file calls. File contents are passed as
# argv (via ctx.actions.args()) rather than interpolated into the command
# string, so no shell quoting of paths is needed.
_FLATTEN_COMMAND = """
set -euo pipefail
out_dir="$1"
shift
mkdir -p "$out_dir"
while [ "$#" -gt 0 ]; do
  basename="$1"
  src="$2"
  shift 2
  cp "$src" "$out_dir/$basename"
done
"""

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
    if ctx.attr.skip_copy:
        # A wpilib_cc_binary halsim dep (e.g. halsim_gui) always shares its own
        # transitive runtime dependencies (wpiHal, wpimath, wpiutil, wpinet,
        # ntcore, ...) with the robot program that dlopen()s it, since both
        # link the same underlying libraries. By the time HAL_LoadExtensions()
        # runs, those libraries are already loaded in the process - and every
        # OS's dynamic loader resolves a newly-loaded library's dependencies
        # against already-loaded instances first (matched by soname/install
        # name on Linux/macOS, by filename via the exe's own directory on
        # Windows), before doing any path search. There's nothing to flatten -
        # point HALSIM_EXTENSIONS straight at each halsim dep's own build
        # output instead of a duplicate copy in a bundle directory. A copy is
        # actively harmful, not just redundant: loading a second, separately-
        # initialized instance of a library like wpiHal alongside the one
        # already in use crashes at runtime (confirmed on Windows - a bundled
        # duplicate wpiHal.dll segfaulted; the original, un-copied build
        # output doesn't).
        own_files = _own_native_lib_files(ctx.attr.halsim_deps)
        manifest = ctx.actions.declare_file(ctx.label.name + ".halsim-extensions.txt")
        ctx.actions.write(
            output = manifest,
            content = "".join(["_main/" + f.short_path + "\n" for f in own_files]),
        )
        return [DefaultInfo(files = depset([manifest]), runfiles = ctx.runfiles(files = own_files))]

    native_libs = _native_lib_files(ctx.attr.deps)

    # halsim_deps are HAL simulation extensions (e.g. halsim_ws_client): like
    # any other dep, their .so ends up flattened into out_dir, but their
    # basenames are also recorded separately (below) so the caller (the
    # java_executable_wrapper.sh launcher) knows which specific files in
    # out_dir to auto-load via HALSIM_EXTENSIONS, as opposed to the rest of
    # out_dir's contents, which are only ever passively dlopen'd on demand.
    halsim_libs = _native_lib_files(ctx.attr.halsim_deps)

    # Dedupe by basename: prefer the canonical (non-solib) copy over one
    # found only in Bazel's "_solib_*" symlink farm.
    chosen = {}
    for f in native_libs + halsim_libs:
        basename = f.basename
        existing = chosen.get(basename)
        if existing == None or (_is_solib_path(existing.path) and not _is_solib_path(f.path)):
            chosen[basename] = f

    out_dir = ctx.actions.declare_directory(ctx.label.name)

    args = ctx.actions.args()
    args.add(out_dir.path)
    for basename, f in chosen.items():
        args.add(basename)
        args.add(f.path)

    ctx.actions.run_shell(
        outputs = [out_dir],
        inputs = chosen.values(),
        arguments = [args],
        command = _FLATTEN_COMMAND,
        mnemonic = "WpilibFlattenNativeLibs",
        progress_message = "Flattening native libraries for %{label}",
    )

    halsim_basenames = sorted({f.basename: None for f in _own_native_lib_files(ctx.attr.halsim_deps)}.keys())
    halsim_manifest = ctx.actions.declare_file(ctx.label.name + ".halsim-extensions.txt")
    ctx.actions.write(
        output = halsim_manifest,
        content = "".join([b + "\n" for b in halsim_basenames]),
    )

    return [DefaultInfo(files = depset([out_dir, halsim_manifest]))]

wpilib_flatten_native_libs = rule(
    implementation = _wpilib_flatten_native_libs_impl,
    attrs = {
        "deps": attr.label_list(mandatory = True),
        "halsim_deps": attr.label_list(default = []),
        "skip_copy": attr.bool(default = False),
    },
)
