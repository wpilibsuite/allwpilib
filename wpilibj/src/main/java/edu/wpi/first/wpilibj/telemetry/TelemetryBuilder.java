package edu.wpi.first.wpilibj.telemetry;

import edu.wpi.first.util.function.FloatSupplier;
import java.util.function.BooleanSupplier;
import java.util.function.DoubleSupplier;
import java.util.function.LongSupplier;
import java.util.function.Supplier;

public interface TelemetryBuilder {
  void selfMetadata(TelemetryMetadata metadata);

  /**
   * Publish a boolean property.
   *
   * @param key the property name
   * @param getter the property getter
   * @return the property object, for further configuration
   */
  TelemetryProperty publishBoolean(String key, BooleanSupplier getter);

  /**
   * Publish a constant boolean property.
   *
   * @param key the property name
   * @param value the property value
   * @return the property object, for further configuration
   */
  TelemetryProperty publishConstBoolean(String key, boolean value);

  /**
   * Log a boolean property.
   *
   * @param key the property name
   * @param getter the property getter
   * @return the property object, for further configuration
   */
  TelemetryProperty logBoolean(String key, BooleanSupplier getter);

  /**
   * Publish a long property.
   *
   * @param key the property name
   * @param getter the property getter
   * @return the property object, for further configuration
   */
  TelemetryProperty publishInteger(String key, LongSupplier getter);

  /**
   * Publish a constant long property.
   *
   * @param key the property name
   * @param value the property value
   * @return the property object, for further configuration
   */
  TelemetryProperty publishConstInteger(String key, long value);

  /**
   * Log a long property.
   *
   * @param key the property name
   * @param getter the property getter
   * @return the property object, for further configuration
   */
  TelemetryProperty logInteger(String key, LongSupplier getter);

  /**
   * Publish a float property.
   *
   * @param key the property name
   * @param getter the property getter
   * @return the property object, for further configuration
   */
  TelemetryProperty publishFloat(String key, FloatSupplier getter);

  /**
   * Publish a constant float property.
   *
   * @param key the property name
   * @param value the property value
   * @return the property object, for further configuration
   */
  TelemetryProperty publishConstFloat(String key, float value);

  /**
   * Log a float property.
   *
   * @param key the property name
   * @param getter the property getter
   * @return the property object, for further configuration
   */
  TelemetryProperty logFloat(String key, FloatSupplier getter);

  /**
   * Publish a double property.
   *
   * @param key the property name
   * @param getter the property getter
   * @return the property object, for further configuration
   */
  TelemetryProperty publishDouble(String key, DoubleSupplier getter);

  /**
   * Publish a constant double property.
   *
   * @param key the property name
   * @param value the property value
   * @return the property object, for further configuration
   */
  TelemetryProperty publishConstDouble(String key, double value);

  /**
   * Log a double property.
   *
   * @param key the property name
   * @param getter the property getter
   * @return the property object, for further configuration
   */
  TelemetryProperty logDouble(String key, DoubleSupplier getter);

  /**
   * Publish a String property.
   *
   * @param key the property name
   * @param getter the property getter
   * @return the property object, for further configuration
   */
  TelemetryProperty publishString(String key, Supplier<String> getter);

  /**
   * Publish a constant String property.
   *
   * @param key the property name
   * @param value the property value
   * @return the property object, for further configuration
   */
  TelemetryProperty publishConstString(String key, String value);

  /**
   * Log a String property.
   *
   * @param key the property name
   * @param getter the property getter
   * @return the property object, for further configuration
   */
  TelemetryProperty logString(String key, Supplier<String> getter);

  /**
   * Publish a byte[] property.
   *
   * @param key the property name
   * @param getter the property getter
   * @return the property object, for further configuration
   */
  TelemetryProperty publishRaw(String key, String typestring, Supplier<byte[]> getter);

  /**
   * Publish a constant byte[] property.
   *
   * @param key the property name
   * @param value the property value
   * @return the property object, for further configuration
   */
  TelemetryProperty publishConstRaw(String key, String typestring, byte[] value);

  /**
   * Log a byte[] property.
   *
   * @param key the property name
   * @param getter the property getter
   * @return the property object, for further configuration
   */
  TelemetryProperty logRaw(String key, Supplier<byte[]> getter);

  /**
   * Publish a boolean[] property.
   *
   * @param key the property name
   * @param getter the property getter
   * @return the property object, for further configuration
   */
  TelemetryProperty publishBooleanArray(String key, Supplier<boolean[]> getter);

  /**
   * Publish a constant boolean[] property.
   *
   * @param key the property name
   * @param value the property value
   * @return the property object, for further configuration
   */
  TelemetryProperty publishConstBooleanArray(String key, boolean[] value);

  /**
   * Log a boolean[] property.
   *
   * @param key the property name
   * @param getter the property getter
   * @return the property object, for further configuration
   */
  TelemetryProperty logBooleanArray(String key, Supplier<boolean[]> getter);

  /**
   * Publish a long[] property.
   *
   * @param key the property name
   * @param getter the property getter
   * @return the property object, for further configuration
   */
  TelemetryProperty publishIntegerArray(String key, Supplier<long[]> getter);

  /**
   * Publish a constant long[] property.
   *
   * @param key the property name
   * @param value the property value
   * @return the property object, for further configuration
   */
  TelemetryProperty publishConstIntegerArray(String key, long[] value);

  /**
   * Log a long[] property.
   *
   * @param key the property name
   * @param getter the property getter
   * @return the property object, for further configuration
   */
  TelemetryProperty logIntegerArray(String key, Supplier<long[]> getter);

  /**
   * Publish a float[] property.
   *
   * @param key the property name
   * @param getter the property getter
   * @return the property object, for further configuration
   */
  TelemetryProperty publishFloatArray(String key, Supplier<float[]> getter);

  /**
   * Publish a constant float[] property.
   *
   * @param key the property name
   * @param value the property value
   * @return the property object, for further configuration
   */
  TelemetryProperty publishConstFloatArray(String key, float[] value);

  /**
   * Log a float[] property.
   *
   * @param key the property name
   * @param getter the property getter
   * @return the property object, for further configuration
   */
  TelemetryProperty logFloatArray(String key, Supplier<float[]> getter);

  /**
   * Publish a double[] property.
   *
   * @param key the property name
   * @param getter the property getter
   * @return the property object, for further configuration
   */
  TelemetryProperty publishDoubleArray(String key, Supplier<double[]> getter);

  /**
   * Publish a constant double[] property.
   *
   * @param key the property name
   * @param value the property value
   * @return the property object, for further configuration
   */
  TelemetryProperty publishConstDoubleArray(String key, double[] value);

  /**
   * Log a double[] property.
   *
   * @param key the property name
   * @param getter the property getter
   * @return the property object, for further configuration
   */
  TelemetryProperty logDoubleArray(String key, Supplier<double[]> getter);

  /**
   * Publish a String[] property.
   *
   * @param key the property name
   * @param getter the property getter
   * @return the property object, for further configuration
   */
  TelemetryProperty publishStringArray(String key, Supplier<String[]> getter);

  /**
   * Publish a constant String[] property.
   *
   * @param key the property name
   * @param value the property value
   * @return the property object, for further configuration
   */
  TelemetryProperty publishConstStringArray(String key, String[] value);

  /**
   * Log a String[] property.
   *
   * @param key the property name
   * @param getter the property getter
   * @return the property object, for further configuration
   */
  TelemetryProperty logStringArray(String key, Supplier<String[]> getter);

  /**
   * Nest another TelemetryNode under this one.
   *
   * @param key the subpath to the child node
   * @param child the child node
   * @return the property object, for further configuration
   */
  TelemetryProperty addChild(String key, TelemetryNode child);
}
