# Tunable C++ API

This file documents the C++ API as a C++ user would consume it.

- For the shared Tunable design and Java-facing API, see [Tunable design](tunable.md).
- For Python-specific behavior, see [Python API](python.md).

## C++ Overview

- The primary value type is `wpi::tunable::Tunable<T>`.
- For common primitive and container types, aliases are provided (`wpi::tunable::TunableBool`, `wpi::tunable::TunableInt32`, `wpi::tunable::TunableInt64`, `wpi::tunable::TunableFloat`, `wpi::tunable::TunableDouble`, `wpi::tunable::TunableString`, `wpi::tunable::TunableRaw`, and vector aliases).
- Values are typically read/written using assignment and implicit conversion operators.
- Complex object publishing uses `wpi::tunable::ComplexTunable` plus `wpi::tunable::TunableTable`.

```cpp
wpi::tunable::TunableDouble kP{0.05};
kP = 0.1;
double p = kP;
```

## C++ Core Types

### `wpi::tunable::Tunable<T>`

`wpi::tunable::Tunable<T>` is a template with compile-time adaptation:

- value storage for value types
- custom adapters via `CustomTunable<T>` or `GetCustomTunable()` ADL
- struct serialization for `StructSerializable` types
- protobuf serialization for `ProtobufSerializable` types

Common aliases:

```cpp
using wpi::tunable::TunableBool;
using wpi::tunable::TunableInt32;
using wpi::tunable::TunableInt64;
using wpi::tunable::TunableFloat;
using wpi::tunable::TunableDouble;
using wpi::tunable::TunableString;
using wpi::tunable::TunableRaw;
using wpi::tunable::TunableBoolVector;
using wpi::tunable::TunableInt32Vector;
using wpi::tunable::TunableInt64Vector;
using wpi::tunable::TunableFloatVector;
using wpi::tunable::TunableDoubleVector;
using wpi::tunable::TunableStringVector;
```

### `wpi::tunable::TunableConfig`

`wpi::tunable::TunableConfig` is a mutable struct, including:

- `properties` (`wpi::util::json` object)
- `robust`
- `typeString`
- `isMutable`
- `onTune` (`std::function<void(wpi::tunable::detail::TunableBase&, wpi::tunable::ComplexTunable*)>`)
- `onRemoteSet` (`std::function<void(wpi::tunable::detail::TunableBase&, wpi::tunable::ComplexTunable*)>`)
- `parent`
- `polling`

`TunableConfig::GetOnChange()` and `TunableConfig::AlwaysGet()` construct configs with the corresponding polling mode preselected.

```cpp
wpi::tunable::TunableConfig config;
config.robust = true;
config.isMutable = true;
config.properties["min"] = 0;
config.properties["max"] = 1;
config.polling = wpi::tunable::TunableConfig::Polling::ALWAYS_GET;
config.onTune = [](wpi::tunable::detail::TunableBase& tunable, wpi::tunable::ComplexTunable* parent) {
  // Called from wpi::tunable::TunableRegistry::Update().
};
```

The callback receives the tuned object and the move-tracked parent pointer. Do not capture a tunable or its owning `ComplexTunable` by `this` for callbacks that must survive moves; use the callback parameters instead. `onTune` callbacks must not throw.

`onRemoteSet` runs after a backend applies a remote value but before the backend publishes any resulting value. It is mainly for language bindings and adapter code that must reconcile getter/setter-backed values before a robust backend echoes the tune; normal user notifications should use `onTune`. `onRemoteSet` callbacks must not throw.

### `wpi::tunable::Tunables`

Root-level helper entry point. The root facade mirrors the hierarchical
publishing surface of `wpi::tunable::TunableTable`, except it does not expose
`TunableTable::GetPath()`.

```cpp
class Tunables final {
 public:
  static wpi::tunable::TunableTable GetTable();
  static wpi::tunable::TunableTable GetTable(std::string_view name);

  template <typename T, typename... Args>
  static wpi::tunable::Tunable<T> Add(std::string_view name, Args&&... params);

  template <std::derived_from<wpi::tunable::ComplexTunable> T, typename... Args>
  static T AddComplex(std::string_view name, Args&&... args);

  static bool Publish(std::string_view name, wpi::tunable::detail::TunableBase& tunable);
  static bool Publish(std::string_view name, wpi::tunable::ComplexTunable& tunable);

  template <typename T, typename... I>
    requires wpi::tunable::detail::IsCustomTunable<T, I...>
  static bool Publish(std::string_view name, wpi::tunable::Tunable<T, I...>& tunable);

  static bool Publish(std::string_view name, wpi::tunable::ComplexTunable* tunable,
                      std::unique_ptr<wpi::tunable::detail::TunableMemberBase> member);

  template <typename T, std::derived_from<wpi::tunable::ComplexTunable> Class, typename... I>
  static bool Publish(std::string_view name, Class* tunable, T Class::*member, I&&... info);

  template <typename T, std::derived_from<wpi::tunable::ComplexTunable> Class, typename... I>
  static bool Publish(std::string_view name, Class* tunable, T Class::*member,
                      const wpi::tunable::TunableConfig& config, I&&... info);

  static void Remove(std::string_view name);
};
```

`GetTable(name)`, `Publish()`, and `Remove()` delegate through the root table, so
root-level and child-table paths are normalized consistently.

Direct `Publish()` methods return `true` when the backend accepts the publication and `false` when it rejects it, such as for a duplicate path.

`AddComplex<T>()` constructs and returns a value; it does not take an existing object parameter.

### `wpi::tunable::TunableTable`

Hierarchical publishing API:

```cpp
class TunableTable final {
 public:
  const std::string& GetPath() const;
  TunableTable GetTable(std::string_view name);
  bool Publish(std::string_view name, wpi::tunable::detail::TunableBase& tunable);
  bool Publish(std::string_view name, wpi::tunable::ComplexTunable& tunable);

  template <typename T, typename... I>
    requires wpi::tunable::detail::IsCustomTunable<T, I...>
  bool Publish(std::string_view name, wpi::tunable::Tunable<T, I...>& tunable);

  bool Publish(std::string_view name, wpi::tunable::ComplexTunable* tunable,
               std::unique_ptr<wpi::tunable::detail::TunableMemberBase> member);

  template <typename T, std::derived_from<wpi::tunable::ComplexTunable> Class, typename... I>
  bool Publish(std::string_view name, Class* tunable, T Class::*member, I&&... info);

  template <typename T, std::derived_from<wpi::tunable::ComplexTunable> Class, typename... I>
  bool Publish(std::string_view name, Class* tunable, T Class::*member,
               const wpi::tunable::TunableConfig& config, I&&... info);

  void Remove(std::string_view name);
};
```

Unlike Java, there are no `PublishDouble()` convenience methods; publish typed tunables directly.

### `wpi::tunable::TunableRegistry` and `wpi::tunable::TunableBackend`

`wpi::tunable::TunableRegistry` owns the mapping from normalized paths to registered backends and tracks tunables by UID so moved C++ objects continue to point at the right tunable.

```cpp
class TunableRegistry final {
 public:
  struct TunableInfo {
    wpi::tunable::detail::TunableBase* tunable;
    const wpi::tunable::TunableConfig* config;
    wpi::tunable::detail::TunableTypeValue type;

    explicit operator bool() const;
    bool IsChanged() const;
    void ResetChanged();
  };

  static void RegisterBackend(std::string_view prefix,
                              std::shared_ptr<wpi::tunable::TunableBackend> backend);
  static std::shared_ptr<wpi::tunable::TunableBackend> GetBackend(std::string_view path);
  static std::string_view NormalizeName(std::string_view path, std::string& buf);
  static bool Publish(std::string_view path, wpi::tunable::detail::TunableBase& tunable);
  static bool Publish(std::string_view path, wpi::tunable::ComplexTunable& tunable);
  static bool Publish(std::string_view path, wpi::tunable::ComplexTunable* tunable,
                      std::unique_ptr<wpi::tunable::detail::TunableMemberBase> member);
  static void Remove(std::string_view path);
  static void Update();
  static void ResetChangedAfterUpdate(uint32_t uid);
  // callback must not throw
  static void RunAfterUpdate(std::function<void()> callback);
  static wpi::util::recursive_mutex& GetUpdateMutex();
  static void Reset();
  static TunableInfo GetTunable(uint32_t uid);
};

class TunableBackend {
 public:
  struct PublishedTunable {
    std::string path;
    uint32_t uid;
  };

  virtual void Retire() {}
  virtual bool Publish(std::string_view path, uint32_t uid,
                       wpi::tunable::detail::TunableBase& tunable,
                       const wpi::tunable::TunableConfig* config,
                       wpi::tunable::detail::TunableTypeValue type) = 0;
  virtual void Remove(std::string_view path) = 0;
  virtual std::vector<PublishedTunable> RemovePrefix(
      std::string_view prefix) = 0;
  virtual void UnregisterTunable(uint32_t uid) = 0;
  virtual void Update() = 0;
};
```

`RegisterBackend()` takes a `std::shared_ptr<TunableBackend>` and uses longest-prefix matching. `GetBackend()` reports a warning and returns a noop backend if no backend matches. If a backend is registered after tunables have already been published, existing tunables whose longest matching prefix now resolves to the new backend are removed from their previous backend and republished by UID to the new backend. Displaced C++ backends that are no longer registered are `Retire()`d. `RobotBase` calls `TunableRegistry::Update()` from the main loop, but backend registration is separate.

Warning handlers installed with `SetReportWarning()`, callbacks queued with `RunAfterUpdate()`, and backend `Retire()`, publish, removal, dirty-marking, unregister, and update methods must not throw. Recoverable backend failures should be reported through `TunableRegistry::ReportWarning()` and skipped so the robot loop and registry state can continue updating.

### `wpi::tunable::ComplexTunable`

Implement this for composite tunables:

```cpp
class ComplexTunable : public wpi::tunable::detail::TunableBase {
 public:
  virtual std::string_view GetTunableType() const { return {}; }
  virtual void PublishTunable(wpi::tunable::TunableTable& table) = 0;
  virtual void UpdateTunable() const {}

 protected:
  void PublishChildTunable(std::string_view name, wpi::tunable::detail::TunableBase& tunable);
  void PublishChildTunable(std::string_view name, wpi::tunable::ComplexTunable& tunable);
  void RemoveChildTunable(std::string_view name);
  void SetChildTunableChanged(std::string_view name);

  virtual ~ComplexTunable() = default;
};
```

The protected child helpers mirror the Java `ComplexTunable` helpers: they can publish, remove, or mark a child changed under every currently published path for the complex object. `PublishTunable()` should still publish the complete current child set so migration and full republish operations can recreate it. `PublishTunable()` and `UpdateTunable()` must not throw; if they do, registry and backend state is not guaranteed to be restored.

### `wpi::tunable::Selectable<T>`

Chooser-style complex tunable:

```cpp
template <class T>
class Selectable final : public wpi::tunable::detail::SelectableBase {
 public:
  void Add(std::string_view name, T object);
  void AddDefault(std::string_view name, T object);
  void Remove(std::string_view name);
  void SetDefault(std::string_view name);
  void Clear();
  CopyType GetSelected() const;
  void OnChange(std::function<void(T)> listener);
};
```

`Selectable<T>` requires `T` to be copy-constructible and default-initializable. If `T` is `std::shared_ptr<U>`, `GetSelected()` returns `std::weak_ptr<U>`. If no selected or default option exists, it returns a value-initialized `CopyType`.

## Thread Safety and Secondary Threads

Tunable objects, complex tunables, and `wpi::tunable::Selectable<T>` are not internally thread-safe. The default model is single-threaded access from the main robot loop. `wpi::tunable::TunableRegistry::Update()` holds one recursive update mutex while it updates complex tunables and backends, resets changes, and invokes callbacks, but the framework does not hold that mutex around user `Periodic()` methods.

Occasional secondary-thread access is supported by locking `wpi::tunable::TunableRegistry::GetUpdateMutex()`. Every thread that might compete for the same tunable or its callback-backed state must use the same mutex, including the main robot thread:

```cpp
double gainCopy;
{
  std::scoped_lock lock{wpi::tunable::TunableRegistry::GetUpdateMutex()};
  driveGain.Set(0.08);
  gainCopy = driveGain.Get();
}
```

For a string, vector, struct, or custom type, copy the value while the lock is held. `Get()` and `Mutate()` can return references; the lock no longer protects a reference after it is released. Moving or destroying a published C++ tunable also changes registry-held raw pointers and must not race with `Update()` or another access. Keep published tunables at a stable lifetime, or perform move and destruction under the update mutex once concurrent updates have started.

Keep critical sections short. The mutex is global and `Update()` holds it across backend work, custom getters/setters, complex updates, and callbacks, so contention can delay both the worker and the robot loop. Do not wait for I/O or another thread while holding it. If these functions also acquire application locks, use a consistent order with the update mutex to avoid deadlock.

For a high-rate worker, use `std::atomic` for scalar handoff, an atomic immutable snapshot such as `std::atomic<std::shared_ptr<const Config>>` for compound state, or a queue consumed by the main loop. Have the worker access that handoff rather than the `wpi::tunable::Tunable` itself. Apply queued worker changes to the tunable on the main thread and publish tuned values back through the atomic snapshot. This avoids a global registry lock in the worker hot path at the cost of up to one robot-loop iteration of latency.

Polling and mutability options do not change this contract. `ALWAYS_GET` controls backend polling and `isMutable = false` prevents remote writes; neither synchronizes the value. Callbacks run on the thread that invokes `Update()`, normally the main robot thread. Concurrent `Update()` calls serialize, but calling it from a worker changes callback thread affinity and is not a replacement for a safe handoff.

## C++ Usage Examples

### Simple tunable constants

```cpp
class DriveSubsystem {
 public:
  wpi::tunable::TunableDouble kP = wpi::tunable::Tunables::Add<double>("drive/kP", 0.05);
  wpi::tunable::TunableDouble kI = wpi::tunable::Tunables::Add<double>("drive/kI", 0.0);
  wpi::tunable::TunableDouble kD = wpi::tunable::Tunables::Add<double>("drive/kD", 0.001);

  void Periodic() {
    pid.SetP(kP);
    pid.SetI(kI);
    pid.SetD(kD);
  }
};
```

### Configured tunable

```cpp
wpi::tunable::TunableConfig config;
config.robust = true;
config.properties["min"] = 0;
config.properties["max"] = 1;

wpi::tunable::TunableDouble tolerance{0.02, config};
wpi::tunable::Tunables::Publish("shooter/tolerance", tolerance);
```

### Struct-serializable type

```cpp
wpi::tunable::Tunable<wpi::Pose2d> targetPose{wpi::Pose2d{}};
wpi::tunable::Tunables::Publish("drive/targetPose", targetPose);
```

### Complex tunable with `Tunable` member variables

If the member is already a `Tunable` object, publish it by reference.

```cpp
class TunablePIDController : public wpi::tunable::ComplexTunable {
 public:
  TunablePIDController(double p, double i, double d)
      : m_kP{p}, m_kI{i}, m_kD{d} {}

  void PublishTunable(wpi::tunable::TunableTable& table) override {
    table.Publish("kP", m_kP);
    table.Publish("kI", m_kI);
    table.Publish("kD", m_kD);
  }

 private:
  wpi::tunable::TunableDouble m_kP;
  wpi::tunable::TunableDouble m_kI;
  wpi::tunable::TunableDouble m_kD;
};

TunablePIDController armPID{1.0, 0.0, 0.1};
wpi::tunable::Tunables::Publish("arm/pid", armPID);
```

### Complex tunable with plain member variables

If the member is a plain field rather than a `Tunable`, publish it using the pointer-to-member overload.

```cpp
class TunablePIDController : public wpi::tunable::ComplexTunable {
 public:
  TunablePIDController(double p, double i, double d)
      : m_kP{p}, m_kI{i}, m_kD{d} {}

  void PublishTunable(wpi::tunable::TunableTable& table) override {
    table.Publish("kP", this, &TunablePIDController::m_kP);
    table.Publish("kI", this, &TunablePIDController::m_kI);
    table.Publish("kD", this, &TunablePIDController::m_kD);
  }

 private:
  double m_kP;
  double m_kI;
  double m_kD;
};

TunablePIDController armPID{1.0, 0.0, 0.1};
wpi::tunable::Tunables::Publish("arm/pid", armPID);
```

### Hierarchical tables

```cpp
wpi::tunable::TunableTable arm = wpi::tunable::Tunables::GetTable("arm");
wpi::tunable::TunableTable pivot = arm.GetTable("pivot");

wpi::tunable::TunableDouble pivotSpeed{2.0};
wpi::tunable::TunableDouble pivotAccel{8.0};

pivot.Publish("maxSpeed", pivotSpeed);
pivot.Publish("maxAccel", pivotAccel);
```

## C++ Unit Testing with `wpi::tunable::MockTunableBackend`

```cpp
auto backend = std::make_shared<wpi::tunable::MockTunableBackend>();
wpi::tunable::TunableRegistry::Reset();
wpi::tunable::TunableRegistry::RegisterBackend("", backend);

DriveSubsystem drive;
backend->SetDouble("/drive/kP", 0.1);
wpi::tunable::TunableRegistry::Update();

EXPECT_NEAR(drive.GetPID().GetP(), 0.1, 1e-9);
```

`Get*()` methods read the current published value for primitive and vector tunables. `Set*()` methods queue remote writes and apply during `TunableRegistry::Update()`. The mock backend methods use C++ type names such as `GetBool()`/`SetBool()`, `GetInt32()`/`SetInt32()`, `GetInt64()`/`SetInt64()`, `GetFloat()`/`SetFloat()`, `GetDouble()`/`SetDouble()`, `GetString()`/`SetString()`, `GetRaw()`/`SetRaw()`, and corresponding vector getters/setters. Struct tunables can be read with `GetStructData()`, `GetStruct<T>()`, or `GetStructVector<T>()`; Protobuf tunables can be read with `GetProtobufData()` or `GetProtobuf<T>()`. `SetStruct()`/`SetStructVector()` and `SetProtobuf()` queue serialized remote writes.

## C++ Migration from WPILib 2026

Key differences from 2026:

- `frc::SmartDashboard::PutNumber("key", value)` / `frc::SmartDashboard::GetNumber("key", default)` called every loop is replaced with a single `wpi::tunable::Tunables::Add<double>("key", initialValue)` declaration that returns a `wpi::tunable::TunableDouble`. Read it with `Get()` or implicit conversion and write it with assignment or `Set()`.
- Direct NetworkTables entry/topic boilerplate is replaced by the same `wpi::tunable::Tunable<T>` pattern; the backend handles the underlying NT entry lifecycle.
- `frc::SendableChooser<T>` is replaced by `wpi::tunable::Selectable<T>`. The API is similar: `Add(name, object)`, `AddDefault(name, object)`, `GetSelected()`.
- The `Sendable` interface and `frc::SmartDashboard::PutData()` are not part of the Tunable API; subsystems and mechanisms that previously implemented `Sendable` should implement `wpi::tunable::ComplexTunable` and register via `wpi::tunable::Tunables::Publish()`.

### SmartDashboard Tuning to Tunable

Use Tunable when the dashboard is allowed to change the value and robot code reads that value back.

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
wpi::tunable::TunableDouble m_intakeSpeed =
    wpi::tunable::Tunables::Add<double>("Intake/speed", 0.65);

void RobotPeriodic() {
  m_intakeMotor.Set(m_intakeSpeed.Get());
}
```

For plain fields that already live in a subsystem, publish them from a `ComplexTunable` instead of manually writing and reading a dashboard key every loop.

**Was (WPILib 2026):**

```cpp
class DriveSubsystem {
 public:
  double GetMaxOutput() const { return m_maxOutput; }

  void SetMaxOutput(double value) {
    m_maxOutput = value;
  }

 private:
  double m_maxOutput = 0.8;
};

void RobotPeriodic() {
  frc::SmartDashboard::PutNumber("Drive/maxOutput",
                                 m_drive.GetMaxOutput());
  m_drive.SetMaxOutput(frc::SmartDashboard::GetNumber(
      "Drive/maxOutput", m_drive.GetMaxOutput()));
}
```

**Is (ComplexTunable member):**

```cpp
class DriveSubsystem : public wpi::tunable::ComplexTunable {
 public:
  double GetMaxOutput() const { return m_maxOutput; }

  void PublishTunable(wpi::tunable::TunableTable& table) override {
    table.Publish("maxOutput", this, &DriveSubsystem::m_maxOutput);
  }

 private:
  double m_maxOutput = 0.8;
};

void RobotInit() {
  wpi::tunable::Tunables::Publish("Drive", m_drive);
}
```

### Editable Field2d to Tunable

**Was (WPILib 2026):**

```cpp
frc::Field2d m_field;
frc::FieldObject2d* m_target = m_field.GetObject("Target");
wpi::math::Pose2d m_driveTargetPose;

void RobotInit() {
  frc::SmartDashboard::PutData("Field", &m_field);
}

void RobotPeriodic() {
  m_field.SetRobotPose(m_poseEstimator.GetEstimatedPosition());
  m_driveTargetPose = m_target->GetPose();
}
```

**Is (Tunable):**

```cpp
wpi::Field2d m_field;
wpi::FieldObject2d* m_target = m_field.GetObject("Target");
wpi::math::Pose2d m_driveTargetPose;

void RobotInit() {
  wpi::tunable::Tunables::Publish("Field", m_field);
}

void RobotPeriodic() {
  m_field.SetRobotPose(m_poseEstimator.GetEstimatedPosition());
  m_driveTargetPose = m_target->GetPose();
}
```

### SendableChooser to Selectable

`wpi::tunable::Selectable<T>` publishes the chooser data through the Tunable backend and returns the selected robot-owned object from `GetSelected()`.

**Was (WPILib 2026):**

```cpp
enum class DriveMode { kFieldRelative, kRobotRelative };

frc::SendableChooser<DriveMode> m_driveModeChooser;

void RobotInit() {
  m_driveModeChooser.SetDefaultOption("Field Relative",
                                      DriveMode::kFieldRelative);
  m_driveModeChooser.AddOption("Robot Relative",
                               DriveMode::kRobotRelative);
  frc::SmartDashboard::PutData("Drive Mode", &m_driveModeChooser);
}

void TeleopPeriodic() {
  m_drive.SetMode(m_driveModeChooser.GetSelected());
}
```

**Is (Selectable):**

```cpp
enum class DriveMode { kFieldRelative, kRobotRelative };

wpi::tunable::Selectable<DriveMode> m_driveMode;

void RobotInit() {
  m_driveMode.AddDefault("Field Relative", DriveMode::kFieldRelative);
  m_driveMode.Add("Robot Relative", DriveMode::kRobotRelative);
  wpi::tunable::Tunables::Publish("Drive/mode", m_driveMode);
}

void TeleopPeriodic() {
  m_drive.SetMode(m_driveMode.GetSelected());
}
```

For composite objects that are not built into WPILib, implement `wpi::tunable::ComplexTunable` and make member values `wpi::tunable::Tunable<T>` or publish non-tunable members with `table.Publish("name", this, &Class::member)`. Expect template-based diagnostics for unsupported custom types; provide `CustomTunable<T>` or serialization traits where needed.
