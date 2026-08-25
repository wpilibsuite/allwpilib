# Telemetry C++ API

This file describes the C++ Telemetry API as it is used from C++ robot code. The API follows the same overall model as the Java API, but the way users extend and consume it is intentionally C++-idiomatic:

- For the shared Telemetry design and Java-facing API, see [Telemetry design](telemetry.md).
- For Python-specific behavior, see [Python API](python.md).

- the main entry points are namespace functions in `wpi::telemetry` and member functions on `wpi::telemetry::TelemetryTable`

- generic logging is template-based rather than overload-plus-reflection based

- structured logging support uses `wpi::util::StructSerializable` and `wpi::util::ProtobufSerializable`, registers the corresponding schema with the selected backend, and logs the encoded bytes as raw values

- custom telemetry support is provided with non-member `LogTo()`, `LogValueTo()`, and `GetTelemetryTypeName()` functions found by ADL, or by member functions bridged by `wpi::telemetry::TelemetryLoggable`

## C++ Overview

For most robot code, a C++ user interacts with three pieces:

- `wpi::telemetry::Log()` for simple one-line publishing

- `wpi::telemetry::TelemetryTable` for grouping related values under a table path

- `wpi::telemetry::TelemetryLoggable` or ADL-based helpers for complex custom types

The API is header-driven and highly generic. In many cases there is no need to manually choose between primitive, string, struct, or protobuf logging. Instead, the correct path is selected by the template constraints on `TelemetryTable::Log()`.

## `wpi::telemetry` Root Functions

The namespace-level functions in `wpi::telemetry` are the top-level convenience API. They forward to the root telemetry table.

```cpp
wpi::telemetry::TelemetryTable& GetTable();
wpi::telemetry::TelemetryTable& GetTable(std::string_view name);

void KeepDuplicates(std::string_view name);
void SetProperty(std::string_view name, std::string_view key,
                 std::string_view value);

template <typename T, typename... I>
void Log(std::string_view name, const T& value, I... info);

template <typename T, typename... I>
void Log(std::string_view name, std::span<const T> value, I... info);

void Log(std::string_view name, bool value);
void Log(std::string_view name, int64_t value);
void Log(std::string_view name, double value);
void Log(std::string_view name, std::string_view value);
void Log(std::string_view name, std::span<const uint8_t> value,
         std::string_view typeString);
```

Typical usage:

```cpp
wpi::telemetry::Log("batteryVoltage", wpi::RobotController::GetBatteryVoltage());
wpi::telemetry::Log("enabled", wpi::DriverStation::IsEnabled());
wpi::telemetry::Log("state", std::string_view{"Ready"});
```

Using the root facade is appropriate when:

- a value naturally lives at the top level

- the code only needs to publish one or two values

- there is no need to cache a table reference in a subsystem or helper object

## `wpi::telemetry::TelemetryTable`

`wpi::telemetry::TelemetryTable` is the main C++ abstraction for grouped telemetry. A table corresponds to a path such as `/Drive/` or `/PoseEstimator/Vision/`.

```cpp
class TelemetryTable final {
 public:
  std::string_view GetPath() const;

  bool SetType(std::string_view typeString);
  std::string GetType() const;
  bool HasType() const;

  TelemetryTable& GetTable(std::string_view name) const;

  void KeepDuplicates(std::string_view name);
  void SetProperty(std::string_view name, std::string_view key,
                   std::string_view value);

  template <typename T, typename... I>
  void Log(std::string_view name, const T& value, I... info);

  template <typename T, typename... I>
  void Log(std::string_view name, std::span<const T> value, I... info);

  void Log(std::string_view name, bool value);
  void Log(std::string_view name, int8_t value);
  void Log(std::string_view name, int16_t value);
  void Log(std::string_view name, int32_t value);
  void Log(std::string_view name, int64_t value);
  void Log(std::string_view name, float value);
  void Log(std::string_view name, double value);
  void Log(std::string_view name, std::string_view value);
  void Log(std::string_view name, std::string_view value,
           std::string_view typeString);

  void Log(std::string_view name, std::span<const bool> value);
  void Log(std::string_view name, std::initializer_list<bool> value);
  void Log(std::string_view name, std::span<const int16_t> value);
  void Log(std::string_view name, std::initializer_list<int16_t> value);
  void Log(std::string_view name, std::span<const int32_t> value);
  void Log(std::string_view name, std::initializer_list<int32_t> value);
  void Log(std::string_view name, std::span<const int64_t> value);
  void Log(std::string_view name, std::initializer_list<int64_t> value);
  void Log(std::string_view name, std::span<const float> value);
  void Log(std::string_view name, std::initializer_list<float> value);
  void Log(std::string_view name, std::span<const double> value);
  void Log(std::string_view name, std::initializer_list<double> value);
  void Log(std::string_view name, std::span<const std::string> value);
  void Log(std::string_view name,
           std::span<const std::string_view> value);
  void Log(std::string_view name, std::span<const uint8_t> value);
  void Log(std::string_view name, std::span<const uint8_t> value,
           std::string_view typeString);
};
```

Typical subsystem pattern:

```cpp
class Shooter {
 public:
  void Periodic() {
    m_telemetry.Log("wheelSpeedRps", m_encoder.GetRate());
    m_telemetry.Log("appliedVoltage", m_lastVoltage.value());
    m_telemetry.Log("ready", AtSpeed());
  }

 private:
  wpi::telemetry::TelemetryTable& m_telemetry = wpi::telemetry::GetTable("Shooter");
  wpi::Encoder m_encoder{0, 1};
  wpi::units::volts<> m_lastVoltage = 0_V;
};
```

Nested tables are used when the hierarchy itself is meaningful:

```cpp
auto& estimator = wpi::telemetry::GetTable("PoseEstimator");
auto& vision = estimator.GetTable("Vision");
auto& odometry = estimator.GetTable("Odometry");

vision.Log("visibleTags", static_cast<int32_t>(tagIds.size()));
odometry.Log("pose", estimatedPose);
```

`SetType()` is used when the table represents a known structured kind of object and consumers should interpret that table consistently:

```cpp
auto& mechanism = wpi::telemetry::GetTable("ArmMechanism");
if (mechanism.SetType("Mechanism2d")) {
  mechanism.Log("layout", mechanismJson, "json");
}
```

## How `TelemetryTable::Log()` Selects Behavior

The C++ API is designed so users usually write the same call shape regardless of the value type:

```cpp
table.Log("name", value);
```

From a user's perspective, that call works as follows:

- if the type supports table-style telemetry via `LogTo()` and a type name via `GetTelemetryTypeName()`, it is logged into a child table after checking or setting that table type

- if the type supports table-style telemetry via `LogTo()` without a type name, it is logged into a child table

- if the type supports value-style telemetry via `LogValueTo()`, it is logged as a single named value

- if the type is `StructSerializable`, the struct schema is registered with the selected backend, then the value is packed and logged as raw bytes with the corresponding struct type string

- if the type is `ProtobufSerializable`, the protobuf schema is registered with the selected backend, then the value is packed and logged as raw bytes with the corresponding protobuf type string

- integral values are widened to `int64_t`, and floating-point values are logged as `double`, unless a more specific overload such as `int16_t`, `int32_t`, or `float` is selected

- string-like types are logged as strings

- span-convertible containers and bounded arrays are routed to the array logging path

- `fmt`-formattable scalar types fall back to formatted strings

This means the common case remains simple while still allowing strong customization for user-defined types.

## Arrays and `std::span`

In C++, array-style logging is based on `std::span`, not Java arrays.

Primitive arrays:

```cpp
std::array<double, 3> wheelSpeeds{left, right, average};
wpi::telemetry::Log("wheelSpeeds", std::span{wheelSpeeds});

auto& table = wpi::telemetry::GetTable("Drive");
table.Log("setpoints", {1.0, 2.0, 3.0});
```

String arrays:

```cpp
std::array<std::string_view, 3> states{"Idle", "Aiming", "Shooting"};
wpi::telemetry::Log("availableStates", std::span{states});
```

Raw bytes with a custom type string:

```cpp
std::span<const uint8_t> packet = GetSerializedFrame();
wpi::telemetry::Log("cameraFrame", packet, "image/jpeg");
```

For arrays of custom objects, the most natural path is usually a `std::span<const T>` where `T` is struct-serializable. Struct arrays are encoded as raw bytes after schema registration. Spans of string-formattable element types can fall back to string arrays; unsupported element types fail at compile time.

```cpp
std::array<wpi::Pose2d, 2> poses{startPose, goalPose};
wpi::telemetry::Log("waypoints", std::span{poses});
```

## Logging Structured Types with `Struct` and `Protobuf`

C++ users do not typically pass serializer objects manually. Instead, the normal pattern is to make the type satisfy the corresponding WPILib serialization concept and then call `Log()` directly.

Example with a struct-serializable type:

```cpp
wpi::Pose2d pose = estimator.GetEstimatedPosition();
wpi::telemetry::Log("estimatedPose", pose);
```

Example with a protobuf-serializable type:

```cpp
MyProtoCompatibleType message = BuildMessage();
wpi::telemetry::Log("visionResult", message);
```

When logging those values, the telemetry layer ensures the corresponding schema is registered with the selected backend and then emits the encoded bytes through `LogRaw()`.

Additional template parameters can be forwarded when a struct serializer requires type info:

```cpp
table.Log("sample", value, info1, info2);
```

From a user's perspective, these extra arguments are part of the type-specific serialization contract for that type.

## Publishing Complex Objects with `wpi::telemetry::TelemetryLoggable`

For non-final classes or types that naturally own a telemetry schema, subclassing `wpi::telemetry::TelemetryLoggable` is the most direct C++ pattern.

```cpp
class DriveSnapshot : public wpi::telemetry::TelemetryLoggable {
 public:
  DriveSnapshot(const wpi::DifferentialDriveWheelSpeeds& wheelSpeeds,
                const wpi::Pose2d& pose, bool closedLoop)
      : m_wheelSpeeds{wheelSpeeds}, m_pose{pose}, m_closedLoop{closedLoop} {}

  std::string_view GetTelemetryType() const override {
    return "DriveSnapshot";
  }

  void LogTo(wpi::telemetry::TelemetryTable& table) const override {
    table.Log("leftMetersPerSecond", m_wheelSpeeds.left.value());
    table.Log("rightMetersPerSecond", m_wheelSpeeds.right.value());
    table.Log("pose", m_pose);
    table.Log("closedLoop", m_closedLoop);
  }

 private:
  wpi::DifferentialDriveWheelSpeeds m_wheelSpeeds;
  wpi::Pose2d m_pose;
  bool m_closedLoop;
};
```

Logging it is just:

```cpp
DriveSnapshot snapshot{m_drive.GetWheelSpeeds(), m_drive.GetPose(), m_drive.IsClosedLoop()};
wpi::telemetry::Log("drive", snapshot);
```

That call creates or reuses a `/drive/` child table, checks the table type if one is provided, and then lets the object populate entries under that table.

## Publishing Custom Types with ADL Hooks

One of the most important differences in the C++ API is that users do not need to inherit from a base class just to make a type loggable. A type can participate in telemetry by defining non-member functions in the same namespace so they are found by argument-dependent lookup.

There are three relevant patterns.

### Pattern 1: Table-style logging via `LogTo()`

Use this when the type expands into multiple fields under a child table.

```cpp
namespace example {

struct VisionMeasurement {
  wpi::Pose2d pose;
  double timestamp;
  int tagCount;
};

inline std::string_view GetTelemetryTypeName(const VisionMeasurement&) {
  return "VisionMeasurement";
}

inline void LogTo(wpi::telemetry::TelemetryTable& table, const VisionMeasurement& value) {
  table.Log("pose", value.pose);
  table.Log("timestamp", value.timestamp);
  table.Log("tagCount", value.tagCount);
}

}  // namespace example
```

Usage:

```cpp
example::VisionMeasurement measurement{pose, timestamp, tagCount};
wpi::telemetry::Log("latestVision", measurement);
```

### Pattern 2: Value-style logging via `LogValueTo()`

Use this when a type should be emitted as a single value rather than a table.

```cpp
namespace example {

struct RgbColor {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

inline void LogValueTo(wpi::telemetry::TelemetryTable& table, std::string_view name,
                       const RgbColor& value) {
  table.Log(name,
            fmt::format("#{:02X}{:02X}{:02X}", value.r, value.g, value.b),
            "color");
}

}  // namespace example
```

Usage:

```cpp
example::RgbColor color{255, 128, 0};
wpi::telemetry::Log("ledColor", color);
```

### Pattern 3: Class member functions without inheritance

The header also provides bridge helpers so a class with member functions named `LogTo()` and `GetTelemetryType()` can work without explicitly inheriting from `wpi::telemetry::TelemetryLoggable`.

```cpp
class SuperstructureSnapshot {
 public:
  std::string_view GetTelemetryType() const { return "SuperstructureSnapshot"; }

  void LogTo(wpi::telemetry::TelemetryTable& table) const {
    table.Log("elevatorHeight", m_elevatorHeight);
    table.Log("armAngle", m_armAngle);
  }

 private:
  double m_elevatorHeight;
  double m_armAngle;
};
```

Because the required member functions exist, `wpi::telemetry::Log("superstructure", snapshot);` works without additional glue.

## Duplicate Preservation and Properties

The C++ API exposes the same per-entry controls as the Java facade, but in C++ spelling.

```cpp
wpi::telemetry::KeepDuplicates("loopOverrunCount");
wpi::telemetry::SetProperty("loopOverrunCount", "unit", "\"count\"");
wpi::telemetry::Log("loopOverrunCount", overrunCount);
```

Or from a table:

```cpp
auto& drive = wpi::telemetry::GetTable("Drive");
drive.KeepDuplicates("leftVelocity");
drive.SetProperty("leftVelocity", "unit", "\"m/s\"");
drive.Log("leftVelocity", leftVelocity);
```

Property values are strings. When the backend expects JSON-formatted property values, callers are responsible for passing a valid JSON string representation.

## Thread Safety

The namespace functions in `wpi::telemetry` and `wpi::telemetry::TelemetryTable` may be used from the main robot loop and secondary user threads at the same time when they route to WPILib-provided backends. There is no periodic update call or main-thread affinity. Concurrent calls to the same path are serialized where required by the backend, but their output order is unspecified.

The values passed through `std::span`, `std::string_view`, references, serializers, and ADL hooks remain owned by the caller. They must stay alive and must not be modified concurrently for the duration of the call. A backend must copy any non-owning data it wants to retain after the call returns.

`LogTo()` and `wpi::telemetry::TelemetryLoggable::LogTo()` can issue several table calls and are not atomic as a group. For a coherent multi-field sample, make an immutable copy of the application state and log that copy. Do not rely on a table mutex to protect the object being inspected.

Custom `wpi::telemetry::TelemetryBackend` and `wpi::telemetry::TelemetryEntry` implementations must support concurrent calls. In particular, `GetEntry()`, schema operations, entry logging and metadata methods, and `RemoveEntry()` can overlap during logging or backend rerouting. A previously returned entry may still be in use when its path is removed. The WPILib-provided backends satisfy this contract.

The C++ mock backend protects writes, but `GetActions()`, `GetLastAction()`, and `GetSchema()` expose references or pointers whose lifetime is not protected after the method returns. Quiesce logging before inspecting or retaining those results; copying values with `GetLastValue<T>()` avoids retaining an internal value.

## C++ Migration from WPILib 2026

For values that were only displayed with `frc::SmartDashboard::Put*()`, use `wpi::telemetry::Log()` or a `wpi::telemetry::TelemetryTable`. For values that were displayed and then read back with `frc::SmartDashboard::Get*()` so the dashboard could change robot behavior, use `wpi::tunables::Tunable` instead.

### SmartDashboard Output to Telemetry

**Was (WPILib 2026):**

```cpp
void RobotPeriodic() {
  frc::SmartDashboard::PutNumber("Drive/leftVelocity",
                                 m_leftEncoder.GetRate());
  frc::SmartDashboard::PutNumber("Drive/rightVelocity",
                                 m_rightEncoder.GetRate());
  frc::SmartDashboard::PutBoolean("Drive/ready", AtSpeed());
}
```

**Is (Telemetry):**

```cpp
wpi::telemetry::TelemetryTable& m_driveTelemetry = wpi::telemetry::GetTable("Drive");

void RobotPeriodic() {
  m_driveTelemetry.Log("leftVelocity", m_leftEncoder.GetRate());
  m_driveTelemetry.Log("rightVelocity", m_rightEncoder.GetRate());
  m_driveTelemetry.Log("ready", AtSpeed());
}
```

### Structured Dashboard Values

**Was (WPILib 2026):**

```cpp
void RobotPeriodic() {
  auto pose = m_poseEstimator.GetEstimatedPosition();
  std::array<double, 3> robotPose{
      pose.X().value(),
      pose.Y().value(),
      pose.Rotation().Radians().value(),
  };
  frc::SmartDashboard::PutNumberArray("RobotPose", robotPose);
}
```

**Is (Telemetry):**

```cpp
void RobotPeriodic() {
  wpi::telemetry::Log("RobotPose", m_poseEstimator.GetEstimatedPosition());
}
```

### Complex Sendable Values

For objects that were previously published once with `frc::SmartDashboard::PutData()`, log the object periodically instead. This keeps the dashboard value refreshed through the Telemetry backend.

**Was (WPILib 2026):**

```cpp
frc::Field2d m_field;

void RobotInit() {
  frc::SmartDashboard::PutData("Field", &m_field);
}

void RobotPeriodic() {
  m_field.SetRobotPose(m_poseEstimator.GetEstimatedPosition());
}
```

**Is (Telemetry):**

```cpp
wpi::Field2d m_field;

void RobotPeriodic() {
  m_field.SetRobotPose(m_poseEstimator.GetEstimatedPosition());
  wpi::telemetry::Log("Field", m_field);
}
```

### SmartDashboard Tuning to Tunable

If the old code used `GetNumber()` or another `Get*()` call to let dashboard changes feed back into robot behavior, migrate that value to the Tunable API instead of Telemetry.

**Was (WPILib 2026):**

```cpp
double m_intakeSpeed = 0.65;

void RobotPeriodic() {
  frc::SmartDashboard::PutNumber("Intake/speed", m_intakeSpeed);
  m_intakeSpeed =
      frc::SmartDashboard::GetNumber("Intake/speed", m_intakeSpeed);
  m_intakeMotor.Set(m_intakeSpeed);
}
```

**Is (Tunable):**

```cpp
wpi::tunables::TunableDouble m_intakeSpeed =
    wpi::tunables::Add<double>("Intake/speed", 0.65);

void RobotPeriodic() {
  m_intakeMotor.Set(m_intakeSpeed.Get());
}
```

## Advanced: User-Facing Registry APIs

Most robot code does not need `wpi::telemetry::TelemetryRegistry`, but advanced C++ users may interact with it in a few cases:

- registering a telemetry backend for a path prefix

- overriding warning reporting for tests or diagnostics

- resetting the registry in unit tests

Relevant API:

```cpp
class TelemetryRegistry final {
 public:
  static void SetReportWarning(
      std::function<void(std::string_view path, std::string_view msg)> func);
  static std::function<void(std::string_view path, std::string_view msg)>
      GetReportWarning();
  static void ReportWarning(std::string_view path, std::string_view msg);
  static void RegisterBackend(std::string_view prefix,
                              std::shared_ptr<TelemetryBackend> backend);
  static std::shared_ptr<TelemetryBackend> GetBackend(std::string_view path);
  static std::shared_ptr<wpi::telemetry::TelemetryEntry> GetEntry(std::string_view path);
  static wpi::telemetry::TelemetryTable& GetTable(std::string_view path);
  static void Reset();

  static bool HasSchema(TelemetryBackend& backend,
                        std::string_view schemaName);
  static void AddSchema(TelemetryBackend& backend,
                        std::string_view schemaName,
                        std::string_view type,
                        std::span<const uint8_t> schema);
  static void AddSchema(TelemetryBackend& backend,
                        std::string_view schemaName,
                        std::string_view type,
                        std::string_view schema);

  template <wpi::util::ProtobufSerializable T>
  static void AddProtobufSchema(TelemetryBackend& backend,
                                wpi::util::ProtobufMessage<T>& msg);

  template <typename T, typename... I>
    requires wpi::util::StructSerializable<T, I...>
  static void AddStructSchema(TelemetryBackend& backend, const I&... info);
};
```

Typical test setup:

```cpp
wpi::telemetry::TelemetryRegistry::Reset();
wpi::telemetry::TelemetryRegistry::RegisterBackend("/", std::make_shared<wpi::telemetry::MockTelemetryBackend>());
```

`RegisterBackend()` stores a `std::shared_ptr<TelemetryBackend>`, uses longest-prefix matching, removes affected entries from their previous backend via `RemoveEntry()`, and clears cached table entries so future logs use the updated mapping. `Reset()` clears registered backends, removes cached entries from their backends, and resets cached table entries; backend object cleanup happens through normal `shared_ptr` lifetime. `GetEntry()` and `GetBackend()` report a warning and return a discard entry or backend if no backend matches a normalized path. A `GetEntry()` result owns a shared handle that keeps both the entry and its backend alive while the entry is used.

Warning handlers installed with `SetReportWarning()` must not throw.

## Backend Layer: `wpi::telemetry::TelemetryEntry`

Most C++ users will never implement `wpi::telemetry::TelemetryEntry`, but it is the interface used by telemetry backends to receive resolved values.

```cpp
class TelemetryEntry {
 public:
  virtual bool IsDiscard() const { return false; }

  virtual void KeepDuplicates() = 0;
  virtual void SetProperty(std::string_view key, std::string_view value) = 0;

  virtual void LogBoolean(bool value) = 0;
  virtual void LogInt8(int8_t value) { LogInt64(value); }
  virtual void LogInt16(int16_t value) { LogInt64(value); }
  virtual void LogInt32(int32_t value) { LogInt64(value); }
  virtual void LogInt64(int64_t value) = 0;
  virtual void LogFloat(float value) = 0;
  virtual void LogDouble(double value) = 0;
  virtual void LogString(std::string_view value,
                         std::string_view typeString) = 0;
  virtual void LogBooleanArray(std::span<const bool> value) = 0;
  virtual void LogBooleanArray(std::span<const int> value) = 0;
  virtual void LogInt16Array(std::span<const int16_t> value) = 0;
  virtual void LogInt32Array(std::span<const int32_t> value) = 0;
  virtual void LogInt64Array(std::span<const int64_t> value) = 0;
  virtual void LogFloatArray(std::span<const float> value) = 0;
  virtual void LogDoubleArray(std::span<const double> value) = 0;
  virtual void LogStringArray(std::span<const std::string> value) = 0;
  virtual void LogStringArray(std::span<const std::string_view> value) = 0;
  virtual void LogRaw(std::span<const uint8_t> value,
                      std::string_view typeString) = 0;
};
```

This layer is primarily relevant to backend authors rather than normal robot-code authors. `TelemetryEntry` implementations must not throw from logging, metadata, or discard-checking methods; recoverable failures should be reported through `TelemetryRegistry::ReportWarning()` and skipped.

`wpi::telemetry::TelemetryBackend` also owns schema publication in C++:

```cpp
class TelemetryBackend {
 public:
  virtual std::shared_ptr<TelemetryEntry> GetEntry(std::string_view path) = 0;
  virtual void RemoveEntry(std::string_view path) {}
  virtual bool HasSchema(std::string_view schemaName) const = 0;
  virtual void AddSchema(std::string_view schemaName,
                         std::string_view type,
                         std::span<const uint8_t> schema) = 0;
  virtual void AddSchema(std::string_view schemaName,
                         std::string_view type,
                         std::string_view schema) = 0;
};
```

`TelemetryBackend` methods must not throw. Recoverable entry creation or schema publication failures should be reported through `TelemetryRegistry::ReportWarning()` and skipped, or represented with a discard entry.

## Typical C++ Usage Guidance

- Use `wpi::telemetry::Log()` for isolated values.

- Hold a `wpi::telemetry::TelemetryTable&` in subsystem code when publishing related values every loop.

- Prefer `LogTo()`/`GetTelemetryTypeName()` or `wpi::telemetry::TelemetryLoggable` for multi-field custom objects.

- Prefer struct/protobuf support for values that already have WPILib serialization support.

- Use `std::span` or initializer lists for arrays.

- Reach for `wpi::telemetry::TelemetryRegistry` and `wpi::telemetry::TelemetryEntry` only when implementing advanced integration points such as custom backends or test harnesses.
