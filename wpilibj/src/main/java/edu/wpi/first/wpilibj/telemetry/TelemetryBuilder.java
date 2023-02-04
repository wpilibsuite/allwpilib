package edu.wpi.first.wpilibj.telemetry;

import edu.wpi.first.networktables.BooleanPublisher;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.util.datalog.BooleanLogEntry;
import edu.wpi.first.util.datalog.DataLog;
import edu.wpi.first.util.function.FloatSupplier;

import java.util.ArrayList;
import java.util.Collection;
import java.util.function.BooleanSupplier;
import java.util.function.DoubleSupplier;
import java.util.function.LongSupplier;
import java.util.function.Supplier;

public abstract class TelemetryBuilder {
  private NetworkTable m_networkTable;
  private DataLog m_dataLog;
  private String m_path;

  private final Collection<TelemetryProperty> m_props = new ArrayList<>();
  /**
   * Publish a boolean .
   *
   * @param key  name
   * @param getter getter function (returns current value)
   */
  public TelemetryProperty publishBoolean(String key, BooleanSupplier getter) {
    BooleanPublisher pub = m_networkTable.getBooleanTopic(key).publish();
    return new TelemetryProperty() {
      final BooleanPublisher m_pub = pub;
      final BooleanSupplier m_supplier = getter;
      {
        m_closeables.add(m_pub);
      }

      @Override
      protected void update() {
        m_pub.set(m_supplier.getAsBoolean());
      }

      @Override
      protected void applyMetadata(TelemetryMetadata metadata) {

      }
    };
  }

  /**
   * Publish an integer.
   *
   * @param key    name
   * @param getter getter function (returns current value)
   */
  public TelemetryProperty publishInteger(String key, LongSupplier getter) {
    return null;
  }

  /**
   * Publish a float .
   *
   * @param key    name
   * @param getter getter function (returns current value)
   */
  public TelemetryProperty publishFloat(String key, FloatSupplier getter) {
    return null;
  }

  /**
   * Publish a double .
   *
   * @param key    name
   * @param getter getter function (returns current value)
   */
  public TelemetryProperty publishDouble(String key, DoubleSupplier getter) {
    return null;
  }

  /**
   * Publish a string .
   *
   * @param key  name
   * @param getter getter function (returns current value)
   */
  abstract TelemetryProperty publishString(String key, Supplier<String> getter);

  /**
   * Publish a boolean array .
   *
   * @param key  name
   * @param getter getter function (returns current value)
   */
  abstract TelemetryProperty publishBooleanArray(String key, Supplier<boolean[]> getter);

  /**
   * Publish an integer array .
   *
   * @param key  name
   * @param getter getter function (returns current value)
   */
  abstract TelemetryProperty publishIntegerArray(String key, Supplier<long[]> getter);

  /**
   * Publish a float array .
   *
   * @param key  name
   * @param getter getter function (returns current value)
   */
  abstract TelemetryProperty publishFloatArray(String key, Supplier<float[]> getter);

  /**
   * Publish a double array .
   *
   * @param key  name
   * @param getter getter function (returns current value)
   */
  abstract TelemetryProperty publishDoubleArray(String key, Supplier<double[]> getter);

  /**
   * Publish a string array .
   *
   * @param key  name
   * @param getter getter function (returns current value)
   */
  abstract TelemetryProperty publishStringArray(String key, Supplier<String[]> getter);

  /**
   * Publish a raw .
   *
   * @param key  name
   * @param typeString type string
   * @param getter getter function (returns current value)
   */
  abstract TelemetryProperty publishRaw(
          String key, String typeString, Supplier<byte[]> getter);

  /**
   * log a boolean .
   *
   * @param key  name
   * @param getter getter function (returns current value)
   */
  public TelemetryProperty logBoolean(String key, BooleanSupplier getter) {
    BooleanLogEntry pub = new BooleanLogEntry(m_dataLog, m_path + key);
    return new TelemetryProperty() {
      final Booleanloger m_pub = pub;
      final BooleanSupplier m_supplier = getter;
      {
        m_closeables.add(m_pub);
      }

      @Override
      protected void update() {
        m_pub.set(m_supplier.getAsBoolean());
      }

      @Override
      protected void applyMetadata(TelemetryMetadata metadata) {

      }
    };
  }

  /**
   * log an integer.
   *
   * @param key    name
   * @param getter getter function (returns current value)
   */
  public TelemetryProperty logInteger(String key, LongSupplier getter) {
    return null;
  }

  /**
   * log a float .
   *
   * @param key    name
   * @param getter getter function (returns current value)
   */
  public TelemetryProperty logFloat(String key, FloatSupplier getter) {
    return null;
  }

  /**
   * log a double .
   *
   * @param key    name
   * @param getter getter function (returns current value)
   */
  public TelemetryProperty logDouble(String key, DoubleSupplier getter) {
    return null;
  }

  /**
   * log a string .
   *
   * @param key  name
   * @param getter getter function (returns current value)
   */
  abstract TelemetryProperty logString(String key, Supplier<String> getter);

  /**
   * log a boolean array .
   *
   * @param key  name
   * @param getter getter function (returns current value)
   */
  abstract TelemetryProperty logBooleanArray(String key, Supplier<boolean[]> getter);

  /**
   * log an integer array .
   *
   * @param key  name
   * @param getter getter function (returns current value)
   */
  abstract TelemetryProperty logIntegerArray(String key, Supplier<long[]> getter);

  /**
   * log a float array .
   *
   * @param key  name
   * @param getter getter function (returns current value)
   */
  abstract TelemetryProperty logFloatArray(String key, Supplier<float[]> getter);

  /**
   * log a double array .
   *
   * @param key  name
   * @param getter getter function (returns current value)
   */
  abstract TelemetryProperty logDoubleArray(String key, Supplier<double[]> getter);

  /**
   * log a string array .
   *
   * @param key  name
   * @param getter getter function (returns current value)
   */
  abstract TelemetryProperty logStringArray(String key, Supplier<String[]> getter);

  /**
   * log a raw .
   *
   * @param key  name
   * @param typeString type string
   * @param getter getter function (returns current value)
   */
  abstract TelemetryProperty logRaw(
          String key, String typeString, Supplier<byte[]> getter);

  /**
   * log a boolean .
   *
   * @param key  name
   */
  abstract TelemetryProperty logChild(String key, TelemetryProperty child);
}
