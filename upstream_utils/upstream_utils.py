import argparse
import os
import re
import shutil
import subprocess
import sys
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
    if url.startswith("file://"):
        os.chdir(os.path.dirname(url[7:]))
    else:
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


def has_git_rev(rev):
    cmd = ["git", "rev-parse", "--verify", "-q", rev]
    return subprocess.run(cmd, stdout=subprocess.DEVNULL).returncode == 0


class Lib:
    def __init__(
        self,
        name,
        url,
        old_tag,
        patch_list,
        copy_upstream_src,
        patch_options={},
        *,
        pre_patch_hook=None,
        pre_patch_commits=0,
    ):
        self.name = name
        self.url = url
        self.old_tag = old_tag
        self.patch_list = patch_list
        self.copy_upstream_src = copy_upstream_src
        self.patch_options = patch_options
        self.pre_patch_hook = pre_patch_hook
        self.pre_patch_commits = pre_patch_commits
        self.wpilib_root = get_repo_root()

    def open_repo(self, *, err_msg_if_absent):
        os.chdir(tempfile.gettempdir())

        repo = os.path.basename(self.url)
        dest = os.path.join(os.getcwd(), repo).removesuffix(".git")

        print(f"INFO: Opening repository at {dest}")

        if not os.path.exists(dest):
            if err_msg_if_absent is None:
                subprocess.run(["git", "clone", "--filter=tree:0", self.url])
            else:
                print(err_msg_if_absent, file=sys.stderr)
                exit(1)
        os.chdir(dest)

    def replace_tag(self, tag):
        path = os.path.join(self.wpilib_root, f"upstream_utils/{self.name}.py")
        with open(path, "r") as file:
            lines = file.readlines()

        previous_text = f"tag = \"{self.old_tag}\""
        new_text = f"tag = \"{tag}\""
        for i in range(len(lines)):
            lines[i] = lines[i].replace(previous_text, new_text)

        with open(path, "w") as file:
            file.writelines(lines)

    def clone(self):
        self.open_repo(err_msg_if_absent=None)

        subprocess.run(["git", "switch", "--detach", self.old_tag])

    def rebase(self, new_tag):
        self.open_repo(
            err_msg_if_absent="There's nothing to rebase. Run the \"clone\" command first."
        )

        subprocess.run(["git", "fetch", "origin", new_tag])

        subprocess.run(["git", "switch", "--detach", self.old_tag])

        if self.pre_patch_hook is not None:
            self.pre_patch_hook()

        for f in self.patch_list:
            git_am(
                os.path.join(
                    self.wpilib_root, f"upstream_utils/{self.name}_patches", f
                ),
                **self.patch_options,
            )

        subprocess.run(["git", "rebase", "--onto", new_tag, self.old_tag])

        # Detect merge conflict by detecting if we stopped in the middle of a rebase
        if has_git_rev("REBASE_HEAD"):
            print(
                f"Merge conflicts when rebasing onto {new_tag}! You must manually resolve them.",
                file=sys.stderr,
            )

    def format_patch(self, tag):
        self.open_repo(
            err_msg_if_absent="There's nothing to run format-patch on. Run the \"clone\" and \"rebase\" commands first."
        )

        if self.pre_patch_commits > 0:
            commits_since_tag_output = subprocess.run(
                ["git", "log", "--format=format:%h", f"{tag}..HEAD"],
                capture_output=True,
            ).stdout
            commits_since_tag = commits_since_tag_output.count(b"\n") + 1
            tag = f"HEAD~{commits_since_tag - self.pre_patch_commits}"

        subprocess.run(
            [
                "git",
                "format-patch",
                f"{tag}..HEAD",
                "--abbrev=40",
                "--zero-commit",
                "--no-signature",
            ]
        )

        patch_dest = os.path.join(
            self.wpilib_root, f"upstream_utils/{self.name}_patches"
        )

        if not os.path.exists(patch_dest):
            print(
                f"WARNING: Patch directory {patch_dest} does not exist", file=sys.stderr
            )

        for f in os.listdir():
            if f.endswith(".patch"):
                os.rename(f, os.path.join(patch_dest, f))

        self.replace_tag(tag)

    def copy_upstream_to_thirdparty(self):
        self.open_repo(
            err_msg_if_absent="There's no repository to copy from. Run the \"clone\" command first."
        )

        subprocess.run(["git", "switch", "--detach", self.old_tag])

        if self.pre_patch_hook is not None:
            self.pre_patch_hook()

        for f in self.patch_list:
            git_am(
                os.path.join(
                    self.wpilib_root, f"upstream_utils/{self.name}_patches", f
                ),
                **self.patch_options,
            )

        self.copy_upstream_src(self.wpilib_root)

    def main(self, argv=sys.argv[1:]):
        parser = argparse.ArgumentParser(
            description=f"CLI manager of the {self.name} upstream library"
        )
        subparsers = parser.add_subparsers(dest="subcommand", required=True)

        subparsers.add_parser(
            "clone", help="Clones the upstream repository in a local tempdir"
        )

        parser_rebase = subparsers.add_parser(
            "rebase", help="Rebases the clone of the upstream repository"
        )
        parser_rebase.add_argument("new_tag", help="The tag to rebase onto")

        parser_format_patch = subparsers.add_parser(
            "format-patch",
            help="Generates patch files for the upstream repository and moves them into the upstream_utils patch directory",
        )
        parser_format_patch.add_argument(
            "new_tag", help="The tag for the commit before the patches"
        )

        subparsers.add_parser(
            "copy-upstream-to-thirdparty",
            help="Copies files from the upstream repository into the thirdparty directory in allwpilib",
        )

        args = parser.parse_args(argv)

        self.wpilib_root = get_repo_root()
        if args.subcommand == "clone":
            self.clone()
        elif args.subcommand == "rebase":
            self.rebase(args.new_tag)
        elif args.subcommand == "format-patch":
            self.format_patch(args.new_tag)
        elif args.subcommand == "copy-upstream-to-thirdparty":
            self.copy_upstream_to_thirdparty()
