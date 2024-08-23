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

Start in the `upstream_utils` folder. Make sure a clone of the upstream repo exists.
```bash
./<lib>.py clone
```

Rebase the clone of the upstream repo.
```bash
./<lib>.py rebase 2.0
```

Update the `upstream_utils` patch files and the tag in the script.
```bash
./<lib>.py format-patch
```

Copy the updated upstream files into the thirdparty files within allwpilib.
```bash
./<lib>.py copy-src
```

## Adding patch to thirdparty library

The example below will add a new patch file to a hypothetical library called
`lib` (Replace `<lib>` with `llvm`, `fmt`, `eigen`, ... in the following steps).

Start in the `upstream_utils` folder. Make sure a clone of the upstream repo exists.
```bash
./<lib>.py clone
```

Update the clone of the upstream repo.
```bash
./<lib>.py reset
```

Navigate to the repo. If you can't find it, the directory of the clone is printed at the start of the `clone` command.
```bash
cd /tmp/<lib>
```

Make a commit with the desired changes.
```bash
git add ...
git commit -m "..."
```

Navigate back to `upstream_utils`.
```bash
cd allwpilib/upstream_utils
```

Update the `upstream_utils` patch files.
```bash
./<lib>.py format-patch
```

Rerun `<lib>.py` to reimport the thirdparty files.
```bash
./<lib>.py copy-src
```
