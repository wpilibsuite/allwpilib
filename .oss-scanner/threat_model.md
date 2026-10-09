# WPILib threat model

## Project and scan scope

WPILib provides the C++, Java, and Python-facing libraries used to develop
FIRST robotics software, together with networking, camera streaming, logging,
simulation, and desktop inspection tools. This enrollment covers the single
repository https://github.com/wpilibsuite/allwpilib on its main development
branch. Other WPILib repositories, team robot programs, vendor libraries not
contained here, controller firmware, and FIRST field-management software are
outside this enrollment.

The main branch targets the Systemcore controller and upcoming releases.
When possible, identify whether a finding also affects a released version;
do not assume development-only code is present in deployed releases.

## Security goals and trust boundaries

Prioritize memory safety, process integrity, bounded resource consumption,
and reliable handling of malformed network messages and files. A malformed
input should not enable arbitrary code execution, unintended file access,
or persistent loss of service in robot software or desktop tools.

Normal robot deployments assume that the local robot network and its
participating devices are trusted. Do not assume that the robot LAN is
adversarial or that robot services are publicly reachable on the Internet.

Development computers and desktop tools may run on untrusted networks;
simulation services may also be enabled there. Consider attackers who can
reach those listening services, operate a peer server or camera that a client
connects to, or supply an external file that a user opens. Identify the actual
connectivity, enabled features, and attacker control for each finding.

Keep malformed robot-network input in scan scope for memory safety, bounded
resource consumption, and robustness. For robot-only findings, explain whether
the input requires a malicious or compromised participant on the otherwise
trusted LAN. Distinguish robustness defects from security vulnerabilities
and assess severity using the demonstrated deployment and trust boundary.

NetworkTables and several simulation/telemetry services intentionally allow
participating peers to publish data without user authentication. Their intended
data-sharing behavior, and absence of authentication or TLS by itself, are not
findings. Classify malformed-input defects using the deployment and attacker
control described above.
Do not treat valid peer traffic as arbitrary-code execution merely because a
team's application chooses to use values to control its robot.

Team-written robot code, local build scripts, and deliberate calls by code
already executing within the process are generally trusted. Distinguish API
misuse by trusted code from values that an attacker can deliver through a
network or file parser. Python and Java bindings must still correctly enforce
their supported argument contracts across native boundaries.

## Highest-priority components and untrusted inputs

- `ntcore`: NetworkTables client/server wire decoding, JSON and MessagePack
  messages, WebSocket connections, topic metadata and payloads, connection
  lifecycle, persistence, and concurrent publication/subscription behavior.
- `wpinet`: HTTP and WebSocket parsing/framing, stream and datagram handling,
  buffer management, asynchronous callbacks, and service discovery.
- `cscore` and `cameraserver`: HTTP camera responses, multipart/MJPEG streams,
  image/frame size handling, and camera-server requests from network peers.
- `datalog` and `wpiutil`: binary log records and metadata, JSON/MessagePack,
  protobuf and struct decoding, schema parsing, length calculations, mapped
  files, and malformed or truncated inputs. Analyze native and Java readers.
- `glass`, `tools/datalogtool`, and `tools/outlineviewer`: files or schemas
  opened by a user, data from connected NetworkTables peers, and handling of
  remote file-transfer results where supported.
- `simulation/halsim_ws_*`, `simulation/halsim_ds_socket`, and
  `simulation/halsim_xrp`: incoming simulation network data and connection
  lifecycle. State explicitly when a simulation extension must be enabled.
- JNI and Python-facing native interfaces: incorrect size/type conversion,
  lifetime management, and exceptions when reachable through supported APIs.

`wpilibc`, `wpilibj`, `hal`, `drivers`, `wpimath`, command frameworks, and other
in-repository components remain in scope, with priority given to a demonstrated
path from untrusted input. Controller-specific HAL code is available for source
review but the desktop build uses the simulation HAL. Hardware-only behavior
requires separate validation and should be identified as such.

Vendored dependencies remain relevant when a vulnerable path is reachable
through WPILib. Identify the dependency and version, check whether the problem
is already known upstream, and prefer a dependency update or a small local fix
over unrelated changes to vendored code. Standalone third-party vulnerabilities
with no demonstrated WPILib impact are lower priority.

## Build and reproduce

The Dockerfile uses the same Linux toolchain image as current allwpilib CI.
The checkout is `/src`; the CMake build is `/src/build-cmake`. C++ libraries are
in `build-cmake/lib`, executables and test binaries in `build-cmake/bin`.
Java classes and test sources are compiled across the repository. The setup
exercises Java/JNI tests for `ntcore`, `wpinet`, `wpiutil`, `datalog`, and `cscore`.
Their JNI installations, test results, and reports are under the subprojects'
`build/` directories; Gradle caches are retained under `/root/.gradle`.

Start service discovery before tests that require it:

```sh
service dbus start
avahi-daemon -D
```

Run the desktop C++ suite or select tests by name:

```sh
ctest --test-dir /src/build-cmake --output-on-failure --parallel 2 --timeout 300
ctest --test-dir /src/build-cmake -N
ctest --test-dir /src/build-cmake -R 'WireDecoder|DataLog|WebSocket|Http' --output-on-failure
```

Run Java/JNI tests using dependencies fetched during setup:

```sh
./gradlew :ntcore:test :wpinet:test :wpiutil:test :datalog:test :cscore:test --offline --no-daemon --max-workers=2 -Ponlylinuxx86-64 -PskipJavaFormat
./gradlew :ntcore:test --offline --no-daemon --max-workers=2 -Ponlylinuxx86-64 -PskipJavaFormat
```

Prepend `cleanTest` to the selected Java test tasks to rerun their tests without
rebuilding all native code.
Use `--rerun-tasks` when a full rebuild is needed.
Other Java components' test sources are compiled, but their JNI libraries may
need to be built before running their tests. Allow for compilation memory and
use one worker and a smaller Gradle JVM heap when rebuilding in an 8 GiB VM.

In a loopback-only environment such as `docker run --network=none`, multicast
service discovery cannot find its announcements. The native
`MulticastServiceAnnouncerTest EmptyText` and Java
`org.wpilib.net.MulticastServiceAnnouncerTest` pass during network-enabled
setup but cannot pass in that environment. Exclude only these discovery
assertions for the offline test run; this is an environment limitation,
not evidence of a security defect. Other networking tests remain enabled:

```sh
ctest --test-dir /src/build-cmake --output-on-failure --parallel 2 --timeout 300 -E '^MulticastServiceAnnouncerTest EmptyText$'
./gradlew cleanTest :ntcore:test :wpinet:test :wpiutil:test :datalog:test :cscore:test --offline --no-daemon --max-workers=2 -Dorg.gradle.jvmargs=-Xmx2g --init-script /opt/wpilib-offline-tests.gradle -Ponlylinuxx86-64 -PskipJavaFormat
```

C++ and Java regression tests live under component `src/test` directories.
Native library headers are in component `src/main/native/include` directories.
The image does not prepare Python binding builds, cross-compiled controller
tests, or tests requiring physical cameras or robot hardware. Source is still
available for reviewing these interfaces; clearly state reproduction limits.

For native sanitizer reproduction, configure a separate CMake build with
`-DCMAKE_BUILD_TYPE=Asan` or `Ubsan`, retain the relevant component options,
and limit parallelism to suit the available memory. Offline scanner resources
are smaller than setup resources. Do not rely on fetching dependencies or
contacting external hosts during reproduction; use loopback peers and fixtures.

## Severity guidance

This is proposed triage guidance for this enrollment; assess actual
reachability, preconditions, and demonstrated impact rather than assigning
severity from a bug class alone. For findings limited to a trusted robot
network, explain the necessary malicious or compromised peer and distinguish
robustness failures from attacks across a demonstrated security boundary.

- Critical: demonstrated arbitrary code execution through a remotely reachable
  WPILib interface without prior process access, under a realistic deployment.
- High: attacker-reachable memory corruption with credible exploit potential,
  significant unauthorized file access, or demonstrated compromise of process
  integrity. Explain any required user interaction, network access, or optional
  component configuration.
- Medium: practical denial of service (crash, deadlock, sustained CPU use, or
  memory exhaustion) caused by malformed network input or an opened file.
  Explain recovery requirements and whether valid high-volume traffic would
  produce the same effect.
- Low/informational: limited correctness issues or trusted-code misuse without
  a demonstrated security boundary crossing. Math edge cases are usually
  correctness issues unless an attacker-controlled path gives concrete impact.

Physical robots make availability important, but a process crash alone does
not demonstrate dangerous robot motion or defeat of a hardware safety system.
Any claimed physical or control-system impact must include evidence and account
for the actual controller/Driver Station behavior and deployment configuration.

## Requested report and patch format

Provide the affected commit and component, exact input and entry point,
attacker prerequisites, expected versus observed behavior, and a self-contained
reproducer that runs offline. Include sanitizer output or a stack trace where
useful. Separate verified impact from suspected exploitability, distinguish new
findings from known issues, and identify affected releases where possible.

Prefer minimal patches that preserve supported C++, Java, and Python API
behavior and wire compatibility. Include a focused regression test and follow
the repository's contribution and formatting conventions. Avoid broad refactors
or claims of exploitability based solely on a sanitizer warning.
