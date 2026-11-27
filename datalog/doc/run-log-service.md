# Run log service and client libraries

Design and implementation specification for SystemCore OS and logging-library
maintainers. October 23, 2026. This document defines the target architecture and
implementation requirements. Unresolved decisions are tracked below.

This is the implementation companion to [Run-based robot log management][]. That
document owns the shared run contract, canonical layout and metadata, producer
behavior, WPILib/vendor requirements, and user workflows. This document owns the
implementation: server and C/embedded libraries, bootstrap, allocation, private
transport, persistence, lifetime, retention, USB reconciliation, producer/tool
adaptations, distribution, implementation trades, and qualification.
Cross-document links identify the contract each implementation must satisfy.

## Scope and architecture

The `wpilib-logd` service hosts the SystemCore coordinator and retention worker.
The separately distributed `wpilib-log-client` library exposes the C ABI used by
every managed producer, directly or through language bindings. A reusable
coordinator library supplies general desktop simulation and custom embedding.
Neither library belongs in `wpiutil` or depends on HAL. Names and the final
binary split remain release decisions.

| Component | Hosting and responsibility |
| --------- | -------------------------- |
| C client | One shared runtime per process; public context/registration/status APIs, private SystemCore communication, and dispatch to the embedded backend. |
| SystemCore server | Allocation before executable loading, supervisor lifetime integration, metadata/aliases, volume preferences, recovery, and retention outside the robot process. |
| Embedded coordinator | Automatically hosted in direct desktop launches; shared first-use context, configurable storage/lifecycle policy, and recovery while a host is running. No separate daemon or required wrapper. |

Keep the public C ABI distinct from private server transport and launch
bootstrap. Follow the mrclib/mrccomm integration model: vendor binaries use the
library, not the wire protocol. Run layout, `.runref`, and metadata remain
documented interchange formats in the [shared storage contract][].

The implementation preserves immutable run identity and stable per-volume
`ROOT/runs/ID` paths. Producers write their own files directly. Labels never
move active write paths; USB preferences permit independent producer transfers
with no run-wide transaction. Frequent hard power loss, crashes, automatic
restart, and offline USB file management are normal operating conditions.

## Library API and distribution

### Standalone C client library and distribution

Place the C API and its native client implementation in a separately built and
versioned library, with `wpilib-log-client` as a working name. Follow mrclib's
external dependency model: allwpilib consumes published headers and shared
binaries, and vendors must consume the same library directly or through bindings
to its C ABI. The library has no build or runtime dependency on wpiutil, HAL,
wpilibc/wpilibj, or a log file format. It may be implemented in C or C++; the
public boundary is a C ABI.

The client owns environment/context validation, producer registration,
preferred-destination discovery, storage-status reporting, and submission of
language-neutral observations. It dispatches to SystemCore IPC or the embedded
desktop coordinator behind the same producer API. DS interpretation and
producer-specific writer control stay in WPILib/vendors. The coordinator owns
allocation, metadata policy, and configured retention.

The C API/ABI is the supported producer integration surface on every platform.
Publish headers, binding documentation, capabilities, and API-level conformance
tests. Keep endpoint discovery, launch-descriptor encoding, message formats,
transport versions, and negotiation private to the library and SystemCore
maintainers. Vendors may provide language bindings to the C ABI, but must not
replace the client implementation with an environment reader or protocol client.
Private transport changes must not require changes to supported vendor binaries.

Publish the reusable coordinator library alongside the client, with a separate
host configuration/lifecycle interface. It is available to general simulation
consumers, including hosts that do not use WPILib, and is supplied automatically
with the ordinary desktop runtime. Share policy code with the SystemCore service
where practical, keeping OS-specific supervision and device integration
separate. Whether these components ship in one native binary or cooperating
shared libraries is a packaging decision; either arrangement must have exactly
one owner for a process's default context. Neither component belongs in
`wpiutil` or depends on HAL.

The packaging precedent is visible in the current source:

- [Gradle's mrclib dependency definition][] consumes
  `org.wpilib.mrclib:mrclib-cpp` headers and platform shared-library archives,
  with the version pinned in the [version catalog][]. Its shared dependency
  marks SystemCore as skipped for runtime packaging.
- [Bazel's mrclib module][] fetches pinned header/platform archives with
  checksums; [its build target][] exposes a `cc_import`, including Windows
  import libraries.
- [RobotPy's native HAL package][] downloads the mrclib native library off
  SystemCore and supplies platform-specific native dependency metadata. This is
  a packaging precedent, not a reason to put the new logging client inside HAL.

For the new client, publish a standalone header package and shared libraries for
SystemCore and supported Linux/macOS/Windows simulation targets, including
Windows import libraries. Provide an exported CMake package/target and
pkg-config metadata for direct consumers. Add matching pinned dependencies in
allwpilib's Gradle and Bazel builds and a CMake import; the current
[HAL CMake build][] builds the simulation sources and does not itself provide a
mrclib import to copy. Keep version updates and Bazel artifact checksums
consistent.

SystemCore installs the compatible client runtime as an OS package before
deploying applications that link it, alongside the separately running service.
Desktop development/simulation packages supply the matching client and embedded
coordinator runtime, ready for direct application execution. WPILib and vendors
must resolve one compatible shared runtime per process, rather than privately
embedding different client implementations in each logger. Define loader paths
and package ownership so vendor bundles do not replace the OS copy. Deployment
tooling checks runtime compatibility before launch where available; direct
desktop launches must resolve packaged libraries without a mandatory preflight
process. A missing required shared library is a deployment error, not a
recoverable IPC outage. The logging-disabled policy for coordinator/allocation
failure assumes that the required client runtime can be loaded.

DataLogManager links the client as a required dependency on every supported
platform. Do not add weak loading, runtime library-presence checks, or a build
option that restores its previous implementation. Backend selection and normal
ABI/capability negotiation belong to the library; the manager always calls the
managed API. A loadable library does not guarantee a reachable OS coordinator or
writable storage, so those operational failures still need bounded error
handling.

Version the public C ABI and metadata schemas independently. The library handles
any private transport negotiation and maps failures to documented API statuses;
vendors neither select wire versions nor parse wire errors. SystemCore packages
must supply a compatible library/server pair, with supported upgrade/rollback
combinations tested by their maintainers. A server-only update must remain
compatible with deployed library runtimes, or the OS update must also provide an
ABI-compatible library update. An incompatible pair fails visibly without
creating a fallback run or enabling cleanup.

Use C-compatible structures, explicit sizes/versions, fixed-width fields, opaque
handles, and defined string/buffer ownership. Do not expose `WPI_String`,
wpiutil handles, C++ standard-library types, or exceptions. Use caller-provided
buffers or library-owned allocations with matching release functions; never
require cross-library allocator compatibility. Specify calling conventions,
symbol visibility, callback threading/lifetime, and shutdown. Compatible library
updates must preserve existing client binaries; unsupported features return
explicit status rather than changing the run or falling back to legacy cleanup.

C++ convenience wrappers, Java/JNI bindings, and RobotPy semiwrap integration
consume this C ABI. Keep the WPILib-facing wrappers in their owning WPILib
modules; do not move the client implementation or a second copy of its state
into wpiutil. Java JNI calls the external client directly, without linking
wpilibc solely for discovery. RobotPy packaging must resolve the native runtime
before loading its wrapper. The final library/repository name, ownership,
artifact coordinates, wrapper packaging, and supported ABI lifetime remain
release decisions.

### Shared context and metadata interfaces

The standalone client exposes the following operations through its C ABI, with
idiomatic C++/Java/RobotPy wrappers. The names below describe operations rather
than finalized exported symbols. Every vendor integration uses this library;
language bindings call the same C ABI and shared runtime. A placement-only
adapter also calls the C library for registration and status, then supplies the
assigned path to an older writer. It may stay at that approved location if it
cannot transfer, but it does not bypass registration by reading the environment.

The C API provides these operations; final signatures remain an API-review
decision:

| Operation | Semantics |
| --------- | --------- |
| `GetRunContext` | The library resolves private SystemCore bootstrap state or initializes/joins the desktop default context once. Return immutable run identity plus preferred destination/revision, backend, and status, without exposing endpoint addresses, raw descriptors, or wire versions. Does not start DataLogManager, allocate a second run, or wait for DS data. Preference does not assert where another producer is writing. |
| `RegisterProducer(producerId, instanceId, capabilities)` | Join before the first write; return the preferred destination and bind lifetime/namespace to this run. Reject sealed/deleting runs and placement at retiring/retired locations. Serialize source placement approval with location retirement; after internal retirement, late/restarted producers join on USB. Declare transfer capabilities or placement-only compatibility; do not wait for other producers' moves. |
| `GetProducerDirectory(registration, destination)` | Validate the run volume and this registration's placement approval before creating the producer child beneath its existing run root. Return a stable path on that volume; distinguish disabled, missing media, permission errors, and retired locations. Do not authorize source reuse after this registration reports transfer completion; a new registration can use it only if the location remains eligible. Never recreate retiring/retired run roots. |
| `GetPreferredDestination` / `DestinationChanged` | Query or receive an advisory revision/path/volume identity. Ignore stale notifications. Each writer chooses when to attempt adoption; there is no required acknowledgement barrier. |
| `ReportProducerStorage` | Asynchronously report actual write locations, source history, transfer progress, copy receipts, and errors with producer-local sequence numbers. A missed report delays cleanup, not other producers. |
| `ReportProducerTransferComplete(registration, source, destination, completion)` | Notify the coordinator through the library after the producer has adopted USB, durably copied all source history, and relinquished source writes. Bind completion to this registration incarnation, volume identities, destination revision, and final inventory/copy evidence. Delivery is asynchronous and retryable; acceptance schedules cleanup evaluation without waiting for other producers. |
| `UnregisterProducer(registration)` | Release after all writes and local transfers stop and handles close. Keep historical locations in the run catalog. A later logger restart registers again and selects its location without truncating prior files. |
| `PublishRunObservation(observation)` | Internal asynchronous WPILib-to-coordinator submission with run ID, sequence, clock, and DS/match fields. Not a filesystem rename API for vendors. |
| `GetRunStatus` | Optional diagnostic snapshot of backend, metadata/storage state, configured retention, per-producer transfer completion, aggregate completion, and cleanup pending/finished status with blockers. Report service connectivity only for the remote backend. Not a per-sample logging dependency. |

Cache immutable identity/discovery fields once per process, with thread-safe
initialization in the external shared runtime, not in each language binding or
vendor library. Keep preferred and actual paths separate; update actual paths
only after that writer successfully switches. The C ABI must specify struct
version/size, string encoding and ownership, error codes, and caller buffer
behavior; C++ and Java return owned values. Public API additions need complete
Doxygen and JavaDoc. Test-only reset support must not imply that production code
can switch its active run. Separate discovery from privileged lifecycle/cleanup
operations. Expose host configuration, storage/lifetime providers, and optional
retention through the
[embedding interface](#reusable-simulation-coordinator-library); ordinary
producers neither select a new default run nor need to initialize that host API.

### Private SystemCore transport and coordinator behavior

A local request/reply transport, such as a Unix-domain socket, is an internal
implementation choice for the library and SystemCore server. Its specification
and tests belong to those maintainers; it is not published as an integration
contract for vendors. Supervisor allocation and lifecycle integration are also
OS/library responsibilities. The embedded coordinator implements the same public
API semantics through direct calls and asynchronous queues, without a socket or
separate server process for ordinary simulation.

Both backends must preserve these behaviors regardless of the private wire
representation:

- Supervisor-only allocation/binding/draining/sealing requests are idempotent by
  invocation identity/request ID. A retry after timeout returns the same
  allocation, not another run.
- Observation submission is idempotent by run ID, publisher, and sequence.
  Acknowledgements report the committed metadata revision.
- All writer registrations bind an instance/process identity, namespace,
  locations, and transfer capability to a particular run. They cannot join a
  `SEALED` or `DELETING` run; other producers moving does not block
  registration.
- Destination revisions and producer status sequences make duplicate or delayed
  messages harmless. Reconnection reports actual placement and pending copy
  receipts and replays unacknowledged completion notifications. Completion
  acknowledgement follows durable acceptance of its evidence; it does not
  establish a run-wide commit decision or authorize the producer to delete
  sources itself.
- Reader holds, pin changes, source-file cleanup, and deletion requests
  serialize with lifecycle/retention decisions. Publishing a preference does not
  invalidate holds on files at an earlier location.
- Source placement approvals and location retirement serialize with the same
  coordinator state. Track every issued approval, including one whose caller has
  not opened a file yet. A returned path cannot remain usable after its
  registration relinquishes that location; a delayed request cannot recreate a
  retiring or retired location.
- Private SystemCore requests have bounded size/timeouts, authenticated local
  caller identity, and internal version negotiation. The library exposes
  documented C API results. Calls do not stream log records: producers register
  once and exchange destination/status updates, not a request for each file open
  or log record during ordinary writing.

## Startup and discovery

SystemCore establishes the run before releasing application code. Desktop hosts
establish it before the first participating producer opens files. Both backends
expose the same C API and satisfy the [producer startup contract][].

### Run allocation and initial storage selection

Generate a random 128-bit run ID and reserve it using exclusive directory
creation, retrying on collision. Randomness alone is not a substitute for the
collision check. Follow the launch transaction below so allocation and its
namespace are protected before producers can write. Producer instances likewise
use exclusive file creation or distinct instance directories to prevent
accidental truncation within their namespace.

On SystemCore, choose the initial volume for the whole run. Prefer an approved,
mounted, writable USB volume, with internal storage as the fallback. Confirm the
mount and actual write capability; an existing `/u` directory alone is not
sufficient. Resolve path aliases before publishing the initial directory and
record the volume identity. If USB mounts later, create its run directory and
publish it as preferred using the destination-publication procedure below. Keep
the internal directory available to producers that have not moved.

### Internal SystemCore launch bootstrap

The following is an implementation sketch for the SystemCore supervisor and
client library. Its environment names, endpoint, and version are private
bootstrap details, not a vendor interface or compatibility promise. Only the
library interprets them; producers obtain identity, paths, and status through
the C API. Example initial descriptor supplied by the supervisor:

```text
WPILIB_LOG_PROTOCOL=1
WPILIB_LOG_MODE=managed
WPILIB_LOG_RUN_ID=8d5b7d3acfc64cf4b8ceca7312efeb10
WPILIB_LOG_RUN_DIR=/home/systemcore/logs/runs/8d5b7d3acfc64cf4b8ceca7312efeb10
WPILIB_LOG_ENDPOINT=/run/wpilib-logd/control.sock
```

The directory is absolute, exists, and is writable by the application at the
moment context is published. `WPILIB_LOG_RUN_DIR` is an initial location hint,
not a lifetime promise. The run ID and coordinator endpoint provide discovery
after transfers. The environment is read-only launch context; no library changes
it to broadcast a new path. No wall time or DS information is needed. This is
internal service discovery; the default desktop backend shares context directly
through the library instead of publishing an endpoint or mutating environment.

Before its first write, a producer calls the library to obtain context and
register its instance, selected location, and transfer capability. The library
validates the bootstrap descriptor and communicates with the coordinator.
Registration returns the preferred directory and a destination revision. This
revision orders preference notifications; it does not revoke existing write
paths. Vendors must link the shared library's C API or a binding to it on both
SystemCore and desktop; this requires no wpiutil, HAL, or DataLogManager. The
library owns transport and version negotiation; a producer must not open the
server endpoint or parse the launch environment itself. If preference changes
just after registration, the producer may start at the earlier approved location
and move later. No global transfer lock is needed. A producer that can only
retain its first assigned path can use a placement/lifetime adapter that calls
the C library and remain there; report that limitation without blocking other
producers.

| Library input state | Required behavior |
| ------------------- | ----------------- |
| Complete version-1 managed context | Register against that run ID and select the returned preferred destination, or explicitly report an approved compatibility location. Validate run and volume identity before first use; do not recreate a missing run root or silently use the initial environment hint. |
| `WPILIB_LOG_MODE=disabled` | Do not start automatic file logging or allocate a desktop run. Report explicit disablement or the supplied failure reason; allow robot control to continue. A run ID may still be supplied for diagnostics, but a writable directory is not promised. |
| Partially specified, unsupported, inconsistent, or inaccessible managed context | Report an error and disable automatic managed file logging. Do not silently invent another run or enable old cleanup. |
| No contract variables, desktop runtime | Initialize or join this process's embedded coordinator on first use. Direct execution is the normal managed path; allocate a fresh ID, never discover a run from old directories or an inspection alias. |
| No contract variables, SystemCore runtime | Missing required launch integration for DataLogManager: report an error and disable its file logging. Only legacy binaries or vendors with a documented transition policy may retain legacy behavior; do not silently substitute a desktop backend. |

An explicit user-selected file/directory may remain an opt-out for vendor APIs
that support it, including when managed logging is disabled. Such an override is
deliberate, separately diagnosed, and outside the shared retention guarantee; it
is never an automatic fallback from a damaged managed descriptor. DataLogManager
always uses managed placement; it has no directory opt-out. Configure its run
root through the shared library/coordinator. Exact-path standalone writing
remains available through the generic writer APIs.

Designated helper children inherit the private descriptor. Independently
launched sidecars require an explicit supervisor-issued context and lifecycle
registration through the C library, including when they start late. Bind
external requests to the invocation using PID/start identity or a process handle
as appropriate; a run ID is not an authorization secret. Long-lived vendor
daemons key output sessions by run ID and close each old session explicitly. On
automatic restart, allocate a new descriptor even after a pre-main crash; do not
inherit the failed application's descriptor. Keep old helpers bound to the old
run and fence or terminate them under supervisor shutdown policy. Their recovery
must not delay the replacement launch.

### SystemCore launch transaction

```text
Supervisor -> coordinator: AllocateRun(invocation identity, preferred root)
Coordinator: durably reserve ID/path/invocation in protected PREPARING state
Coordinator: exclusively create runs/ID; persist initial identity and namespace
Coordinator -> supervisor: version-1 launch descriptor with immutable run ID
Supervisor: prepare process-group tracking and pass environment to child
Supervisor -> coordinator: bind child identity; durably mark ACTIVE (launch permitted)
Supervisor: release child to exec/load the application
Vendor/WPILib initialization: register; obtain preferred path; open own files
WPILib DS observer -> coordinator: time/match observations, asynchronously
Coordinator: update run.json and human-readable alias
Supervisor -> coordinator: application exited/crashed; begin DRAINING
Supervisor: automatic restart gets a fresh run, independently of old-run recovery
Coordinator: reconcile old writers and surviving files; seal the old run
Retention worker: eventually reclaim eligible sealed run
```

The process cannot enter application code before it is covered by the protected
invocation record. The exact supervisor handshake may differ, but this ordering
must hold even if cleanup is running concurrently. `PREPARING` is protected
before producers can access the directory. Persist the allocation intent before
creating its namespace, and acknowledge allocation only after the minimal
identity and directory entries are durable under the platform's storage
contract. The coordinator owns creation in the managed run root, so it can
reconcile an interrupted reservation without guessing ownership from a
timestamp. Never adopt a conflicting preexisting directory. A durable
launch-permitted record means code may have run even if the later startup
acknowledgement is absent. Classify a failed launch only when the supervisor
proves no process was released.

The service is available before normal robot launches. Allocation has a bounded
startup timeout chosen by the OS team. If it cannot create a valid context,
launch with `WPILIB_LOG_MODE=disabled` and a visible diagnostic instead of
blocking robot operation indefinitely. A registered producer does not need the
service for individual file opens, writes, or a previously authorized
producer-local transfer. First registration and publication of new destinations
require the service. A service crash leaves established paths usable; producers
retain pending location reports, and reclamation is suspended until recovery.

## Desktop simulation and embedding

### Simulation and direct launches

Run the C++ executable, JVM, or Python entry point normally. The desktop client
automatically hosts the coordinator library in that process. No launcher
handshake, socket listener, separate process, or administrator access is needed.
Gradle, IDE, and RobotPy tooling package the shared runtime and may configure
the log root; they are not required to allocate the run. A process that never
uses the run API need not create an empty run directory.

Use one root-selection rule across native vendors and language bindings. Java's
current [launch-directory helper][Java Filesystem] adjusts `user.dir` for a JNI
build directory; this is not identical to native working-directory selection.
Tooling that needs a project-root override must supply it through the shared
early configuration, before a native producer initializes context. A later Java
or Python adapter cannot relocate an already active run.

The first `GetRunContext` or `RegisterProducer` call creates the default context
exactly once; other APIs needing a run resolve that same context. Concurrent
first callers wait for that same bounded initialization result. Persist the
allocation intent before exclusive directory creation, then record durable
identity/ownership and namespace publication before returning a usable context.
Only then create producer namespaces and allow writes. This boundary precedes
producer file creation, rather than application loading. An early crash before
any client use may therefore have no run directory. Allocation time is not
necessarily process start time; record an independently known invocation start
time or leave it unknown. On initialization failure, report a shared
disabled/error state and allow application execution to continue; do not let
each producer allocate a fallback run.

All native vendors, WPILib, JNI bindings, and Python extensions in the process
must resolve the same shared runtime and default context. No binding keeps its
own allocator or privately linked coordinator. Initialization cannot depend on
HAL, DS startup, DataLogManager, or another producer's constructor order.
Qualify supported pre-main entry points, including native library loading
restrictions; do not start logging or worker threads from an unsupported loader
callback. Later joins, logger restarts, and labels use the existing context.
Producer operations call the local coordinator directly; metadata and storage
work run on bounded background queues, outside the simulation periodic loop.

Every newly launched process gets a fresh default run. Concurrent simulations
can use the same root because allocation is exclusive and labels contain full
IDs. The default context covers producers in that process, not unrelated tools
or child applications. A custom host that groups helpers must explicitly bind
their identities and track their lifetime; ordinary child launches are separate
runs. Do not propagate stale managed descriptors from an IDE or shell into an
independent simulation invocation.

The coordinator dies with its host process. Closing it after all producers stop
can finalize metadata, but crashes and power loss require no close callback. A
later library initialization or explicit catalog/recovery operation can
reconcile known old runs in the background after proving their owners inactive.
It must preserve concurrently running simulations and unresolved ownership.
Starting the new run does not wait for this scan. Automatic desktop retention is
off, and no cleanup or recovery continues while no host is running.

### Reusable simulation coordinator library

Expose the embedded coordinator as a supported library for general simulation
hosts, custom frameworks, replay tools, and vendor/WPILib integration. Ordinary
simulation uses its default configuration automatically; custom embedding is not
restricted to tests and does not require a daemon process.

Keep host configuration separate from producer APIs. Before the default context
is first used, a host may supply a log root, metadata/clock sources, storage
providers, and lifecycle policy. Reject attempts to replace that configuration
or rebind the default run after initialization. Hosts with pre-main loggers need
a supported earlier configuration hook or launch-time configuration values;
calling a setter in `main` is too late once such a logger has joined. Language
bindings must use the same configuration and context as native producers.

A robot-like configuration can supply internal/removable roots, announce mount
and removal events, publish destination preferences, track explicitly attached
writers, and enable retention for designated managed roots. Apply the same
independent transfer, liveness, recovery, copy-evidence, and offline USB rules
as SystemCore. Use shared coordinator policy code where practical; OS
supervision and physical mount detection remain platform integrations. Retention
is opt-in and scoped to the host's managed catalog, never the user's general log
folders.

Also provide controllable clocks, storage events, and fault-injection hooks for
testing. A harness can model an application exiting while its embedded
coordinator stays alive, but killing the host kills both. Actual
service-restart, process-supervision, and power-loss qualification still require
SystemCore tests. Any isolated contexts for multi-robot hosts or tests must be
explicitly passed to participating producers; they must not reset or replace a
live default run. The configuration ABI, helper attachment, and isolated-context
support remain interface decisions below, not prerequisites for ordinary direct
execution.

## Naming and metadata publication

The [naming and metadata contract][] specifies label progression, clocks,
observations, and interchange fields. The coordinator is the only publisher of
`run.json` and display aliases. Implement publication as follows.

### Observation ingestion and label evaluation

WPILib publishes observations from its DS/framework integration even when
DataLogManager is never enabled or is stopped. Java and C++ call the standalone
client's observation API through JNI/C++ bindings; RobotPy uses the C++ binding.
Do not let language-specific managers or vendors independently choose run names.
Frameworks that do not use this DS path may register one equivalent publisher.

Observation-hook placement remains an implementation decision. The existing
[C++ DS refresh path][] and [Java DS refresh path][] provide integration points
to evaluate. Capture a consistent refreshed snapshot, then enqueue outside DS
locks. The standalone client accepts these language-neutral observations; Java
need not link to the wpilibc implementation. The coordinator applies the shared
label policy rather than duplicating it in each DS backend.

Only the coordinator writes `run.json` and display aliases. Updates carry the
run ID and an observation sequence; it rejects wrong-run, unauthorized, or
out-of-order updates. Send state changes and periodic clock samples through a
bounded asynchronous queue, outside the robot periodic loop. If disconnected,
retain bounded metadata updates and resubmit them when possible. Report overflow
as incomplete metadata; logging data itself must not wait on the coordinator.

Use monotonic elapsed-time checks for the DS/FMS attachment intervals in the
shared label policy rather than assuming a 50 Hz update rate. Reset pending
observations on detach. Map event labels to ASCII letters, digits, `_`, and `-`,
collapse substitutions, cap the component at 48 bytes, and replace an empty
result with `event`. Preserve the raw event text in metadata. Bound the total
path length, including the `.runref` suffix, before publication.

### Alias publication and crash consistency

For a label change, publish the new alias to `runs/ID`, then remove the previous
verified alias. Create a symlink exclusively. For a `.runref`, write and flush a
complete sibling temporary file, then publish it using a same-filesystem rename
under the coordinator's alias lock, without overwriting an unexpected existing
entry. Do not write into the visible reference file in place. Ignore unpublished
temporary files during discovery. A brief period with two aliases is acceptable.
Do not remove the old alias until the new one exists, and never replace an alias
belonging to a different run. If a crash leaves both aliases, reconcile them
against the metadata later.

Reference readers enforce the `.runref` grammar and size limit in the shared
storage contract, verify that the target is an actual canonical run directory,
and check its metadata against the referenced ID. Do not follow reference chains
or accept a display name in place of the canonical target.

Choose the symlink or text representation according to the root's capabilities.
If changing representation, publish and verify the new form before removing the
old one. Tools may encounter both forms and deduplicate by run ID. If alias
publication fails, keep the previous valid alias and report the failure;
canonical writes continue. Text references do not bypass a read-only or full
filesystem. Apply the same platform replacement and durability requirements
below to text-reference publication.

Write each metadata snapshot to a sibling temporary file, flush it as supported,
and replace `run.json` atomically. On POSIX storage, sync the relevant directory
entries when durable publication is needed. Use the platform's supported atomic
replacement operation on Windows, with bounded retries for transient sharing
failures. Readers open/read/close snapshots promptly. Keep the previous valid
snapshot if replacement fails.

Metadata replacement and alias changes are not a single transaction. Commit
metadata first, then reconcile the alias; a crash can leave a stale label, never
a changed producer path. Power-loss durability depends on the filesystem and
device; atomic namespace replacement is not a promise that the latest buffered
log or metadata survives sudden power loss. The stable ID is the recovery key.

## Power loss and crash recovery

### Durability without a clean shutdown

An interrupted run is expected to lack final metadata, a clean-close marker, or
some buffered records. Preserve the surviving files and usable records. Neither
successful export nor eligibility for retention requires the application to have
shut down cleanly. `SEALED` proves that no producer can write more; it does not
certify a complete log or explain why the application stopped.

Keep the minimal recovery record independent of producer buffers and mutable
display metadata. The service registry needs durable allocation, boot/invocation
identity, known volume locations, and deletion/copy-cleanup decisions. Use a
storage engine or journal/checkpoint scheme that recovers complete records after
an interrupted write, identifies a torn final update, and never interprets
partial bytes as permission to delete. Routine recovery of an interrupted update
must be automatic; ambiguous corruption remains protected. Reserve enough space
for these records even when log storage is near its limit.

| Data or operation | Required persistence |
| ----------------- | -------------------- |
| Allocation and permission to write | Persist the minimal identity, ownership, namespace, and invocation binding before releasing application code on SystemCore or returning a usable embedded context on desktop. Recover interrupted reservations without requiring a producer to register first. |
| Additional run volume | Persist the volume identity and run location before announcing it to writers. A later producer status report may lag without making the new files undiscoverable after reboot. |
| Copy cleanup and retention | Persist validated intent before destructive work. A copied destination and its namespace must be durable before a source can be removed; record progress so interrupted cleanup is repeatable. |
| Source location retirement | Persist the per-run location's retirement intent outside that directory before removing it. Keep the destination's run metadata and data durable, reject further source placement, and recover interrupted removal without recreating the location or sealing the still-active run. |
| Display labels, DS observations, and progress snapshots | Coalesce updates asynchronously. Losing the latest update may leave stale or incomplete metadata; it cannot change run identity or authorize deletion. |
| Producer data | Define and qualify a periodic background flush/sync policy, including file creation/rotation. Report errors and the intended unsynced-data window; a final shutdown flush is only an optimization. |

Distinguish draining a library buffer, writing to the kernel, and completing a
storage sync. Check sync errors. On Linux, syncing a file does not by itself
persist the directory entry naming it; namespace publication needs the
applicable directory syncs too. See [Linux fsync][]. Define equivalent supported
behavior for each platform/filesystem, including USB media. Do not claim durable
source replacement on a destination whose persistence behavior has not been
qualified; keep the source copy in that case. Devices can still fail, and
records buffered since the last completed persistence point may be lost.

WPILib's current [native background writer][] periodically attempts `fdatasync`
on Linux and `fsync` on macOS after draining data, but does not check those
return values. Its `Flush()` schedules work and returns; it is not a completion
receipt. [Java calls it through JNI][]. The implementation must audit
completion/error reporting and new-file directory persistence before treating
these paths as durability evidence. Do not put synchronous storage waits in
robot periodic execution or require one producer to sync because another is
moving.

### Automatic recovery and restart

On SystemCore, recover from trusted registry records and actual supervisor state
without requiring the next application to repair its predecessor. Do not rely on
destructors, signal handlers, `atexit`, or footer records. Recovery itself must
tolerate another power cut. In simulation, no coordinator survives host
termination: subsequent embedded-library initialization or an explicit recovery
operation reconciles known runs. First distinguish still-active hosts from
abandoned invocations using process-start/boot identity and platform liveness
checks, not a PID or stale marker alone. Desktop retention remains off by
default.

The following sequence describes SystemCore recovery; the embedded host applies
the same evidence and storage rules to its managed catalog, without promising
recovery while no host is running:

1. Replay valid registry state and protect unresolved run locations before
   enabling deletion. Bound the work on the launch path; scan old directories,
   verify copies, and rebuild labels in the background. New allocation can
   proceed once its own identity and protection are established.
2. On an application crash in the same boot, use the supervisor's invocation and
   process-group state to reconcile the old run. A dead producer cannot send a
   final acknowledgement. Account for surviving helpers, external writers, and
   copy workers; no response timeout alone proves they are gone. Launch the
   replacement with a new ID independently of this reconciliation.
3. After reboot, the old local boot's processes are gone. Recover known old runs
   as interrupted, including records still marked `PREPARING`, `ACTIVE`, or
   `DRAINING`, without awaiting old clients or replaying their logging session.
   Reconcile any nonlocal writer separately if such writers are supported. An
   interrupted launch with uncertain execution remains explicitly uncertain.
4. Seal once no writer or copier can mutate the recovered run. An unavailable
   USB volume may leave placement/inventory incomplete, but does not make old
   local processes live again. Defer deletion/export-completeness claims for
   missing unique data. Reconcile that run when the same volume returns; never
   attach its files to the newly running executable. Apply the
   [offline USB reconciliation rules](#offline-usb-file-management) before
   resuming old copy or cleanup work.
5. Repair metadata and aliases from trusted identity/history, enumerate
   surviving files, and reconcile incomplete copies and cleanup. Keep uncertain
   or damaged files for diagnosis rather than truncating or rewriting them in
   place. Resume only previously authorized deletion work, checking volume
   identity and current file/copy evidence again. An old receipt does not prove
   its destination still exists after the volume was used elsewhere.

Interrupted runs enter the ordinary retention policy after recovery; repeated
power cycles must not leave them permanently protected merely for lacking clean
closure. Rapid crash/restart loops must also be sustainable: retain useful
recent failure history within a bounded policy and keep allocating distinct IDs.
Set recovery, startup, sync, and crash-history budgets during qualification.
Large backlogs or a corrupt old run must not force a full-store scan before
robot code can start. If minimal allocation itself cannot complete within its
budget, use the existing explicit disabled-logging startup policy.

Producer formats and readers must satisfy the separate
[usable-log requirements][]. Recovery must preserve their original bytes even
when metadata, a tail record, or a final footer is incomplete.

## Lifecycle and retention

### Proving a run is inactive

```text
PREPARING -> ACTIVE -> DRAINING -> SEALED -> DELETING -> removed
     |                    ^
     +-- failed launch ---+       # only after proving no live writers
```

`SEALED` means producers cannot add or modify log content. The service may copy
that immutable content for export or consolidation under the protections below.
The outcome distinguishes normal exit, crash, failed launch, and power-loss
recovery; it does not claim that a crash-produced log has a complete footer.
Stopping one logger is not a request to seal the run. A quiet, disabled,
DS-disconnected, or unrenamed run can still be active.

On SystemCore, the supervisor's invocation/process-group identity is
authoritative. On Linux, track all processes in the assigned cgroup and the
boot/invocation identity, not just the main PID; main-process exit does not
prove children have stopped. For systemd-based integration, align service
lifetime and shutdown with that tracking rather than assuming its default
main-PID lifetime covers writers. See [systemd service lifetime][]. The default
embedded desktop context tracks its own process invocation. A custom host that
includes helpers must supply equally strong lifetime tracking; ending one
simulated robot or losing a connection alone does not prove its external writers
have exited.

External writers outside the group must register before writing and remain
protected until they acknowledge final close or are positively known to have
exited. Registration makes them subject to supervisor shutdown policy. Losing an
IPC connection or heartbeat is not proof they have stopped writing. Reopening a
sealed run requires an explicit administrative workflow; normal producers cannot
join it. Shutdown drains or terminates the old participants before sealing; a
timeout alone never makes an unaccounted writer safe to delete.

Before sealing, persist the best-known outcome and surviving metadata,
explicitly marking missing termination or observation details. Reconcile
producer-local transfers and background copiers to completed, stopped, or
positively dead; never await a final message from a dead process. Account for
surviving files on every known volume, including an unavailable-volume status
where necessary. Sealing requires neither clean shutdown nor convergence onto
USB. A shutdown request cannot delete files out from under an unfinished
transfer. After sealing, late observations cannot mutate the snapshot used for a
verified export. Administrative annotations need a separate versioned mechanism
or an explicit reseal operation serialized with reader holds and deletion. Any
later service consolidation preserves the sealed content and its inventory;
record placement changes separately and serialize source removal with reader
holds and retention. It must not reopen the run to producers.

On SystemCore, keep authoritative allocation, lifecycle, and protection records
in a service-owned registry outside producer-writable log trees, for example
under `/var/lib/wpilib-logd`, with runtime IPC under `/run/wpilib-logd`. On
removable media, bind records to the volume identity as well as run ID.
`run.json`, filenames, mtime, stale marker files, and PID numbers by themselves
do not authorize deletion. Run directory permissions may let robot code modify
metadata; it is therefore an interchange snapshot, not a privilege boundary.

The embedded library keeps its registry in user-writable host storage outside
producer namespaces. Hosts sharing a catalog must serialize registry updates and
preserve each other's live runs. This supplies recovery evidence, not a
privilege boundary against code in the same user/process. Explicitly enabled
desktop retention must satisfy the same ownership and liveness checks; otherwise
leave uncertain runs protected. Registry location and host-locking details are
part of the embedding interface decisions.

Apply [automatic recovery](#automatic-recovery-and-restart) after service
restart or reboot. Normal interrupted registry updates and unclosed runs recover
without operator action. Lost ownership evidence, corruption beyond the
supported recovery boundary, or an unfamiliar imported directory requires
conservative recovery or explicit adoption, not an inference from age.
Unregistered directories remain untouched; uncertain old data does not block a
fresh run.

### Device-wide retention policy

The SystemCore service owns retention for all registered log volumes and all
participating formats. It runs periodically, before allocation if necessary, and
on low-space notification. It uses low/high watermarks to avoid repeatedly
deleting one small file at a time. Configuration includes:

| Setting | Behavior |
| ------- | -------- |
| Low/high available-space watermarks | Start reclaiming below LOW; stop only above HIGH, with `HIGH > LOW`. |
| Maximum log bytes and maximum age | Optional additional triggers for sealed runs. Count canonical runs once, not every alias. |
| Minimum recent-run count/age | Soft retention preferences during ordinary cleanup; may be overridden at a documented critical-space threshold. |
| Recent interrupted-run history | Retain useful failure diagnostics within a bounded count/age budget. Ordinary hard power-offs do not permanently pin every run; crash loops remain subject to retention. |
| Pinned runs and export holds | Hard exclusions from automatic reclamation. Surface how much storage they protect. |
| Filesystem reserve | Leave space for robot operation, durable registry updates, and new run directories, including non-log users of that filesystem. Account for file/inode exhaustion as well as bytes. |

Do not copy the current 200 MB threshold as the final shared budget. Size the
watermark gap using measured aggregate producer rate, service scan/reclaim
latency, and burst allowance: at minimum, the gap exceeds expected bytes written
during reclamation. Device capacities and USB media differ. Exact default bytes,
fractions, and age/count policies require SystemCore measurements and agreement
before enabling the service by default.

Reclamation procedure:

1. Measure actual filesystem-available space and log usage on each volume.
2. Select only registered `SEALED` runs without pins or export holds. Order by
   trusted allocation/completion order; use a persistent supervisor sequence
   when wall time is invalid or changes. Do not sort solely by the display name.
   Exclude active runs on every volume, including transfer staging files.
   Removing a verified redundant source file after transfer follows the rules
   below; it is not ordinary run-age retention. Defer automatic deletion if a
   required run location is unavailable or placement is unresolved; retired
   locations are already resolved and do not block retention. Confirmed absence
   after a successful scan of the recognized volume is resolved placement, not a
   permanent retention hold; apply normal policy to remaining managed files.
3. Under the coordinator's lifecycle lock, recheck eligibility and durably
   transition the run to `DELETING` before destructive work. Reject new producer
   joins and new export holds for it.
4. Remove verified aliases and quarantine each remaining volume's run directory
   to `ROOT/.trash/ID` on that same filesystem. Record progress per volume
   before mutation and persist the resulting namespace changes so recovery can
   find directories at either location. Whole-run retention is a logical policy,
   not an atomic cross-volume filesystem operation.
5. Recursively remove that tree without following symlinks, crossing mount
   boundaries, or resolving producer-provided paths outside the registered root.
   Resume partially completed trash deletion after a crash.
6. Remeasure available space after deletion. File sizes alone do not establish
   reclaimed capacity; open handles and filesystem behavior can delay release.
   Continue until all active policy triggers are satisfied or no candidate
   remains.

Directory-relative traversal, root/volume validation, and no-follow behavior are
needed even for a trusted robot ecosystem: malformed producer paths must not
turn a cleanup bug into deletion outside the run. Protect the registry and IPC
with OS permissions. The service does not need root privileges for log data if
ownership/groups and supervisor integration provide the required access.

If only active or protected runs remain, stop reclaiming and report storage
pressure. Never delete the current run's oldest segment just to meet a target.
Producers must handle `ENOSPC`, quota failures, unavailable media, and write
errors using bounded buffers and visible loss/pause status. A separate service
cannot guarantee that storage will always be available. A single long-running
run may fill its volume; v1 accepts logging pause/failure in that case, with
explicit whole-run quotas or multi-part-run eviction deferred to a later design.

Keep the existing native writer's emergency free-space protection initially.
Audit its failure, resume, and segment behavior separately; the current code's
blocked state is not a documented automatic recovery protocol. Define and test
how each producer retries or resumes after space returns, without promising that
moving retention to a service fixes these writer behaviors automatically.

## Late-mounted USB transfer

Implement the [independent USB transfer contract][] using the mechanisms below.
Producer-local I/O and receipt creation follow the
[writer transfer requirements][]; the server never treats a changing file as
immutable or requires producers to move together. Preserve each producer's
actual location independently of the preferred destination.

### Destination discovery and independent adoption

The SystemCore coordinator watches approved USB mount events and also checks
periodically while internal storage is preferred. Use five seconds as the
initial polling interval, following #9570; a mount event can trigger the check
sooner. Verify mount identity, write capability, and available capacity.
Explicit user-selected destinations are not automatically moved. Internal-to-USB
adoption is required; arbitrary rebalancing and automatic reverse failover are
not.

1. **Publish a destination.** Exclusively create and register
   `USB_ROOT/runs/ID`, with metadata and a display alias or `.runref`. If it
   already exists, validate that it is this run on the expected volume; never
   adopt or overwrite an unknown directory merely because its name matches.
   Persist both locations and the new destination's identity/namespace before
   publishing a monotonically increasing destination revision. Keep internal
   storage valid. This records a discoverable destination, not a coordinated
   cutover by the producers.
2. **Notify without waiting.** Push an advisory destination change to registered
   producers; queries/polling return the same latest preference. Late producers
   can start on USB immediately. Missing a notification is harmless: querying
   again returns the current preference. No other producer must acknowledge
   first.
3. **Move independently.** Each producer decides when and how its writer can
   adopt the destination, within its size/time/buffering limits. It may transfer
   its current file, start a new USB segment and copy closed files later, or
   defer the move. Serialization is local to the affected writer/files. Changing
   one producer's directory does not change another's paths or invalidate the
   source.
4. **Report actual placement.** Report active write locations, remaining source
   files, pending copies, and failures. A successful switch updates only that
   writer's actual path. Destination preference, writing on USB, and all history
   present on USB are distinct statuses. Stopped/paused writers keep their state
   and use their own successful location or explicitly adopt the preference on
   resume; a preference notification must not start logging.
5. **Notify completion.** Once both ongoing output and all source history have
   moved, call `ReportProducerTransferComplete`. The library forwards the
   notification to the service or embedded coordinator, which reevaluates source
   cleanup. A successful writer switch alone is not this notification.

Registration and initial selection can race with preference publication safely:
starting at an earlier approved location is allowed, and it remains protected. A
placement-only compatibility writer may stay internal for the whole execution
without preventing other producers from moving. Track its source location and
lifetime even when it cannot consume destination-change notifications.

### Producer-local transfer and historical files

Use writer I/O threads; robot periodic threads do not wait for copying. A writer
that transfers an open file must serialize its own writes with the move, flush
accepted records, copy a consistent byte stream, make the destination durable as
supported, and switch its handles/cached paths only after success. It retains
source data on failure. #9570 provides this pattern for its current file. Other
writers may instead close a source segment and open a new uniquely named segment
on USB, preserving required headers/schema records and record order. A generic
service must not copy an actively changing file as if it were complete.

No producer must flush or pause because a different producer is moving. Bound
each writer's buffering and report any lost records. #9570's 10,000,000-byte
limit is a reference for one current file, not a whole-run cap. A large file or
a producer that cannot reopen may remain internal. Apply device-wide I/O
throttling and reserve checks to limit contention, without making progress
contingent on all producers fitting one transfer budget.

Previously closed segments and auxiliary files also belong to the run. Their
owner may copy them asynchronously, or declare specific files permanently closed
so a shared background copier can do so. A stopped logger is not sufficient
proof that its files will never reopen. Copy only immutable files or a
writer-controlled snapshot; reject unsupported entries rather than following
interior symlinks or silently omitting data. After sealing, the service may
finish copying remaining history subject to policy, but this is not a
prerequisite for active writers to use USB or for a run to be sealed. Offline
removal takes precedence over automatic consolidation: do not recreate missing
old-run files when a stick returns from use elsewhere.

Preserve producer namespaces and use unique file/segment names across volumes
and restarts. Publish completed copies without overwriting unrelated files;
staging files are not exportable logs. Record a copy receipt identifying the
source file incarnation, byte count/checksum, destination, and successful
durable copy. Sync the copied data and the namespace through which it will be
found before making that claim; persist the receipt before source cleanup. For a
current file that continues growing on USB, the receipt describes the preserved
prefix, not a permanent whole-file checksum. Duplicate filenames or an uncertain
retry require verification, never blind overwrite or truncation.

### Producer completion and cleanup trigger

`ReportProducerTransferComplete` is a producer-to-coordinator notification
through the C library. A producer sends it after its independent move has
finished, without waiting for other producers. Completion covers one
registration incarnation and source/destination volume pair within the run. Its
payload identifies the run, registration, volume identities, destination
revision, final producer-local status sequence, and final source inventory with
copy receipts or references to receipts already reported. Exact C struct layout
and exported symbol spelling remain API-review details.

Before sending completion, the producer must:

- Finish its own source writes and transfers, close source write and directory
  handles, and direct future output, including rotation and paused-writer
  resume, to USB.
- Account for every source file it owns: current output, previously closed
  segments, and auxiliary files. Publish durable copy evidence for each file;
  for an ongoing USB file, the receipt may cover the complete preserved source
  prefix. Pending, failed, or delegated copies keep completion pending.
- Relinquish source writes for that registration. It cannot later reuse cached
  source paths, even if USB disappears. Any future source use requires a new
  registration and coordinator-approved placement before writing, and is
  rejected once location retirement begins. Other registrations may continue
  using the source independently until they also release it.

A producer that starts directly on USB has no source-copy work for that
registration. Record its placement accordingly, but do not let this erase
history from an earlier registration of the same producer. A shared copier can
satisfy explicitly delegated closed-file obligations with the same copy
evidence; the producer's notification remains pending until they are satisfied.
If the producer has exited, recovery can close its obligations from positive
lifetime and file evidence without waiting for its notification.

The client queues delivery outside robot periodic execution and retries after
disconnection. Expose pending, accepted, and rejected notification status;
enqueueing locally is not an acknowledgement. Queue exhaustion is reported and
leaves completion unconfirmed until retried. The coordinator validates and
durably records the completion and its required inventory/receipts before
acknowledging it. Retrying the same registration/sequence is idempotent. Reject
conflicting payloads and stale registration updates; a completion for an older
destination can describe historical copies but cannot establish completion to a
different destination. A lost acknowledgement delays cleanup reporting, never
logging or an already completed writer switch.

Track completion across all producer instances that used or can still use the
source, including console capture and helpers. Include unregistered producers'
historical files and delegated copies. Missing reports, unknown files,
unsupported transfers, and unfinished registration/placement decisions remain
outstanding; a connection loss or `UnregisterProducer` does not imply copied
history. A simple count of currently connected producers is insufficient.

When all relevant obligations are satisfied, schedule a cleanup pass for the
redundant source files. Serialize the eligibility check with producer
registration, placement, reader holds, and cleanup authorization. Recheck file
incarnations, durable destination copies, and holds before removing each source;
never recursively delete an active source tree based on an all-complete flag.
New or restarted producers update the aggregate status before receiving source
write approval, and their files are outside previously authorized cleanup sets.
Missing or changed USB media invalidates current cleanup eligibility until copy
evidence is revalidated. Completion records remain historical evidence, not a
permanent claim that the destination still exists.

Report per-producer completion and aggregate completion separately from cleanup
pending, blocked, or finished. Reevaluate on accepted completion, registration
or placement changes, copier/recovery progress, hold release, and volume return;
a hold that outlives the last notification must not leave cleanup forgotten.
Existing verified per-file cleanup may proceed before every producer completes;
the aggregate notification path also enables a final pass over remaining
redundant files. When no source users or holds remain, retire and remove the
source run location as described below without waiting for run sealing. Neither
cleanup nor acknowledgement is a barrier for producers to switch or continue
writing on USB.

### Tracking, source cleanup, and export

The registry tracks all run volumes, producer write locations, and completed or
pending file moves, along with producer completion records and outstanding
cleanup work. Metadata snapshots on each volume carry the same run ID and known
peer-volume identities. `.runref` remains a volume-local `runs/ID` reference; it
does not redirect across volumes. A source directory can contain active,
not-yet-copied, and redundant files at the same time; never label the entire
directory obsolete merely because some producers have switched.

Removing a verified redundant source file is transfer housekeeping, not
retention. In managed mode, retain the source until the coordinator can
authorize/remove it: the writer has relinquished that file, its durable copy is
verified on the expected volume, and no reader hold protects it. A writer using
a #9570-style operation therefore needs a retain-source option or equivalent
cleanup integration. This does not delay its switch to USB or require other
writers to participate. If reporting or cleanup is unavailable, keep the
redundant source and retry later.

After reboot or a removable volume's return, revalidate the destination's actual
file identity/content before using a historical copy receipt for source cleanup.
Check the bytes covered by the receipt, including a preserved prefix where the
destination subsequently grew. An absent destination or failed revalidation
invalidates that cleanup proof. The surviving source then remains subject to
ordinary run retention, not immediate deletion as a redundant copy.

Power loss between copying and source cleanup may leave two files; recover that
automatically from the receipt and actual bytes. Power loss before a receipt is
durable leaves the source protected. A missing receipt does not justify deleting
either file. If file identity/content can be reverified after writers are gone,
the service can rebuild the receipt; otherwise preserve the ambiguity. These are
per-file rules and never require a common producer commit point.

Keep the source run directory and aliases while any producer retains source
placement approval, unique history is unresolved, or a reader hold protects the
location. Once those obligations are resolved, the coordinator may retire the
location and remove its directory while the logical run remains active on USB.
This is redundant-copy cleanup, not whole-run retention. Ordinary retention
still acts on the sealed logical run across its remaining locations.

Run-aware tools union the known volume locations by run ID. Deduplicate only
verified copies; a growing destination may supersede its verified source prefix.
Unresolved duplicates are retained/reported, not silently discarded. A full
export requires all locations containing unique data to be available; if USB
alone is incomplete, say so. Live exports remain best-effort snapshots and
cannot trigger source deletion. Reader holds prevent transfer cleanup from
removing held files, while allowing producers to switch and continue writing
independently.

### Retiring the internal run location

Retirement applies to one `(run ID, volume ID, location)`; it does not retire
the storage device or change the run's lifecycle state. The SystemCore service
and configured embedded coordinator use the same procedure. Store retirement
state in the coordinator registry outside the directory being removed, including
on desktop. No new producer-wide handshake is required: each producer's accepted
completion already relinquishes its source approval.

1. **Check eligibility and stop new source use.** Under the coordinator's
   placement/lifecycle synchronization, verify that every source approval has
   been released, all producer instances and historical/delegated copies are
   accounted for, all source data has verified durable USB coverage, and no
   reader hold protects the source. Include approvals returned before a
   producer's first file open, paused producers, helpers, and console capture.
   Persist `RETIRING` for the location before destructive work. From that point,
   reject new source approvals and source holds; new registrations and exports
   use USB. A racing source approval or hold that wins first defers retirement.
2. **Preserve coordinator-owned state.** Persist current run metadata,
   inventories, and copy evidence on USB and in the registry as appropriate. The
   USB snapshot identifies internal storage as retiring and containing no unique
   history required for export. Stop and drain source metadata/alias publication
   and copy jobs before removing their files; subsequent DS updates publish only
   to remaining locations. Stale queued work must consult the retirement record
   and must not recreate the source.
3. **Remove only accounted-for contents.** Apply the existing per-file copy
   checks and source cleanup rules, then remove verified source aliases,
   coordinator-owned metadata, and empty producer/run directories without
   following symlinks. Do not recursively discard unexpected files. If USB
   becomes unavailable, verification fails, or an unknown entry appears, keep
   the remaining source contents, report retirement blocked, and retry only
   after the evidence is resolved. No error reauthorizes source writes.
4. **Record completion.** Persist the removed namespace entries and mark the
   location `RETIRED`. Retain the registry record and publish the resulting
   placement on USB. The catalog distinguishes deliberate retirement from
   missing data: historical volume membership remains available, but a retired
   location is not a required peer for export or retention. The run continues on
   USB with the same ID and active producer registrations.

Replay `RETIRING` and `RETIRED` before accepting new placement requests after a
coordinator restart. Resume interrupted removal using current copy evidence and
the recorded progress; if USB is missing or was changed offline, preserve any
remaining source data until its disposition is resolved. A missing source
directory with a valid retirement record is expected, not a reason to repair its
metadata or aliases. Missing/corrupt retirement evidence requires conservative
reconciliation. An application restart still gets a fresh run and can select
internal storage independently of the old run's retired location.

Late or restarted producers obtain the remaining USB location through the C
library. The original bootstrap directory is only a hint; neither it nor a
cached path grants reuse of the retired location. No per-record service call is
needed: approvals remain valid until relinquished, and retirement waits for all
source approvals to be released. If USB disappears after removal, use the
existing media-loss behavior rather than recreating the internal directory.
Automatic reverse failover remains a separate design decision.

### Failure, recovery, and USB removal

A destination-full error, size limit, timeout, or unsupported producer affects
only that writer's attempt. It keeps its established source when the switch has
not succeeded; another writer already on USB continues there. A segment-based
switch may leave valid closed history on both volumes even if later copying
fails. Retry according to producer policy and report the remaining placement.
There is no attempt to restore a single run-wide location after partial success.

After a coordinator restart, reconcile producer reports, known volumes, and copy
receipts and completion records. Existing writers continue on their actual paths
and resend pending reports/notifications; they do not wait for a global transfer
decision. If a producer crashes mid-copy or reports are lost, protect all
surviving files and reconcile them before deleting any source or staging file.
Uncertainty delays reclamation, not unrelated logging. A coordinator outage
prevents new destination publication and first registrations, but need not stop
a local move to an already approved volume; preserve source copies until cleanup
can be reconciled.

After an application crash or power cycle, this reconciliation belongs to the
old run. Its restarted executable always writes a new run, even if the old copy
was nearly complete. Recover the old files without reopening them for new log
records or waiting for the failed producer to acknowledge its move.

If USB disappears, producers already using it report logging unavailable and
apply their bounded-buffer/error policy. Producers still internal continue. Do
not recreate run directories on the underlying mountpoint. The OS must fence new
opens during mount replacement and make the exposed mountpoint unavailable or
non-writable when media is absent. Producers must not recursively reconstruct
missing run ancestors; validate run and volume identity before reopening. Resume
USB logging only after the same medium is recognized and the run is still
active. Automatic reverse failover is a separate open trade; it must preserve
all prior fragments rather than treating the old internal copy as complete.

### Offline USB file management

Turning the robot off, removing the stick, moving files on another computer,
reinserting it, and powering the robot on is a supported ordinary workflow. It
requires neither a graceful robot shutdown nor a special export receipt. Copy or
move the actual run directories/files; display symlinks and `.runref` files only
reference those directories. Old metadata may still say `ACTIVE`; copying
surviving files does not require the robot to seal the run first.

For a previous run `R1`, the next boot allocates a fresh run `R2`, using USB if
it is available. `R2` can start without waiting for `R1`'s inventory scan. The
service seals `R1` from old-boot lifetime evidence and reconciles its remaining
locations in the background. If USB mounts late, only the active run follows the
normal independent producer-adoption policy.

Treat every reboot or mount return as an opportunity for offline changes, even
if the robot did not observe the unplug. Before restarting old-run copy,
consolidation, or cleanup jobs on that volume, identify the medium and inspect
the relevant run locations. A matching mount path alone is insufficient. Do not
require scanning unrelated files elsewhere on the stick.

| Observed state | Reconciliation |
| -------------- | -------------- |
| Recognized, readable volume; expected files still present | Preserve them and reconcile metadata. Revalidate any copy evidence needed for destructive work. |
| Recognized, readable volume; expected run directory or files absent | Record confirmed absence after successful enumeration of the relevant parent/run namespace. Cancel automatic recreation at those locations; absence is not proof of export or deliberate deletion. |
| Files renamed, replaced, or modified while offline | Preserve what is present, invalidate stale identity/checksum receipts, and report changes. Do not overwrite them with an old copy or move them back automatically. |
| Volume unavailable, wrong identity, unreadable, or enumeration incomplete | Keep affected placement unresolved and defer dependent cleanup. A mount/read/permission error is not evidence that a file was removed. |

Persist confirmed absence and canceled work in the service registry, scoped to
the volume, run, and affected file incarnation/path, or to the whole missing run
directory. This record must survive another restart so replayed jobs cannot
resurrect removed files. A missing old directory must not be reconstructed just
to repair its `run.json` or aliases. Remove only verified stale service-owned
aliases; repair metadata/aliases for surviving known directories as appropriate.
Persist cancellation before resuming old-run copy/cleanup work there. Do not
expire an absence record while a queued job or retained source can regenerate
the removed output; compact it only with corresponding work/retention state.

**Do not refill the stick with removed old logs.** Old transfer retries and
automatic sealed-run consolidation must honor those absence records. An
interrupted copy that never completed can also be absent after an offline gap;
the service cannot infer the team's intent, so it preserves any internal source
and leaves that automatic retry canceled. Explicit restoration or export may
copy the remaining data again. A fresh run's destination and its producers are
unaffected by an older run's absence records.

**Do not use historical USB copies as current cleanup proof.** If the team moved
a destination off the stick, its old receipt cannot authorize deletion of an
internal source. Reverify surviving destinations before cleanup, preserving the
source when evidence is missing or changed. Missing files do not establish that
a download succeeded, so the catalog must not label them verified exports or use
their absence to authorize delete-after-download.

Confirmed absence does resolve placement for ordinary retention. Once the old
run is inactive and otherwise eligible, its remaining managed internal/USB files
follow normal whole-run retention; they are not pinned forever because another
fragment was removed offline. Export tools distinguish an inventory of all
surviving files from a complete original recording and report unavailable or
absent history. If nothing remains, retain only the bounded catalog/replay
suppression state needed to prevent stale work from recreating the old run.

The volume-identity mechanism must survive ordinary clearing or moving of the
log tree; it cannot depend solely on a run's metadata or a marker inside a log
directory the team may remove. Reformatting a stick or losing its identity is a
different case: do not infer absence on the old volume from an empty replacement
at the same mountpoint. Register new storage separately; retirement of an
unavailable old volume uses an explicit policy or administrative action.

## Producer and tool integration

### WPILib implementation

Implement the [WPILib integration requirements][] using the required C library
on every supported platform. The library API and distribution sections above
define its ABI, packaging, private transport, and embedded coordinator. WPILib
supplies the language adapters, observation hooks, and writer integration below.

#### Manager and writer changes

Both DataLogManager implementations always use the required logging library and
managed producer API. This applies to automatic startup, explicit `Start()`
calls, and stop/start cycles on SystemCore and desktop. There is no managed
versus legacy branch and no fallback to old naming, storage discovery, or
cleanup. A disabled context, missing SystemCore descriptor, registration
failure, or unavailable storage produces a diagnostic and disabled file output;
it does not select a standalone path or prevent robot control from continuing.
An established writer may continue at its approved location during a service
outage, as described above.

The managers must:

- Always use the shared `wpilib` producer directory and a stable base filename
  such as `WPILIB.wpilog`, retaining unique segment/restart handling.
- Stop scanning/deleting old logs and stop requesting per-file time/match
  renames. Update descriptions of the low-space alert to identify coordinator
  policy, including disabled desktop retention; report the selected volume and
  managed status.
- Register the writer and consume destination announcements. The coordinator
  owns USB detection/eligibility; the manager requests a writer-local move to
  that approved destination. Clear its internal-storage alert only after its
  writer actually uses USB, independently of other producers. Report any
  historical files left internally separately. Submit
  `ReportProducerTransferComplete` after the writer and all historical/auxiliary
  copies finish; retain pending notification status across coordinator outages.
- Preserve existing NetworkTables, message, console, and DS-disconnection
  pause/resume behavior unless separately changed and tested. These are producer
  policies, independent of run allocation and sealing.
- Preserve filename and flush-period customization within the assigned producer
  namespace. The existing `Start(dir, filename, period)` directory argument must
  not bypass managed placement. Configure a custom managed root through the
  library/coordinator; use `DataLogWriter` or `DataLogBackgroundWriter` for
  exact-path standalone output. Finalize deprecation/removal or validation of
  the directory argument during API review; do not silently reinterpret an old
  exact-directory argument as a run root.
- Make `GetLogDir()`/`getLogDir()` report the directory actually used by that
  manager, changing only when its destination is adopted, as in #9570. Do not
  turn either into the new cross-vendor context API. Document their existing
  initialization differences or change them in a separate API decision.
- Ensure stop/start of the manager remains in the same executable run and never
  truncates an earlier segment. Check Java and C++ separately.
- Treat process restart differently from logger stop/start: discover the new
  SystemCore descriptor or initialize the new desktop context before automatic
  logging and never resume the previous executable's files. Qualify periodic
  persistence, sync-error reporting, and interrupted-file reading on the shared
  native writer and both language APIs.

Do not make the generic `DataLogWriter` or `DataLogBackgroundWriter`
constructors implicitly consume run context; they are also used for custom
files, tools, and replay. Their explicit path contracts and the `.wpilog` binary
format remain unchanged. Ordinary naming requires no live-directory rename
support. Reuse #9570's writer-local `SetDir()` behavior, cached-path updates,
exclusive destination creation, sync/error handling, and language bindings. Add
completion/status reporting sufficient to distinguish preference, adoption,
full-history transfer completion, and cleanup. Connect writer/copy completion to
the manager's C-library notification; an asynchronous `SetDir()` or `Flush()`
request returning is not transfer completion. Managed transfers need a
retain-source option and copy receipt so cleanup can respect reader holds;
standalone calls can retain their existing source-removal behavior. Account for
previously closed segments separately. No prepare/commit/abort API across
producers is required. Cover independent transfer, failure, and normal segment
rotation.

Apply the same always-managed contract to the duplicated DataLogManager in
`ntcoreffi`, or replace it with shared context and policy helpers. Remove its
independent selection, naming, and cleanup as well. Audit its exported
symbols/build integration instead of assuming it directly uses the wpilibc
implementation.

Coordinate console capture with the OS supervisor: the current managers read
`/home/systemcore/WPILIB_UserProgram.log` into a log entry. An optional per-run
stdout/stderr file, when enabled, is opened by the supervisor before launch.
Keep or adapt the existing console reader deliberately to avoid tailing the next
run or duplicating capture unexpectedly. Failure to start WPILib must still
leave supervisor startup/exit diagnostics associated with the run when storage
works. Console capture is another independent producer. A supervisor capture
worker may switch output, or continue internally and copy its closed file later;
neither choice blocks other producers. A raw inherited stdout/stderr file
descriptor cannot be redirected by changing a path.

#### Source/build/test surfaces

| Area | Planned changes |
| ---- | --------------- |
| `wpilibc` and `wpilibj` | Required external client dependency, C++ and Java/JNI adapters, always-managed DataLogManager with old selection/naming/cleanup removed, and DS observation hook outside logger lifetime. |
| `ntcoreffi` | Require the client and apply the same always-managed DataLogManager contract; remove independent selection/naming/cleanup and review FFI argument compatibility. |
| RobotPy | Bind WPILib's C++ wrappers, package/resolve the external client runtime, update DataLogManager wrapper documentation, and write Python tests with C++/Java tests. |
| `datalog` | Independent writer transfer building on #9570, completion/copy receipts, and deferred source cleanup; tests for label updates, transfer failure, restart/rotation, and preserved records; no format change. |
| DataLogTool and ecosystem viewers | Run listing across volumes, verified-copy deduplication, complete/partial all-format export, metadata display, and coordinated deletion. |
| Build definitions | Add the external client to Gradle, CMake, and Bazel with matching version/platform artifacts; update deployment/JNI/RobotPy runtime packaging. No new C client implementation belongs in wpiutil. Java builds remain Gradle/Bazel as applicable. |

Keep these consumers aligned with the server, runtime publication, and
desktop-host surfaces in the implementation and release plan below.

### Vendor integration

Implement the [vendor integration requirements][] through the published C ABI.
Register before the first automatic log file opens, including startup from
native static initialization, Java initialization, or Python imports. Query the
preferred destination rather than relying on the initial path hint, and qualify
the minimum ABI and required capabilities against supported OS/desktop runtime
packages. Serialize a switch with the affected writer's I/O, and update
rotation, reopen, auxiliary-file, and framework path caches only when that
writer succeeds. Sidecars also bind their run-specific lifetime to supervisor
protection through the library. Declare an initial-path-only adapter explicitly
when transfer is unsupported.

Connect successful writer and historical-copy completion to
`ReportProducerTransferComplete`; a path setter returning is insufficient.
Qualify retry/acknowledgement handling through the C library and retain sources
for coordinator cleanup. Producers unable to copy all their history keep
reporting partial placement instead of sending full completion.

CTRE can use its existing path setter for initial placement and independently
starting output on USB. Qualify its restart/flush behavior and separately
account for older files; a path setter alone does not promise to copy them.
Register during native startup so logging does not depend on user robot-code
ordering. Confirm cleanup suppression, buffering, and restart/new-segment
behavior; label changes never call the setter.

REV needs a managed-context integration in its automatic startup path and an
explicit way to suppress its managed-output cleanup, along with independent
destination-change support. Its currently documented public methods are
insufficient to promise a complete application-only adapter.

AdvantageKit routes its default writer to `advantagekit` and preserves explicit
file paths. Audit and suppress its own retention in managed mode and report its
writer's locations, including output paths cached by its framework. Adopt USB
independently and account for any earlier segments left internally. Keep replay
inputs read-only; default managed replay outputs go in a new run with
`replay_source_run_id` when known. Existing explicit output-beside-input
configurations remain opt-outs. Update automatic viewer-opening to pass the
actual generated file and its run context.

For any producer with hard-coded paths and no configuration hook, complete
grouping requires a vendor update.

### Catalog, import, and export integration

Implement the [download and inspection contract][] with the following discovery,
copy, and deletion procedures. Importers check for an existing run ID and
distinguish the same run from a collision; matching IDs alone never authorize
overwriting an existing run.

Device tools enumerate the coordinator's run catalog across registered volumes
through supported library/tool APIs; they do not depend on the private server
protocol. Offline tools enumerate canonical `runs/ID` directories and metadata.
The run layout, `.runref` format, and metadata schemas remain documented
interchange contracts for tools, independently of the hidden control transport.
Neither recursively traverses every top-level alias. Group all known locations
by run ID; distinguish unique data, verified redundant copies, and unfinished
copy staging. Do not choose a single volume merely because it is preferred. When
a user selects an alias, resolve either its symlink target or its validated
`.runref` contents to that canonical directory. Show time, event/match history,
outcome, active state, producer list, per-producer placement, missing volumes,
and total run bytes. Keep legacy flat files in a separate visible category.
Unknown producer extensions are still part of the run; grouping is not inferred
from `.wpilog` filenames.

At sealing and export, enumerate the actual run trees at all remaining known
locations rather than relying only on producer-reported inventories. Exclude
retired locations from required input, preserving their history in the export
metadata. Reports are advisory while active: files can appear before their
reports arrive. Record any missing volume and do not claim a complete inventory
while unique history is unavailable.

Export a run as a directory or archive named with its current display name,
containing real files and an export metadata snapshot describing completeness
and source volumes. Union the known locations, avoid following arbitrary
interior symlinks, and do not export top-level symlink/reference aliases,
verified redundant copies, or absolute references. Preserve/report unverified
name conflicts instead of overwriting them. Request a reader hold covering the
selected source locations before enumeration/copy and renew it while exporting.
Resolve locations again if retirement wins the race to acquire a hold. Holds
have explicit ownership and crash recovery so a failed downloader does not pin
storage forever; loss of a hold invalidates the export operation and must be
reported.

Show whether the recording ended cleanly, ended with a confirmed crash, or was
recovered with an unknown/interrupted outcome. Show per-file damage separately
from unavailable volumes and confirmed offline absence. Export raw surviving
files even if a format-specific reader cannot open them; partial tails must not
hide the other producers.

A live export is best-effort and labeled incomplete; it must not trigger source
deletion. For a sealed run, require all unique data from every known location,
then verify the copied inventory, sizes, and checksums before a
delete-after-download request. The service then rechecks run identity, state,
and protections, releases the requesting export hold as part of that
transaction, and deletes the whole run only if eligible. Manual deletion of one
file while the robot is running remains outside the grouped retention workflow.
Ordinary offline USB moves/deletions use the supported reconciliation rules
above and do not require a live service transaction.

## Implementation and release plan

### Implementation surfaces

| Area | Planned changes |
| ---- | --------------- |
| Standalone `wpilib-log-client` project | Public C ABI and reusable coordinator library, automatic desktop context, host configuration/lifecycle APIs, registration/location tracking, private SystemCore transport, metadata/storage policy, public API conformance and internal compatibility tests, and published headers/shared runtimes. |
| SystemCore deployment/supervision | Install/check the client runtime and OS service; allocate and inject context before executable loading. Coordinate changes outside allwpilib with their owners. |
| Desktop simulation/tooling | Package the shared runtime for direct executable/JVM/Python launches. Supply the reusable coordinator and optional host configuration/bindings; qualify native first use before WPILib startup. No launcher wrapper or daemon is required. |

WPILib consumers, writer changes, JNI/RobotPy bindings, and tooling are listed
in the producer and tool integration section above. Publish matching headers,
shared binaries, CMake targets, pkg-config metadata, and Gradle/Bazel artifacts;
keep platform versions and checksums aligned with those consumers.

### Delivery sequence

1. **Publish the library boundary.** Finalize the C ABI and supported platform
   matrix, runtime ownership, bindings, and the standalone C-only harness.
   Separate public API conformance from private server/bootstrap fixtures.
2. **Deliver both hosts.** Implement shared coordinator policy, SystemCore
   allocation/supervision, and desktop first-use context with general embedding
   configuration. Package the runtime for ordinary executable/JVM/Python
   launches and qualify all supported early-startup paths.
3. **Establish durable recovery.** Implement the registry, allocation
   protection, metadata publication, lifetime reconciliation, and bounded
   restart recovery. Prove repeated crashes and power cuts without clean-close
   callbacks.
4. **Deliver USB reconciliation.** Publish late-mounted destinations, track
   actual placement, validate copy evidence, coordinate source cleanup, and
   persist offline absence/cancellation. Preserve independent producer progress.
5. **Qualify retention before enabling it.** Run cleanup in report-only mode,
   measure storage rates/reserves, and pass lifecycle, disk-pressure,
   interrupted deletion, offline USB, and export/hold tests. Desktop retention
   stays off by default. Coordinate the service gate with the
   [ecosystem rollout plan][].

### Ecosystem delivery sequence

1. **Agree the public contract.** WPILib, vendors, SystemCore, and tool
   maintainers review the required C API/ABI, metadata schemas, namespace
   ownership, explicit-path compatibility, label semantics, independent
   destination adoption, and location reporting. Assign library ownership and
   agree its ABI/package matrix. Library/SystemCore maintainers separately
   specify private transport/bootstrap and supported runtime/server
   combinations.
2. **Deliver the service and libraries.** Complete the service/library delivery
   sequence above. Consume its published headers/runtimes and add the WPILib
   language adapters. Qualify SystemCore launch integration, direct desktop
   launches, recovery, and late USB adoption before deployment; keep cleanup in
   report-only mode until the retention gate passes.
3. **Integrate WPILib and tools.** Make the library required and DataLogManager
   always managed; remove its old selection/naming/cleanup immediately. Cover
   ntcoreffi/RobotPy and migrate directory arguments. Publish DS metadata
   independently and ship run-aware download/export. Preserve legacy browsing.
4. **Integrate vendors.** Require the C library or bindings to its ABI. Qualify
   native auto-start registration, independent adoption/status reporting, local
   transfer buffering, and historical segments through that API. Disable
   independent retention and qualify media/error handling. Publish supported
   ABI/runtime versions and known opt-outs; no vendor wire-protocol integration
   is supported.
5. **Enable retention.** Measure aggregate rates and tune storage policy. Pass
   repeated power-cut/crash-loop, disk-full, late-USB transfer, external-writer,
   offline USB file-management, and export/deletion tests. Only then enable
   service reclamation on SystemCore. New DataLogManager already requires
   managed placement; qualifying retention does not introduce a manager mode
   switch.
6. **Retire remaining legacy integration.** Retire transitional vendor behavior
   and support for older binaries after supported launch paths and producers
   have migrated. DataLogManager has no transitional cleanup to remove. Retain
   generic standalone file-writing APIs.

Deploy the required library/runtime and OS integration before using the new
DataLogManager. It has no legacy mode; an application rollback requires a
compatible older binary and deployment configuration. Never redirect an active
run or discard outstanding transfer evidence. The deployment and rollback rules
below define service/runtime pairing and recovery.

Use this document's open decisions and the [ecosystem open decisions][] to close
the remaining agreements at each release gate.

### Deployment and rollback

Rollback is by deployment configuration for the **next** application launch. The
new DataLogManager cannot roll back to a legacy mode; an application rollback
requires a compatible older binary and its deployment configuration. Never
rewrite a live run's launch descriptor or invalidate a producer's actual
location. Preserve outstanding location reports and copy receipts across service
updates; retain a client runtime ABI compatible with deployed vendor and WPILib
binaries and a tested private-transport pairing with the server. Coordinate
runtime/server rollback within the OS package policy; vendors do not negotiate
that protocol. Quiesce source cleanup if compatibility is uncertain. The new
service can stop deleting immediately; existing managed directories stay
readable and protected, and legacy tools may browse canonical files manually.

## Trades

The [shared design trades][] cover directory naming, producer discovery,
mandatory C-library integration, independent USB adoption, and simulation
ownership. The implementation decisions below supply the associated durability
and lifetime guarantees without changing that contract.

| Decision | Alternatives considered | Design choice and benefit | Cost or limitation |
| -------- | ----------------------- | ------------------------- | ------------------ |
| Liveness and cleanup authority | File age, PID files, heartbeat expiry, or supervisor-tracked participants | Invocation/process-group tracking plus registered external writers; ambiguous runs remain protected. | Requires lifecycle integration and can retain data indefinitely when recovery cannot prove inactivity. |
| Persistence cadence | Depend on clean shutdown, sync every record, or persist critical control state plus periodic producer data | Persist identity and destructive-operation evidence at defined boundaries; batch display updates and sync producer data in the background. Supports frequent power loss without per-record control-loop storage waits. | Recent buffered data/metadata can be lost; sync latency, media behavior, and the intended loss window need qualification. |
| Startup when allocation fails | Block robot startup, create per-producer fallbacks, or launch with automatic logging disabled | Bounded allocation wait, then explicit disabled mode. Robot control can start without fragmenting the run across fallback paths. | Early or entire-run logs may be unavailable during a service/storage failure. |

### Optional inspection alias publication

The [shared design trades][] leave shipping `current`/`current.runref` open. If
provided, implement it as follows; it never participates in producer discovery.

For inspection, the alias offers convenient shell and file-browser access. If
implemented, create a temporary relative symlink in `ROOT`, then atomically
replace `current` with a same-filesystem rename. For the text fallback, write
and flush a temporary file, then atomically replace `current.runref` using the
platform's supported same-filesystem replacement operation. Do not unlink the
old alias first or rewrite the visible text file in place. Serialize publishers
so an older launch cannot overwrite a newer launch's pointer. On exit, remove
the alias only if it still identifies that run, under the same publisher lock. A
reader of `current.runref` has the same cross-run selection race as a reader of
`current`; changing the representation does not make it producer discovery. Its
absence or staleness must never affect a producer.

## Open trades and decisions

These implementation choices require owner agreement, a specified interface, or
qualification measurements. The [ecosystem open decisions][] separately cover
public metadata/naming, namespace governance, producer APIs and migration, and
tool workflows. Required C-library use, private transport, direct desktop
launches, and independent producer transfers are established constraints.

| Open decision | Current design and remaining choices | Owners | Evidence or agreement needed to close |
| ------------- | ------------------------------------ | ------ | ------------------------------------- |
| SystemCore allocation ownership and outage behavior | The design uses service allocation. Decide whether supervisor-local allocation coordinated with the service is needed, select the startup timeout, and determine whether any common fallback is required beyond disabled logging. | SystemCore, WPILib | Specify the pre-executable handshake and demonstrate behavior when the service is absent, slow, or restarts during allocation. Any fallback must preserve one run identity and cleanup protection. |
| Standalone library packaging and ABI lifecycle | The C client and reusable coordinator are separate from wpiutil. Finalize names/repository, maintainers, artifacts, client/coordinator binary split, language bindings, ABI lifetime, and OS versus desktop ownership. Use mrclib-style shared-library consumption. | Library maintainers, WPILib, SystemCore, vendors | Publish a platform/version matrix; demonstrate C-only consumption, compatible upgrades, one default desktop context, and matching Gradle/CMake/Bazel/RobotPy packaging. |
| Private SystemCore transport and bootstrap | Protocol privacy is settled. Library/OS maintainers choose transport, encoding, internal version negotiation, endpoint discovery, supervisor bootstrap, authorization, and supported runtime/server upgrade combinations. | Library maintainers and SystemCore | Test negotiation/failure handling and upgrade/rollback combinations internally; demonstrate that private protocol changes preserve supported vendor binaries through the public C ABI. |
| Power-loss persistence and recovery budgets | Routine automatic recovery is required. Select the registry storage engine/journal and checkpoint rules, durable acknowledgement boundaries, producer sync/loss budgets, and maximum launch-path recovery work. | SystemCore, client library maintainers, WPILib, vendors | Cut power during allocation, file creation/rotation, copy cleanup, and deletion on supported media; measure recovery/startup latency and surviving data without graceful shutdown. |
| Process supervision and lifetime | Choose the exact supervisor/cgroup integration, external-writer registration/shutdown, and proof of final close. The safety contract excludes heartbeat expiry alone as proof of inactivity. | SystemCore and vendors with sidecars | Demonstrate orphan-child, daemon restart, IPC disconnect, and overlapping-invocation behavior; define when a run can be sealed. |
| Volume selection and USB recovery | Define approved filesystems, ordering among USB devices, mount identity/fencing, and conditions for each writer to resume on the same medium. Decide whether reverse failover is needed beyond v1. | SystemCore and vendors | Test removal during independent moves and replacement at the same mountpoint; internal writers must continue and USB writers must not write beneath an absent mount. |
| Producer transfer budgets and copy scheduling | Completion notification through the C library is required. Finalize its payload/signatures and accepted/pending status, plus per-writer limits, retry/buffer budgets, and I/O throttling. Assign closed-file copying to owners or a shared copier, consistent with the agreed consolidation policy. #9570's 10 MB is a single-file reference point. | WPILib, SystemCore, vendors | Measure pauses and throughput, qualify bounded buffering, and demonstrate independent progress, historical-file coverage, reliable notification, and eventual cleanup with actual writers. |
| Copy evidence and export-hold implementation | Specify file-incarnation/copy-receipt formats, verified-prefix deduplication, hold renewal/expiry, and coordination of export completion with deletion. | SystemCore, vendors, tool maintainers | Test transfer cleanup against reader holds, growing destination files, lost receipts, conflicting names, failed downloaders, and offline volumes. |
| Retention defaults and long runs | Set watermarks, reserve, age/byte/count limits, pin policy, and critical-space overrides. Confirm whether whole-run quotas or multi-part eviction can remain deferred. | SystemCore, WPILib, vendors | Measure aggregate rates, burst sizes, device capacities, and reclaim latency; exercise a run larger than the available storage budget. |
| Embedded simulation interfaces and coverage | Direct launches and a reusable in-process library are required. Finalize host configuration/early-init APIs, registry location/locking, lifetime proof, storage providers, helper attachment, and whether isolated contexts ship in v1. No separate daemon is required. | Library maintainers, WPILib, vendors, simulation/tooling maintainers | Test direct C++/Java/Python launches on Linux/macOS/Windows, concurrent first callers and sessions, configurable general-purpose hosts, pre-main initialization, ordinary permissions, and recovery after host death. |
| Offline reconciliation records and volume retirement | Ordinary offline moves/deletions are supported. Finalize absence-record granularity, durable cancellation/replay rules, safe record compaction, volume identity, and explicit retirement of unavailable/reformatted volumes. | SystemCore and tool maintainers | Demonstrate that missing old files stay absent across restarts, stale receipts cannot delete their internal sources, and resolved absence does not permanently block normal retention. |
| Registry recovery and imported runs | Distinguish automatic recovery of interrupted writes/known prior-boot runs from exceptional lost ownership, severe corruption, and unfamiliar imports. Define duplicate-ID handling and administrative annotations after sealing. | SystemCore and tool maintainers | Demonstrate repeated unattended recovery without a growing set of permanently unresolved normal runs; explicit adoption must never authorize deletion of a live or unverified run. |
| Crash-loop history and storage reserve | Set a bounded count/age policy for recent failed runs, registry/namespace reserve, and behavior when rapid restarts exhaust bytes or file counts. Interrupted runs otherwise follow normal retention. | SystemCore and WPILib | Exercise sustained pre-main and post-start crashes with invalid time and near-full storage; preserve useful diagnostics while maintaining bounded launch latency. |

Close persistence, lifecycle, reconciliation, and retention decisions before
enabling destructive cleanup. Document deferred capabilities explicitly; an
unresolved implementation choice is not permission to weaken the shared run
contract.

## Verification and rollout

### Acceptance tests

Run these implementation tests in addition to the
[producer and workflow tests][]. Use disposable roots/media for destructive
cases and preserve the public API boundary in all vendor conformance tests.

| Scenario | Required result |
| -------- | --------------- |
| Standalone C vendor harness with no wpiutil/HAL/WPILib libraries | Published headers and packaged runtime suffice for registration, destination discovery, and storage-status reporting on both backends. No private environment parsing or direct server connection. A separate desktop process gets its own run; consumers loaded into one process share its default context. |
| Private protocol change with an ABI-compatible library/server update | Supported vendor binaries continue unchanged through the C API. Library/server tests cover supported upgrade/rollback pairs; incompatible pairs return documented API failure without a fallback run or cleanup. |
| Concurrent first calls from native vendors, JNI, and Python bindings in supported combinations | Exactly one default context or shared initialization error is published; no duplicate allocations, constructor-order dependency, deadlock, or privately linked state. |
| General simulation host configures the embedded library | Custom root and metadata/storage providers work through the ordinary producer API without HAL or a daemon. Configuration after first use is rejected without changing active paths or identity. |
| Embedded robot-like storage/lifecycle setup | Inject late mounts/removal, independent transfer outcomes, and explicitly enabled retention in disposable roots. Apply the same metadata, copy-evidence, and offline reconciliation rules; host-controlled clocks/events make tests repeatable. |
| Embedded host crashes, then a new simulation starts alongside another live simulation | New process gets a fresh run; background recovery preserves the other live host and seals only proven-inactive runs. No close callback or work after host death is assumed; default retention stays off. |
| Missing/incompatible runtime versus unavailable SystemCore coordinator or desktop allocation failure | Deployment checks diagnose runtime/ABI problems where available; direct launch resolves packaged libraries. With a loadable runtime, bounded service/allocation failure disables managed logging without per-producer fallback. |
| Kill SystemCore coordinator at each allocation, metadata, alias, sealing, and trash step | Recover the same run; established writers continue, including local moves to approved destinations; uncertain runs are protected; partial trash can be resumed. |
| Reboot with known runs still marked PREPARING, ACTIVE, or DRAINING | Recover old-boot runs automatically, preserve uncertainty about termination, and make inactive runs normally retainable. A missing clean-close marker does not require manual adoption. |
| Orphan child, external writer disconnect, or SystemCore coordinator-only restart | Preserve same-boot live writers and their paths; seal only after positive lifetime reconciliation. A service restart does not itself create a new application run. Custom embedded hosts must track any explicitly attached helpers equivalently. |
| Repeated hard cuts during allocation intent, directory publication, and permission to write | Recover reservations and known run identity without overwriting collisions. SystemCore cannot release application code, and desktop cannot return a usable context, before its required persistence boundary. Never reuse the interrupted run on restart. |
| Power loss after copy publication, receipt persistence, source removal, or trash rename | Recover file-by-file from durable evidence and actual volumes; keep uncertain copies and resume authorized cleanup. Independent producers require no common commit decision. |
| Crash loop with invalid wall time, near-full media, and a large recovery backlog | Use distinct IDs and a bounded recent-failure policy; reclaim proven-inactive interrupted runs and keep startup within its budget without scanning the full store first. |
| Concurrent late writer join/export/deletion | State serialization prevents joining a sealed/deleting run or losing a held run. |
| Coordinator/producer crash during copy, local switch, or status reporting | Surviving writers keep their actual paths; protect uncertain copies and reconcile pending reports/receipts. No global commit decision is needed to keep logging. |
| Completion sent after current-file switch while an older segment or auxiliary file remains uncopied | Do not accept full completion or reclaim that source. Completion covers the final source inventory and relinquished source writes, not just the writer's current path. |
| Duplicate, reordered, lost, or replayed completion notifications | Durable idempotent acceptance is scoped to run, registration incarnation, source/destination identities, and sequence. A stale notification cannot complete a replacement registration or a different destination; acknowledgement loss never blocks logging. |
| Last completion races a late registration, logger restart, hold release, or new source files | Serialize source placement/holds with retirement. An earlier approved source user defers retirement; a later request receives USB or retired/unavailable status. Never recreate a retired source or delete an unaccounted file. |
| Producer/service dies before notification or after durable acceptance, followed by offline USB changes | Do not infer completion from disconnect or unregister. Recovery verifies files and lifetime; missing/modified destinations invalidate cleanup proof even with an accepted historical completion. No dead-producer acknowledgement is required to recover the run. |
| All producers complete while the run remains ACTIVE | Persist source retirement, remove verified internal contents, aliases, and the run directory, and keep USB writers running under the same run ID. No sealing or coordinated producer cutover occurs. |
| Power loss during retirement intent, metadata publication, source removal, or final retirement record | Replay retiring/retired state before new placement approvals. Preserve remaining data if USB evidence cannot be revalidated; finish eligible removal without reconstructing the source or requiring a dead producer's acknowledgement. |
| Delayed path request or metadata/copy job targets a retired location | Reject source placement and suppress stale publication/copy work, including after coordinator restart. A retired internal peer does not make a USB-only export incomplete. |
| Old transfer/consolidation job replays after confirmed absence or a second reboot | Persisted cancellation prevents recreating removed output. An incomplete old copy stays canceled; explicit restoration remains possible and the new run is unaffected. |
| Successful scan finds old files absent, versus unreadable/disconnected/reformatted media | Resolve absence only on the recognized readable volume. Failed/incomplete scans remain unresolved; a new volume at the same path is not evidence about the old one. |
| Sealed run has remaining internal files after confirmed offline USB deletion | Apply ordinary retention without a permanent missing-fragment hold, while preserving pins/reader holds and reporting absent history in exports. |
| Text-reference publication interrupted, malformed/stale target, or both alias forms present | Readers see a complete reference or report it unavailable, reject invalid targets, and deduplicate by run ID. Canonical writes continue; recovery rebuilds aliases from metadata. |
| Malformed bootstrap/metadata, incompatible private transport, traversal and interior symlinks | Library/server tests verify safe public API failures: no accidental standalone run, truncation, or deletion outside registered storage. Vendors need no knowledge of the malformed wire inputs. |

Write standalone C ABI/client tests in the new library and C++, Java, and
RobotPy integration tests alongside WPILib changes. Include a C-only consumer
build and binary compatibility checks against published client releases. Vendor
conformance tests use only the published library API. Keep private protocol and
bootstrap fixtures in library/SystemCore implementation tests; test the
library's mapping of internal failures to public statuses and compatible
runtime/server updates without requiring vendor protocol implementations.
HAL-based tests reset DS/time/mock state at their start. Use subprocess
integration tests for pre-main behavior, direct launches, environment
inheritance, and runtime sharing across loaded libraries; thread-only unit tests
cannot establish those properties. Use the embedded coordinator for broad
simulation integration and deterministic robot-like policy tests. These tests do
not substitute for qualifying actual OS service/supervisor integration. Use
fault injection and disposable volumes for retention and power-loss recovery,
never the developer's actual log directory.

Process-kill tests exercise missing callbacks and automatic restart, but do not
stand in for loss of power to storage. Add repeated controlled power-cut tests
on representative SystemCore and USB filesystem/media combinations, plus
deterministic interrupted-write/sync-error injection. Check both immediate
reboot and a USB device that returns later. Interrupt recovery itself and verify
that the next recovery still converges. No graceful shutdown or application
finalizer may be used to make these acceptance tests pass.

Run affected Gradle tests, Java PMD and Spotless checks, appropriate native
Gradle/CMake/Bazel validation, and Bazel RobotPy tests. Confirm exact available
targets in the implementation checkout. Vendor release qualification must run
the same startup, rotation, disk-pressure, and restart scenarios with their
actual binaries on SystemCore and supported simulation platforms.

Use the affected standalone-library and OS test suites as well. Complete the
implementation and ecosystem release gates before enabling service deletion.

[Java Filesystem]: ../../wpilibj/src/main/java/org/wpilib/system/Filesystem.java
[Native background writer]: ../src/main/native/cpp/DataLogBackgroundWriter.cpp
[Java calls it through JNI]: ../src/main/java/org/wpilib/datalog/DataLogBackgroundWriter.java
[systemd service lifetime]: https://freedesktop.org/software/systemd/man/latest/systemd.service.html
[Gradle's mrclib dependency definition]: ../../shared/libmrclib.gradle
[version catalog]: ../../gradle/libs.versions.toml
[Bazel's mrclib module]: ../../shared/bazel/thirdparty/mrclib/mrclib.MODULE.bazel
[its build target]: ../../shared/bazel/thirdparty/mrclib/BUILD.bazel
[RobotPy's native HAL package]: ../../hal/src/main/python/native-pyproject.toml
[HAL CMake build]: ../../hal/CMakeLists.txt
[Linux fsync]: https://man7.org/linux/man-pages/man2/fsync.2.html
[Run-based robot log management]: run-log-management.md
[shared storage contract]: run-log-management.md#storage-layout
[producer startup contract]: run-log-management.md#startup-and-discovery
[naming and metadata contract]: run-log-management.md#naming-and-metadata
[usable-log requirements]: run-log-management.md#usable-logs-after-interruption
[independent USB transfer contract]: run-log-management.md#late-mounted-usb-transfer
[writer transfer requirements]: #producer-local-transfer-and-historical-files
[WPILib integration requirements]: run-log-management.md#wpilib-integration-requirements
[ecosystem rollout plan]: #ecosystem-delivery-sequence
[shared design trades]: run-log-management.md#trades
[ecosystem open decisions]: run-log-management.md#open-trades-and-decisions
[producer and workflow tests]: run-log-management.md#acceptance-tests
[C++ DS refresh path]: ../../wpilibc/src/main/native/cpp/driverstation/internal/DriverStationBackend.cpp
[Java DS refresh path]: ../../wpilibj/src/main/java/org/wpilib/driverstation/internal/DriverStationBackend.java
[vendor integration requirements]: run-log-management.md#vendor-integration-requirements
[download and inspection contract]: run-log-management.md#download-inspection-and-compatibility
