// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package wpilib.robot;

import java.util.concurrent.TimeUnit;
import org.openjdk.jmh.annotations.Benchmark;
import org.openjdk.jmh.annotations.BenchmarkMode;
import org.openjdk.jmh.annotations.Level;
import org.openjdk.jmh.annotations.Mode;
import org.openjdk.jmh.annotations.OperationsPerInvocation;
import org.openjdk.jmh.annotations.OutputTimeUnit;
import org.openjdk.jmh.annotations.Param;
import org.openjdk.jmh.annotations.Scope;
import org.openjdk.jmh.annotations.Setup;
import org.openjdk.jmh.annotations.State;
import org.openjdk.jmh.annotations.TearDown;
import org.openjdk.jmh.annotations.Threads;
import org.wpilib.backend.NetworkTablesTunableBackend;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.networktables.GenericEntry;
import org.wpilib.networktables.NetworkTableInstance;
import org.wpilib.telemetry.Telemetry;
import org.wpilib.telemetry.TelemetryBackend;
import org.wpilib.telemetry.TelemetryEntry;
import org.wpilib.telemetry.TelemetryLoggable;
import org.wpilib.telemetry.TelemetryRegistry;
import org.wpilib.telemetry.TelemetryTable;
import org.wpilib.tunable.TunableConfig;
import org.wpilib.tunable.TunableDouble;
import org.wpilib.tunable.TunableRegistry;
import org.wpilib.tunable.Tunables;
import org.wpilib.util.protobuf.Protobuf;
import org.wpilib.util.struct.Struct;

/** Allocation-focused benchmarks for telemetry logging and tunable updates. */
@BenchmarkMode(Mode.AverageTime)
@OutputTimeUnit(TimeUnit.NANOSECONDS)
@Threads(1)
public class TelemetryTunableAllocationBenchmark {
  private static final int TELEMETRY_ITERATIONS = 1024;
  private static final int PRIMITIVE_TELEMETRY_OPERATIONS = TELEMETRY_ITERATIONS * 6;
  private static final int ARRAY_TELEMETRY_OPERATIONS = TELEMETRY_ITERATIONS * 5;
  private static final int OBJECT_TELEMETRY_OPERATIONS = TELEMETRY_ITERATIONS * 2;
  private static final int TUNABLE_UPDATE_OPERATIONS = 1024;
  private static final int REMOTE_TUNE_OPERATIONS = 128;

  /** State for primitive telemetry benchmarks. */
  @State(Scope.Benchmark)
  public static class PrimitiveTelemetryState {
    CountingTelemetryBackend backend;

    /** Initializes telemetry backend and cached entries. */
    @Setup(Level.Trial)
    public void setup() {
      backend = setupTelemetry();
      Telemetry.log("primitiveBoolean", true);
      Telemetry.log("primitiveInteger", 1);
      Telemetry.log("primitiveLong", 2L);
      Telemetry.log("primitiveFloat", 3.0f);
      Telemetry.log("primitiveDouble", 4.0);
      Telemetry.log("primitiveString", "ready");
      backend.clear();
    }

    /** Resets telemetry global state. */
    @TearDown(Level.Trial)
    public void tearDown() {
      TelemetryRegistry.reset();
    }
  }

  /** State for array telemetry benchmarks. */
  @State(Scope.Benchmark)
  public static class ArrayTelemetryState {
    /** Array length. */
    @Param({"4", "64"})
    public int length;

    CountingTelemetryBackend backend;
    boolean[] booleans;
    int[] integers;
    double[] doubles;
    String[] strings;
    byte[] raw;

    /** Initializes telemetry backend, arrays, and cached entries. */
    @Setup(Level.Trial)
    public void setup() {
      backend = setupTelemetry();
      booleans = new boolean[length];
      integers = new int[length];
      doubles = new double[length];
      strings = new String[length];
      raw = new byte[length];

      for (int i = 0; i < length; i++) {
        booleans[i] = (i & 1) == 0;
        integers[i] = i;
        doubles[i] = i * 0.25;
        strings[i] = "value" + i;
        raw[i] = (byte) i;
      }

      Telemetry.log("arrayBooleans", booleans);
      Telemetry.log("arrayIntegers", integers);
      Telemetry.log("arrayDoubles", doubles);
      Telemetry.log("arrayStrings", strings);
      Telemetry.log("arrayRaw", raw);
      backend.clear();
    }

    /** Resets telemetry global state. */
    @TearDown(Level.Trial)
    public void tearDown() {
      TelemetryRegistry.reset();
    }
  }

  /** State for object telemetry benchmarks. */
  @State(Scope.Benchmark)
  public static class ObjectTelemetryState {
    CountingTelemetryBackend backend;
    Pose2d pose;
    LoggableValue loggable;

    /** Initializes telemetry backend, objects, and cached entries. */
    @Setup(Level.Trial)
    public void setup() {
      backend = setupTelemetry();
      pose = new Pose2d(1.0, 2.0, Rotation2d.fromDegrees(45.0));
      loggable = new LoggableValue();

      Telemetry.log("objectPose", pose);
      Telemetry.log("objectLoggable", loggable);
      backend.clear();
    }

    /** Resets telemetry global state. */
    @TearDown(Level.Trial)
    public void tearDown() {
      TelemetryRegistry.reset();
    }
  }

  /** State for idle tunable update benchmarks. */
  @State(Scope.Benchmark)
  public static class IdleTunableState {
    /** Number of published tunables. */
    @Param({"0", "10", "100", "1000"})
    public int entries;

    NetworkTableInstance inst;

    /** Initializes a NetworkTables tunable backend with unchanged simple tunables. */
    @Setup(Level.Trial)
    public void setup() {
      inst = setupNetworkTablesTunableBackend();
      for (int i = 0; i < entries; i++) {
        Tunables.publish("idle" + i, TunableDouble.create(i));
      }
      inst.flush();
      TunableRegistry.update();
    }

    /** Closes NetworkTables tunable backend state. */
    @TearDown(Level.Trial)
    public void tearDown() {
      tearDownNetworkTablesTunableBackend(inst);
    }
  }

  /** State for getter-backed tunable update benchmarks. */
  @State(Scope.Benchmark)
  public static class GetterTunableState {
    /** Number of published getter-backed tunables. */
    @Param({"10", "100", "1000"})
    public int entries;

    NetworkTableInstance inst;
    double[] values;

    /** Initializes a NetworkTables tunable backend with stable getter-backed tunables. */
    @Setup(Level.Trial)
    public void setup() {
      inst = setupNetworkTablesTunableBackend();
      values = new double[entries];
      for (int i = 0; i < entries; i++) {
        final int index = i;
        values[i] = i;
        Tunables.publish(
            "getter" + i,
            TunableDouble.createConfig(
                () -> values[index],
                value -> values[index] = value,
                new TunableConfig().withRobust(true).withAlwaysGet(true)));
      }
      inst.flush();
      TunableRegistry.update();
    }

    /** Closes NetworkTables tunable backend state. */
    @TearDown(Level.Trial)
    public void tearDown() {
      tearDownNetworkTablesTunableBackend(inst);
    }
  }

  /** State for remote tune event benchmarks. */
  @State(Scope.Benchmark)
  public static class RemoteTuneState {
    /** Number of tunables receiving remote events. */
    @Param({"1", "10"})
    public int entries;

    NetworkTableInstance inst;
    GenericEntry[] tuneEntries;
    TunableDouble[] tunables;
    private int index;
    private double nextValue;

    /** Initializes a NetworkTables tunable backend with remote tune topics. */
    @Setup(Level.Trial)
    public void setup() {
      inst = setupNetworkTablesTunableBackend();
      tuneEntries = new GenericEntry[entries];
      tunables = new TunableDouble[entries];

      for (int i = 0; i < entries; i++) {
        tunables[i] =
            TunableDouble.createConfig(
                i, new TunableConfig().withRobust(true).withAlwaysGet(false));
        Tunables.publish("remote" + i, tunables[i]);
        tuneEntries[i] = inst.getTopic("/BenchTunables/remote" + i + "/tune").getGenericEntry();
      }
      inst.flush();
      TunableRegistry.update();
    }

    void queueTune() {
      GenericEntry entry = tuneEntries[index];
      index++;
      if (index == tuneEntries.length) {
        index = 0;
      }
      entry.setDouble(++nextValue);
    }

    /** Closes NetworkTables tunable backend state. */
    @TearDown(Level.Trial)
    public void tearDown() {
      tearDownNetworkTablesTunableBackend(inst);
    }
  }

  /**
   * Benchmarks primitive telemetry logging.
   *
   * @param state benchmark state
   * @return sink value
   */
  @Benchmark
  @OperationsPerInvocation(PRIMITIVE_TELEMETRY_OPERATIONS)
  public long primitiveTelemetry(PrimitiveTelemetryState state) {
    for (int i = 0; i < TELEMETRY_ITERATIONS; i++) {
      Telemetry.log("primitiveBoolean", (i & 1) == 0);
      Telemetry.log("primitiveInteger", i);
      Telemetry.log("primitiveLong", (long) i);
      Telemetry.log("primitiveFloat", (float) i);
      Telemetry.log("primitiveDouble", (double) i);
      Telemetry.log("primitiveString", "ready");
    }
    return state.backend.sink();
  }

  /**
   * Benchmarks primitive array telemetry logging.
   *
   * @param state benchmark state
   * @return sink value
   */
  @Benchmark
  @OperationsPerInvocation(ARRAY_TELEMETRY_OPERATIONS)
  public long arrayTelemetry(ArrayTelemetryState state) {
    for (int i = 0; i < TELEMETRY_ITERATIONS; i++) {
      Telemetry.log("arrayBooleans", state.booleans);
      Telemetry.log("arrayIntegers", state.integers);
      Telemetry.log("arrayDoubles", state.doubles);
      Telemetry.log("arrayStrings", state.strings);
      Telemetry.log("arrayRaw", state.raw);
    }
    return state.backend.sink();
  }

  /**
   * Benchmarks object telemetry dispatch.
   *
   * @param state benchmark state
   * @return sink value
   */
  @Benchmark
  @OperationsPerInvocation(OBJECT_TELEMETRY_OPERATIONS)
  public long objectTelemetry(ObjectTelemetryState state) {
    for (int i = 0; i < TELEMETRY_ITERATIONS; i++) {
      Telemetry.log("objectPose", state.pose);
      Telemetry.log("objectLoggable", state.loggable);
    }
    return state.backend.sink();
  }

  /**
   * Benchmarks idle tunable updates with unchanged simple tunables.
   *
   * @param state benchmark state
   */
  @Benchmark
  @OperationsPerInvocation(TUNABLE_UPDATE_OPERATIONS)
  public void idleTunableUpdate(IdleTunableState state) {
    for (int i = 0; i < TUNABLE_UPDATE_OPERATIONS; i++) {
      TunableRegistry.update();
    }
  }

  /**
   * Benchmarks getter-backed tunable updates with stable getter values.
   *
   * @param state benchmark state
   * @return sink value
   */
  @Benchmark
  @OperationsPerInvocation(TUNABLE_UPDATE_OPERATIONS)
  public double getterTunableUpdate(GetterTunableState state) {
    for (int i = 0; i < TUNABLE_UPDATE_OPERATIONS; i++) {
      TunableRegistry.update();
    }
    return state.values[0];
  }

  /**
   * Benchmarks queued remote tune events through TunableRegistry.update().
   *
   * @param state benchmark state
   * @return sink value
   */
  @Benchmark
  @OperationsPerInvocation(REMOTE_TUNE_OPERATIONS)
  public double remoteTuneEvents(RemoteTuneState state) {
    for (int i = 0; i < REMOTE_TUNE_OPERATIONS; i++) {
      state.queueTune();
      state.inst.flush();
      TunableRegistry.update();
    }
    return state.tunables[0].get();
  }

  private static CountingTelemetryBackend setupTelemetry() {
    TelemetryRegistry.reset();
    CountingTelemetryBackend backend = new CountingTelemetryBackend();
    TelemetryRegistry.registerBackend("", backend);
    return backend;
  }

  private static NetworkTableInstance setupNetworkTablesTunableBackend() {
    TunableRegistry.reset();
    NetworkTableInstance inst = NetworkTableInstance.create();
    TunableRegistry.registerBackend("", new NetworkTablesTunableBackend(inst, "/BenchTunables"));
    return inst;
  }

  private static void tearDownNetworkTablesTunableBackend(NetworkTableInstance inst) {
    TunableRegistry.reset();
    if (inst != null) {
      inst.close();
    }
  }

  private static final class LoggableValue implements TelemetryLoggable {
    @Override
    public void logTo(TelemetryTable table) {
      table.log("enabled", true);
      table.log("value", 1.0);
    }

    @Override
    public String getTelemetryType() {
      return "BenchmarkLoggable";
    }
  }

  private static final class CountingTelemetryBackend implements TelemetryBackend {
    private final CountingTelemetryEntry m_entry = new CountingTelemetryEntry(this);
    private long m_sink;
    private Object m_objectSink;

    @Override
    public void close() {}

    @Override
    public TelemetryEntry getEntry(String path) {
      m_sink += path.length();
      return m_entry;
    }

    void clear() {
      m_sink = 0;
      m_objectSink = null;
    }

    long sink() {
      return m_sink ^ (m_objectSink == null ? 0 : System.identityHashCode(m_objectSink));
    }
  }

  private static final class CountingTelemetryEntry implements TelemetryEntry {
    private final CountingTelemetryBackend m_backend;

    CountingTelemetryEntry(CountingTelemetryBackend backend) {
      m_backend = backend;
    }

    @Override
    public void keepDuplicates() {
      m_backend.m_sink++;
    }

    @Override
    public void setProperty(String key, String value) {
      m_backend.m_sink += key.length() + value.length();
    }

    @Override
    public <T> void logStruct(T value, Struct<? super T> struct) {
      m_backend.m_objectSink = value;
      m_backend.m_sink += struct.getSize();
    }

    @Override
    public <T> void logProtobuf(T value, Protobuf<? super T, ?> proto) {
      m_backend.m_objectSink = value;
      m_backend.m_sink += proto.getTypeString().length();
    }

    @Override
    public <T> void logStructArray(T[] value, Struct<? super T> struct) {
      m_backend.m_objectSink = value;
      m_backend.m_sink += value.length + struct.getSize();
    }

    @Override
    public void logBoolean(boolean value) {
      m_backend.m_sink += value ? 1 : 0;
    }

    @Override
    public void logLong(long value) {
      m_backend.m_sink += value;
    }

    @Override
    public void logFloat(float value) {
      m_backend.m_sink += Float.floatToRawIntBits(value);
    }

    @Override
    public void logDouble(double value) {
      m_backend.m_sink += Double.doubleToRawLongBits(value);
    }

    @Override
    public void logString(String value, String typeString) {
      m_backend.m_objectSink = value;
      m_backend.m_sink += value.length() + typeString.length();
    }

    @Override
    public void logBooleanArray(boolean[] value) {
      m_backend.m_objectSink = value;
      m_backend.m_sink += value.length;
    }

    @Override
    public void logShortArray(short[] value) {
      m_backend.m_objectSink = value;
      m_backend.m_sink += value.length;
    }

    @Override
    public void logIntArray(int[] value) {
      m_backend.m_objectSink = value;
      m_backend.m_sink += value.length;
    }

    @Override
    public void logLongArray(long[] value) {
      m_backend.m_objectSink = value;
      m_backend.m_sink += value.length;
    }

    @Override
    public void logFloatArray(float[] value) {
      m_backend.m_objectSink = value;
      m_backend.m_sink += value.length;
    }

    @Override
    public void logDoubleArray(double[] value) {
      m_backend.m_objectSink = value;
      m_backend.m_sink += value.length;
    }

    @Override
    public void logStringArray(String[] value) {
      m_backend.m_objectSink = value;
      m_backend.m_sink += value.length;
    }

    @Override
    public void logRaw(byte[] value, String typeString) {
      m_backend.m_objectSink = value;
      m_backend.m_sink += value.length + typeString.length();
    }
  }
}
