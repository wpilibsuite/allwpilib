# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

"""Read native symbols on robot and desktop targets without executing binaries."""

import os
import re
import shutil
import subprocess


def llvm_tool(name):
    explicit = os.environ.get(name.upper().replace("-", "_"))
    if explicit:
        return explicit
    for command in (name, name + "-19", name + "-20", name + "-18"):
        if shutil.which(command):
            return command
    raise ValueError(f"Missing inspection tool: {name}")


def format_of(path):
    with path.open("rb") as stream:
        magic = stream.read(8)
    if magic.startswith(b"\x7fELF"):
        return "elf"
    if magic.startswith(b"MZ"):
        return "pe"
    if magic[:4] in (
        b"\xcf\xfa\xed\xfe",
        b"\xfe\xed\xfa\xcf",
        b"\xca\xfe\xba\xbe",
        b"\xbe\xba\xfe\xca",
    ):
        return "macho"
    if magic == b"!<arch>\n":
        return "archive"
    raise ValueError(f"Unsupported binary (including thin archives): {path.name}")


def symbols(path, dynamic=False, architecture=None):
    kind = format_of(path)
    if kind == "pe":
        text = subprocess.check_output(
            [llvm_tool("llvm-readobj"), "--coff-exports", "--coff-imports", str(path)],
            text=True,
            timeout=120,
        )
        defined = set(re.findall(r"Export \{.*?\n  Name: ([^\n]+)", text, re.DOTALL))
        undefined = set(re.findall(r"^  Symbol: (.*?) \(\d+\)$", text, re.MULTILINE))
        return defined, undefined - defined
    # GNU nm also handles cross-architecture ELF symbol tables. LLVM handles Mach-O/COFF.
    command = [
        "nm" if kind == "elf" else llvm_tool("llvm-nm"),
        "--format=posix",
        "--no-demangle",
        "--extern-only",
    ]
    if dynamic and kind == "elf":
        command.append("--dynamic")
    if architecture and kind in ("macho", "archive"):
        command.append("--arch=" + architecture)
    result = subprocess.run(
        [*command, str(path)], capture_output=True, text=True, timeout=120, check=True
    )
    defined, undefined = set(), set()
    for line in result.stdout.splitlines():
        fields = line.split()
        if len(fields) < 2 or len(fields[1]) != 1:
            continue
        name, symbol_kind = fields[:2]
        name = name.removeprefix("__imp_")
        name = name.split("@")[0] if not name.startswith("?") else name
        if kind == "macho" or architecture:
            name = name.removeprefix("_")
        name = name.removeprefix("__imp_")
        if symbol_kind in ("U", "w", "v"):
            undefined.add(name)
        elif symbol_kind.isupper() or symbol_kind in ("u", "i"):
            defined.add(name)
    if dynamic and kind == "macho":
        # The symbol table may retain private externals that dyld cannot bind.
        command = [llvm_tool("llvm-objdump"), "--macho", "--exports-trie"]
        if architecture:
            command.append("--arch=" + architecture)
        exports = subprocess.check_output([*command, str(path)], text=True, timeout=120)
        defined = {
            name.removeprefix("_")
            for name in re.findall(r"^0x[0-9A-Fa-f]+\s+(\S+)", exports, re.MULTILINE)
        }
    return defined, undefined - defined


def demangle(names):
    """Retain raw names when LLVM cannot decode a symbol, without losing the batch."""
    names = sorted(names)
    if not names:
        return {}, []
    result = subprocess.run(
        [llvm_tool("llvm-cxxfilt")],
        input="\n".join(names) + "\n",
        capture_output=True,
        text=True,
        timeout=120,
        check=True,
    )
    values = result.stdout.splitlines()
    if len(values) != len(names):
        raise ValueError("Demangler did not return one name per symbol")
    decoded = dict(zip(names, values, strict=True))
    undecoded = []
    microsoft = [name for name in names if name.startswith("?")]
    if microsoft:
        result = subprocess.run(
            [llvm_tool("llvm-undname")],
            input="\n".join(microsoft) + "\n",
            capture_output=True,
            text=True,
            timeout=120,
            check=False,
        )
        # Exit 1 also means unsupported mangling, with usable output for other names.
        if result.returncode and (
            result.returncode != 1
            or not result.stderr.strip()
            or any(
                line != "error: Invalid mangled name"
                for line in result.stderr.splitlines()
            )
        ):
            result.check_returncode()
        expected, seen = set(microsoft), set()
        for block in result.stdout.split("\n\n"):
            lines = block.splitlines()
            if not lines:
                continue
            name = lines[0]
            if name not in expected or name in seen or len(lines) > 2:
                raise ValueError("Unexpected Microsoft demangler output")
            seen.add(name)
            if len(lines) == 2:
                decoded[name] = re.sub(
                    r"\b(?:struct|class|union|enum)\s+", "", lines[1]
                )
            else:
                undecoded.append(name)
        if seen != expected:
            raise ValueError("Microsoft demangler did not return every symbol")
    return decoded, undecoded
