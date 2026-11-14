# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
import argparse
import re
import shutil
import subprocess
import sys
import tempfile
from collections.abc import Sequence
from enum import Flag, auto
from pathlib import Path


class GeneratorTypes(Flag):
    NONE = 0
    QUICKBUF = auto()
    NANOPB = auto()


def make_arg_parser(
    subproject_root: Path, repo_root: Path, types: GeneratorTypes = GeneratorTypes.NONE
) -> argparse.ArgumentParser:
    """Creates an ArgumentParser configured for QuickBuffers or nanopb generation.

    Keyword arguments:
    subproject_root -- Path to the subproject root. Determines default output and proto directories.
    repo_root -- Path to the repo root. Used to find the nanopb generator.
    types -- Flagset of all the types of generators that arguments are needed for.

    Returns:
    The ArgumentParser with the necessary arguments.
    """
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--output_directory",
        help="Optional. If set, will output the generated files to this directory, otherwise it will use a path relative to the script",
        default=subproject_root / "src/generated",
        type=Path,
    )
    if GeneratorTypes.QUICKBUF in types or GeneratorTypes.NANOPB in types:
        parser.add_argument(
            "--proto_directory",
            help="Optional. If set, will use this directory to glob for protobuf files",
            default=subproject_root / "src/main/proto",
            type=Path,
        )
    if GeneratorTypes.QUICKBUF in types:
        parser.add_argument(
            "--protoc",
            help="Protoc executable command",
            default="protoc",
        )
        parser.add_argument(
            "--quickbuf_plugin",
            help="Optional if you use protoc-quickbuf as protoc. Path to the quickbuf protoc plugin.",
        )
    if GeneratorTypes.NANOPB in types:
        parser.add_argument(
            "--nanopb",
            help="Nanopb generator command",
            default=repo_root
            / "wpiutil/src/main/native/thirdparty/nanopb/generator/nanopb_generator.py",
        )
    return parser


def add_jinja_args(
    parser: argparse.ArgumentParser,
    subproject_root: Path,
    default_schema_file: str | Path | None,
) -> None:
    """Configures an ArgumentParser for Jinja generation.

    Keyword arguments:
    parser -- The ArgumentParser to configure.
    subproject_root -- Path to the subproject root. Determines default template directory.
    default_schema_file -- The path to the schema file, or None to skip using a schema file.
    """
    parser.add_argument(
        "--template_root",
        help="Optional. If set, will use this directory as the root for the jinja templates",
        default=subproject_root / "src/generate",
        type=Path,
    )
    if default_schema_file is not None:
        parser.add_argument(
            "--schema_file",
            help="Optional. If set, will use this file for the joystick schema",
            default=default_schema_file,
            type=Path,
        )


def write_file(output_dir: Path, outfn: str, contents: str) -> None:
    """Writes contents to a file.

    Keyword arguments:
    output_dir -- The output directory.
    outfn -- The output file name.
    contents -- The contents of the file.
    """
    output_dir.mkdir(parents=True, exist_ok=True)
    output_file = output_dir / outfn
    output_file.write_text(contents, encoding="utf-8", newline="\n")


def generate_quickbuf(
    protoc: Path, quickbuf_plugin: Path | None, output_directory: Path, proto_dir: Path
) -> None:
    """Generates QuickBuffers files.

    Keyword arguments:
    protoc -- The Protocol Buffers compiler.
    quickbuf_plugin -- The QuickBuffers protoc plugin, or None if using protoc-quickbuf.
    output_directory -- The base output directory to write generated files to.
    proto_dir -- The base directory with protobuf files.
    """
    # Wipe away all protobuf files (and only protobuf files) first to ensure correctness
    for file in output_directory.glob("**/Protobuf*.java"):
        file.unlink()
    # Nice to have if you delete the whole generated directory
    output_directory.mkdir(parents=True, exist_ok=True)
    for path in proto_dir.glob("**/*.proto"):
        args = [str(protoc)]
        if quickbuf_plugin:
            # Optional if using protoc-quickbuf
            args += [f"--plugin=protoc-gen-quickbuf={quickbuf_plugin}"]
        args += [
            f"--quickbuf_out=gen_descriptors=true:{output_directory.absolute()!s}",
            f"-I{proto_dir.absolute()}",
            str(path.absolute()),
        ]
        subprocess.check_call(args)
    # Prepend files with license
    for java_file in output_directory.glob("**/Protobuf*.java"):
        with java_file.open(encoding="utf-8") as f:
            content = f.read()

        java_file.write_text(
            "// Copyright (c) FIRST and other WPILib contributors.\n// Open Source Software; you can modify and/or share it under the terms of\n// the WPILib BSD license file in the root directory of this project.\n"
            + content,
            encoding="utf-8",
            newline="\n",
        )


def generate_python_protobuf(
    protoc: Sequence[str], output_directory: Path, proto_dir: Path
) -> None:
    """Generates Python protobuf files (*_pb2.py and *_pb2.pyi) with the official protoc.

    Keyword arguments:
    protoc -- The command that runs the Protocol Buffers compiler.
    output_directory -- The package directory to write generated files to.
    proto_dir -- The base directory with protobuf files.
    """
    protos = sorted(proto_dir.glob("**/*.proto"))
    with tempfile.TemporaryDirectory() as tmp:
        subprocess.check_call(
            [
                *protoc,
                f"-I{proto_dir.absolute()}",
                f"--python_out={tmp}",
                f"--pyi_out={tmp}",
                *(str(p.absolute()) for p in protos),
            ]
        )
        generated = sorted(Path(tmp).glob("**/*_pb2.py*"))

        names = [g.name for g in generated]
        if len(names) != len(set(names)):
            raise ValueError(
                f"Generated protobuf modules collide when flattened: {names}"
            )

        # Wipe away all protobuf files (and only protobuf files) first to ensure correctness
        for file in output_directory.glob("*_pb2.py*"):
            file.unlink()
        output_directory.mkdir(parents=True, exist_ok=True)

        modules = "|".join(sorted({g.name.split(".")[0] for g in generated}))
        # Only rewrite imports of modules generated here, never google.protobuf's.
        local_import = re.compile(
            rf"^from (?!google\.)[\w.]+ import ({modules})\b", re.MULTILINE
        )
        for file in generated:
            (output_directory / file.name).write_text(
                local_import.sub(r"from . import \1", file.read_text(encoding="utf-8")),
                encoding="utf-8",
                newline="\n",
            )

        # Mark the directory as a package. This is written here so that syncing the
        # whole directory into the source tree doesn't delete it.
        (output_directory / "__init__.py").touch()


def generate_nanopb(
    nanopb: Path,
    output_directory: Path,
    header_directory: Path,
    proto_dir: Path,
    extra_search_dirs: Sequence[Path] = (),
) -> None:
    """Generates QuickBuffers files.

    Keyword arguments:
    nanopb -- The nanopb generator.
    output_directory -- The base output directory to write generated files to.
    proto_dir -- The base directory with protobuf files.
    extra_search_dirs -- Extra directories to search for protobuf files.
    """
    # Wipe away all protobuf files (and only protobuf files) first to ensure correctness
    for file in output_directory.glob("**/*.npb.*"):
        file.unlink()
    # Nice to have if you delete the whole generated directory
    output_directory.mkdir(parents=True, exist_ok=True)
    header_directory.mkdir(parents=True, exist_ok=True)
    for path in proto_dir.glob("**/*.proto"):
        subprocess.check_call(
            ([sys.executable] if str(nanopb).endswith(".py") else [])
            + [
                str(nanopb),
                *extra_search_dirs,
                f"-I{proto_dir.absolute()}",
                f"-D{output_directory.absolute()}",
                "-q",
                "-S.cpp",
                "-e.npb",
                str(path.absolute()),
            ],
        )
    # Prepend files with license
    for file in output_directory.glob("**/*.npb.*"):
        with file.open(encoding="utf-8") as f:
            content = f.read()

        file.write_text(
            "// Copyright (c) FIRST and other WPILib contributors.\n// Open Source Software; you can modify and/or share it under the terms of\n// the WPILib BSD license file in the root directory of this project.\n"
            + content,
            encoding="utf-8",
            newline="\n",
        )

    for file in output_directory.glob("**/*.npb.h"):
        target = header_directory / file.relative_to(output_directory)
        # Subtree may not exist if the header folder was emptied
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.move(file, target)
