# Use the same Linux toolchain image as allwpilib's main-branch CI.
# It includes GCC 14, JDK 25, and WPILib's native dependencies.
FROM wpilib/systemcore-cross-debian:trixie

USER root
ENV DEBIAN_FRONTEND=noninteractive
# The CI base image supplies Bash, certificates, curl, Git, Python, CMake,
# and Avahi. Request only the remaining tools/libraries and skip upgrades
# of any already-installed packages in this list.
RUN apt-get update && apt-get install -y --no-install-recommends --no-upgrade \
      python3-yaml ninja-build pkg-config dbus libsdl3-dev libssh-dev \
    && rm -rf /var/lib/apt/lists/*

# Install Bazelisk independently of the source so checkout changes can reuse
# this layer. The fetch below uses .bazelversion to select the Bazel version.
RUN curl --fail --location --retry 3 \
      https://github.com/bazelbuild/bazelisk/releases/download/v1.29.0/bazelisk-linux-amd64 \
      --output /usr/local/bin/bazel \
    && echo "5a408715e932c0250d28bd84555f12edbf70117de42f9181691c736eacc4a992  /usr/local/bin/bazel" | sha256sum --check \
    && chmod +x /usr/local/bin/bazel

# The scanner supplies the allwpilib checkout as the build context.
COPY . /src
WORKDIR /src

# Build desktop C++ libraries, simulation modules, GUI tools, and tests with
# debug symbols. Limit parallelism because wpimath uses substantial memory.
ARG BUILD_JOBS=8
RUN cmake -S /src -B /src/build-cmake -G Ninja \
      -DCMAKE_BUILD_TYPE=RelWithDebInfo \
      -DCMAKE_C_FLAGS="-g -fno-omit-frame-pointer" \
      -DCMAKE_CXX_FLAGS="-g -fno-omit-frame-pointer" \
      -DWPILIB_WITH_TESTS=ON \
      -DWPILIB_WITH_GUI=ON \
      -DWPILIB_WITH_CSCORE=ON \
      -DWPILIB_WITH_SIMULATION_MODULES=ON \
      -DWPILIB_WITH_BENCHMARK=OFF \
    && cmake --build /src/build-cmake --parallel "${BUILD_JOBS}"

# Java/JNI use Gradle; current allwpilib CMake builds only C++.
# Compile all Java test sources and exercise networking, logging, and cameras
# online to populate dependencies, then force a second test run offline.
# Keep installed JNI libraries for later testing
# by intentionally omitting -PbuildServer (which cleans test installations).
# The wrapper is committed with LF; Windows checkouts may convert it to CRLF.
# Normalize the local Docker build context for Linux execution.
RUN sed -i 's/\r$//' ./gradlew \
    && chmod +x ./gradlew \
    && service dbus start \
    && avahi-daemon -D \
    && ./gradlew compileJava testClasses \
         :ntcore:test :wpinet:test :wpiutil:test :datalog:test :cscore:test \
         --no-daemon --max-workers="${BUILD_JOBS}" \
         -Ponlylinuxx86-64 -PskipJavaFormat \
    && ./gradlew cleanTest \
         :ntcore:test :wpinet:test :wpiutil:test :datalog:test :cscore:test \
         --offline --no-daemon \
         --max-workers="${BUILD_JOBS}" -Ponlylinuxx86-64 -PskipJavaFormat

# Fetch Bazel dependencies while online and retain the launcher, Bazel binary,
# and repository caches for additional builds during the offline audit.
RUN bazel --batch fetch //...

# As in the scanner template, native test failures should leave an auditable
# image rather than discard the build. Retry failures up to three times and
# retain CTest's failure logs; compilation and Java setup above remain fatal.
RUN service dbus start \
    && avahi-daemon -D \
    && (ctest --test-dir /src/build-cmake --output-on-failure \
          --parallel 2 --timeout 300 --repeat until-pass:3 \
        || echo "WARNING: native tests failed; see build-cmake/Testing/Temporary")

# Optional init script for loopback-only environments (e.g. tools/check's
# --network=none shell). The full multicast test runs during setup above.
RUN printf '%s\n' \
      'allprojects {' \
      '    tasks.withType(org.gradle.api.tasks.testing.Test).configureEach {' \
      '        filter { excludeTestsMatching("org.wpilib.net.MulticastServiceAnnouncerTest") }' \
      '    }' \
      '}' \
      > /opt/wpilib-offline-tests.gradle

CMD ["bash"]
