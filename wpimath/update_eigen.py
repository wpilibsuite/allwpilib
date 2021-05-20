#!/usr/bin/env python3

import os
import re
import shutil
import subprocess
import tempfile


def clone_repo(url, treeish):
    """Clones a git repo at the given URL and checks out the given tree-ish
    (either branch or tag).

    Keyword argument:
    url -- The URL of the git repo
    treeish -- The tree-ish to check out (branch or tag)
    """
    repo = os.path.basename(url)
    dest = os.path.join(os.getcwd(), repo).rstrip(".git")

    # Clone Git repository into current directory or update it
    if not os.path.exists(dest):
        subprocess.run(["git", "clone", url, dest])
        os.chdir(dest)
    else:
        os.chdir(dest)
        subprocess.run(["git", "fetch", "origin", treeish])

    # Get list of heads
    # Example output of "git ls-remote --heads":
    #   From https://gitlab.com/libeigen/eigen.git
    #   77c66e368c7e355f8be299659f57b0ffcaedb505  refs/heads/3.4
    #   3e006bfd31e4389e8c5718c30409cddb65a73b04  refs/heads/master
    ls_out = subprocess.check_output(["git", "ls-remote",
                                      "--heads"]).decode().rstrip()
    heads = [x.split()[1] for x in ls_out.split("\n")[1:]]

    if f"refs/heads/{treeish}" in heads:
        # Checking out the remote branch avoids needing to handle syncing a
        # preexisting local one
        subprocess.run(["git", "checkout", f"origin/{treeish}"])
    else:
        subprocess.run(["git", "checkout", treeish])


def eigen_inclusions(dp, f):
    """Returns true if the given file in the "Eigen" include directory of the
    Eigen git repo should be copied into allwpilib

    Keyword arguments:
    dp -- directory path
    f -- filename
    """
    abspath = os.path.join(dp, f)

    # Exclude build system
    if f == "CMakeLists.txt":
        return False

    # Exclude code licensed under MPL2
    if f == "NonMPL2.h":
        return False

    # Exclude BLAS support
    if f.endswith("_BLAS.h") or "blas" in f:
        return False

    # Exclude LAPACK support
    if f.endswith("_LAPACKE.h") or "lapack" in f:
        return False

    # Exclude MKL support
    if "MKL" in f:
        return False

    # Include architectures we care about
    if "Core/arch/" in abspath:
        return ("arch/AVX/" in abspath or "arch/Default" in abspath or
                "arch/NEON" in abspath or "arch/SSE" in abspath)

    # Include the following modules
    modules = [
        "Cholesky",
        "Core",
        "Eigenvalues",
        "Householder",
        "Jacobi",
        "LU",
        "QR",
        "SVD",
        "StlSupport",
        "misc",
        "plugins",
    ]
    modules_rgx = r"|".join("/" + m for m in modules)

    # "Std" matches StdDeque, StdList, and StdVector headers
    if re.search(modules_rgx, abspath) or "Std" in f:
        return True
    else:
        # Exclude all other modules
        return False


def unsupported_inclusions(dp, f):
    """Returns true if the given file in the "unsupported" include directory of
    the Eigen git repo should be copied into allwpilib

    Keyword arguments:
    dp -- directory path
    f -- filename
    """
    abspath = os.path.join(dp, f)

    # Exclude build system and READMEs
    if f == "CMakeLists.txt" or "README" in f:
        return False

    # Include the AutoDiff and MatrixFunctions modules
    return "AutoDiff" in abspath or "MatrixFunctions" in abspath


def comment_out_invalid_includes(filename):
    """Comment out #include directives that include a nonexistent file

    Keyword arguments:
    filename -- file to search for includes
    """
    # Read header
    with open(filename) as f:
        old_contents = f.read()

    new_contents = ""
    pos = 0
    for match in re.finditer(r"#include \"([^\"]+)\"", old_contents):
        include = match.group(1)

        # Write contents from before this match
        new_contents += old_contents[pos:match.span()[0]]

        # Comment out #include if the file doesn't exist
        if not os.path.exists(os.path.join(os.path.dirname(filename), include)):
            new_contents += "// "

        new_contents += match.group()
        pos = match.span()[1]

    # Write rest of file if it wasn't all processed
    if pos < len(old_contents):
        new_contents += old_contents[pos:]

    # Write modified file back out
    if old_contents != new_contents:
        with open(filename, "w") as f:
            f.write(new_contents)


def main():
    cwd = os.getcwd()
    os.chdir(tempfile.gettempdir())
    clone_repo("https://gitlab.com/libeigen/eigen.git", "3.3.9")

    # Get list of Eigen headers to copy
    files = [
        os.path.join(dp, f)
        for dp, dn, fn in os.walk("Eigen")
        for f in fn
        if eigen_inclusions(dp, f)
    ]
    files += [
        os.path.join(dp, f)
        for dp, dn, fn in os.walk("unsupported")
        for f in fn
        if unsupported_inclusions(dp, f)
    ]

    # Delete old Eigen install
    include_root = os.path.join(cwd, "src/main/native/eigeninclude")
    for folder in ["Eigen", "unsupported"]:
        shutil.rmtree(os.path.join(include_root, folder), ignore_errors=True)

    # Copy new Eigen into allwpilib
    for f in files:
        dest_file = os.path.join(include_root, f)
        dest_dir = os.path.dirname(dest_file)
        if not os.path.exists(dest_dir):
            os.makedirs(dest_dir)
        shutil.copyfile(f, dest_file)

    # Comment out missing headers
    for f in files:
        dest_file = os.path.join(include_root, f)
        comment_out_invalid_includes(dest_file)


if __name__ == "__main__":
    main()
