#!/usr/bin/env bash
# Bazel Java launcher — resolves native-library paths via runfiles.
#
# Backs both `wpilib_java_junit5_test` (wrapping an sh_test around a
# java_test) and `wpilib_java_binary` (wrapping an sh_binary around a
# java_binary) in java_rules.bzl: java_test and java_binary share the same
# underlying stub launcher, so this script's rlocation/exec logic is
# identical either way.
#
# On every OS we resolve the flat native-libs directory from the runfiles
# manifest and inject -Djava.library.path via --wrapper_script_flag (which
# the launcher applies after the params-file jvm_flags, so it always wins).
# That covers the JVM's own initial System.loadLibrary() call for the
# top-level JNI library (e.g. libwpiHaljni.so).
#
# -Djava.library.path/RPATH is not enough on any OS, though: once the JVM
# dlopen()s that first library, resolving *its* transitive native
# dependencies (e.g. libwpiHal.so, a sibling in the same flattened directory)
# is entirely up to the OS loader, which does not consult java.library.path.
# Relying on the .so's own baked-in RPATH/RUNPATH for this has proven
# unreliable for this flattened-directory layout (some $ORIGIN-relative
# entries point at Bazel's solib symlink farm, which can contain dangling
# symlinks; even the plain "$ORIGIN" fallback entry does not reliably find
# a sibling file that demonstrably exists in the same directory). So on
# every OS we also export the OS loader's own library-search-path env var
# pointing at the flattened native-libs dir, which does reliably work:
#   - Linux:   LD_LIBRARY_PATH
#   - macOS:   DYLD_FALLBACK_LIBRARY_PATH only (never DYLD_LIBRARY_PATH: unlike
#     LD_LIBRARY_PATH, it's consulted *before* a binary's own load-time paths,
#     which hijacks the java launcher's own bootstrap dylibs like libjli.dylib
#     and breaks JVM startup entirely ("Failed setting boot class path").
#     DYLD_FALLBACK_LIBRARY_PATH is only consulted once the normal search
#     already failed, so it can't clobber a lookup that was already working.)
#   - Windows: PATH (MSYS2's compiled .exe launcher additionally can't do
#     shell variable expansion at all, so PATH is prepended unconditionally
#     there; the Windows DLL loader also uses PATH to find transitive DLL
#     dependencies, same rationale as LD_LIBRARY_PATH/DYLD_LIBRARY_PATH).
#
# Required environment variables (set by the enclosing target in java_rules.bzl):
#   NATIVE_LIBS_RLOCATION     rlocation key of the flat native-libs directory
#   JAVA_EXECUTABLE_RLOCATION rlocation key of the _java_impl executable
#
# Optional (wpilib_java_binary only, when it has halsim_deps):
#   HALSIM_MANIFEST_RLOCATION rlocation key of a newline-delimited list of
#                              native-libs-dir basenames to auto-load via
#                              HALSIM_EXTENSIONS (see hal/src/main/native/sim/
#                              Extensions.cpp) - HAL simulation extensions
#                              like halsim_ws_client, as opposed to the rest
#                              of the flattened directory, which is only ever
#                              passively dlopen'd on demand.
#
# Optional (smoke tests only):
#   SMOKE_TEST_TIMEOUT_SECONDS  if set, don't exec the java executable as the
#                              final process. Instead run it in the
#                              background, and treat it still being alive
#                              after this many seconds as success (these are
#                              robot simulation programs that run forever
#                              once started, so a program that exits early -
#                              e.g. from a runtime-only bug like two
#                              DigitalInputs on the same port - is the
#                              failure signal). Implemented with plain bash
#                              job control rather than the external
#                              `timeout(1)`, since macOS doesn't ship it and
#                              this script otherwise supports macOS/Windows.

# --- begin runfiles.bash initialization v3 ---
# Copy-pasted from the Bazel Bash runfiles library v3.
set -uo pipefail; set +e; f=bazel_tools/tools/bash/runfiles/runfiles.bash
# shellcheck disable=SC1090
source "${RUNFILES_DIR:-/dev/null}/$f" 2>/dev/null || \
  source "$(grep -sm1 "^$f " "${RUNFILES_MANIFEST_FILE:-/dev/null}" | cut -f2- -d' ')" 2>/dev/null || \
  source "$0.runfiles/$f" 2>/dev/null || \
  source "$(grep -sm1 "^$f " "$0.runfiles_manifest" | cut -f2- -d' ')" 2>/dev/null || \
  source "$(grep -sm1 "^$f " "$0.exe.runfiles_manifest" | cut -f2- -d' ')" 2>/dev/null || \
  { echo>&2 "ERROR: cannot find $f"; exit 1; }; f=; set -e
# --- end runfiles.bash initialization v3 ---

# The exec'd java executable below is itself a Bazel-built binary with its
# own runfiles library, and looks for $JAVA_RUNFILES specifically (unlike
# this script, which only needs RUNFILES_DIR/RUNFILES_MANIFEST_FILE). `bazel
# test` happens to set $JAVA_RUNFILES directly as part of its test-runner
# environment, but `bazel run` does not, so without this call the exec'd
# child fails with "Cannot locate runfiles directory" under `bazel run`
# specifically. runfiles_export_envvars (from the sourced runfiles.bash
# library) fills in JAVA_RUNFILES from RUNFILES_DIR/RUNFILES_MANIFEST_FILE so
# subprocesses can find it either way.
runfiles_export_envvars

java_bin_key="${JAVA_EXECUTABLE_RLOCATION:?JAVA_EXECUTABLE_RLOCATION must be set}"
native_libs_key="${NATIVE_LIBS_RLOCATION:?NATIVE_LIBS_RLOCATION must be set}"

# Resolve the java executable.
# On Windows, Bazel appends .exe to binary rlocation keys in the manifest.
java_bin="$(rlocation "$java_bin_key" 2>/dev/null || true)"
if [[ -z "$java_bin" ]]; then
  java_bin="$(rlocation "${java_bin_key}.exe" 2>/dev/null || true)"
fi
if [[ -z "$java_bin" ]]; then
  echo >&2 "ERROR: cannot resolve java executable: $java_bin_key"
  exit 1
fi

# Not every target has native libs (e.g. pure-Java modules with no JNI
# deps), in which case the flattened directory is empty and may not resolve
# via rlocation at all. Treat that as "nothing to inject" rather than a
# fatal error.
native_libs_dir="$(rlocation "$native_libs_key" 2>/dev/null || true)"

wrapper_flags=()
if [[ -n "$native_libs_dir" ]]; then
  wrapper_flags+=("--wrapper_script_flag=--jvm_flag=-Djava.library.path=$native_libs_dir")

  case "$(uname -s)" in
    MINGW*|MSYS*|CYGWIN*)
      # rlocation returns a mixed-style Windows path (C:/...) on Windows.
      # PATH prepend: use as-is; MSYS2 converts it for the Windows exe child.
      # This ensures the whole transitive DLL chain is found
      # (LOAD_WITH_ALTERED_SEARCH_PATH only covers first-level deps, PATH
      # covers the rest).
      export PATH="$native_libs_dir:$PATH"
      ;;
    Darwin*)
      export DYLD_FALLBACK_LIBRARY_PATH="$native_libs_dir:${DYLD_FALLBACK_LIBRARY_PATH:-}"
      ;;
    *)
      export LD_LIBRARY_PATH="$native_libs_dir:${LD_LIBRARY_PATH:-}"
      ;;
  esac
fi

# HALSIM_MANIFEST_RLOCATION is only set by wpilib_java_binary targets that
# declared halsim_deps; test targets and plain examples never set it, so
# this whole block is a no-op for them.
halsim_manifest_key="${HALSIM_MANIFEST_RLOCATION:-}"
if [[ -n "$halsim_manifest_key" && -n "$native_libs_dir" ]]; then
  halsim_manifest="$(rlocation "$halsim_manifest_key" 2>/dev/null || true)"
  if [[ -n "$halsim_manifest" && -s "$halsim_manifest" ]]; then
    halsim_delim=":"
    case "$(uname -s)" in
      MINGW*|MSYS*|CYGWIN*)
        # Matches DELIM in hal/src/main/native/sim/Extensions.cpp.
        halsim_delim=";"
        ;;
    esac

    halsim_extensions=""
    # `read`, not mapfile/readarray: the latter is a bash 4+ builtin, and
    # macOS ships bash 3.2 (see the final exec's own bash-3.2 note below).
    while IFS= read -r halsim_basename || [[ -n "$halsim_basename" ]]; do
      [[ -z "$halsim_basename" ]] && continue
      if [[ -n "$halsim_extensions" ]]; then
        halsim_extensions="${halsim_extensions}${halsim_delim}${native_libs_dir}/${halsim_basename}"
      else
        halsim_extensions="${native_libs_dir}/${halsim_basename}"
      fi
    done < "$halsim_manifest"

    if [[ -n "$halsim_extensions" ]]; then
      export HALSIM_EXTENSIONS="$halsim_extensions"
    fi
  fi
fi

# The ${arr[@]+"${arr[@]}"} form (rather than plain "${wrapper_flags[@]}") is
# required for macOS's default bash (3.2, frozen there for licensing reasons):
# it throws "unbound variable" under `set -u` when expanding an empty array,
# a bug fixed upstream in bash 4.4+ but never backported by Apple.
if [[ -z "${SMOKE_TEST_TIMEOUT_SECONDS:-}" ]]; then
  exec "$java_bin" ${wrapper_flags[@]+"${wrapper_flags[@]}"} "$@"
fi

# Smoke test mode: run in the background instead of exec'ing, so this script
# can observe whether the program is still alive once the budget elapses.
"$java_bin" ${wrapper_flags[@]+"${wrapper_flags[@]}"} "$@" &
child_pid=$!

elapsed=0
while [[ "$elapsed" -lt "$SMOKE_TEST_TIMEOUT_SECONDS" ]] && kill -0 "$child_pid" 2>/dev/null; do
  sleep 1
  elapsed=$((elapsed + 1))
done

if kill -0 "$child_pid" 2>/dev/null; then
  echo "Smoke test PASS: still running after ${SMOKE_TEST_TIMEOUT_SECONDS}s, no startup crash."
  kill "$child_pid" 2>/dev/null || true
  grace=0
  while [[ "$grace" -lt 5 ]] && kill -0 "$child_pid" 2>/dev/null; do
    sleep 1
    grace=$((grace + 1))
  done
  kill -9 "$child_pid" 2>/dev/null || true
  wait "$child_pid" 2>/dev/null || true
  exit 0
fi

rc=0
wait "$child_pid" || rc=$?
echo "Smoke test FAIL: exited after ${elapsed}s with code $rc, before the ${SMOKE_TEST_TIMEOUT_SECONDS}s timeout - see output above for the crash." >&2
exit 1
