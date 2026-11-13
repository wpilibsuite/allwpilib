# Run-based robot log management

Design specification for WPILib, logging vendors, and SystemCore OS maintainers.
October 23, 2026. This document defines the target behavior and integration
requirements. Implementation work and unresolved decisions are tracked below.

This document defines the shared run contract, producer behavior, and WPILib,
vendor, and tool requirements: what each participant does and what users can
expect. [Run log service and client libraries][] defines how those requirements
are implemented: the server and libraries, allocation, persistence, retention,
USB reconciliation, producer/tool adaptations, packaging, and qualification.

## Design overview

Each invocation of the robot application has its own log run directory. On
SystemCore, the OS establishes its immutable run ID and discovery context before
application code executes. Desktop simulation runs directly: the first
participating producer initializes a shared in-process context before opening
files, and later producers join it. Each producer owns a namespace inside that
directory and keeps its existing file formats. A run coordinator maintains time
and match metadata and a human-readable directory alias. A separate SystemCore
service deletes completed runs as units.

A standalone native client library supplies the shared C API, packaged
separately from allwpilib using the mrclib dependency model. WPILib and vendors
must use that library, directly or through bindings to its C ABI; its
implementation and ABI do not depend on `wpiutil`. Following the mrclib/mrccomm
model, the library's communication with the SystemCore server is a private
implementation detail. Vendors do not implement the transport protocol. A
reusable coordinator library runs inside the application for ordinary desktop
simulation launches. Its configuration interfaces also support custom simulators
and robot-like setups, including vendor/WPILib tests. No desktop mode requires a
launch wrapper or a separate daemon process.

DataLogManager always uses this managed contract on SystemCore and desktop. The
new library is a required dependency, so the manager has no optional-library
probe, legacy mode, or independent directory-selection/retention fallback.

**Naming changes are independent of producer write paths. A human-readable alias
changes when time or match information becomes available.** Use a symlink where
available, or a plain-text reference file containing the same relative target
where symlinks are unavailable. Both resolve to the run's directory on that
volume.

Logs can transfer from internal storage to a USB drive that mounts after launch.
The coordinator publishes a preferred destination, and each producer switches
independently using its own safe transfer mechanism. Preserve the run ID and
namespace layout on both volumes. A run may span both volumes during transfer or
longer if a producer cannot move. Track actual producer and file locations
rather than requiring a run-wide cutover. A label update does not cause a
transfer or interrupt logging.

The [Trades](#trades) section compares this approach with physical directory
renaming, discovery through `current`, and other design alternatives.
[Open trades and decisions](#open-trades-and-decisions) identifies the remaining
agreements and implementation choices.

This design standardizes placement, identity, and retention. It does not require
a common file format, a logging daemon in the data path, or a common logging
start/stop policy. Grouping files also does not synchronize their sample clocks.

Hard power-offs and application crashes followed by automatic restart are normal
operating conditions. Correctness must not depend on shutdown hooks, final log
flushes, or a clean-close marker. Every application restart gets a new run;
recovering the previous run happens automatically and does not gate the new
launch. See [Power loss and crash recovery](#power-loss-and-crash-recovery).

## Current behavior and constraints

Source observations below are against allwpilib commit
`52a175c0de5c0e920452c88895c21853fd9d7c99`.

| Component | Observed behavior | Consequence |
| --------- | ----------------- | ----------- |
| [C++ DataLogManager][] and [Java DataLogManager][] | Default to `/u/logs` when their USB checks succeed, otherwise `/home/systemcore/logs` on SystemCore; simulation uses the operating directory's `logs` folder. An explicit directory overrides selection. Java uses `toRealPath()` and writability; C++ checks directory status and the others-write permission bit. | Storage selection is currently producer-owned, and the two implementations are not identical. The coordinator supplies a common preferred destination; producers adopt it independently. |
| The two managers | Initially use `WPILIB_TBD_<random>.wpilog`. After more than 50 DS-attached updates and valid system time, request a timestamp filename. After more than 250 FMS-attached updates and a non-`NONE` match type, request an event/match filename. These counters approximate one and five seconds at 50 Hz. An explicit filename disables those renames. | Move shared labeling out of a particular file writer. The current FMS branch does not itself gate its timestamp on valid system time; the new design must handle match and time validity independently. |
| The two managers | Delete old `WPILIB_TBD_*.wpilog` at initial startup. On the management thread's startup, delete eligible `WPILIB_*.wpilog` oldest-first below 200,000,000 bytes available, keeping at least ten eligible files. Check the low-space alert every five seconds, warning below 400,000,000 bytes. | Cleanup is format-specific and primarily startup-based. Runs without a DS connection can contain important startup failures and must no longer be discarded solely for lacking a timestamp. |
| [Native background writer][] | Retains directory/file paths, checks whether its file exists, creates another segment when a file disappears or exceeds 1.8 GB, and renames files by path. Refuses/stops writing around its 5 MiB minimum-space guard. [Java calls it through JNI][]. | Preserving an open handle alone is insufficient, including for WPILib itself. Keep the local space guard separate from retention policy. |
| Manager discovery/lifetime | C++ `GetLogDir()` obtains the singleton and can start logging. Java `getLogDir()` returns an empty string before logging starts. Their stop/restart implementations also differ. | A producer must not use DataLogManager initialization or its lifetime to establish executable-run identity. |
| [ntcoreffi manager][] | Contains another copy of directory selection, filename changes, and cleanup. | Include this surface in migration; changing only wpilibc/wpilibj leaves another policy owner. |
| [DataLogTool downloader][] | Browses directories, lists regular `.wpilog` files, and offers individual file deletion; it is not an all-format run exporter. | Run discovery, recursive export, and run deletion require tool changes. |

Related work: [#9570, Periodically check for USB and transfer log file][],
reviewed at head `3b5e18df755a2b61e38f803c419397ab04300234`, is open and not
part of the checkout baseline above. It checks for USB every five seconds after
automatic selection falls back to internal storage. Its native background
writer's `SetDir()` flushes buffered records, transfers the current file if it
is at most 10,000,000 bytes, syncs the destination, switches the writer, and
then removes the old file. It does not overwrite existing destinations;
size/copy failure leaves logging on the source. `GetDir()` changes only on
success. The patch covers Java/JNI, C++, RobotPy, and the duplicated ntcoreffi
policy.

Preserve that late-mount behavior in the shared-run design. Its single-writer
transfer is a useful building block: one producer may switch while another
continues internally. Track that partial placement explicitly. Moving the
current file does not account for already-closed segments, which may need
separate copying.

Vendor observations are from the public documentation linked here, consulted
October 23, 2026. They establish integration constraints, not SystemCore support
commitments:

- CTRE exposes `SignalLogger.setPath()`; the directory must exist, and changing
  it restarts logging. Set it before logging and use its documented restart
  behavior when adopting another volume, not on each match update. See
  [Phoenix 6 SignalLogger][].
- REV Status Logger starts automatically on initial REVLib use and has its own
  free-space deletion policy. Its documented Java interface exposes start, stop,
  and disabling automatic startup, but no path-setting operation. Managed
  placement and disabling retention therefore need vendor support, rather than
  an assumed existing setter. See [REV logging][] and [StatusLogger API][].
- AdvantageKit's `WPILOGWriter` accepts a file or directory path; directory-only
  configuration generates a time/match filename. Its replay examples explicitly
  place outputs beside the replay input. Preserve explicit destinations and
  distinguish new replay executions from the original recorded run. See
  [WPILOGWriter][] and [traditional replay][].

## Contract and ownership

A **run** is one invocation of the robot application: a supervisor launch on
SystemCore, including its designated helper processes, or a directly launched
desktop process. A restart, redeployment, or new simulation/replay launch
creates a new run, even within the same second or match. Pausing a logger,
disabling/enabling the robot, reconnecting the DS, and changing match
information do not create a new run. Transferring storage also preserves the run
identity. A helper explicitly launched into an existing run inherits that run;
an application replacement gets a fresh one, even if the supervisor implements
replacement using `exec` and retains a PID.

The following are requirements for managed mode:

1. The run ID never changes. Each volume uses `ROOT/runs/ID`; a producer changes
   its write path only when it successfully adopts an approved destination.
   Time/match label updates do not affect write paths.
2. Context exists before each participating producer opens files. SystemCore
   allocates it before executable loading. Desktop initialization is thread-safe
   on the first client call, including from a native static constructor, Java
   class initializer, or Python import, without depending on HAL/DataLogManager.
3. A late producer joins the same run and obtains its preferred destination,
   regardless of labels, DS connection, or whether WPILib's own logger ever
   starts.
4. Producers register before writing, report their actual location and transfer
   capability, and use their namespace on an approved run volume. They do not
   create a new run when a logger restarts or changes storage. One producer's
   progress or failure does not gate another's switch to USB.
5. Producers may create multiple files and segments inside their namespace; none
   may delete another producer's files or perform retention on managed runs.
   They retain bounded-buffer and write-error handling.
6. Automatic retention deletes only proven-inactive, unprotected runs, as whole
   directory trees across known volumes. Ambiguous liveness protects data.
7. Failure of naming, metadata updates, cleanup, or transfer must not stop robot
   control. A failed producer transfer preserves its source data and does not
   roll back successful transfers by other producers. Logging failures and
   incomplete placement are explicit.
8. Abrupt termination is a supported path through the lifecycle. Preserve useful
   records from interrupted runs, recover their lifetime without a writer's
   final acknowledgement, and make them eligible for normal retention once
   inactivity is proven. Do not reuse an old run for an automatic restart.
9. All participating producers use the published C library, directly or through
   a binding or adapter that calls it. The supported integration boundary is the
   C API/ABI, not launch-environment parsing or the private server protocol.

| Owner | Responsibility |
| ----- | -------------- |
| SystemCore application supervisor | Establish the run before launch, bind it to an application invocation/process group, inject context, and report process lifetime. |
| Run coordinator | Allocate durable identity/storage, publish preferred destinations, track producer locations, serialize metadata, manage aliases, and recover/seal interrupted runs. SystemCore hosts it in the `wpilib-logd` service with USB monitoring and retention; desktop hosts it in the application with those policies configurable. |
| Standalone logging library maintainers | Publish the versioned C client API, reusable simulation coordinator library, headers, shared runtimes, and compatibility tests. Own private transport/bootstrap compatibility with the SystemCore server maintainers. |
| WPILib | Consume the standalone C API through language bindings, publish DS/time/match observations independently of DataLogManager, and write its own data in the assigned namespace. |
| CTRE, REV, AdvantageKit, and other producers | Use the shared C library or a binding to it for discovery, registration, and status. Register before automatic logging, own files/rotation and independent transfer behavior, use approved destinations, disable independent retention in managed mode, and report actual locations/failures. |
| SystemCore retention worker | Apply device-wide storage policy to sealed runs. It may share the `wpilib-logd` process, but does not run inside the robot executable. |
| Download/analysis tools | Enumerate runs, display metadata, export all producer files once, and request coordinated deletion. |

Service and API names identify the components and operations in this design;
final names and policy defaults are tracked in the open decisions. The
SystemCore supervisor implementation is outside this repository and must be
integrated by its maintainers; no existing unit name or service API is assumed.

## Storage layout

Example on internal SystemCore storage, with `ID` standing for the full run ID:

```text
/home/systemcore/logs/
  runs/
    ID/                                  # this volume's files for the run
      run.json                           # coordinator's metadata snapshot
      wpilib/
        WPILIB.wpilog
        WPILIB.2.wpilog
      ctre/
        <producer-selected-name>.hoot
      rev/
        <producer-selected-name>.revlog
      advantagekit/
        <producer-selected-name>.wpilog
      console/
        stdout.log                       # optional supervisor capture
  WPILIB_20261023_184210Z_pnw_champ_Q12_R1__ID -> runs/ID
  .trash/                               # sealed runs being reclaimed
```

The canonical directory on each volume is `ROOT/runs/ID`; transfer changes a
producer's `ROOT`, while preserving `ID` and the namespace layout. A run can
have files on internal storage and USB at the same time. The ID is a randomly
generated 128-bit identifier rendered as 32 lowercase hexadecimal characters,
independent of wall time, PID, or match number. The full ID remains in every
display name. Concurrent simulation sessions and imports from different devices
do not rely on a common clock. Imports never overwrite an existing run merely
because IDs match.

Producer namespaces are lowercase ASCII identifiers matching
`[a-z][a-z0-9_-]{0,31}`. Reserve `wpilib`, `ctre`, `rev`, `advantagekit`, and
`console`; document a registry/convention for additional libraries. Producers
can create a subdirectory and optionally a producer-owned metadata file after
registering their location. Multiple instances of one producer must coordinate
their filenames or use distinct instance subdirectories so one instance cannot
truncate another's output. Only the coordinator owns `run.json`.

A compatibility adapter can initially put uniquely named files directly in the
run root, but must avoid coordinator-owned names and advertise that layout to
tools.

On SystemCore, an eligible USB volume is the preferred initial destination;
internal storage is the fallback. An eligible USB volume that becomes available
later becomes preferred, and producers adopt it independently. A run may span
both volumes, with earlier locations remaining usable by producers that have not
moved. Eligibility checks and allocation are defined in the companion
[initial storage selection design][].

Simulation uses the same layout under an absolute log root, defaulting to `logs`
beneath the native process working directory when context is first initialized.
Launching from the project root therefore uses `<project>/logs`. Custom hosts
can configure another root before initialization. Later working-directory
changes do not redirect logging. Ordinary desktop mode uses this one root with
automatic USB discovery and retention off. The reusable coordinator supports
explicitly configured additional roots and storage policy.

### Plain-text alias fallback

Symlink availability depends on filesystem support and user permissions. Where
symlinks are unavailable, represent each display alias as an ordinary UTF-8 text
file named `<display-name>.runref`. The `.runref` suffix identifies this
reference format; it is not a log file or a directory. For example:

```text
WPILIB_20261023_184210Z_pnw_champ_Q12_R1__8d5b7d3acfc64cf4b8ceca7312efeb10.runref
```

Its entire contents are the relative target followed by a newline:

```text
runs/8d5b7d3acfc64cf4b8ceca7312efeb10
```

The target uses `/` separators and is resolved relative to the log root
containing the reference file. Store the canonical run ID, not another display
name that can change. Writers emit one line with an LF terminator and no BOM;
readers may also accept CRLF or a missing final newline. Limit the file to 64
bytes and accept only `runs/` followed by exactly 32 lowercase hexadecimal
characters, with that optional line terminator. Reject additional lines,
absolute paths, traversal, and chains of references. A valid reference targets
an existing canonical run directory with matching run identity in its metadata.

Run-aware tools resolve the reference explicitly. A normal filesystem open of
`<display-name>.runref/file.wpilog` does not follow it as a directory. Producers
obtain their destination through producer registration; this fallback does not
change their discovery or transfer behavior. The portable run catalog remains
`runs/ID/run.json`, so aliases can be rebuilt or bypassed if missing or damaged.
Never create empty directories pretending to be aliases.

## Startup and discovery

All producers call the shared C library before opening files. It returns the
immutable run identity, an assigned producer directory, the preferred
destination/revision, and status. The library's [public API operations][] are
specified in the companion design. Producers do not read private launch
variables or implement the SystemCore protocol. Placement-only adapters use the
same API and may retain their first approved location.

On SystemCore the OS establishes context before application code executes. A
missing or invalid context disables DataLogManager file output with a
diagnostic; it never selects a legacy path. An established writer can continue
at an approved location during a coordinator outage, while first registrations
and new destinations require coordination. Disabled/error status must not
prevent robot control from continuing. Allocation and outage handling are
described in the [SystemCore launch design][].

An explicit user destination may remain an opt-out for vendor APIs that support
it; report that choice as outside shared retention. DataLogManager always uses
managed placement. Configure its run root through the library/coordinator; use
generic writer APIs for exact-path standalone output.

### Unordered and late startup

- A vendor can start logging before WPILib or HAL, including during static
  initialization. All participating producers in the invocation share one run.
- A producer started after a label change or USB preference change receives the
  same run ID and current preferred destination. It need not observe prior
  events or wait for existing producers to finish moving. A stopped producer
  keeps its last actual location until it adopts another.
- Designated helpers and late sidecars join their explicitly assigned run.
  Long-lived vendor daemons keep each run's output separate; they cannot infer
  the intended invocation from the newest directory or an inspection alias.
- Automatic restart gets a fresh run even after a pre-main crash or many
  restarts within one second. Surviving old helpers remain associated with the
  old run. Recovery, copying, and sealing of that run do not delay the new one.

### Simulation and direct launches

Launch the C++ executable, JVM, or Python entry point normally. The packaged
runtime hosts the coordinator in the application and allocates one shared
context on first use. No wrapper or separate daemon is required. Native vendors,
JNI, and Python bindings in the same process use that context; each new process
gets a fresh run. The default root is `logs` beneath the native working
directory captured at initialization. Custom roots must be configured before
first use.

The coordinator library is available to general simulation hosts as well as
robot-like setups and tests. Automatic desktop USB discovery and retention are
off by default. Coordinator work stops when its host exits; a later host or
recovery operation can reconcile known old runs without delaying new logging.
See [desktop context initialization][] and the
[simulation embedding interface][] for configuration, lifetime, runtime sharing,
and early-startup requirements.

## Naming and metadata

### One metadata publisher, independent of file logging

WPILib supplies DS, time, and match observations independently of whether
DataLogManager is enabled or running. Frameworks that do not use WPILib's DS
integration may supply one equivalent publisher. The coordinator owns the shared
label policy, `run.json`, and display aliases; individual producers do not
choose run names.

Metadata updates do not block robot control or log writes. Coordinator outages
can leave metadata stale, and lost observations are reported as incomplete
metadata. Observations from another run, an unauthorized publisher, or an older
update cannot replace newer accepted state. The companion
[metadata ingestion design][] defines observation hooks, ordering, and
buffering.

### Display-name progression

```text
WPILIB_TBD__ID
WPILIB_20261023_184210Z__ID
WPILIB_20261023_184210Z_pnw_champ_Q12_R1__ID
```

Use UTC with a literal `Z`. The timestamp is the first trusted UTC observation
for the run, frozen for naming; it is not falsely presented as the exact launch
time. Store launch monotonic time and any estimated launch UTC separately with
their provenance. Preserve later clock corrections as metadata, without renaming
on each clock adjustment.

Match information and time validity are independent: a valid match can produce
`WPILIB_TBD_pnw_champ_Q12_R1__ID` before trusted time arrives. Unknown time or
match information must not delay file creation. Use `P`, `Q`, or `E` for match
type, include the replay number when supplied, and keep the raw event name in
metadata.

The initial label policy follows the existing intent: approximately one second
of continuous DS attachment before a time label, and five seconds of continuous
FMS attachment plus an actual non-`NONE` match observation before a match label.
DS identity fields are observations, not a guarantee that a match is being
played. Metadata includes the associated enabled/disabled state and observation
interval; a detach interrupts the pending attachment interval. Retain corrected
and subsequent match identities in metadata. The first accepted identity
supplies the label; if more than one accepted identity occurs in the run, add
`_MULTI` once. Tools show the full match history, including corrections, rather
than trusting the label alone.

Event labels contain only ASCII letters, digits, `_`, and `-`, with a maximum of
48 bytes and `event` as the fallback for an empty sanitized name. The `WPILIB_`
prefix and fixed structure avoid bare Windows reserved names; exclude path
separators, traversal components, control characters, trailing dots/spaces, and
other platform-invalid characters. Raw metadata retains the original text. Bound
total component/path lengths, including the `.runref` suffix when used, and
report an alias failure without affecting the canonical directory. The run ID
resolves collisions from time, replay, or event-name sanitization.

### Metadata example

The portable UTF-8 JSON snapshot is versioned and contains no required absolute
paths, so an exported directory remains self-contained:

```json
{
  "schema_version": 1,
  "run_id": "8d5b7d3acfc64cf4b8ceca7312efeb10",
  "revision": 4,
  "destination_revision": 1,
  "volume_id": "internal-volume",
  "preferred_volume_id": "internal-volume",
  "known_volume_ids": ["internal-volume"],
  "placement_status": "single-volume",
  "state": "active",
  "mode": "robot",
  "display_name": "WPILIB_20261023_184210Z_pnw_champ_Q12_R1__8d5b7d3acfc64cf4b8ceca7312efeb10",
  "device_id": "configured-device-id",
  "boot_id": "supervisor-boot-identity",
  "invocation_id": "supervisor-invocation-identity",
  "previous_run_id": null,
  "launch_reason": "initial",
  "start_monotonic_us": 12000000,
  "monotonic_clock": "host-monotonic",
  "first_valid_utc": "2026-10-23T18:42:10Z",
  "time_source": "wpilib-system-time-valid",
  "time_observed_monotonic_us": 15000000,
  "estimated_start_utc": "2026-10-23T18:42:07Z",
  "matches": [
    {
      "event": "pnw_champ",
      "type": "qualification",
      "number": 12,
      "replay": 1,
      "observed_monotonic_us": 20000000,
      "fms_attached": true,
      "enabled": false
    }
  ],
  "outcome": null,
  "replay_source_run_id": null
}
```

Unknown times are `null`, not epoch zero. Document the actual clock source on
each platform, its units, and any conversion between WPILib/HAL time and host
monotonic time. Simulation clock pause/replay time must not be used for process
liveness or retention. Record the run ID in each producer's file metadata when
the format supports it, so an extracted file can be correlated. Vendors retain
their own clock-domain descriptions and clock mappings; a shared run ID alone
cannot align timestamps.

The schema also needs optional software versions/build identity, end time, exit
status, per-producer active and historical locations, transfer progress and
completion, aggregate copy-completion and cleanup status, metadata completeness,
retired storage locations, and producer summaries. A retired location is
deliberately removed after its data is accounted for elsewhere; it is not
missing history and is no longer required for a complete export. Record the
previous run and restart reason when the supervisor knows them; missing evidence
stays unknown. Separate confirmed exit status/signal from an interrupted
previous-boot run: absence of a clean-close record alone does not establish the
cause or time of termination. Destination revisions express preference, not
run-wide write authority. Each volume's snapshot identifies that volume and
known peers; updates may lag or a peer may be offline. Being preferred does not
mean a volume contains the complete run. Make producer file inventories advisory
while active: files may appear before their reports arrive. Sealed-run
inventories and exports account for all known history; an unavailable location
containing unique data makes them incomplete, whereas a retired location does
not. Unknown schema fields are ignored; unsupported major versions are readable
as opaque data but ineligible for automatic deletion without trusted registry
data.

### Alias publication and crash consistency

Only the coordinator publishes metadata and aliases. A failed or interrupted
label update may leave a stale or duplicate alias, but must not change producer
paths. Tools deduplicate by run ID and can always enumerate canonical run
directories. Publication is recoverable without a clean shutdown; it does not
guarantee that the newest buffered metadata or log records survive power loss.
The companion [publication algorithm][] defines atomic replacement, persistence,
and alias reconciliation.

## Power loss and crash recovery

Hard power-offs and application crashes are normal. Every replacement executable
gets a fresh run; recovering old files must not delay it or require callbacks
from dead producers. Preserve useful records even when final metadata or a
footer is missing. Sealing proves that writers are inactive, not that every
record was persisted or that the cause of termination is known.

Producers must define and qualify periodic background persistence and report
sync/write errors and their expected data-loss window. A shutdown flush is an
optimization. Successful transfer cleanup needs durable copy evidence, not an
asynchronous flush request. The service/library
[persistence and recovery design][] specifies allocation durability, registry
recovery, repeated power cuts, and bounded crash-loop history. Desktop recovery
runs only while a host or recovery tool is running.

### Usable logs after interruption

Producers must document which data remains readable without a final close and
what a truncated record, missing index/footer, or interrupted compression block
means for their format. Readers expose recoverable complete records and label
damaged/truncated tails rather than reject an otherwise useful run. Keep the
original bytes; any repair output is a separate derivative. A single invalid
producer file must not hide other producers' data or prevent raw-file export. An
export can contain every surviving file while still reporting that the recording
was interrupted; it cannot recover bytes that never reached storage.

## Lifecycle and retention

The SystemCore service owns whole-run retention across all managed formats and
known volumes. It may reclaim only proven-inactive, unpinned runs without reader
holds. Logger stop/start, a quiet file, DS disconnection, or a lost client
connection does not establish inactivity. Surviving helpers and transfers remain
protected. Interrupted runs become normally eligible once inactivity is proven;
they do not require a clean-close marker.

Removing a verified redundant transfer source is separate from run retention and
requires coordinator authorization. This includes retiring and removing the
internal run directory after all producers have completed the move to USB, even
while the run remains active there. When no safe retention candidates remain,
report storage pressure; producers retain bounded-buffer and write-error
handling. No producer may delete another producer's data. Automatic desktop
retention remains off unless a custom host explicitly configures it.

The companion [lifecycle and retention design][] specifies lifetime evidence,
registry ownership, policy settings, deletion ordering, filesystem safeguards,
and recovery of interrupted reclamation.

## Late-mounted USB transfer

Late-mounted USB support is a v1 requirement. USB uses the same `runs/ID` layout
and becomes the preferred destination. Producers adopt it independently; there
is no run-wide prepare barrier, commit point, or rollback. One producer may move
successfully while another stays on internal storage, and files from one
producer may also remain on both volumes.

The target is to place ongoing output and transferable history on USB. Partial
progress is a supported outcome, not a reason to undo successful moves. Report
which producers are writing there and which files remain elsewhere. Do not
report the stick as containing the complete run until all known history is
accounted for there; an active run's status is a snapshot, not a future
guarantee.

### Destination discovery and independent adoption

Obtain destination preferences through the C library. A notification is
advisory: each producer moves when its writer permits, reports its actual path,
and keeps paused/stopped state. A late join receives the latest preference
without waiting for other writers. Earlier approved locations stay valid until
explicitly retired; changing preference does not move existing handles or make
old files redundant.

The service/library [USB destination and reconciliation design][] defines mount
validation, announcement ordering, location tracking, source-cleanup authority,
and interrupted-transfer recovery. If a destination fails, preserve that
writer's source and report partial placement; do not roll back other producers.
A writer must not recreate missing run ancestors or write beneath an absent
mount.

### Producer-local transfer and historical files

A transfer preserves the affected writer's record order and accepted data within
its documented buffering and transfer limits. Failure preserves the source and
reports incomplete placement. Transfers do not block robot periodic execution,
force another producer to flush or pause, or restart a stopped logger. Buffer
exhaustion and any resulting record loss are visible.

Earlier segments and auxiliary files remain part of the run even after ongoing
output moves to USB. They may transfer later; a producer that cannot move may
remain internal for the whole run. Existing files are not overwritten, and
unfinished copies are not presented as completed logs. Full-history status
accounts for all known files, separately from where ongoing output is written.

Only verified redundant source data is eligible for transfer cleanup, with
coordinator authorization and respect for reader holds. Unique history remains
subject to whole-run retention. Offline removals are not reconstructed by later
automatic transfers. The companion [writer transfer design][] specifies writer
serialization, copy verification, receipts, and historical-file handling.

### Completion notification and cleanup

Each producer notifies the C library when its move to USB is complete. This
means ongoing output uses USB, all of that producer's source history and
auxiliary files have durable USB copies, and it has finished using those source
files. Merely switching the current output file does not indicate completion.
Paused producers remain paused and use USB when they resume.

The library forwards completion to the SystemCore service or the embedded
coordinator. Once all relevant producers and their historical files are
accounted for, the coordinator schedules cleanup of verified redundant internal
copies. Download holds and unresolved copy evidence defer cleanup; cleanup is
reconsidered when those blockers clear. Status distinguishes individual
completion, all-producer completion, and cleanup pending or finished.

Completion is a notification after each independent move, not a barrier to
moving or continuing to log. Late joins and logger restarts are included in the
current completion status; a missing notification, failed move, or crash is not
assumed complete. Recovery can establish completion from surviving files and
writer-lifetime evidence without a callback from a dead producer. A previous
completion does not override offline USB changes or missing media.

Once every producer has released the internal location, all source data and run
metadata are safely available on USB, and no download hold protects the source,
the coordinator retires and removes the internal run directory and its display
aliases. The run remains active on USB; sealing is not required. A pending
producer, unresolved history, or source hold defers removal.

Late producers and logger restarts use USB after retirement. Neither an old
cached path nor a restart makes the internal location writable again. If USB
then becomes unavailable, logging reports that failure without recreating the
retired internal directory; automatic reverse failover remains a separate
decision. A new executable run can still use internal storage normally.

The companion [completion and cleanup design][] defines notification delivery
and completion tracking; [source location retirement][] defines removal,
registration races, and recovery.

### Offline USB file management

Turning off the robot, removing the stick, moving actual run directories/files
on another computer, reinserting it, and booting again is a supported workflow.
It requires neither a graceful shutdown nor an export receipt. The new
executable gets a fresh run while the service reconciles old files in the
background. Display symlinks and `.runref` files alone do not contain the logs.

Do not recreate old files removed while offline, and do not use stale USB copy
receipts to delete surviving internal sources. Confirmed absence on a
recognized, readable volume differs from an unavailable or unreadable volume.
Resolved absence permits ordinary retention of remaining inactive fragments; it
does not prove a successful export. Reformatting or replacing a volume is a
separate case. The companion [offline reconciliation rules][] define persistent
cancellation, copy revalidation, volume identity, and behavior after another
power loss.

## WPILib integration requirements

DataLogManager always uses the required library and managed placement on
SystemCore and desktop, including automatic startup, explicit `Start()` calls,
and stop/start cycles. Disabled or invalid context, failed registration, or
unavailable storage produces a diagnostic and disabled file output while robot
control continues. An established writer can continue at its approved location
during a coordinator outage. There is no legacy naming, storage-selection, or
cleanup fallback.

The C++, Java, RobotPy, and ntcoreffi integrations share these requirements:

- Output uses the `wpilib` namespace and a stable base filename such as
  `WPILIB.wpilog`. Logger stop/start stays in the same run without truncating an
  earlier segment; an executable restart creates a new run.
- Labels and retention belong to the coordinator. DataLogManager reports storage
  pressure, the volume actually in use, and managed status, including when
  desktop retention is disabled.
- USB adoption is independent of other producers. The internal-storage alert
  clears only when this writer uses USB; history left elsewhere is reported
  separately. `GetLogDir()`/`getLogDir()` returns the manager's actual
  directory, changing only after adoption. It is not the cross-vendor context
  API. The manager notifies the library when its complete move, including
  earlier segments and auxiliary files, has finished.
- Existing NetworkTables, message, console, and DS-disconnection pause/resume
  behavior remains unless separately changed. Logger policy does not determine
  run identity or sealing.
- Filename and flush-period customization remain within the assigned namespace.
  The `Start(dir, filename, period)` directory argument cannot bypass managed
  placement or silently change meaning from an exact directory to a run root.
  Its API migration remains an open decision. Custom managed roots are
  configured through the library/coordinator.
- Generic `DataLogWriter` and `DataLogBackgroundWriter` retain their explicit
  path contracts for standalone files, tools, and replay. The `.wpilog` format
  is unchanged.

Supervisor startup/exit diagnostics remain associated with the run when storage
works, including when WPILib fails to start. Optional per-run console capture is
another independent producer; its output must not bleed into a later run or be
duplicated unexpectedly. Console migration and the managers' existing
`GetLogDir()` initialization differences remain explicit API decisions.

The companion [WPILib implementation][] specifies writer changes, bindings,
source/build surfaces, console integration, and validation.

## Vendor integration requirements

Vendors use the standalone C library directly or through a binding to its ABI on
SystemCore and desktop. The integration requires neither wpiutil nor HAL.
Sidecars and compatibility adapters use the same library; launch variables and
the server protocol are private implementation details.

1. Automatic logging joins the assigned run before its first file, regardless of
   initialization order. A late start receives the current preference;
   initial-path-only adapters report that capability explicitly.
2. Default output uses the producer's assigned namespace. Supported explicit
   user destinations remain opt-outs and are reported as outside shared run
   retention.
3. Actual and preferred locations are distinct. Independent USB adoption
   preserves paused/stopped state and reports files left elsewhere. Deferral or
   failure never blocks another producer. Notify the library when both ongoing
   output and all source history have moved, enabling coordinated source
   cleanup.
4. Managed output uses shared destination selection and retention. Vendors
   disable their independent free-space/history deletion while retaining
   bounded-buffer and write-error safeguards; stopping automatic logging alone
   is insufficient. Transfer cleanup respects coordinator authorization and
   reader holds.
5. Producer version, output format, active lifetime, locations, and unsupported
   capabilities are visible to tools. File metadata includes the run ID where
   practical. Existing filenames may remain; label changes do not rename them.
6. Each producer defines media-loss, low-space, transfer-failure, and retry
   behavior. Logger stop/start stays in the run; application restart joins a new
   run without reopening old files for new records.
7. Useful records remain readable after hard termination without stop/close
   callbacks. Producers document persistence and expected loss windows. Recovery
   belongs to the service or embedded coordinator and does not require repair by
   the replacement application. Embedded recovery needs a running host or
   recovery tool.

CTRE, REV, and AdvantageKit follow this same contract. AdvantageKit also keeps
replay inputs read-only: default managed replay outputs belong to a new run,
with `replay_source_run_id` when known. Explicit output-beside-input paths
remain opt-outs, and viewer-opening uses the actual output file and run context.

Complete grouping requires a vendor update when paths or cleanup cannot be
configured. The companion [vendor integration work][] describes the necessary
adaptations and qualification for each producer.

## Download, inspection, and compatibility

Device tools expose one catalog across registered volumes through supported
library/tool APIs. Offline tools use the documented layout, `.runref` format,
and metadata. A run appears once by run ID, regardless of how many aliases,
volumes, or verified copies it has. Unknown producer formats remain part of the
run; legacy flat files appear separately.

The catalog shows time, event/match history, outcome, active state, producers,
actual placements, total bytes, and missing history. Recording outcome, per-file
damage, unavailable volumes, and confirmed offline absence are distinct
statuses. Raw surviving files remain exportable even when a format-specific
reader cannot open them.

An export is a self-contained directory or archive named with the current
display name, containing real producer files and a metadata snapshot describing
completeness and source volumes. It includes unique data from all known
locations, omits verified redundant copies and unfinished staging, and preserves
unresolved name conflicts. Display aliases alone are not exported as log data.
Retention and transfer cleanup protect files while export holds are valid; loss
of that protection invalidates the export and is reported. Failed downloaders do
not protect storage forever.

A live export is best-effort, marked incomplete, and cannot trigger source
deletion. Delete-after-download is available only for a sealed run with all
unique history available and verified in the export, and only while the run
remains eligible for coordinated whole-run deletion. Ordinary offline USB
moves/deletions require no live service transaction or export receipt. Manual
deletion of individual files while the robot runs remains outside grouped
retention. The companion [catalog and export implementation][] defines
discovery, copy verification, holds, and deletion ordering.

During rollout:

| Combination | Behavior |
| ----------- | -------- |
| New SystemCore launcher, new producer | Managed placement, independent late-USB adoption, location reporting, and service retention. |
| New SystemCore launcher, old configurable producer | Adapter may provide placement/lifetime tracking; qualify automatic startup and cleanup suppression. If transfer is unsupported, keep that producer at its initial location without blocking others. |
| New SystemCore launcher, old unconfigurable producer | Continues legacy behavior; report incomplete adoption. Do not move its open files or guess run identity afterward. |
| Old SystemCore launcher, new DataLogManager | Missing required context disables file logging with a diagnostic. Upgrade OS launch integration; there is no legacy fallback in the new manager. |
| Old SystemCore launcher, transitioning vendor | Legacy behavior is possible only under that vendor's documented policy; managed placement still requires OS launch integration. |
| Direct desktop launch, new producers | Shared runtime automatically hosts the coordinator and allocates one process context. No wrapper or daemon; retention and USB discovery are off by default. |
| Custom simulation host, new producers | Configure the reusable coordinator library before first use. General simulation and robot-like storage/lifecycle setups use the same producer API and run format. |
| Desktop launch, old producer | A configurable producer can join through an adapter. Otherwise report incomplete adoption; no shared library can redirect an unmodified hard-coded destination. |
| Mixed producers | Managed runs and legacy flat files coexist. No legacy writer or cleaner may recursively enter the managed run store. Managed producers may remain on different approved volumes; report partial placement and audit cleanup suppression before default enablement. |

Remove the new DataLogManager's old directory selection, per-file naming, and
cleanup unconditionally when integrating the library, including the ntcoreffi
copy. Do not retain them behind a mode flag or library-availability check.
Legacy behavior exists only in older binaries or separately transitioning
vendors. SystemCore must disable producer cleanup for all managed participants
before claiming centralized retention. If old vendor cleaners recursively scan
the root, use a separately protected managed root during migration or block
deployment until they are fixed; new metadata cannot constrain an old binary.

Legacy flat files have no reliable run boundaries. Do not manufacture grouping
by similar timestamps. Provide explicit import/export and a separate opt-in
legacy retention policy after confirming which writers still use them. Missing
metadata does not authorize the new service to delete arbitrary files.

## Trades

These trades record the design decisions, alternatives considered, rationale,
and accepted costs. Unresolved choices and qualification requirements are listed
in [Open trades and decisions](#open-trades-and-decisions).

### Stable storage paths and directory naming

**Design:** keep the actual directory named `runs/ID` on each volume and update
a human-readable alias as time/match metadata arrives. Only an explicit storage
transfer changes the root/path. Producers need no notification of a label
change. The cost is an opaque physical directory name, additional alias entries,
and explicit reference resolution in tools when symlinks are unavailable.

Use symlinks when available and plain-text `.runref` files otherwise. A text
reference preserves the friendly name and portable relative target using an
ordinary file, and people can read it in an editor. Its cost is that file
browsers and log viewers must understand the convention to navigate directly to
the run. Omitting aliases would lose the friendly filesystem entry;
platform-specific shortcuts would add another format for tools to interpret.
Keep `run.json` authoritative for display metadata and both alias forms
rebuildable. The text representation changes no producer path semantics.

The filesystem distinction is between an open object and a path used for a
future lookup. Linux documents that existing open descriptors are unaffected by
rename; `openat()` can resolve a new relative filename through a retained
directory descriptor. Apple's rename documentation likewise describes renaming
the symlink itself rather than its target. These support a stable-write-path
design, but not a multi-operation rename/retarget transaction. See
[Linux rename][], [Linux open/openat][], and [Apple rename][].

Renaming the actual directory preserves existing POSIX file handles, but cached
paths used for new files, rotation, existence checks, or late producer startup
become stale. Renaming a directory and updating a symlink pointing to it are two
separate operations; changing their order does not provide an uninterrupted
path. WPILib's background writer also performs these later path lookups.

| Approach | Assessment |
| -------- | ---------- |
| Real directory rename with producers retaining its original path | Existing handles continue, but subsequent path lookups fail. Updating the path in one producer does not coordinate other producers or concurrent operations. |
| Real directory rename plus a per-run symlink that is retargeted | Avoids cross-run ambiguity, but still has a two-operation gap. Retrying all opens/stat/rename operations can mitigate it only for cooperative producers; cannot promise compatibility with arbitrary path-only libraries. |
| Real directory rename plus a held directory FD and `openat`/`renameat`/`fstatat` | Viable for fully adapted POSIX producers. Every namespace operation, including free-space and existence checks, must use retained handles. Late attachment needs a coordinator to supply a handle by immutable run ID. Java/vendor path APIs and Windows require separate adaptations. More invasive than v1. |
| Linux bind mount or `/proc/self/fd/N` path | Can provide Linux-specific indirection, but brings mount/FD lifetime, privilege, canonicalization, and portability requirements. Not a portable vendor contract; `/dev/fd` is not assumed equivalent on macOS. |
| Rename the actual directory only after sealing | Safe for writers once all are gone, but invalidates cached inspection/download paths and cannot provide physical match-based names during execution. Useful for an explicit export, not needed for managed storage. |
| Stable actual directory, mutable display alias | Selected design. Use a symlink or a plain-text reference fallback. Existing paths, new files, and late producers all work without reacting to label changes. |

Windows symlink creation can require permissions or developer configuration, so
it cannot be required for simulation. Existing Windows file handles also have
sharing semantics that need separate validation before any physical-rename
design is attempted. Stable canonical paths work without that dependency. See
[Microsoft CreateSymbolicLink][].

If actual on-disk directory renaming while active is a hard requirement, select
the descriptor-based alternative and make vendor adoption of all relevant
operations a prerequisite. This design makes the human-visible directory entry
carry the useful name while preserving canonical paths on each volume. Moving
between volumes is a separate, producer-local operation.

### Discovery before producers start

**Design:** use supervisor allocation and immutable launch context on
SystemCore; use first-call allocation in one shared native runtime on desktop.
Both establish identity before a participating producer opens files, without HAL
or DataLogManager. SystemCore covers startup before application loading and
supervised helpers. Desktop supports normal executable/JVM/Python launches and
unordered producer initialization without a wrapper or daemon.

A mandatory simulation wrapper could allocate before any application code, but
would change direct-launch workflows and require every launch tool to integrate
it. A desktop daemon could outlive applications, but would add installation and
service management. Neither is required: the reusable coordinator library runs
in the simulation host, and recovery resumes when another host or recovery tool
runs. The accepted cost is no run for a crash before the first library call and
no coordinator work after host termination. Custom hosts may initialize earlier.

Allocating in DataLogManager or independently in each vendor would make grouping
depend on startup order. The shared-runtime design instead serializes first use
and owns one default context across participating native and language bindings.
Privately linked copies, environment-only readers, and unmodified hard-coded
writers cannot join that guarantee. Supporting explicitly grouped helper
processes requires a host interface and lifetime tracking; it is not inferred
from directory scans or parent process identity.

An environment-only producer interface is simpler but cannot tell a late
producer that USB is now preferred, and is not a supported integration path. The
selected C library hides discovery and registration. Internally, an explicit
launch descriptor or service lookup by caller identity can establish context;
that choice is owned by the library/SystemCore maintainers. SystemCore first
registration depends on service availability; embedded registration calls the
local coordinator. Established writes and local transfers to approved
destinations do not require a service request. Accepting concurrent use of old
and new locations removes the need to serialize every first open against a
global cutover. A newest-directory scan, PID alone, or wall-clock naming cannot
reliably distinguish overlapping invocations. Changing the working directory or
adding a symlink cannot retrofit a hard-coded destination with no configuration
hook.

### A current alias for discovery or inspection

**Design:** producers never use `current` as their retained write path. An
optional inspection-only alias may be provided as a symlink or, where
unavailable, a `current.runref` with the same text format as other references.
Whether to ship either form by default remains open.

```text
ROOT/current -> runs/ID
```

A global `current` is attractive because every producer could open the same
well-known path without an environment contract. However, a new executable can
retarget it while the old executable or a helper is still running. Existing open
handles keep writing to the old run, while later opens or rotation can write
into the new run. Resolving it once at startup avoids later retargeting, but
still selects the wrong run if an old invocation starts a producer after the
replacement executable has published its context. A late sidecar cannot infer
its intended invocation from that pointer either.

Pointing `current` at a physically renamed directory adds the rename/retarget
gap described above. Pointing it directly at stable `runs/ID` avoids that gap
but does not solve cross-run discovery. For concurrent simulations, `current`
can mean only the latest published launch, not the only active run.

For inspection, either form offers convenient access to the latest published
run. Its absence or staleness never affects a producer, and neither
representation resolves the cross-run selection race. Publication and recovery
details are in the companion [inspection alias implementation][].

A temporary-directory Linux experiment for this design confirmed that an open
file remains writable after its parent is renamed, the old path and a symlink
targeting it break, and a retained directory FD can open another child. It also
confirmed that a canonical producer path survives display-alias changes while a
later open through a retargeted `current` reaches a different run. This
illustrates the failure modes; it is not macOS/Windows/vendor qualification.

### Transferring to USB after launch

**Required behavior:** support a USB drive that mounts after launch while
preserving the run ID and existing data. **Design:** publish a common preferred
destination and let each producer adopt it independently, reusing #9570-style
writer-local transfers where appropriate. Partial placement across volumes is a
normal supported state. No run-wide transaction is required.

| Approach | Trade |
| -------- | ----- |
| Keep internal storage for the whole execution | Simple path lifetime, but does not support active late-mounted USB adoption. Still a valid outcome for a particular producer that cannot transfer. |
| Copy/export while continuing to write internally | Provides a USB snapshot but leaves ongoing internal storage pressure. Useful for closed history or export; does not move ongoing output. |
| Independent producer moves to a common destination | Selected design. Reuses writer-specific APIs, allows different limits/timing, and a failure does not block other producers. Requires location tracking and tools that combine a run across volumes. |
| Start new USB segments and copy closed history later | Supported producer strategy. Shortens the pause for ongoing output; the stick is incomplete until older files are copied, and earlier files may remain internal. |
| Retarget a symlink or bind mount | Does not move open file/directory handles or preserve their data. Each writer still needs its own safe switch; an alias alone cannot implement transfer. |
| Pre-copy active files, then finalize locally | May reduce a writer's pause for large files, but needs format-specific snapshot/delta guarantees. Optional producer optimization, not a shared requirement. |
| Freeze every producer and commit the whole run together | Offers one writable volume after the cutover, but the least capable/slowest writer gates everyone and failures need distributed recovery. Not required by this design. |

The accepted cost of independent progress is that removing a USB stick may not
retrieve the entire run. Metadata and tools must show which locations contain
unique data. Preserve incomplete history rather than making all producers wait
for a complete-run copy. Per-writer limits, copy scheduling, and eventual
sealed-run consolidation policy remain open.

### Other key design trades

| Decision | Alternatives considered | Design choice and benefit | Cost or limitation |
| -------- | ----------------------- | ------------------------- | ------------------ |
| Run boundary | Per boot, per match, per logger session, or per executable invocation | One executable invocation, including designated helpers. Groups startup failures and all producer sessions under one identity. | A long-running executable may span multiple matches and accumulate a large run. |
| Run identity | Timestamp, PID, match tuple, or random ID | Random 128-bit ID with exclusive allocation. Works before trusted time and across restarts/concurrent simulations. | IDs are not descriptive; labels and metadata provide human context, and imports still check collisions. |
| Producer layout and formats | Flat files with coordinated names, producer namespaces, or a common log format | Producer namespaces with existing formats. Avoids collisions and minimizes writer/converter changes. | Adds directory depth; exporters and viewers must discover multiple formats. |
| C API ownership and distribution | Embed the client in wpiutil, duplicate it in vendors, or publish standalone libraries | Standalone C client ABI and reusable coordinator, consumed like mrclib. Vendors and simulation hosts can integrate without wpiutil or HAL; WPILib supplies language adapters. | Requires releases, ABI compatibility, OS/desktop packaging, and one shared owner of the desktop default context. |
| Vendor integration boundary | Public wire protocol with independent clients, environment-only discovery, or mandatory C library | Require the shared C library or bindings to its ABI on every platform. Keep SystemCore transport and bootstrap private, following the mrclib/mrccomm model. | Vendors package/link the supported runtime. Library/OS maintainers own compatible server/runtime updates; tools need supported high-level APIs. |
| DataLogManager integration | Optional library with legacy fallback, or required library with one managed implementation | Require the library and always use managed placement on robot and desktop. Remove manager-owned selection, naming, and retention. | New managers need compatible runtime/OS integration. Operational failures disable file output; the old directory argument needs an API migration. |
| Coordination and data writes | All data through a daemon, a WPILib-owned manager, or coordinator policy with direct producer writes | SystemCore service hosts coordination/retention; desktop embeds the coordinator. Producer data stays outside the control path. | OS integration and embedded lifecycle differ; only the service survives application crashes. Each producer still handles its own I/O errors. |
| Metadata ownership | Every producer edits shared metadata, infer it from filenames, or one coordinator writes snapshots | One coordinator, fed by WPILib observations independently of DataLogManager. Gives consistent labels and avoids multiwriter updates. | Coordinator outages can leave stale metadata; bounded queues and reconciliation are required. |
| Time and match labels | Exact/estimated launch time, latest time, latest match, or a frozen primary label | First trusted UTC and first accepted match, with full history and `_MULTI`. Limits label churn and records uncertainty separately. | The name alone does not describe every match or the exact launch time; tools must read metadata. |
| Crash restart and recovery | Reuse the old run, wait for its finalization, or allocate a fresh run while recovering it | Fresh run for every application restart; automatic recovery and sealing of old runs outside the launch path. Handles repeated crashes without depending on cleanup callbacks. | Requires durable invocation identity and reconciliation of surviving helpers; interruption may leave unknown exit details. |
| Retention unit | Delete individual files/segments, including active ones, or delete sealed runs | Delete whole sealed runs. Preserves cross-producer completeness and avoids interfering with active writers. | One large active run or pinned data can exhaust storage; v1 accepts logging pause/failure in that case. |
| Storage changes during a run | Fixed volume, independent moves, or a run-wide transaction | Publish a common USB preference; producers move independently and notify completion through the library. After all moves finish, the coordinator can retire and remove the internal run directory while logging continues on USB. | A run may span volumes indefinitely; tools distinguish ongoing USB output, full-history completion, and cleanup. Retirement must prevent stale-path reuse by late producers and survive crashes; it does not provide automatic reverse failover. |
| Offline USB changes | Require a managed export receipt, automatically reconstruct missing files, or reconcile ordinary filesystem changes | Support offline moves/deletions without a receipt. Record confirmed absence, suppress old-run reconstruction, and revalidate copy evidence before source cleanup. | Absence cannot prove export or intent; an incomplete old copy may remain internal. Catalog state must distinguish absence from an unavailable volume. |
| Simulation ownership | Desktop service, per-launch wrapper, or reusable embedded coordinator | Direct launches automatically use the in-process library; general simulation hosts can configure it, including robot-like setups and tests. No daemon; desktop retention is off by default. | All producers must share the runtime. Earliest allocation is first use; work stops with the host. Explicitly grouped helpers need lifecycle integration. |
| Explicit destinations and migration | Redirect all paths, reject custom paths, or preserve explicit opt-outs | Keep exact paths in generic writer APIs and supported vendor opt-outs. DataLogManager always uses managed placement; custom run roots are configured through the library/coordinator. | Existing manager directory arguments need migration. Vendor opt-outs and old binaries cannot be included in the complete grouping/retention guarantee. |
| Export and deletion | Copy/delete active files immediately, take filesystem snapshots, or hold a sealed run during verified export | Reader hold plus sealed-run inventory/checksum verification before whole-run deletion. Portable and coordinated with retention. | Requires tool/service support and temporary storage protection; live exports remain incomplete and cannot trigger deletion. |

The companion [implementation trades][] cover lifecycle evidence, persistence
cadence, and allocation failures. The choices here describe the shared behavior
that producer and tool integrations depend on.

## Open trades and decisions

These decisions concern the shared contract and ecosystem integration. The
companion [server/library open decisions][] track packaging, private transport,
embedded hosting, retention policy, persistence, and reconciliation details.

The following items remain unresolved or require validation before the relevant
release gate. Each row identifies the current design where applicable and the
decision or evidence still needed. If resolving an item changes the design,
update the corresponding contract and acceptance tests before implementation.

| Open decision | Current design and remaining choices | Owners | Evidence or agreement needed to close |
| ------------- | ------------------------------------ | ------ | ------------------------------------- |
| Physical naming requirement | The design uses stable physical directories with display aliases. Confirm that this meets the physical naming requirement; live directory renaming would require descriptor-based producers and a design change. | WPILib, vendors, SystemCore, tool maintainers | Confirm browsing/export requirements and whether every producer could support handle-relative operations if physical renaming is required. |
| Public discovery API and namespace governance | Vendor use of the C library is required. Finalize C ABI/bindings, run identity/status, producer registration, preferred versus actual locations, notifications, and namespaces. Private bootstrap/endpoint fields are excluded. | Library maintainers, WPILib, vendors | Qualify late joins and notification races through the library without blocking on other writers; cover language bindings and placement-only adapters that call the C ABI. |
| DataLogManager directory-argument migration | Managed mode and the required library are settled. Choose deprecation/removal of the directory argument or validation against the assigned managed directory; define diagnostics and source/binary migration. It cannot opt out or silently become a run-root override. | WPILib and binding maintainers | Review C++, Java, ntcoreffi, and RobotPy signatures and callers. Document shared-root configuration and direct-writer replacements for exact-path output. |
| Public metadata and tool interfaces | Finalize required/optional JSON fields, clock mappings, observation limits, and supported catalog/export/hold/deletion APIs. The examples are illustrative; the private server protocol is not a tool interface. | Library maintainers, WPILib, SystemCore, tool maintainers | Publish versioned metadata schemas, API documentation, and cross-language fixtures; identify supported clocks and bounds for queued/history metadata. |
| Human-readable naming policy | Confirm first trusted UTC versus estimated start time, DS/FMS debounce, match corrections/replays, `_MULTI`, the `.runref` suffix/format, and whether inspection-only `current`/`current.runref` is enabled by default. | WPILib, vendors, tool maintainers | Review real DS/FMS sequences and team workflows; agree how names and displayed history represent corrections and multi-match runs, and qualify reference-file readers/publication on supported filesystems. |
| Independent USB transfer and consolidation policy | Late-mounted USB adoption is required; a run-wide transaction is not. Agree supported transfer limits, failure/retry behavior, progress reporting, and whether sealed-run history moves automatically. | WPILib, SystemCore, vendors | Demonstrate partial success, source preservation, visible loss/limits, and historical-file coverage with actual binaries. Implementation budgets and copy scheduling are tracked in the companion design. |
| Producer recovery after I/O failure | Decide whether each writer automatically retries, requires explicit resume, or waits for a new run after low space or media errors; preserve the same run while active. | WPILib and each vendor | Document bounded buffering, loss/status reporting, retry limits, and segment continuity; qualify the actual writer binaries. |
| Console capture | Retain the current global-file reader, replace it with per-run supervisor capture, or bridge the two during migration. | SystemCore and WPILib | Confirm startup diagnostics, stdout/stderr ownership, rotation, and how to prevent cross-run tailing or unintended duplicate entries. |
| Vendor and legacy rollout | Identify supported vendor versions and cleanup controls, support duration for older binaries, and whether migration needs a separately protected managed root. New DataLogManager has no legacy mode. | WPILib, CTRE, REV, AdvantageKit, SystemCore | Verify startup/path configuration and cleanup suppression; audit old recursive cleaners before deploying the new managers or enabling service retention. |
| Export protection and deletion UX | Agree protection/status behavior during downloads, missing-volume and conflict labeling, and delete-after-download eligibility and feedback. | SystemCore, vendors, tool maintainers | Qualify multi-volume all-format export, interrupted downloads, incomplete history, and deletion requests. Copy evidence and hold mechanics are specified in the companion design. |

Resolve contract, naming, and interface choices before publishing vendor
integration requirements. Close storage, lifecycle, recovery, and retention
decisions before enabling automatic deletion. Track deferred capabilities
explicitly so that deferral does not imply they are supported by v1.

## Verification and rollout

### Acceptance tests

These scenarios qualify producer behavior and complete user workflows. The
companion [server/library qualification plan][] covers internal lifecycle,
persistence, transport, ABI, and embedded-host tests. Both sets are release
gates.

| Scenario | Required result |
| -------- | --------------- |
| Vendor static initialization before WPILib/HAL; randomized producer start order | SystemCore writers join the preallocated run; desktop writers initialize/join one shared context before opening files. No call to DataLogManager is required. |
| Vendor, C++, Java/JNI, and RobotPy consumers in supported runtime combinations | All use the external client's compatible ABI and backend contract. Test older binaries with compatible runtime updates, capability rejection, loader paths, one desktop context, and SystemCore versus desktop packaging. |
| Vendor sidecar or placement-only compatibility adapter | Discovery, registration, and status flow through the shared C library. The writer may keep its first assigned path, but the adapter does not read private bootstrap variables or implement transport. |
| Direct desktop executable, JVM, or Python launch without context variables, listener, or daemon | Packaged runtime creates one run on first use; all participating producers share it. Project-root launches use project `logs`; no automatic USB discovery or retention occurs. No launcher wrapper is needed. |
| DataLogManager startup/restart in C++, Java, RobotPy, and ntcoreffi | Always use the required client and registered `wpilib` namespace. No optional-library probe, legacy branch, independent USB selection, time/match file rename, or retention scan executes. Filename/period customization preserves managed placement. |
| DataLogManager with disabled/invalid context, missing SystemCore descriptor, or failed registration | Report disabled/error state and keep control running. Never create legacy output or invoke old cleanup, including through an explicit start call or directory argument. Desktop without descriptor still creates its normal embedded context. |
| Producer first starts after labels change or USB preference changes | It joins the original run ID at the preferred destination without waiting for other producers. An initial-path compatibility adapter reports its actual location. |
| Continuous writes, rotation, existence checks, and logger restart during label changes | No missing-path error, unintended reopen/truncation, lost segment, or filename collision. Exercise the actual native background writer and vendor writers, not just an open FD. |
| Fast executable restarts, PID reuse, overlapping old helpers, simultaneous simulations | Unique run IDs; no producer follows a retargeted inspection alias; old helpers remain protected until exit. |
| Invalid clock, large clock steps, FMS before trusted UTC, no DS ever | Logging starts immediately; metadata preserves uncertainty; no run discarded for being `TBD`. |
| Multiple matches/replays/corrections within one process | One run, complete observation history or an explicit incompleteness indication, stable primary label with `_MULTI` when appropriate. |
| Unusual/long event strings and colliding sanitized names | Portable bounded aliases; raw metadata preserved; ID prevents collision. |
| Application crash without close callbacks, followed by automatic restart | Allocate a distinct new run immediately within the startup budget; recover/seal old files asynchronously, without a dead writer's final acknowledgement or appending new data to them. |
| Power loss during file creation/rotation, periodic sync, or a partial record | Keep surviving raw bytes and expose recoverable records with an interrupted/damaged status. Verify C++, Java, RobotPy, and actual vendor reader behavior independently. |
| Low space with mixed formats, one huge active run, pins, and export holds | Reclaim whole eligible runs, respect hard protections, remeasure space, and report exhaustion if no safe candidates remain. |
| USB mounts after startup with multiple active/paused writers and closed segments | Producers move independently under the same run ID; paused writers stay paused. Track ongoing writes and historical files separately on both volumes. |
| Producers finish copying to USB at different times | Each notifies the library after its ongoing output and full source history have moved. After all relevant producers and holds are accounted for, retire and remove the internal run directory and its aliases while logging continues on USB. No sealing or shared cutover is required. |
| Last producer completes while a download hold exists or a late producer joins | Report cleanup pending while held; reconsider when the hold clears. Include the new producer's actual placement and history before claiming all-producer completion; preserve any new or uncopied source data. |
| Producer registers, restarts, or opens another file while others move | It need not wait for their transfers. Preference races leave valid protected locations; each writer serializes its own switch and path-cache changes. |
| One producer exceeds its limit, cannot move, or fails after another has switched | Keep the failed/deferred writer internal and the successful one on USB. Preserve all source history, report partial placement, and perform no run-wide rollback. |
| Destination is full/read-only, a path collides, or a local copy fails verification | Preserve the affected source and unrelated destinations; do not truncate files or falsely report adoption/completion. Other producers follow their own outcomes. |
| Append during a local transfer, slow media, and buffer exhaustion | Preserve each writer's record order within its supported budget; report loss outside it. Neither robot periodic execution nor unrelated writers wait for that copy. |
| Retention/live export overlaps independent moves or source cleanup | Protect active write locations and held files; remove only verified redundant source data. Source directory retirement waits for all source use, history, and holds to be resolved; never delete uncopied history. |
| Producer starts or restarts after the internal location is retired | Join the same active run on USB; reject cached internal paths and never recreate the retired directory. Tools show retired storage separately from missing history. |
| USB disappears after internal directory removal | Report logging unavailable without reconstructing the old source. A new executable run may select internal storage normally. |
| USB lacks symlinks or source/destination use different filesystems | Copy across volumes, publish locally, and maintain volume-local `.runref` aliases; do not depend on cross-volume rename or changing open handles. |
| USB unplug/replug, different volume at same mountpoint, read-only media | USB writers report unavailable and never write beneath the mount or onto the wrong medium; internal writers continue. Record all surviving locations. |
| Hard power-off, offline move/delete of USB logs, reinsert, and reboot | Start a fresh run without waiting for the old inventory. Reconcile the old run without a clean-close/export receipt; record absence and do not recreate removed files/directories or aliases targeting missing directories. |
| Partial offline deletion or modification with an internal source and old copy receipt | Revalidate destinations; a missing/changed copy cannot authorize redundant-source deletion. Keep remaining sources until normal retention applies and never report missing data as a verified export. |
| Team removes the entire log tree without reformatting the stick | Recognize the volume independently of the removed log metadata, create storage for the fresh run, and reconcile old directories as absent without restoring them. |
| Export with two populated volumes, a missing peer, or uncertain duplicate filenames | Group by run ID, deduplicate only verified copies, and preserve conflicts. Missing unique history makes the export incomplete and forbids delete-after-download. |
| No symlinks, ordinary Windows account, macOS, Linux, approved USB filesystems | Canonical path works; `.runref` fallback resolves the same run; exports contain real files and remain self-contained. |
| Disabled managed mode, exact-path generic writers, vendor overrides, legacy binaries, replay input | DataLogManager remains managed with file output disabled; deliberately independent writers retain their contracts. Control continues; opt-outs are clear; no source replay modification or false full-adoption claim. |

These workflows must pass with actual WPILib and vendor writers/readers on
SystemCore and supported simulation platforms. The companion
[server/library qualification plan][] defines the test harnesses, build checks,
fault injection, and power-cut qualification.

### Release gates

The required client runtime and SystemCore launch integration must be available
before deploying the new DataLogManager. Public API, schema, namespace, and
migration decisions are agreed before vendor integration is published.

Centralized retention is enabled only after participating producers have
disabled independent cleanup and the lifecycle, power-loss, disk-pressure, USB,
and export/deletion scenarios pass. Legacy binaries and unsupported producer
paths remain visible limitations; they do not introduce a fallback mode in
DataLogManager.

The companion [ecosystem delivery sequence][] specifies implementation order and
release coordination, with [deployment and rollback rules][] for compatible
runtime/server/application combinations. The open decisions in both documents
track the agreements needed at each gate.

[C++ DataLogManager]: ../../wpilibc/src/main/native/cpp/system/DataLogManager.cpp
[Java DataLogManager]: ../../wpilibj/src/main/java/org/wpilib/system/DataLogManager.java
[Native background writer]: ../src/main/native/cpp/DataLogBackgroundWriter.cpp
[Java calls it through JNI]: ../src/main/java/org/wpilib/datalog/DataLogBackgroundWriter.java
[ntcoreffi manager]: ../../ntcoreffi/src/main/native/cpp/DataLogManager.cpp
[DataLogTool downloader]: ../../tools/datalogtool/src/main/native/cpp/Downloader.cpp
[#9570, Periodically check for USB and transfer log file]: https://github.com/wpilibsuite/allwpilib/pull/9570
[Phoenix 6 SignalLogger]: https://api.ctr-electronics.com/phoenix6/stable/java/com/ctre/phoenix6/SignalLogger.html
[REV logging]: https://docs.revrobotics.com/revlib/logs
[StatusLogger API]: https://codedocs.revrobotics.com/java/com/revrobotics/util/statuslogger
[WPILOGWriter]: https://docs.advantagekit.org/javadoc/org/littletonrobotics/junction/wpilog/WPILOGWriter.html
[traditional replay]: https://docs.advantagekit.org/getting-started/traditional-replay/
[Linux rename]: https://man7.org/linux/man-pages/man2/rename.2.html
[Linux open/openat]: https://man7.org/linux/man-pages/man2/open.2.html
[Apple rename]: https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/rename.2.html
[Microsoft CreateSymbolicLink]: https://learn.microsoft.com/en-us/windows/win32/api/WinBase/nf-winbase-createsymboliclinka
[Run log service and client libraries]: run-log-service.md
[public API operations]: run-log-service.md#shared-context-and-metadata-interfaces
[SystemCore launch design]: run-log-service.md#systemcore-launch-transaction
[desktop context initialization]: run-log-service.md#simulation-and-direct-launches
[simulation embedding interface]: run-log-service.md#reusable-simulation-coordinator-library
[publication algorithm]: run-log-service.md#alias-publication-and-crash-consistency
[persistence and recovery design]: run-log-service.md#power-loss-and-crash-recovery
[lifecycle and retention design]: run-log-service.md#lifecycle-and-retention
[USB destination and reconciliation design]: run-log-service.md#late-mounted-usb-transfer
[offline reconciliation rules]: run-log-service.md#offline-usb-file-management
[library API and distribution design]: run-log-service.md#library-api-and-distribution
[implementation and release plan]: run-log-service.md#implementation-and-release-plan
[implementation trades]: run-log-service.md#trades
[server/library open decisions]: run-log-service.md#open-trades-and-decisions
[server/library qualification plan]: run-log-service.md#verification-and-rollout
[deployment and rollback rules]: run-log-service.md#deployment-and-rollback
[initial storage selection design]: run-log-service.md#run-allocation-and-initial-storage-selection
[metadata ingestion design]: run-log-service.md#observation-ingestion-and-label-evaluation
[writer transfer design]: run-log-service.md#producer-local-transfer-and-historical-files
[WPILib implementation]: run-log-service.md#wpilib-implementation
[vendor integration work]: run-log-service.md#vendor-integration
[catalog and export implementation]: run-log-service.md#catalog-import-and-export-integration
[inspection alias implementation]: run-log-service.md#optional-inspection-alias-publication
[ecosystem delivery sequence]: run-log-service.md#ecosystem-delivery-sequence
[completion and cleanup design]: run-log-service.md#producer-completion-and-cleanup-trigger
[source location retirement]: run-log-service.md#retiring-the-internal-run-location
