# Upstream utils

## Layout

Each thirdparty library has a Python script for updating it. They generally:

1. Check out a thirdparty Git repository to a specific commit or tag
2. Apply patch files to the thirdparty repo to fix things specific to our build
3. Copy a subset of the thirdparty files into our repo
4. Comment out any header includes that were invalidated, if needed

`upstream_utils.py` contains utilities common to these update scripts.

Patches are generated in the thirdparty repo with
`git format-patch --no-signature` so they can be applied as individual commits
and easily rebased onto newer versions. Each library has its own patch directory
(e.g., `lib_patches`).

## Updating thirdparty library version

The example below will update a hypothetical library called `lib` to the tag
`2.0`.

Start in the `upstream_utils` folder. Restore the original repo.
```bash
./update_lib.py
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

Rebase any patches onto the new version.
```bash
git rebase 2.0
```

Generate patch files for the new version.
```bash
git format-patch 2.0..HEAD
```

Move the patch files to `upstream_utils`.
```
mv *.patch allwpilib/upstream_utils/lib_patches
```

Navigate back to `upstream_utils`
```bash
cd allwpilib/upstream_utils
```

Modify the version number in the call to `setup_upstream_repo()` in
`update_lib.py`, then  rerun `update_lib.py` to reimport the thirdparty files.
```bash
./update_lib.py
```
