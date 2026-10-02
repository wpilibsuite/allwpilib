# DataLog Tool

`datalogtool` provides the graphical interface. `datalogtool-cli` provides local
CSV export and entry inspection, plus remote log listing, download, and deletion
without a display. Both executables are included in the DataLogTool distribution.

Run `datalogtool-cli --help` or `datalogtool-cli <command> --help` for options.
`--version` prints the WPILib version. The CLI does not read or change GUI
workspace settings.

## Local logs

```sh
# Inspect entry names, types, and metadata from one or more logs.
datalogtool-cli entries match1.wpilog match2.wpilog

# Write match1.csv and match2.csv to an existing directory.
datalogtool-cli export --output-dir csv match1.wpilog match2.wpilog

# Export selected entries in a table, grouping timestamps within 0.5 ms.
datalogtool-cli export --output-dir csv --style table \
  --timestamp-fuzziness 0.5 --entry-prefix /Drive/ --entry battery \
  --exclude /Drive/debug match1.wpilog
```

`export` defaults to the GUI's **List** format (`Timestamp,Name,Value`). **Table**
format has one column per selected entry, sorted by name. Both use the same CSV
writer as the GUI, with timestamps in seconds, quoted strings, and semicolon
separators within arrays. Unsupported types and invalid values produce
`<invalid>`.

`--timestamp-fuzziness` is in milliseconds and requires `--style table`. It
limits the entire timestamp span of each row, including out-of-order timestamps.
A row uses its earliest timestamp and the last value for each entry. Missing
values are empty cells. The default, zero, merges only identical timestamps.

All entries are selected by default. Repeat `--entry` and `--entry-prefix` to
include the union of exact names and prefixes. Repeat `--exclude` and
`--exclude-prefix` to remove entries from that selection. These options also
apply to `entries`. Prefixes are literal and case-sensitive; include a trailing
`/` when selecting a slash-delimited subtree.

`entries` writes CSV with `File,Name,Type,Metadata` columns and reports total
record and entry counts on standard error. Distinct start definitions are
retained when an entry has different types or metadata across files or restarts.
As in the GUI, metadata is taken from start records. Data values are decoded
using the active definition within each input file. An incomplete trailing
record is ignored according to the datalog reader's normal behavior.

## Remote logs

```sh
# List remote directories and .wpilog files, including file sizes in bytes.
datalogtool-cli list --host robot.local

# Download selected files, keeping the remote copies.
datalogtool-cli download --host robot.local --output-dir logs match1.wpilog

# Download every .wpilog file and delete each successfully downloaded copy.
datalogtool-cli download --host robot.local --output-dir logs --all --delete-after

# Explicitly delete selected remote files without downloading them.
datalogtool-cli delete --host robot.local --yes match1.wpilog match2.wpilog

# Use custom connection settings and a different remote directory.
datalogtool-cli list --host 10.0.0.2 --port 22 \
  --username lvuser --password '' --remote-dir /home/lvuser/logs
```

`--host` accepts an address or team number. Numeric team numbers resolve to
`robot.local`, matching the current GUI. Defaults are port 22, username and
password `systemcore`, and remote directory `/home/systemcore/logs`.

`list` writes CSV with `Name,Type,Size` columns. Choose another directory with
`--remote-dir`; invoke `list` again to refresh. Downloads and deletions accept
exact `.wpilog` basenames, or `--all` to select all regular `.wpilog` files in
that directory. They do not recurse. `delete` requires `--yes`, corresponding to
the GUI's deletion confirmation. Downloads keep remote files unless
`--delete-after` is supplied. Short reads, detected growth, and local write or
close failures remove incomplete local files and leave the remote file intact.

## Output and failures

Output directories must already exist. Existing files are never overwritten,
including when input files from different directories would produce the same
CSV filename. Batch operations continue after per-file failures and return a
nonzero status if any file failed. Listings and help go to standard output;
progress and diagnostics go to standard error.

Exit codes are `0` for success, `1` for file/connection/transfer failures, and `2`
for invalid arguments. Use `--` before positional filenames beginning with `-`.

## Building and testing

```sh
bazel build //tools/datalogtool:datalogtool-cli
bazel test //tools/datalogtool:datalogtool-test
bazel test --config=imgui_tests //tools/datalogtool:datalogtool-imgui-test

# Linux desktop Gradle targets (other platforms have corresponding targets).
./gradlew :tools:datalogtool:linkDatalogtoolCliLinuxx86-64DebugExecutable
./gradlew :tools:datalogtool:runDatalogtoolTestLinuxx86-64DebugGoogleTestExe

# In a configured CMake build with libssh available:
cmake --build build --target datalogtool-cli datalogtool_test
```

CMake can build the CLI with `WPILIB_WITH_GUI=OFF`; the CLI requires datalog,
wpiutil, and libssh, and has no GUI dependency. CMake's existing libssh package
configuration must be discoverable, for example via `LIBSSH_DIR`.
