package edu.wpi.first.wpilibj.telemetry;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.networktables.GenericEntry;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.networktables.NetworkTableType;
import edu.wpi.first.util.WPIUtilJNI;
import java.util.HashMap;
import java.util.Map;

/**
 * Root telemetry controller.
 *
 * <p>Telemetry data can be manipulated in two ways, using this class:
 * <li>Iterative/immediate sending values, using the {@link #putBoolean(String, boolean)}
 * <li>Declarative publishing of {@link TelemetryNode} objects.
 */
public class Telemetry {
  private static NetworkTableInstance m_instance;
  private static final Map<String, TelemetryBuilderImpl> m_builders = new HashMap<>();
  private static final Map<String, GenericEntry> m_immediateTelemetry = new HashMap<>();

  static {
    resetWithNetworkTableInstance(NetworkTableInstance.getDefault());
    // TODO: add resource type
    HAL.report(tResourceType.kResourceType_SmartDashboard, 0);
  }

  /**
   * Set the NetworkTable instance used for entries, clearing all data. For testing purposes; use
   * with caution.
   *
   * @param inst NetworkTable instance
   */
  public static synchronized void resetWithNetworkTableInstance(NetworkTableInstance inst) {
    m_instance = inst;
    m_builders.forEach((unused, builder) -> builder.close());
    m_builders.clear();

    m_immediateTelemetry.forEach(
        (unused, entry) -> {
          // TODO: do we need both of these?
          entry.unpublish();
          entry.close();
        });
    m_immediateTelemetry.clear();
  }

  /**
   * Publish a telemetry node. The node will automatically update.
   *
   * @param path the NT path the node should appear under
   * @param node the node
   */
  public static void publishNode(String path, TelemetryNode node) {
    var builder = getBuilder(path);
    node.declareTelemetry(builder);
  }

  /**
   * Send a boolean value.
   *
   * @param path the topic key the value will appear under
   * @param value the value
   */
  public static void putBoolean(String path, boolean value) {
    getImmediateEntry(path, NetworkTableType.kBoolean).setBoolean(value);
  }

  /**
   * Read a boolean value.
   *
   * @param path the topic key the value will appear under
   * @param defaultValue default value to be returned if no network value is found
   */
  public static void getBoolean(String path, boolean defaultValue) {
    getImmediateEntry(path, NetworkTableType.kBoolean).getBoolean(defaultValue);
  }

  /**
   * Send a int value.
   *
   * @param path the topic key the value will appear under
   * @param value the value
   */
  public static void putInteger(String path, int value) {
    getImmediateEntry(path, NetworkTableType.kInteger).setInteger(value);
  }

  /**
   * Read a int value.
   *
   * @param path the topic key the value will appear under
   * @param defaultValue default value to be returned if no network value is found
   */
  public static void getInteger(String path, int defaultValue) {
    getImmediateEntry(path, NetworkTableType.kInteger).getInteger(defaultValue);
  }

  /**
   * Send a float value.
   *
   * @param path the topic key the value will appear under
   * @param value the value
   */
  public static void putFloat(String path, float value) {
    getImmediateEntry(path, NetworkTableType.kFloat).setFloat(value);
  }

  /**
   * Read a float value.
   *
   * @param path the topic key the value will appear under
   * @param defaultValue default value to be returned if no network value is found
   */
  public static void getFloat(String path, float defaultValue) {
    getImmediateEntry(path, NetworkTableType.kFloat).getFloat(defaultValue);
  }

  /**
   * Send a double value.
   *
   * @param path the topic key the value will appear under
   * @param value the value
   */
  public static void putDouble(String path, double value) {
    getImmediateEntry(path, NetworkTableType.kDouble).setDouble(value);
  }

  /**
   * Read a double value.
   *
   * @param path the topic key the value will appear under
   * @param defaultValue default value to be returned if no network value is found
   */
  public static void getDouble(String path, double defaultValue) {
    getImmediateEntry(path, NetworkTableType.kDouble).getDouble(defaultValue);
  }

  /**
   * Send a String value.
   *
   * @param path the topic key the value will appear under
   * @param value the value
   */
  public static void putString(String path, String value) {
    getImmediateEntry(path, NetworkTableType.kString).setString(value);
  }

  /**
   * Read a String value.
   *
   * @param path the topic key the value will appear under
   * @param defaultValue default value to be returned if no network value is found
   */
  public static void getString(String path, String defaultValue) {
    getImmediateEntry(path, NetworkTableType.kString).getString(defaultValue);
  }

  /**
   * Send a byte[] value.
   *
   * @param path the topic key the value will appear under
   * @param value the value
   */
  public static void putRaw(String path, byte[] value) {
    getImmediateEntry(path, NetworkTableType.kRaw).setRaw(value);
  }

  /**
   * Read a byte[] value.
   *
   * @param path the topic key the value will appear under
   * @param defaultValue default value to be returned if no network value is found
   */
  public static void getRaw(String path, byte[] defaultValue) {
    getImmediateEntry(path, NetworkTableType.kRaw).getRaw(defaultValue);
  }

  /**
   * Send a boolean[] value.
   *
   * @param path the topic key the value will appear under
   * @param value the value
   */
  public static void putBooleanArray(String path, boolean[] value) {
    getImmediateEntry(path, NetworkTableType.kBooleanArray).setBooleanArray(value);
  }

  /**
   * Read a boolean[] value.
   *
   * @param path the topic key the value will appear under
   * @param defaultValue default value to be returned if no network value is found
   */
  public static void getBooleanArray(String path, boolean[] defaultValue) {
    getImmediateEntry(path, NetworkTableType.kBooleanArray).getBooleanArray(defaultValue);
  }

  /**
   * Send a integer array value.
   *
   * @param path the topic key the value will appear under
   * @param value the value
   */
  public static void putIntegerArray(String path, long[] value) {
    getImmediateEntry(path, NetworkTableType.kIntegerArray).setIntegerArray(value);
  }

  /**
   * Read a integer array value.
   *
   * @param path the topic key the value will appear under
   * @param defaultValue default value to be returned if no network value is found
   */
  public static void getIntegerArray(String path, long[] defaultValue) {
    getImmediateEntry(path, NetworkTableType.kIntegerArray).getIntegerArray(defaultValue);
  }

  /**
   * Send a float[] value.
   *
   * @param path the topic key the value will appear under
   * @param value the value
   */
  public static void putFloatArray(String path, float[] value) {
    getImmediateEntry(path, NetworkTableType.kFloatArray).setFloatArray(value);
  }

  /**
   * Read a float[] value.
   *
   * @param path the topic key the value will appear under
   * @param defaultValue default value to be returned if no network value is found
   */
  public static void getFloatArray(String path, float[] defaultValue) {
    getImmediateEntry(path, NetworkTableType.kFloatArray).getFloatArray(defaultValue);
  }

  /**
   * Send a double[] value.
   *
   * @param path the topic key the value will appear under
   * @param value the value
   */
  public static void putDoubleArray(String path, double[] value) {
    getImmediateEntry(path, NetworkTableType.kDoubleArray).setDoubleArray(value);
  }

  /**
   * Read a double[] value.
   *
   * @param path the topic key the value will appear under
   * @param defaultValue default value to be returned if no network value is found
   */
  public static void getDoubleArray(String path, double[] defaultValue) {
    getImmediateEntry(path, NetworkTableType.kDoubleArray).getDoubleArray(defaultValue);
  }

  /**
   * Send a String[] value.
   *
   * @param path the topic key the value will appear under
   * @param value the value
   */
  public static void putStringArray(String path, String[] value) {
    getImmediateEntry(path, NetworkTableType.kStringArray).setStringArray(value);
  }

  /**
   * Read a String[] value.
   *
   * @param path the topic key the value will appear under
   * @param defaultValue default value to be returned if no network value is found
   */
  public static void getStringArray(String path, String[] defaultValue) {
    getImmediateEntry(path, NetworkTableType.kStringArray).getStringArray(defaultValue);
  }

  /** Update all telemetry registered with this class. */
  public static void update() {
    // TODO: didn't we replace WPIUtilJNI.now() with something else?
    m_builders.forEach((unused, builder) -> builder.update(WPIUtilJNI.now()));
  }

  private static TelemetryBuilder getBuilder(String path) {
    return m_builders.computeIfAbsent(
        path, key -> new TelemetryBuilderImpl(m_instance.getTable(key)));
  }

  private static GenericEntry getImmediateEntry(String path, NetworkTableType type) {
    return m_immediateTelemetry.computeIfAbsent(
        path,
        p ->
            m_instance
                .getTable("ImmediateTelemetry")
                .getTopic(p)
                .getGenericEntry(type.getValueStr()));
  }

  private Telemetry() {
    throw new UnsupportedOperationException("Utility class");
  }
}
