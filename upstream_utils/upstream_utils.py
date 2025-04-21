import argparse
import os
import re
import shutil
import subprocess
import sys
import tempfile


def get_repo_root():
    """Returns the Git repository root as an absolute path.

    An empty string is returned if no repository root was found.
    """
    return subprocess.run(
        ["git", "rev-parse", "--show-toplevel"],
        stdout=subprocess.PIPE,
        stderr=subprocess.DEVNULL,
        encoding="ascii",
    ).stdout.rstrip()


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


def copy_to(files, root, rename_c_to_cpp=False):
    """Copies list of files to root by appending the relative paths of the files
    to root.

    The leading directories of root will be created if they don't yet exist.

    Keyword arguments:
    files -- list of files to copy
    root -- destination
    rename_c_to_cpp -- whether to rename .c files to .cpp (default: False)

    Returns:
    The list of files in their destination.
    """
    if not os.path.exists(root):
        os.makedirs(root)

    dest_files = []
    for f in files:
        dest_file = os.path.join(root, f)

        # Rename .cc or .cxx file to .cpp
        if dest_file.endswith(".cc") or dest_file.endswith(".cxx"):
            dest_file = os.path.splitext(dest_file)[0] + ".cpp"

        if rename_c_to_cpp and dest_file.endswith(".c"):
            dest_file = os.path.splitext(dest_file)[0] + ".cpp"

        # Make leading directory
        dest_dir = os.path.dirname(dest_file)
        if not os.path.exists(dest_dir):
            os.makedirs(dest_dir)

        shutil.copyfile(f, dest_file)
        dest_files.append(dest_file)
    return dest_files


def walk_cwd_and_copy_if(pred, root, rename_c_to_cpp=False):
    """Walks the current directory, generates a list of files for which the
    given predicate is true, then copies that list to root by appending the
    relative paths of the files to root.

    The leading directories of root will be created if they don't yet exist.

    Keyword arguments:
    pred -- a function that takes a directory path and a filename, then returns
            True if the file should be included in the output list
    root -- destination
    rename_c_to_cpp -- whether to rename .c files to .cpp (default: False)

    Returns:
    The list of files in their destination.
    """
    files = walk_if(".", pred)
    files = copy_to(files, root, rename_c_to_cpp)
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
    """Checks whether the Git repository in the current directory has the given
    revision.

    Keyword arguments:
    rev -- The revision to check

    Returns:
    True if the revision exists, otherwise False.
    """
    cmd = ["git", "rev-parse", "--verify", "-q", rev]
    return subprocess.run(cmd, stdout=subprocess.DEVNULL).returncode == 0


class Lib:
    def __init__(
        self,
        name,
        url,
        tag,
        copy_upstream_src,
        patch_options={},
        *,
        pre_patch_hook=None,
        pre_patch_commits=0,
    ):
        """Initializes a Lib instance.

        Keyword arguments:
        name -- The name of the library.
        url -- The URL of the upstream repository.
        tag -- The tag in the upstream repository to use. Can be any
               <commit-ish> (e.g., commit hash or tag).
        copy_upstream_src -- A callable that takes the path to the wpilib root
                             and copies the files from the clone of the upstream
                             into the appropriate thirdparty directory. Will
                             only be called when the current directory is the
                             upstream clone.
        patch_options -- The dictionary of options to use when applying patches.
                         Corresponds to the parameters of git_am.

        Keyword-only arguments:
        pre_patch_hook -- Optional callable taking no parameters that will be
                          called before applying patches.
        pre_patch_commits -- Number of commits added by pre_patch_hook.
        """
        self.name = name
        self.url = url
        self.old_tag = tag
        self.copy_upstream_src = copy_upstream_src
        self.patch_options = patch_options
        self.pre_patch_hook = pre_patch_hook
        self.pre_patch_commits = pre_patch_commits
        self.wpilib_root = get_repo_root()

    def get_repo_path(self, tempdir=None):
        """Returns the path to the clone of the upstream repository.

        Keyword argument:
        tempdir -- The path to the temporary directory to use. If None (the
                   default), uses tempfile.gettempdir().

        Returns:
        The path to the clone of the upstream repository. Will be absolute if
        tempdir is absolute.
        """
        if tempdir is None:
            tempdir = tempfile.gettempdir()
        repo = os.path.basename(self.url)
        dest = os.path.join(tempdir, repo)
        dest = dest.removesuffix(".git")
        return dest

    def open_repo(self, *, err_msg_if_absent):
        """Changes the current working directory to the upstream repository. If
        err_msg_if_absent is not None and the upstream repository does not
        exist, the program exits with return code 1.

        Keyword-only argument:
        err_msg_if_absent -- The error message to print to stderr if the
        upstream repository does not exist. If None, the upstream repository
        will be cloned without emitting any warnings.
        """
        os.chdir(tempfile.gettempdir())

        dest = self.get_repo_path(os.getcwd())

        print(f"INFO: Opening repository at {dest}")

        if not os.path.exists(dest):
            if err_msg_if_absent is None:
                subprocess.run(["git", "clone", "--filter=tree:0", self.url])
            else:
                print(err_msg_if_absent, file=sys.stderr)
                exit(1)
        os.chdir(dest)

    def get_root_tags(self):
        """Returns a list of potential root tags.

        Returns:
        A list of the potential root tags.
        """
        root_tag_output = subprocess.run(
            ["git", "tag", "--list", "upstream_utils_root-*"],
            capture_output=True,
            text=True,
        ).stdout
        return root_tag_output.splitlines()

    def get_root_tag(self):
        """Returns the root tag (the default tag to apply the patches relative
        to). If there are multiple candidates, prints an error to stderr and the
        program exits with return code 1.

        Returns:
        The root tag.
        """
        root_tags = self.get_root_tags()
        if len(root_tags) == 0:
            print(
                "ERROR: Could not determine root tag: No tags match 'upstream_utils_root-*'",
                file=sys.stderr,
            )
            exit(1)
        if len(root_tags) > 1:
            print(
                f"ERROR: Could not determine root tag: Multiple candidates: {root_tags}",
                file=sys.stderr,
            )
            exit(1)
        return root_tags[0]

    def set_root_tag(self, tag):
        """Sets the root tag, deleting any potential candidates first.

        Keyword argument:
        tag -- The tag to set as the root tag.
        """
        root_tags = self.get_root_tags()

        if len(root_tags) > 1:
            print(f"WARNING: Deleting multiple root tags {root_tags}", file=sys.stderr)

        for root_tag in root_tags:
            subprocess.run(["git", "tag", "-d", root_tag])

        subprocess.run(["git", "tag", f"upstream_utils_root-{tag}", tag])

    def get_patch_directory(self):
        """Returns the path to the directory containing the patch files.

        Returns:
        The absolute path to the directory containing the patch files.
        """
        return os.path.join(self.wpilib_root, f"upstream_utils/{self.name}_patches")

    def get_patch_list(self):
        """Returns a list of the filenames of the patches to apply.

        Returns:
        A list of the filenames of the patches to apply, sorted in lexicographic
        order by the Unicode code points."""
        if not os.path.exists(self.get_patch_directory()):
            return []
        return sorted(
            f for f in os.listdir(self.get_patch_directory()) if f.endswith(".patch")
        )

    def apply_patches(self):
        """Applies the patches listed in the patch list to the current
        directory.
        """
        if self.pre_patch_hook is not None:
            self.pre_patch_hook()

        for f in self.get_patch_list():
            git_am(
                os.path.join(self.get_patch_directory(), f),
                **self.patch_options,
            )

    def replace_tag(self, tag):
        """Replaces the tag in the script.

        Keyword argument:
        tag -- The tag to replace the script tag with.
        """
        path = os.path.join(self.wpilib_root, f"upstream_utils/{self.name}.py")
        with open(path, "r") as file:
            lines = file.readlines()

        previous_text = f'tag = "{self.old_tag}"'
        new_text = f'tag = "{tag}"'
        for i in range(len(lines)):
            if previous_text in lines[i]:
                if i - 1 >= 0 and "#" in lines[i - 1]:
                    print(
                        f"WARNING: Automatically updating tag in line {i + 1} with a comment above it that may need updating.",
                        file=sys.stderr,
                    )
                lines[i] = lines[i].replace(previous_text, new_text)

        with open(path, "w") as file:
            file.writelines(lines)

    def info(self):
        """Prints info about the library to stdout."""
        print(f"Repository name: {self.name}")
        print(f"Upstream URL: {self.url}")
        print(f"Upstream tag: {self.old_tag}")
        print(f"Path to upstream clone: {self.get_repo_path()}")
        print(f"Patches to apply: {self.get_patch_list()}")
        print(f"Patch options: {self.patch_options}")
        print(f"Pre patch commits: {self.pre_patch_commits}")
        print(f"WPILib root: {self.wpilib_root}")

    def clone(self):
        """Clones the upstream repository and sets it up."""
        self.open_repo(err_msg_if_absent=None)

        subprocess.run(["git", "switch", "--detach", self.old_tag])

        self.set_root_tag(self.old_tag)

    def reset(self):
        """Resets the clone of the upstream repository to the state specified by
        the script and patches.
        """
        self.open_repo(
            err_msg_if_absent='There\'s nothing to reset. Run the "clone" command first.'
        )

        subprocess.run(["git", "switch", "--detach", self.old_tag])

        self.apply_patches()

        self.set_root_tag(self.old_tag)

    def rebase(self, new_tag):
        """Rebases the patches.

        Keyword argument:
        new_tag -- The tag to rebase onto.
        """
        self.open_repo(
            err_msg_if_absent='There\'s nothing to rebase. Run the "clone" command first.'
        )

        subprocess.run(["git", "fetch", "origin", new_tag])

        subprocess.run(["git", "switch", "--detach", self.old_tag])

        self.apply_patches()

        self.set_root_tag(new_tag)

        subprocess.run(["git", "rebase", "--onto", new_tag, self.old_tag])

        # Detect merge conflict by detecting if we stopped in the middle of a rebase
        if has_git_rev("REBASE_HEAD"):
            print(
                f"Merge conflicts when rebasing onto {new_tag}! You must manually resolve them.",
                file=sys.stderr,
            )

    def format_patch(self):
        """Generates patch files for the upstream repository and moves them into
        the patch directory.
        """
        self.open_repo(
            err_msg_if_absent='There\'s nothing to run format-patch on. Run the "clone" and "rebase" commands first.'
        )

        root_tag = self.get_root_tag()
        script_tag = root_tag.removeprefix("upstream_utils_root-")

        start_commit = root_tag
        if self.pre_patch_commits > 0:
            commits_since_tag_output = subprocess.run(
                ["git", "log", "--format=format:%h", f"{start_commit}..HEAD"],
                capture_output=True,
            ).stdout
            commits_since_tag = commits_since_tag_output.count(b"\n") + 1
            start_commit = f"HEAD~{commits_since_tag - self.pre_patch_commits}"

        subprocess.run(
            [
                "git",
                "format-patch",
                f"{start_commit}..HEAD",
                "--abbrev=40",
                "--zero-commit",
                "--no-signature",
            ]
        )

        if os.path.exists(self.get_patch_directory()):
            shutil.rmtree(self.get_patch_directory())

        is_first = True
        for f in os.listdir():
            if f.endswith(".patch"):
                if is_first:
                    os.mkdir(self.get_patch_directory())
                    is_first = False
                shutil.move(f, self.get_patch_directory())

        self.replace_tag(script_tag)

    def copy_upstream_to_thirdparty(self):
        """Copies files from the upstream repository into the thirdparty
        directory.
        """
        self.open_repo(
            err_msg_if_absent='There\'s no repository to copy from. Run the "clone" command first.'
        )

        subprocess.run(["git", "switch", "--detach", self.old_tag])

        self.apply_patches()

        self.copy_upstream_src(self.wpilib_root)

    def main(self):
        """Process arguments of upstream_utils script"""
        parser = argparse.ArgumentParser(
            description=f"CLI manager of the {self.name} upstream library"
        )
        subparsers = parser.add_subparsers(dest="subcommand", required=True)

        subparsers.add_parser(
            "info", help="Displays information about the upstream library"
        )

        subparsers.add_parser(
            "clone", help="Clones the upstream repository in a local tempdir"
        )

        subparsers.add_parser(
            "reset",
            help="Resets the clone of the upstream repository to the tag and applies patches",
        )

        parser_rebase = subparsers.add_parser(
            "rebase", help="Rebases the clone of the upstream repository"
        )
        parser_rebase.add_argument("new_tag", help="The tag to rebase onto")

        subparsers.add_parser(
            "format-patch",
            help="Generates patch files for the upstream repository and moves them into the upstream_utils patch directory",
        )

        subparsers.add_parser(
            "copy-src",
            help="Copies files from the upstream repository into the thirdparty directory in allwpilib",
        )

        args = parser.parse_args()

        self.wpilib_root = get_repo_root()
        if args.subcommand == "info":
            self.info()
        elif args.subcommand == "clone":
            self.clone()
        elif args.subcommand == "reset":
            self.reset()
        elif args.subcommand == "rebase":
            self.rebase(args.new_tag)
        elif args.subcommand == "format-patch":
            self.format_patch()
        elif args.subcommand == "copy-src":
            self.copy_upstream_to_thirdparty()
