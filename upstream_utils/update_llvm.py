#!/usr/bin/env python3

import os
import re
import requests
import shutil
import tarfile
import tempfile

from upstream_utils import get_repo_root, apply_patches


class Version:

    def __init__(self, version_string):
        m = re.search(r"([0-9]+)\.([0-9]+)\.([0-9]+)", version_string)
        self.major = m.group(1)
        self.minor = m.group(2)
        self.patch = m.group(3)


def replace_config_defines(content, llvm_version):
    version = Version(llvm_version)

    # Config variable replacements
    config_variables = {
        "${BACKTRACE_HEADER}": "backtrace.h",
        "${BUG_REPORT_URL}": "",
        "${HAVE_BACKTRACE}": "0",
        "${HAVE_DEREGISTER_FRAME}": "1",
        "${HAVE_DLFCN_H}": "__has_include(<dlfcn.h>)",
        "${HAVE_DLOPEN}": "__has_include(<dlfcn.h>)",
        "${HAVE_DLADDR}": "__has_include(<dlfcn.h>)",
        "${HAVE_ERRNO_H}": "__has_include(<errno.h>)",
        "${HAVE_FCNTL_H}": "__has_include(<fcntl.h>)",
        "${HAVE_FENV_H}": "__has_include(<fenv.h>)",
        "${HAVE_FFI_CALL}": "__has_include(<ffi.h>)",
        "${HAVE_FFI_FFI_H}": "__has_include(<ffi/ffi.h>)",
        "${HAVE_FFI_H}": "__has_include(<ffi.h>)",
        "${HAVE_FUTIMENS}": "__has_include(<sys/time.h>)",
        "${HAVE_FUTIMES}": "__has_include(<sys/time.h>)",
        "${HAVE_GETPAGESIZE}": "__has_include(<unistd.h>)",
        "${HAVE_GETRLIMIT}": "__has_include(<sys/resource.h>)",
        "${HAVE_GETRUSAGE}": "__has_include(<sys/resource.h>)",
        "${HAVE_LIBEDIT}": "0",
        "${HAVE_LIBPFM}": "0",
        "${HAVE_LIBPSAPI}": "0",
        "${HAVE_LIBPTHREAD}": "__has_include(<pthread.h>)",
        "${HAVE_LINK_H}": "__has_include(<link.h>)",
        "${HAVE_LSEEK64}": "__has_include(<unistd.h>)",
        "${HAVE_MACH_MACH_H}": "__has_include(<mach/mach.h>)",
        "${HAVE_PTHREAD_GETNAME_NP}": "__has_include(<pthread.h>)",
        "${HAVE_PTHREAD_SETNAME_NP}": "__has_include(<pthread.h>)",
        "${HAVE_MALLCTL}": "__has_include(<malloc.h>)",
        "${HAVE_MALLINFO}": "__has_include(<malloc.h>)",
        "${HAVE_MALLINFO2}": "__has_include(<malloc.h>)",
        "${HAVE_MALLOC_MALLOC_H}": "__has_include(<malloc/malloc.h>)",
        "${HAVE_MALLOC_ZONE_STATISTICS}": "0",
        "${HAVE_POSIX_FALLOCATE}": "__has_include(<fcntl.h>)",
        "${HAVE_POSIX_SPAWN}": "__has_include(<spawn.h>)",
        "${HAVE_PREAD}": "__has_include(<unistd.h>)",
        "${HAVE_PTHREAD_GETSPECIFIC}": "__has_include(<pthread.h>)",
        "${HAVE_PTHREAD_H}": "__has_include(<pthread.h>)",
        "${HAVE_PTHREAD_MUTEX_LOCK}": "__has_include(<pthread.h>)",
        "${HAVE_PTHREAD_RWLOCK_INIT}": "__has_include(<pthread.h>)",
        "${HAVE_REGISTER_FRAME}": "1",
        "${HAVE_SBRK}": "__has_include(<unistd.h>)",
        "${HAVE_SETENV}": "__has_include(<stdlib.h>)",
        "${HAVE_SETRLIMIT}": "__has_include(<sys/resource.h>)",
        "${HAVE_SIGALTSTACK}": "__has_include(<signal.h>)",
        "${HAVE_SIGNAL_H}": "__has_include(<signal.h>)",
        "${HAVE_STD_IS_TRIVIALLY_COPYABLE}": "1",
        "${HAVE_STRERROR}": "__has_include(<string.h>)",
        "${HAVE_STRERROR_R}": "__has_include(<string.h>)",
        "${HAVE_STRUCT_STAT_ST_MTIMESPEC_TV_NSEC}": "1",
        "${HAVE_STRUCT_STAT_ST_MTIM_TV_NSEC}": "1",
        "${HAVE_SYSCONF}": "__has_include(<unistd.h>)",
        "${HAVE_SYSEXITS_H}": "__has_include(<sysexits.h>)",
        "${HAVE_SYS_IOCTL_H}": "__has_include(<sys/ioctl.h>)",
        "${HAVE_SYS_MMAN_H}": "__has_include(<sys/mman.h>)",
        "${HAVE_SYS_PARAM_H}": "__has_include(<sys/param.h>)",
        "${HAVE_SYS_RESOURCE_H}": "__has_include(<sys/resource.h>)",
        "${HAVE_SYS_STAT_H}": "__has_include(<sys/stat.h>)",
        "${HAVE_SYS_TIME_H}": "__has_include(<sys/time.h>)",
        "${HAVE_SYS_TYPES_H}": "__has_include(<sys/types.h>)",
        "${HAVE_TERMIOS_H}": "__has_include(<termios.h>)",
        "${HAVE_UNISTD_H}": "__has_include(<unistd.h>)",
        "${HAVE_VALGRIND_VALGRIND_H}": "__has_include(<valgrind/valgrind.h>)",
        "${HAVE__ALLOCA}": "0",
        "${HAVE__CHSIZE_S}": "0",
        "${HAVE__UNWIND_BACKTRACE}": "0",
        "${HAVE___ALLOCA}": "0",
        "${HAVE___ASHLDI3}": "0",
        "${HAVE___ASHRDI3}": "0",
        "${HAVE___CHKSTK}": "0",
        "${HAVE___CHKSTK_MS}": "0",
        "${HAVE___CMPDI2}": "0",
        "${HAVE___DIVDI3}": "0",
        "${HAVE___FIXDFDI}": "0",
        "${HAVE___FIXSFDI}": "0",
        "${HAVE___FLOATDIDF}": "0",
        "${HAVE___LSHRDI3}": "0",
        "${HAVE___MAIN}": "0",
        "${HAVE___MODDI3}": "0",
        "${HAVE___UDIVDI3}": "0",
        "${HAVE___UMODDI3}": "0",
        "${HAVE____CHKSTK}": "0",
        "${HAVE____CHKSTK_MS}": "0",
        "${HOST_LINK_VERSION}": "",
        "${LIBPFM_HAS_FIELD_CYCLES}": "0",
        "${LLVM_DEFAULT_TARGET_TRIPLE}": "",
        "${LLVM_ENABLE_LIBXML2}": "0",
        "${LLVM_ENABLE_TERMINFO}": "__has_include(<curses.h>)",
        "${LLVM_GISEL_COV_PREFIX}": "",
        "${LLVM_HAVE_LIBXAR}": "0",
        "${LLVM_HOST_TRIPLE}": "",
        "${LLVM_NATIVE_ARCH}": "",
        "${LLVM_ON_UNIX}": "1",
        "${LLVM_PLUGIN_EXT}": ".so",
        "${LLVM_TARGET_TRIPLE_ENV}": "",
        "${LLVM_VERSION_INFO}": llvm_version,
        "${LLVM_VERSION_MAJOR}": version.major,
        "${LLVM_VERSION_MINOR}": version.minor,
        "${LLVM_VERSION_PATCH}": version.patch,
        "${LLVM_WITH_Z3}": "0",
        "${LTDL_SHLIB_EXT}": ".so",
        "${PACKAGE_BUGREPORT}": "",
        "${PACKAGE_NAME}": "",
        "${PACKAGE_STRING}": "",
        "${PACKAGE_VENDOR}": "WPILib",
        "${PACKAGE_VERSION}": llvm_version,
        "${RETSIGTYPE}": "void",
        "${stricmp}": "stricmp",
        "${strdup}": "strdup",
    }

    # Defines in this list will be defined. All others will be undefined.
    define_enables = [
        "LLVM_ENABLE_REVERSE_ITERATION", "LLVM_ENABLE_THREADS",
        "LLVM_HAS_ATOMICS"
    ]

    # Perform config variable replacements
    for key in config_variables.keys():
        content = content.replace(key, config_variables[key])

    define01_rgx = re.compile(r"#cmakedefine01 (\w+)$")
    define_rgx = re.compile(r"#cmakedefine (\w+)$")

    output = ""
    for line in content.split("\n"):
        # #cmakedefine01
        m = define01_rgx.search(line)
        if m:
            define = m.group(1)
            if define in define_enables:
                output += f"#define {define} 1\n"
            else:
                output += f"#define {define} 0\n"
            continue

        # #cmakedefine
        m = define_rgx.search(line)
        if m:
            define = m.group(1)
            if define in define_enables:
                output += f"#define {define}\n"
            else:
                output += f"#undef {define}\n"
            continue

        line = line.replace("#cmakedefine ", "#define ")
        output += line + "\n"

    return output


def main():
    llvm_version = "13.0.0"
    tar_filename = os.path.join(tempfile.gettempdir(),
                                f"llvmorg-{llvm_version}.tar.gz")

    print(f"Downloading llvmorg-{llvm_version}.tar.gz to {tar_filename}...",
          end="",
          flush=True)
    if not os.path.exists(tar_filename):
        # Download LLVM source tarball if it doesn't yet exist, then write it to
        # disk. We download the .tar.gz because it's smaller than the .zip.
        response = requests.get(
            f"https://github.com/llvm/llvm-project/archive/refs/tags/llvmorg-{llvm_version}.tar.gz"
        )
        open(tar_filename, "wb").write(response.content)
    print(" done.", flush=True)

    # Extract relevant source files from tarball
    extracted_files = []
    with tarfile.open(tar_filename) as tar:
        print(f"Getting list of files in {tar_filename}...", end="", flush=True)
        members = tar.getmembers()
        print(" done.", flush=True)

        print(f"Extracting files from {tar_filename}...", end="", flush=True)
        for member in members:
            if not member.isfile():
                continue

            name = member.name[len(f"llvm-project-llvmorg-{llvm_version}/"):]

            # These files are contained by the automatically updated
            # directories, but they are manually edited
            if name.endswith("ErrorHandling.h") or name.endswith(
                    "ErrorHandling.cpp") or name.endswith("StringExtras.h"):
                continue

            # These directories are automatically updated
            if name.startswith("llvm/include/llvm/ADT/") or name.startswith(
                    "llvm/include/llvm/Config") or name.startswith(
                        "llvm/include/llvm/Support/") or name.startswith(
                            "llvm/lib/Support/"):
                dest = os.path.join(tempfile.gettempdir(), member.name)

                # Make leading directories
                if not os.path.exists(os.path.dirname(dest)):
                    os.makedirs(os.path.dirname(dest))

                # Write extracted file
                open(dest, "wb").write(tar.extractfile(member).read())

                extracted_files.append(dest)
        print(" done.", flush=True)

    # Get list of files in wpiutil
    root = get_repo_root()
    wpiutil = os.path.join(root, "wpiutil")
    wpi_files = [
        os.path.join(dp, f) for dp, dn, fn in os.walk(
            os.path.join(wpiutil, "src/main/native/include/wpi")) for f in fn
    ] + [
        os.path.join(dp, f) for dp, dn, fn in os.walk(
            os.path.join(wpiutil, "src/main/native/cpp/llvm")) for f in fn
    ]

    # Iterate through files in wpiutil. If a file has "LLVM Compiler" or
    # "LLVM Project" in it, copy the corresponding file from LLVM
    print("Copying files from LLVM...", end="", flush=True)
    for wpi_file in wpi_files:
        content = open(wpi_file).read()
        # ReverseIteration.h is missing a license header
        if "LLVM Compiler" in content or "LLVM Project" in content or wpi_file.endswith(
                "ReverseIteration.h"):
            for extracted_file in extracted_files:
                if os.path.basename(wpi_file) == os.path.basename(
                        extracted_file):
                    shutil.copyfile(extracted_file, wpi_file)
                    break

    # Copy in config.h and rewrite #defines
    content = open(
        os.path.join(
            tempfile.gettempdir(),
            f"llvm-project-llvmorg-{llvm_version}/llvm/include/llvm/Config/config.h.cmake"
        )).read()
    content = replace_config_defines(content, llvm_version)
    open(os.path.join(wpiutil, "src/main/native/include/wpi/config.h"),
         "w").write(content)

    # Copy in llvm-config.h and rewrite #defines
    content = open(
        os.path.join(
            tempfile.gettempdir(),
            f"llvm-project-llvmorg-{llvm_version}/llvm/include/llvm/Config/llvm-config.h.cmake"
        )).read()
    content = replace_config_defines(content, llvm_version)
    open(os.path.join(wpiutil, "src/main/native/include/wpi/llvm-config.h"),
         "w").write(content)

    print(" done.", flush=True)

    print("Fixing up source files...", end="", flush=True)
    for wpi_file in wpi_files:
        content = open(wpi_file).read()

        # Rename namespace from llvm to wpi
        content = content.replace("namespace llvm", "namespace wpi")
        content = content.replace("llvm::", "wpi::")

        # Fix #includes
        content = content.replace("include \"llvm/ADT", "include \"wpi")
        content = content.replace("include \"llvm/Config", "include \"wpi")
        content = content.replace("include \"llvm/Support", "include \"wpi")

        # Fix include guards
        content = content.replace("LLVM_ADT_", "WPIUTIL_WPI_")
        content = content.replace("LLVM_SUPPORT_", "WPIUTIL_WPI_")

        # Remove abi-breaking.h references and replace #defines from it
        content = content.replace("#include \"wpi/abi-breaking.h\"\n", "")
        content = content.replace("#if LLVM_ENABLE_ABI_BREAKING_CHECKS",
                                  "#if 0")
        content = content.replace("#if LLVM_ENABLE_REVERSE_ITERATION", "#if 1")

        # Replace ArrayRef with span
        content = content.replace("include \"wpi/ArrayRef.h\"",
                                  "include <span>")
        content = re.sub(r"MutableArrayRef<([^>]+)>", r"span<\1>", content)
        content = re.sub(r"ArrayRef<([^>]+)>", r"span<const \1>", content)

        # Remove raw_ostream colors
        if wpi_file.endswith("raw_ostream.h"):
            content = re.sub(
                r"// color order matches ANSI.*?enum class Colors \{[^\}]+\};\n",
                "",
                content,
                flags=re.DOTALL | re.MULTILINE)
            content = re.sub(r"  static constexpr Colors \w+ = Colors::\w+;\n",
                             r"", content)
            content = re.sub(
                r"/// Compute whether colors.*?bool prepare_colors\(\);\n",
                "",
                content,
                flags=re.DOTALL | re.MULTILINE)
            content = re.sub(
                r"/// Changes the foreground color.*?virtual raw_ostream &changeColor\(.*?\);\n",
                "",
                content,
                flags=re.DOTALL | re.MULTILINE)
            content = re.sub(
                r"/// Resets the colors.*?virtual raw_ostream &resetColor\(\);\n",
                "",
                content,
                flags=re.DOTALL | re.MULTILINE)
            content = re.sub(
                r"/// Reverses the foreground.*?virtual raw_ostream &reverseColor\(\);\n",
                "",
                content,
                flags=re.DOTALL | re.MULTILINE)
        if wpi_file.endswith("raw_ostream.cpp"):
            content = re.sub(
                r"constexpr raw_ostream::Colors raw_ostream::\w+;\n", "",
                content)
            content = re.sub(
                r"bool raw_ostream::prepare_colors\(\) \{[^\}]+\}\n\n",
                "",
                content,
                flags=re.MULTILINE)
            content = re.sub(
                r"raw_ostream &raw_ostream::changeColor\([^\}]+\}\n\n",
                "",
                content,
                flags=re.MULTILINE)
            content = re.sub(
                r"raw_ostream &raw_ostream::resetColor\([^\}]+\}\n\n",
                "",
                content,
                flags=re.MULTILINE)
            content = re.sub(
                r"raw_ostream &raw_ostream::reverseColor\([^\}]+\}\n\n",
                "",
                content,
                flags=re.MULTILINE)

        # Remove extra raw_ostream::operator<<() overloads from raw_ostream.h
        # and raw_ostream.cpp

        if wpi_file.endswith("raw_ostream.h"):
            content = content.replace(
                "  raw_ostream &write_hex(unsigned long long N);\n", "")
        if wpi_file.endswith("raw_ostream.cpp"):
            content = re.sub(
                r"raw_ostream &raw_ostream::write_hex\(unsigned long long N\) \{.*?\n\}\n\n",
                "",
                content,
                flags=re.DOTALL | re.MULTILINE)

        if wpi_file.endswith("raw_ostream.h"):
            content = content.replace(
                "  raw_ostream &write_uuid(const uuid_t UUID);\n", "")
        if wpi_file.endswith("raw_ostream.cpp"):
            content = re.sub(
                r"raw_ostream &raw_ostream::write_uuid\(const uuid_t UUID\) \{.*?\n\}\n\n",
                "",
                content,
                flags=re.DOTALL | re.MULTILINE)

        if wpi_file.endswith("raw_ostream.h"):
            content = content.replace("  raw_ostream &operator<<(double N);\n",
                                      "")
        if wpi_file.endswith("raw_ostream.cpp"):
            content = re.sub(
                r"raw_ostream &raw_ostream::operator<<\(double N\) \{.*?\n\}\n\n",
                "",
                content,
                flags=re.DOTALL | re.MULTILINE)

        if wpi_file.endswith("raw_ostream.h"):
            content = content.replace(
                "  raw_ostream &operator<<(unsigned long N);\n", "")
        if wpi_file.endswith("raw_ostream.cpp"):
            content = re.sub(
                r"raw_ostream &raw_ostream::operator<<\(unsigned long N\) \{.*?\n\}\n\n",
                "",
                content,
                flags=re.DOTALL | re.MULTILINE)

        if wpi_file.endswith("raw_ostream.h"):
            content = content.replace("  raw_ostream &operator<<(long N);\n",
                                      "")
        if wpi_file.endswith("raw_ostream.cpp"):
            content = re.sub(
                r"raw_ostream &raw_ostream::operator<<\(long N\) \{.*?\n\}\n\n",
                "",
                content,
                flags=re.DOTALL | re.MULTILINE)

        if wpi_file.endswith("raw_ostream.h"):
            content = content.replace(
                "  raw_ostream &operator<<(unsigned long long N);\n", "")
        if wpi_file.endswith("raw_ostream.cpp"):
            content = re.sub(
                r"raw_ostream &raw_ostream::operator<<\(unsigned long long N\) \{.*?\n\}\n\n",
                "",
                content,
                flags=re.DOTALL | re.MULTILINE)

        if wpi_file.endswith("raw_ostream.h"):
            content = content.replace(
                "  raw_ostream &operator<<(long long N);\n", "")
        if wpi_file.endswith("raw_ostream.cpp"):
            content = re.sub(
                r"raw_ostream &raw_ostream::operator<<\(long long N\) \{.*?\n\}\n\n",
                "",
                content,
                flags=re.DOTALL | re.MULTILINE)

        if wpi_file.endswith("raw_ostream.h"):
            content = content.replace(
                "  raw_ostream &operator<<(const void *P);\n", "")
        if wpi_file.endswith("raw_ostream.cpp"):
            content = re.sub(
                r"raw_ostream &raw_ostream::operator<<\(const void \*P\) \{.*?\n\}\n\n",
                "",
                content,
                flags=re.DOTALL | re.MULTILINE)

        if wpi_file.endswith("raw_ostream.h"):
            content = content.replace("  raw_ostream &operator<<(Colors C);\n",
                                      "")
        if wpi_file.endswith("raw_ostream.cpp"):
            content = re.sub(
                r"raw_ostream &raw_ostream::operator<<\(Colors C\) \{.*?\n\}\n\n",
                "",
                content,
                flags=re.DOTALL | re.MULTILINE)

        if wpi_file.endswith("raw_ostream.h"):
            content = re.sub(
                r"  // Formatted output, see the .*? function in .*?\.\n"
                r"  raw_ostream &operator<<\(const format_object_base &Fmt\);\n\n",
                "", content)
        if wpi_file.endswith("raw_ostream.cpp"):
            content = re.sub(
                r"raw_ostream &raw_ostream::operator<<\(const format_object_base &Fmt\) \{.*?\n\}\n\n",
                "",
                content,
                flags=re.DOTALL | re.MULTILINE)

        if wpi_file.endswith("raw_ostream.h"):
            content = re.sub(
                r"  // Formatted output, see the .*? function in .*?\.\n"
                r"  raw_ostream &operator<<\(const formatv_object_base &\);\n\n",
                "", content)
        if wpi_file.endswith("raw_ostream.cpp"):
            content = re.sub(
                r"raw_ostream &raw_ostream::operator<<\(const formatv_object_base &Obj\) \{.*?\n\}\n\n",
                "",
                content,
                flags=re.DOTALL | re.MULTILINE)

        if wpi_file.endswith("raw_ostream.h"):
            content = re.sub(
                r"  // Formatted output, see the .*? function in .*?\.\n"
                r"  raw_ostream &operator<<\(const FormattedString &\);\n\n",
                "", content)
        if wpi_file.endswith("raw_ostream.cpp"):
            content = re.sub(
                r"raw_ostream &raw_ostream::operator<<\(const FormattedString &FS\) \{.*?\n\}\n\n",
                "",
                content,
                flags=re.DOTALL | re.MULTILINE)

        if wpi_file.endswith("raw_ostream.h"):
            content = re.sub(
                r"  // Formatted output, see the .*? function in .*?\.\n"
                r"  raw_ostream &operator<<\(const FormattedNumber &\);\n\n",
                "", content)
        if wpi_file.endswith("raw_ostream.cpp"):
            content = re.sub(
                r"raw_ostream &raw_ostream::operator<<\(const FormattedNumber &FN\) \{.*?\n\}\n\n",
                "",
                content,
                flags=re.DOTALL | re.MULTILINE)

        if wpi_file.endswith("raw_ostream.h"):
            content = re.sub(
                r"  // Formatted output, see the .*? function in .*?\.\n"
                r"  raw_ostream &operator<<\(const FormattedBytes &\);\n\n", "",
                content)
        if wpi_file.endswith("raw_ostream.cpp"):
            content = re.sub(
                r"raw_ostream &raw_ostream::operator<<\(const FormattedBytes &FB\) \{.*?\n\}\n\n",
                "",
                content,
                flags=re.DOTALL | re.MULTILINE)

        # Remove format_object_base::home()
        if wpi_file.endswith("raw_ostream.cpp"):
            content = content.replace(
                "// Out of line virtual method.\nvoid format_object_base::home() {\n}\n",
                "")

        # Replace sys::ChangeStdoutMode()
        if wpi_file.endswith("raw_ostream.cpp"):
            content = content.replace(
                """    // Change stdout's text/binary mode based on the Flags.
    sys::ChangeStdoutMode(Flags);
""", """    // If user requested binary then put stdout into binary mode if
    // possible.
    if (!(Flags & fs::OF_Text)) {
#if defined(_WIN32)
      _setmode(_fileno(stdout), _O_BINARY);
#endif
    }
""")

        # Remove FileLocker member functions in raw_fd_ostream
        if wpi_file.endswith("raw_ostream.h"):
            content = re.sub(r"/// Locks the underlying file..*? lock\(\);\n",
                             "",
                             content,
                             flags=re.DOTALL | re.MULTILINE)
            content = re.sub(
                r"/// Tries to lock the underlying file..*? tryLockFor\(std::chrono::milliseconds Timeout\);\n",
                "",
                content,
                flags=re.DOTALL | re.MULTILINE)
        if wpi_file.endswith("raw_ostream.cpp"):
            content = re.sub(
                r"Expected<sys::fs::FileLocker> raw_fd_ostream::lock\(\) \{[^\}]+\}\n\n",
                "",
                content,
                flags=re.MULTILINE)
            content = re.sub(
                r"Expected<sys::fs::FileLocker>\nraw_fd_ostream::tryLockFor\(std::chrono::milliseconds Timeout\) \{[^\}]+\}\n\n",
                "",
                content,
                flags=re.MULTILINE)

        # Remove wpi::writeToOutput()
        if wpi_file.endswith("raw_ostream.h"):
            content = re.sub(
                r"class Error;\n\n/// This helper creates an output stream.*?;\n\n",
                "",
                content,
                flags=re.DOTALL | re.MULTILINE)
        if wpi_file.endswith("raw_ostream.cpp"):
            content = re.sub(r"Error wpi::writeToOutput\(.*?\n\}\n",
                             "",
                             content,
                             flags=re.DOTALL | re.MULTILINE)

        # Replace sys::Process usage
        if wpi_file.endswith("raw_ostream.cpp"):
            content = content.replace(
                "if (auto EC = sys::Process::SafelyCloseFileDescriptor(FD))",
                "if (::close(FD) < 0)")
            content = content.replace(
                "sys::Process::FileDescriptorIsDisplayed(FD)", "false")
            content = content.replace(
                "sys::Process::FileDescriptorHasColors(FD)", "false")

        # Fix file I/O flags
        if wpi_file.endswith("raw_ostream.cpp"):
            content = content.replace(
                """  int FD;
  if (Access & sys::fs::FA_Read)
    EC = sys::fs::openFileForReadWrite(Filename, FD, Disp, Flags);
  else
    EC = sys::fs::openFileForWrite(Filename, FD, Disp, Flags);
  if (EC)
    return -1;
""", """  fs::file_t F;
  if (Access & fs::FA_Read) {
    F = fs::OpenFileForReadWrite(fs::path{std::string_view{Filename.data(), Filename.size()}}, EC, Disp, Flags);
  } else {
    F = fs::OpenFileForWrite(fs::path{std::string_view{Filename.data(), Filename.size()}}, EC, Disp, Flags);
  }
  if (EC)
    return -1;
  int FD = fs::FileToFd(F, EC, Flags);
  if (EC)
    return -1;
""")

        if wpi_file.endswith("raw_ostream.h") or wpi_file.endswith(
                "raw_ostream.cpp"):
            content = content.replace("sys::fs::", "fs::")

        # Remove extra header-only raw_ostream::operator<<() overloads
        if wpi_file.endswith("raw_ostream.h"):
            content = re.sub(
                r"  raw_ostream &operator<<\(StringRef Str\) \{.*?\n  \}\n\n",
                "",
                content,
                flags=re.DOTALL | re.MULTILINE)
            content = re.sub(
                r"  raw_ostream &operator<<\(unsigned int N\) \{.*?\n  \}\n\n",
                "",
                content,
                flags=re.DOTALL | re.MULTILINE)
            content = re.sub(
                r"  raw_ostream &operator<<\(int N\) \{.*?\n  \}\n\n",
                "",
                content,
                flags=re.DOTALL | re.MULTILINE)

            # Add raw_ostream::write(const uint8_t*, size_t)
            content = re.sub(
                r"  raw_ostream &write\(const char \*Ptr, size_t Size\);",
                r"""\g<0>
  raw_ostream &write(const uint8_t *Ptr, size_t Size) {
    return write(reinterpret_cast<const char *>(Ptr), Size);
  }""",
                content,
                flags=re.MULTILINE)

            # Add raw_ostream::operator<<(span<const uint8_t>)
            content = re.sub(
                r"  raw_ostream &operator<<\(const SmallVectorImpl<char> &Str\) {[^}]+}\n",
                r"""\g<0>
  raw_ostream &operator<<(span<const uint8_t> Arr) {
    // Inline fast path, particularly for arrays with a known length.
    size_t Size = Arr.size();

    // Make sure we can use the fast path.
    if (Size > (size_t)(OutBufEnd - OutBufCur))
      return write(Arr.data(), Size);

    if (Size) {
      memcpy(OutBufCur, Arr.data(), Size);
      OutBufCur += Size;
    }
    return *this;
  }
""", content)

        # Replace StringRef with std::string_view
        content = content.replace("include \"wpi/StringRef.h\"",
                                  "include <string_view>")
        content = content.replace("class StringRef;\n", "")
        content = re.sub(r"\bwpi::StringRef\b", r"std::string_view", content)
        content = re.sub(r"\bStringRef\b", r"std::string_view", content)
        content = content.replace(".bytes()", "")
        content = re.sub(r"(\w+)\.drop_front\(", r"drop_front(\1, ", content)

        # Replace Optional with std::optional
        content = content.replace("include \"wpi/None.h\"",
                                  "include <optional>")
        content = content.replace("Optional<", "std::optional<")
        content = re.sub(r"\bNone\b", r"std::nullopt", content)
        content = content.replace("NoneType", "std::nullopt_t")

        # Replace wpi/FileSystem.h with wpi/fs.h
        content = content.replace("include \"wpi/FileSystem.h\"",
                                  "include \"wpi/fs.h\"")

        # Replace llvm_unreachable() with wpi_unreachable()
        content = content.replace("llvm_unreachable", "wpi_unreachable")

        # Remove unused headers
        content = content.replace("#include \"llvm-c/ErrorHandling.h\"\n", "")
        content = content.replace("#include \"wpi/Debug.h\"\n", "")
        content = content.replace("#include \"wpi/Errc.h\"\n", "")
        content = content.replace("#include \"wpi/Error.h\"\n", "")
        content = content.replace("#include \"wpi/Format.h\"\n", "")
        content = content.replace("#include \"wpi/FormatVariadic.h\"\n", "")
        content = content.replace("#include \"wpi/NativeFormatting.h\"\n", "")
        content = content.replace("#include \"wpi/Threading.h\"\n", "")
        content = content.replace("llvm_is_multithreaded()", "1")

        # Add missing #includes to Unicode.h
        if wpi_file.endswith("Unicode.h"):
            content = re.sub(r"#define WPIUTIL_WPI_UNICODE_H\n",
                             r"\g<0>\n#include <string_view>\n", content)

        # Fix multi-line comment warnings in UnicodeCaseFold.cpp
        if wpi_file.endswith("UnicodeCaseFold.cpp"):
            content = re.sub(r"(//.*?) \\", r"\g<1>", content)

        # Add missing #includes to StringMapEntry.h
        if wpi_file.endswith("StringMapEntry.h"):
            content = content.replace(
                "#define WPIUTIL_WPI_STRINGMAPENTRY_H\n\n#include <string_view>\n",
                "#define WPIUTIL_WPI_STRINGMAPENTRY_H\n\n#include <cassert>\n#include <cstring>\n#include <string_view>\n#include <optional>\n"
            )

        # Add missing #includes to StringMap.h
        if wpi_file.endswith("StringMap.h"):
            content = content.replace(
                "#include <iterator>\n",
                "#include <iterator>\n#include <utility>\n\n#include \"wpi/iterator.h\"\n#include \"wpi/iterator_range.h\"\n"
            )

        # Add iterator decrement back to StringMap.h
        if wpi_file.endswith("StringMap.h"):
            content = re.sub(
                r"DerivedTy operator\+\+\(int\) \{[^\}]+\}\n", r"""\g<0>
  DerivedTy &operator--() { // Predecrement
    --Ptr;
    ReversePastEmptyBuckets();
    return static_cast<DerivedTy &>(*this);
  }

  DerivedTy operator--(int) { // Post-decrement
    DerivedTy Tmp(Ptr);
    --*this;
    return Tmp;
  }
""", content)
            content = re.sub(
                r"void AdvancePastEmptyBuckets\(\) \{[^\}]+\}\n", r"""\g<0>
  void ReversePastEmptyBuckets() {
    while (*Ptr == nullptr || *Ptr == StringMapImpl::getTombstoneVal())
      --Ptr;
  }
""", content)

        # Add #include <optional> to raw_ostream.h
        if wpi_file.endswith("raw_ostream.h"):
            content = content.replace(
                "#include <string_view>\n",
                "#include <string_view>\n#include <optional>\n")

        # Add #include "wpi/fs.h" and "wpi/span.h" to raw_ostream.h
        if wpi_file.endswith("raw_ostream.h"):
            content = content.replace(
                "#include <type_traits>\n",
                "#include <type_traits>\n\n#include \"wpi/fs.h\"\n#include \"wpi/span.h\"\n"
            )

        # Remove extra #includes from raw_ostream.cpp
        if wpi_file.endswith("raw_ostream.cpp"):
            content = content.replace("#include \"wpi/Process.h\"\n", "")
            content = content.replace("#include \"wpi/Program.h\"\n", "")
            content = content.replace("#include \"wpi/STLExtras.h\"\n", "")

        # Add array_lengthof() to raw_ostream.cpp
        if wpi_file.endswith("raw_ostream.cpp"):
            content = re.sub(
                r"using namespace wpi;\n", r"""\g<0>
namespace {
// Find the length of an array.
template <class T, std::size_t N>
constexpr inline size_t array_lengthof(T (&)[N]) {
  return N;
}
}  // namespace""", content)

        # Remove #include "wpi/Twine.h"
        content = content.replace("#include \"wpi/Twine.h\"\n", "")

        # Add #include "wpi/span.h" to ConvertUTF.h
        if wpi_file.endswith("ConvertUTF.h"):
            content = content.replace(
                "#include <system_error>\n",
                "#include <system_error>\n\n#include \"wpi/span.h\"\n")

        # Add #include <cstring> to ConvertUTFWrapper.cpp
        if wpi_file.endswith("ConvertUTFWrapper.cpp"):
            content = content.replace("#include <span>",
                                      "#include <cstring>\n#include <span>")
            content = content.replace(
                "#include \"wpi/SwapByteOrder.h\"",
                "#include \"wpi/SmallVector.h\"\n#include \"wpi/SwapByteOrder.h\""
            )

        # Add #include "wpi/StringExtras.h" to DJB.cpp
        if wpi_file.endswith("DJB.cpp"):
            content = content.replace(
                "#include \"wpi/ConvertUTF.h\"",
                "#include \"wpi/ConvertUTF.h\"\n#include \"wpi/StringExtras.h\""
            )

        open(wpi_file, "w").write(content)

    print(" done.", flush=True)

    print("Applying patches...", end="", flush=True)
    apply_patches(root, [
        "upstream_utils/llvm-mathextras-sgn-lerp.patch",
        "upstream_utils/llvm-mathextras-remove-numbers.patch"
    ])
    print(" done.", flush=True)


if __name__ == "__main__":
    main()
