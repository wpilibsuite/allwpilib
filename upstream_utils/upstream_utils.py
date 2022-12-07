import os
import re
import shutil
import subprocess
import tempfile


def clone_repo(url, treeish, shallow=True):
    """Clones a Git repo at the given URL into a temp folder, checks out the
    given tree-ish (either branch or tag), then returns the repo root.

    Keyword argument:
    url -- The URL of the Git repo
    treeish -- The tree-ish to check out (branch or tag)
    shallow -- Whether to do a shallow clone

    Returns:
    root -- root directory of the cloned Git repository
    """
    cwd = os.getcwd()
    os.chdir(tempfile.gettempdir())

    repo = os.path.basename(url)
    dest = os.path.join(os.getcwd(), repo)
    if dest.endswith(".git"):
        dest = dest[:-4]

    # Clone Git repository into current directory or update it
    if not os.path.exists(dest):
        cmd = ["git", "clone"]
        if shallow:
            cmd += ["--branch", treeish, "--depth", "1"]
        subprocess.run(cmd + [url, dest])
        os.chdir(dest)
    else:
        os.chdir(dest)
        subprocess.run(["git", "fetch", "origin", treeish])

    # Get list of heads
    # Example output of "git ls-remote --heads":
    #   From https://gitlab.com/libeigen/eigen.git
    #   77c66e368c7e355f8be299659f57b0ffcaedb505  refs/heads/3.4
    #   3e006bfd31e4389e8c5718c30409cddb65a73b04  refs/heads/master
    ls_out = subprocess.check_output(["git", "ls-remote", "--heads"]).decode().rstrip()
    heads = [x.split()[1] for x in ls_out.split("\n")[1:]]

    if f"refs/heads/{treeish}" in heads:
        # Checking out the remote branch avoids needing to handle syncing a
        # preexisting local one
        subprocess.run(["git", "checkout", f"origin/{treeish}"])
    else:
        subprocess.run(["git", "checkout", treeish])

    os.chdir(cwd)
    return dest


def get_repo_root():
    """Returns the Git repository root as an absolute path.

    An empty string is returned if no repository root was found.
    """
    current_dir = os.path.abspath(os.getcwd())
    while current_dir != os.path.dirname(current_dir):
        if os.path.exists(current_dir + os.sep + ".git"):
            return current_dir
        current_dir = os.path.dirname(current_dir)
    return ""


def walk_if(top, pred):
    """Walks the current directory, then returns a list of files for which the
    given predicate is true.

    Keyword arguments:
    top -- the top directory to walk
    pred -- a function that takes a directory path and a filename, then returns
            True if the file should be included in the output list
    """
    return [
        os.path.join(dp, f) for dp, dn, fn in os.walk(top) for f in fn if pred(dp, f)
    ]


def copy_to(files, root):
    """Copies list of files to root by appending the relative paths of the files
    to root.

    The leading directories of root will be created if they don't yet exist.

    Keyword arguments:
    files -- list of files to copy
    root -- destination

    Returns:
    The list of files in their destination.
    """
    if not os.path.exists(root):
        os.makedirs(root)

    dest_files = []
    for f in files:
        dest_file = os.path.join(root, f)

        # Rename .cc file to .cpp
        if dest_file.endswith(".cc"):
            dest_file = os.path.splitext(dest_file)[0] + ".cpp"
        if dest_file.endswith(".c"):
            dest_file = os.path.splitext(dest_file)[0] + ".cpp"

        # Make leading directory
        dest_dir = os.path.dirname(dest_file)
        if not os.path.exists(dest_dir):
            os.makedirs(dest_dir)

        shutil.copyfile(f, dest_file)
        dest_files.append(dest_file)
    return dest_files


def walk_cwd_and_copy_if(pred, root):
    """Walks the current directory, generates a list of files for which the
    given predicate is true, then copies that list to root by appending the
    relative paths of the files to root.

    The leading directories of root will be created if they don't yet exist.

    Keyword arguments:
    pred -- a function that takes a directory path and a filename, then returns
            True if the file should be included in the output list
    root -- destination

    Returns:
    The list of files in their destination.
    """
    files = walk_if(".", pred)
    files = copy_to(files, root)
    return files


def comment_out_invalid_includes(filename, include_roots):
    """Comment out #include directives that include a nonexistent file

    Keyword arguments:
    filename -- file to search for includes
    include_roots -- list of search paths for includes
    """
    # Read header
    with open(filename) as f:
        old_contents = f.read()

    new_contents = ""
    pos = 0
    for match in re.finditer(r"#include \"([^\"]+)\"", old_contents):
        include = match.group(1)

        # Write contents from before this match
        new_contents += old_contents[pos : match.span()[0]]

        # Comment out #include if the file doesn't exist in current directory or
        # include root
        if not os.path.exists(
            os.path.join(os.path.dirname(filename), include)
        ) and not any(
            os.path.exists(os.path.join(include_root, include))
            for include_root in include_roots
        ):
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


def git_am(patch, use_threeway=False, ignore_whitespace=False):
    """Apply patch to a Git repository in the current directory using "git am".

    Keyword arguments:
    patch -- patch file relative to the root
    use_threeway -- use a three-way merge when applying the patch
    ignore_whitespace -- ignore whitespace in the patch file
    """
    args = ["git", "am"]
    if use_threeway:
        args.append("-3")
    if ignore_whitespace:
        args.append("--ignore-whitespace")

    subprocess.check_output(args + [patch])
