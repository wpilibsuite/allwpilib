# Upstream utils

## Layout

Each thirdparty library has a Python script for updating it. They generally:

1. Check out a thirdparty Git repository to a specific commit or tag
2. Apply patch files to the thirdparty repo to fix things specific to our build
3. Copy a subset of the thirdparty files into our repo
4. Comment out any header includes that were invalidated, if needed

`upstream_utils.py` contains utilities common to these update scripts.

Patches are generated in the thirdparty repo with git's format-patch command so
they can be applied as individual commits and easily rebased onto newer
versions. Each library has its own patch directory (e.g., `lib_patches`).

## Updating thirdparty library version

The example below will update a hypothetical library called `lib` to the tag
`2.0`.

Start in the `upstream_utils` folder. Restore the original repo.
```bash
./update_<lib>.py
```

Navigate to the repo.
```bash
cd /tmp/lib
```

Fetch the desired version using one of the following methods.
```bash
# Fetch a full branch or tag
git fetch origin 2.0

# Fetch just a tag (useful for expensive-to-clone repos)
git fetch --depth 1 origin tag 2.0
```

Rebase any patches onto the new version. If the old version and new version are
on the same branch, run the following.
```bash
git rebase 2.0
```

If the old version and new version are on different branches (e.g.,
llvm-project), use interactive rebase instead and remove commits that are common
between the two branches from the list of commits to rebase. In other words,
only commits representing downstream patches should be listed.
```bash
git rebase -i 2.0
```

Generate patch files for the new version.
```bash
git format-patch 2.0..HEAD --zero-commit --abbrev=40 --no-signature
```

Move the patch files to `upstream_utils`.
```
mv *.patch allwpilib/upstream_utils/lib_patches
```

Navigate back to `upstream_utils`.
```bash
cd allwpilib/upstream_utils
```

Modify the version number in the call to `setup_upstream_repo()` in
`update_<lib>.py`, then rerun `update_<lib>.py` to reimport the thirdparty
files.
```bash
./update_<lib>.py
```

## Adding patch to thirdparty library

The example below will add a new patch file to a hypothetical library called
`lib` (Replace `<lib>` with `llvm`, `fmt`, `eigen`, ... in the following steps).

Start in the `upstream_utils` folder. Restore the original repo.
```bash
./update_<lib>.py
```

Navigate to the repo.
```bash
cd /tmp/<lib>
```

Make a commit with the desired changes.
```bash
git add ...
git commit -m "..."
```

Generate patch files.
```bash
git format-patch 2.0..HEAD --zero-commit --abbrev=40 --no-signature
```
where `2.0` is replaced with the version specified in `update_<lib>.py`.

Move the patch files to `upstream_utils`.
```
mv *.patch allwpilib/upstream_utils/<lib>_patches
```

Navigate back to `upstream_utils`.
```bash
cd allwpilib/upstream_utils
```

Update the list of patch files in `update_<lib>.py`, then rerun
`update_<lib>.py` to reimport the thirdparty files.
```bash
./update_<lib>.py
```
