package edu.wpi.first.wpilibj.telemetry;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.JsonNodeType;
import edu.wpi.first.networktables.BooleanArrayPublisher;
import edu.wpi.first.networktables.BooleanPublisher;
import edu.wpi.first.networktables.DoubleArrayPublisher;
import edu.wpi.first.networktables.DoublePublisher;
import edu.wpi.first.networktables.FloatArrayPublisher;
import edu.wpi.first.networktables.FloatPublisher;
import edu.wpi.first.networktables.IntegerArrayPublisher;
import edu.wpi.first.networktables.IntegerPublisher;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.RawPublisher;
import edu.wpi.first.networktables.StringArrayPublisher;
import edu.wpi.first.networktables.StringPublisher;
import edu.wpi.first.util.datalog.BooleanArrayLogEntry;
import edu.wpi.first.util.datalog.BooleanLogEntry;
import edu.wpi.first.util.datalog.DataLog;
import edu.wpi.first.util.datalog.DoubleArrayLogEntry;
import edu.wpi.first.util.datalog.DoubleLogEntry;
import edu.wpi.first.util.datalog.FloatArrayLogEntry;
import edu.wpi.first.util.datalog.FloatLogEntry;
import edu.wpi.first.util.datalog.IntegerArrayLogEntry;
import edu.wpi.first.util.datalog.IntegerLogEntry;
import edu.wpi.first.util.datalog.RawLogEntry;
import edu.wpi.first.util.datalog.StringArrayLogEntry;
import edu.wpi.first.util.datalog.StringLogEntry;
import edu.wpi.first.util.function.FloatSupplier;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.Map;
import java.util.Objects;
import java.util.function.BooleanSupplier;
import java.util.function.DoubleSupplier;
import java.util.function.LongSupplier;
import java.util.function.Supplier;

public class TelemetryBuilderImpl implements TelemetryBuilder, AutoCloseable {
  private final NetworkTable m_networkTable;
  // TODO: this would be started before the match!
  private DataLog m_dataLog;

  private final Collection<TelemetryProperty> m_props = new ArrayList<>();

  public TelemetryBuilderImpl(NetworkTable table) {
    m_networkTable = table;
  }

  public void update(long timestamp) {
    m_props.forEach(it -> it.update(timestamp));
  }

  @Override
  public void selfMetadata(TelemetryMetadata metadata) {
    // TODO think how we want to do this
    JsonNode json = metadata.toJson();
    assert json.isObject();
    for (Iterator<Map.Entry<String, JsonNode>> iter = json.fields(); iter.hasNext(); ) {
      var field = iter.next();
      JsonNode value = field.getValue();
      JsonNodeType nodeType = value.getNodeType();
      if (Objects.requireNonNull(nodeType) == JsonNodeType.STRING) {
        publishConstString("." + field.getKey().toLowerCase(), value.asText());
      } else if (nodeType == JsonNodeType.BOOLEAN) {
        publishConstBoolean("." + field.getKey().toLowerCase(), value.asBoolean());
      } else if (nodeType == JsonNodeType.NUMBER) {
        publishConstDouble("." + field.getKey().toLowerCase(), value.asDouble());
        //      } else if (nodeType == JsonNodeType.ARRAY) {
        //      } else if (nodeType == JsonNodeType.OBJECT) {
        //      } else if (nodeType == JsonNodeType.BINARY) {
        //      } else if (nodeType == JsonNodeType.MISSING) {
        //      } else if (nodeType == JsonNodeType.NULL) {
        //      } else if (nodeType == JsonNodeType.POJO) {
      } else {
        // TODO: Don't we want to support other stuff?
        throw new IllegalArgumentException("Metadata can't include values of type " + nodeType);
      }
    }
  }

  @Override
  public TelemetryProperty publishBoolean(String key, BooleanSupplier getter) {
    BooleanPublisher pub = m_networkTable.getBooleanTopic(key).publish();
    TelemetryProperty prop =
        new TelemetryProperty(pub) {
          final BooleanPublisher m_pub = pub;
          final BooleanSupplier m_supplier = getter;

          {
            m_closeables.add(m_pub);
          }

          @Override
          protected void update(long timestamp) {
            m_pub.set(m_supplier.getAsBoolean(), timestamp);
          }

          @Override
          protected void applyMetadata(TelemetryMetadata metadata) {
            m_pub.getTopic().setProperties(metadata.toJson().toString());
          }
        };
    m_props.add(prop);
    return prop;
  }

  @Override
  public TelemetryProperty publishConstBoolean(String key, boolean value) {
    BooleanPublisher pub = m_networkTable.getBooleanTopic(key).publish();
    TelemetryProperty prop =
        new TelemetryProperty(pub) {
          final BooleanPublisher m_pub = pub;
          boolean hasPublished = false;

          {
            m_closeables.add(m_pub);
          }

          @Override
          protected void update(long timestamp) {
            if (!hasPublished) {
              m_pub.set(value, timestamp);
              hasPublished = true;
            }
          }

          @Override
          protected void applyMetadata(TelemetryMetadata metadata) {
            m_pub.getTopic().setProperties(metadata.toString());
          }
        };
    m_props.add(prop);
    return prop;
  }

  @Override
  public TelemetryProperty logBoolean(String key, BooleanSupplier getter) {
    TelemetryProperty prop =
        new TelemetryProperty(null) {
          final BooleanLogEntry m_log =
              new BooleanLogEntry(
                  m_dataLog, m_networkTable.getPath() + NetworkTable.PATH_SEPARATOR + key);
          final BooleanSupplier m_supplier = getter;

          @Override
          protected void update(long timestamp) {
            m_log.append(m_supplier.getAsBoolean(), timestamp);
          }

          @Override
          protected void applyMetadata(TelemetryMetadata metadata) {
            m_log.setMetadata(metadata.toString());
          }
        };
    m_props.add(prop);
    return prop;
  }

  @Override
  public TelemetryProperty publishInteger(String key, LongSupplier getter) {
    IntegerPublisher pub = m_networkTable.getIntegerTopic(key).publish();
    TelemetryProperty prop =
        new TelemetryProperty(pub) {
          final IntegerPublisher m_pub = pub;
          final LongSupplier m_supplier = getter;

          {
            m_closeables.add(m_pub);
          }

          @Override
          protected void update(long timestamp) {
            m_pub.set(m_supplier.getAsLong(), timestamp);
          }

          @Override
          protected void applyMetadata(TelemetryMetadata metadata) {
            m_pub.getTopic().setProperties(metadata.toJson().toString());
          }
        };
    m_props.add(prop);
    return prop;
  }

  @Override
  public TelemetryProperty publishConstInteger(String key, long value) {
    IntegerPublisher pub = m_networkTable.getIntegerTopic(key).publish();
    TelemetryProperty prop =
        new TelemetryProperty(pub) {
          final IntegerPublisher m_pub = pub;
          boolean hasPublished = false;

          {
            m_closeables.add(m_pub);
          }

          @Override
          protected void update(long timestamp) {
            if (!hasPublished) {
              m_pub.set(value, timestamp);
              hasPublished = true;
            }
          }

          @Override
          protected void applyMetadata(TelemetryMetadata metadata) {
            m_pub.getTopic().setProperties(metadata.toString());
          }
        };
    m_props.add(prop);
    return prop;
  }

  @Override
  public TelemetryProperty logInteger(String key, LongSupplier getter) {
    TelemetryProperty prop =
        new TelemetryProperty(null) {
          final IntegerLogEntry m_log =
              new IntegerLogEntry(
                  m_dataLog, m_networkTable.getPath() + NetworkTable.PATH_SEPARATOR + key);
          final LongSupplier m_supplier = getter;

          @Override
          protected void update(long timestamp) {
            m_log.append(m_supplier.getAsLong(), timestamp);
          }

          @Override
          protected void applyMetadata(TelemetryMetadata metadata) {
            m_log.setMetadata(metadata.toString());
          }
        };
    m_props.add(prop);
    return prop;
  }

  @Override
  public TelemetryProperty publishFloat(String key, FloatSupplier getter) {
    FloatPublisher pub = m_networkTable.getFloatTopic(key).publish();
    TelemetryProperty prop =
        new TelemetryProperty(pub) {
          final FloatPublisher m_pub = pub;
          final FloatSupplier m_supplier = getter;

          {
            m_closeables.add(m_pub);
          }

          @Override
          protected void update(long timestamp) {
            m_pub.set(m_supplier.getAsFloat(), timestamp);
          }

          @Override
          protected void applyMetadata(TelemetryMetadata metadata) {
            m_pub.getTopic().setProperties(metadata.toJson().toString());
          }
        };
    m_props.add(prop);
    return prop;
  }

  @Override
  public TelemetryProperty publishConstFloat(String key, float value) {
    FloatPublisher pub = m_networkTable.getFloatTopic(key).publish();
    TelemetryProperty prop =
        new TelemetryProperty(pub) {
          final FloatPublisher m_pub = pub;
          boolean hasPublished = false;

          {
            m_closeables.add(m_pub);
          }

          @Override
          protected void update(long timestamp) {
            if (!hasPublished) {
              m_pub.set(value, timestamp);
              hasPublished = true;
            }
          }

          @Override
          protected void applyMetadata(TelemetryMetadata metadata) {
            m_pub.getTopic().setProperties(metadata.toString());
          }
        };
    m_props.add(prop);
    return prop;
  }

  @Override
  public TelemetryProperty logFloat(String key, FloatSupplier getter) {
    TelemetryProperty prop =
        new TelemetryProperty(null) {
          final FloatLogEntry m_log =
              new FloatLogEntry(
                  m_dataLog, m_networkTable.getPath() + NetworkTable.PATH_SEPARATOR + key);
          final FloatSupplier m_supplier = getter;

          @Override
          protected void update(long timestamp) {
            m_log.append(m_supplier.getAsFloat(), timestamp);
          }

          @Override
          protected void applyMetadata(TelemetryMetadata metadata) {
            m_log.setMetadata(metadata.toString());
          }
        };
    m_props.add(prop);
    return prop;
  }

  @Override
  public TelemetryProperty publishDouble(String key, DoubleSupplier getter) {
    DoublePublisher pub = m_networkTable.getDoubleTopic(key).publish();
    TelemetryProperty prop =
        new TelemetryProperty(pub) {
          final DoublePublisher m_pub = pub;
          final DoubleSupplier m_supplier = getter;

          {
            m_closeables.add(m_pub);
          }

          @Override
          protected void update(long timestamp) {
            m_pub.set(m_supplier.getAsDouble(), timestamp);
          }

          @Override
          protected void applyMetadata(TelemetryMetadata metadata) {
            m_pub.getTopic().setProperties(metadata.toJson().toString());
          }
        };
    m_props.add(prop);
    return prop;
  }

  @Override
  public TelemetryProperty publishConstDouble(String key, double value) {
    DoublePublisher pub = m_networkTable.getDoubleTopic(key).publish();
    TelemetryProperty prop =
        new TelemetryProperty(pub) {
          final DoublePublisher m_pub = pub;
          boolean hasPublished = false;

          {
            m_closeables.add(m_pub);
          }

          @Override
          protected void update(long timestamp) {
            if (!hasPublished) {
              m_pub.set(value, timestamp);
              hasPublished = true;
            }
          }

          @Override
          protected void applyMetadata(TelemetryMetadata metadata) {
            m_pub.getTopic().setProperties(metadata.toString());
          }
        };
    m_props.add(prop);
    return prop;
  }

  @Override
  public TelemetryProperty logDouble(String key, DoubleSupplier getter) {
    TelemetryProperty prop =
        new TelemetryProperty(null) {
          final DoubleLogEntry m_log =
              new DoubleLogEntry(
                  m_dataLog, m_networkTable.getPath() + NetworkTable.PATH_SEPARATOR + key);
          final DoubleSupplier m_supplier = getter;

          @Override
          protected void update(long timestamp) {
            m_log.append(m_supplier.getAsDouble(), timestamp);
          }

          @Override
          protected void applyMetadata(TelemetryMetadata metadata) {
            m_log.setMetadata(metadata.toString());
          }
        };
    m_props.add(prop);
    return prop;
  }

  @Override
  public TelemetryProperty publishString(String key, Supplier<String> getter) {
    StringPublisher pub = m_networkTable.getStringTopic(key).publish();
    TelemetryProperty prop =
        new TelemetryProperty(pub) {
          final StringPublisher m_pub = pub;
          final Supplier<String> m_supplier = getter;

          {
            m_closeables.add(m_pub);
          }

          @Override
          protected void update(long timestamp) {
            m_pub.set(m_supplier.get(), timestamp);
          }

          @Override
          protected void applyMetadata(TelemetryMetadata metadata) {
            m_pub.getTopic().setProperties(metadata.toJson().toString());
          }
        };
    m_props.add(prop);
    return prop;
  }

  @Override
  public TelemetryProperty publishConstString(String key, String value) {
    StringPublisher pub = m_networkTable.getStringTopic(key).publish();
    TelemetryProperty prop =
        new TelemetryProperty(pub) {
          final StringPublisher m_pub = pub;
          boolean hasPublished = false;

          {
            m_closeables.add(m_pub);
          }

          @Override
          protected void update(long timestamp) {
            if (!hasPublished) {
              m_pub.set(value, timestamp);
              hasPublished = true;
            }
          }

          @Override
          protected void applyMetadata(TelemetryMetadata metadata) {
            m_pub.getTopic().setProperties(metadata.toString());
          }
        };
    m_props.add(prop);
    return prop;
  }

  @Override
  public TelemetryProperty logString(String key, Supplier<String> getter) {
    TelemetryProperty prop =
        new TelemetryProperty(null) {
          final StringLogEntry m_log =
              new StringLogEntry(
                  m_dataLog, m_networkTable.getPath() + NetworkTable.PATH_SEPARATOR + key);
          final Supplier<String> m_supplier = getter;

          @Override
          protected void update(long timestamp) {
            m_log.append(m_supplier.get(), timestamp);
          }

          @Override
          protected void applyMetadata(TelemetryMetadata metadata) {
            m_log.setMetadata(metadata.toString());
          }
        };
    m_props.add(prop);
    return prop;
  }

  @Override
  public TelemetryProperty publishRaw(String key, String typestring, Supplier<byte[]> getter) {
    RawPublisher pub = m_networkTable.getRawTopic(key).publish(typestring);
    TelemetryProperty prop =
        new TelemetryProperty(pub) {
          final RawPublisher m_pub = pub;
          final Supplier<byte[]> m_supplier = getter;

          {
            m_closeables.add(m_pub);
          }

          @Override
          protected void update(long timestamp) {
            m_pub.set(m_supplier.get(), timestamp);
          }

          @Override
          protected void applyMetadata(TelemetryMetadata metadata) {
            m_pub.getTopic().setProperties(metadata.toJson().toString());
          }
        };
    m_props.add(prop);
    return prop;
  }

  @Override
  public TelemetryProperty publishConstRaw(String key, String typestring, byte[] value) {
    RawPublisher pub = m_networkTable.getRawTopic(key).publish(typestring);
    TelemetryProperty prop =
        new TelemetryProperty(pub) {
          final RawPublisher m_pub = pub;
          boolean hasPublished = false;

          {
            m_closeables.add(m_pub);
          }

          @Override
          protected void update(long timestamp) {
            if (!hasPublished) {
              m_pub.set(value, timestamp);
              hasPublished = true;
            }
          }

          @Override
          protected void applyMetadata(TelemetryMetadata metadata) {
            m_pub.getTopic().setProperties(metadata.toString());
          }
        };
    m_props.add(prop);
    return prop;
  }

  @Override
  public TelemetryProperty logRaw(String key, Supplier<byte[]> getter) {
    TelemetryProperty prop =
        new TelemetryProperty(null) {
          final RawLogEntry m_log =
              new RawLogEntry(
                  m_dataLog, m_networkTable.getPath() + NetworkTable.PATH_SEPARATOR + key);
          final Supplier<byte[]> m_supplier = getter;

          @Override
          protected void update(long timestamp) {
            m_log.append(m_supplier.get(), timestamp);
          }

          @Override
          protected void applyMetadata(TelemetryMetadata metadata) {
            m_log.setMetadata(metadata.toString());
          }
        };
    m_props.add(prop);
    return prop;
  }

  @Override
  public TelemetryProperty publishBooleanArray(String key, Supplier<boolean[]> getter) {
    BooleanArrayPublisher pub = m_networkTable.getBooleanArrayTopic(key).publish();
    TelemetryProperty prop =
        new TelemetryProperty(pub) {
          final BooleanArrayPublisher m_pub = pub;
          final Supplier<boolean[]> m_supplier = getter;

          {
            m_closeables.add(m_pub);
          }

          @Override
          protected void update(long timestamp) {
            m_pub.set(m_supplier.get(), timestamp);
          }

          @Override
          protected void applyMetadata(TelemetryMetadata metadata) {
            m_pub.getTopic().setProperties(metadata.toJson().toString());
          }
        };
    m_props.add(prop);
    return prop;
  }

  @Override
  public TelemetryProperty publishConstBooleanArray(String key, boolean[] value) {
    BooleanArrayPublisher pub = m_networkTable.getBooleanArrayTopic(key).publish();
    TelemetryProperty prop =
        new TelemetryProperty(pub) {
          final BooleanArrayPublisher m_pub = pub;
          boolean hasPublished = false;

          {
            m_closeables.add(m_pub);
          }

          @Override
          protected void update(long timestamp) {
            if (!hasPublished) {
              m_pub.set(value, timestamp);
              hasPublished = true;
            }
          }

          @Override
          protected void applyMetadata(TelemetryMetadata metadata) {
            m_pub.getTopic().setProperties(metadata.toString());
          }
        };
    m_props.add(prop);
    return prop;
  }

  @Override
  public TelemetryProperty logBooleanArray(String key, Supplier<boolean[]> getter) {
    TelemetryProperty prop =
        new TelemetryProperty(null) {
          final BooleanArrayLogEntry m_log =
              new BooleanArrayLogEntry(
                  m_dataLog, m_networkTable.getPath() + NetworkTable.PATH_SEPARATOR + key);
          final Supplier<boolean[]> m_supplier = getter;

          @Override
          protected void update(long timestamp) {
            m_log.append(m_supplier.get(), timestamp);
          }

          @Override
          protected void applyMetadata(TelemetryMetadata metadata) {
            m_log.setMetadata(metadata.toString());
          }
        };
    m_props.add(prop);
    return prop;
  }

  @Override
  public TelemetryProperty publishIntegerArray(String key, Supplier<long[]> getter) {
    IntegerArrayPublisher pub = m_networkTable.getIntegerArrayTopic(key).publish();
    TelemetryProperty prop =
        new TelemetryProperty(pub) {
          final IntegerArrayPublisher m_pub = pub;
          final Supplier<long[]> m_supplier = getter;

          {
            m_closeables.add(m_pub);
          }

          @Override
          protected void update(long timestamp) {
            m_pub.set(m_supplier.get(), timestamp);
          }

          @Override
          protected void applyMetadata(TelemetryMetadata metadata) {
            m_pub.getTopic().setProperties(metadata.toJson().toString());
          }
        };
    m_props.add(prop);
    return prop;
  }

  @Override
  public TelemetryProperty publishConstIntegerArray(String key, long[] value) {
    IntegerArrayPublisher pub = m_networkTable.getIntegerArrayTopic(key).publish();
    TelemetryProperty prop =
        new TelemetryProperty(pub) {
          final IntegerArrayPublisher m_pub = pub;
          boolean hasPublished = false;

          {
            m_closeables.add(m_pub);
          }

          @Override
          protected void update(long timestamp) {
            if (!hasPublished) {
              m_pub.set(value, timestamp);
              hasPublished = true;
            }
          }

          @Override
          protected void applyMetadata(TelemetryMetadata metadata) {
            m_pub.getTopic().setProperties(metadata.toString());
          }
        };
    m_props.add(prop);
    return prop;
  }

  @Override
  public TelemetryProperty logIntegerArray(String key, Supplier<long[]> getter) {
    TelemetryProperty prop =
        new TelemetryProperty(null) {
          final IntegerArrayLogEntry m_log =
              new IntegerArrayLogEntry(
                  m_dataLog, m_networkTable.getPath() + NetworkTable.PATH_SEPARATOR + key);
          final Supplier<long[]> m_supplier = getter;

          @Override
          protected void update(long timestamp) {
            m_log.append(m_supplier.get(), timestamp);
          }

          @Override
          protected void applyMetadata(TelemetryMetadata metadata) {
            m_log.setMetadata(metadata.toString());
          }
        };
    m_props.add(prop);
    return prop;
  }

  @Override
  public TelemetryProperty publishFloatArray(String key, Supplier<float[]> getter) {
    FloatArrayPublisher pub = m_networkTable.getFloatArrayTopic(key).publish();
    TelemetryProperty prop =
        new TelemetryProperty(pub) {
          final FloatArrayPublisher m_pub = pub;
          final Supplier<float[]> m_supplier = getter;

          {
            m_closeables.add(m_pub);
          }

          @Override
          protected void update(long timestamp) {
            m_pub.set(m_supplier.get(), timestamp);
          }

          @Override
          protected void applyMetadata(TelemetryMetadata metadata) {
            m_pub.getTopic().setProperties(metadata.toJson().toString());
          }
        };
    m_props.add(prop);
    return prop;
  }

  @Override
  public TelemetryProperty publishConstFloatArray(String key, float[] value) {
    FloatArrayPublisher pub = m_networkTable.getFloatArrayTopic(key).publish();
    TelemetryProperty prop =
        new TelemetryProperty(pub) {
          final FloatArrayPublisher m_pub = pub;
          boolean hasPublished = false;

          {
            m_closeables.add(m_pub);
          }

          @Override
          protected void update(long timestamp) {
            if (!hasPublished) {
              m_pub.set(value, timestamp);
              hasPublished = true;
            }
          }

          @Override
          protected void applyMetadata(TelemetryMetadata metadata) {
            m_pub.getTopic().setProperties(metadata.toString());
          }
        };
    m_props.add(prop);
    return prop;
  }

  @Override
  public TelemetryProperty logFloatArray(String key, Supplier<float[]> getter) {
    TelemetryProperty prop =
        new TelemetryProperty(null) {
          final FloatArrayLogEntry m_log =
              new FloatArrayLogEntry(
                  m_dataLog, m_networkTable.getPath() + NetworkTable.PATH_SEPARATOR + key);
          final Supplier<float[]> m_supplier = getter;

          @Override
          protected void update(long timestamp) {
            m_log.append(m_supplier.get(), timestamp);
          }

          @Override
          protected void applyMetadata(TelemetryMetadata metadata) {
            m_log.setMetadata(metadata.toString());
          }
        };
    m_props.add(prop);
    return prop;
  }

  @Override
  public TelemetryProperty publishDoubleArray(String key, Supplier<double[]> getter) {
    DoubleArrayPublisher pub = m_networkTable.getDoubleArrayTopic(key).publish();
    TelemetryProperty prop =
        new TelemetryProperty(pub) {
          final DoubleArrayPublisher m_pub = pub;
          final Supplier<double[]> m_supplier = getter;

          {
            m_closeables.add(m_pub);
          }

          @Override
          protected void update(long timestamp) {
            m_pub.set(m_supplier.get(), timestamp);
          }

          @Override
          protected void applyMetadata(TelemetryMetadata metadata) {
            m_pub.getTopic().setProperties(metadata.toJson().toString());
          }
        };
    m_props.add(prop);
    return prop;
  }

  @Override
  public TelemetryProperty publishConstDoubleArray(String key, double[] value) {
    DoubleArrayPublisher pub = m_networkTable.getDoubleArrayTopic(key).publish();
    TelemetryProperty prop =
        new TelemetryProperty(pub) {
          final DoubleArrayPublisher m_pub = pub;
          boolean hasPublished = false;

          {
            m_closeables.add(m_pub);
          }

          @Override
          protected void update(long timestamp) {
            if (!hasPublished) {
              m_pub.set(value, timestamp);
              hasPublished = true;
            }
          }

          @Override
          protected void applyMetadata(TelemetryMetadata metadata) {
            m_pub.getTopic().setProperties(metadata.toString());
          }
        };
    m_props.add(prop);
    return prop;
  }

  @Override
  public TelemetryProperty logDoubleArray(String key, Supplier<double[]> getter) {
    TelemetryProperty prop =
        new TelemetryProperty(null) {
          final DoubleArrayLogEntry m_log =
              new DoubleArrayLogEntry(
                  m_dataLog, m_networkTable.getPath() + NetworkTable.PATH_SEPARATOR + key);
          final Supplier<double[]> m_supplier = getter;

          @Override
          protected void update(long timestamp) {
            m_log.append(m_supplier.get(), timestamp);
          }

          @Override
          protected void applyMetadata(TelemetryMetadata metadata) {
            m_log.setMetadata(metadata.toString());
          }
        };
    m_props.add(prop);
    return prop;
  }

  @Override
  public TelemetryProperty publishStringArray(String key, Supplier<String[]> getter) {
    StringArrayPublisher pub = m_networkTable.getStringArrayTopic(key).publish();
    TelemetryProperty prop =
        new TelemetryProperty(pub) {
          final StringArrayPublisher m_pub = pub;
          final Supplier<String[]> m_supplier = getter;

          {
            m_closeables.add(m_pub);
          }

          @Override
          protected void update(long timestamp) {
            m_pub.set(m_supplier.get(), timestamp);
          }

          @Override
          protected void applyMetadata(TelemetryMetadata metadata) {
            m_pub.getTopic().setProperties(metadata.toJson().toString());
          }
        };
    m_props.add(prop);
    return prop;
  }

  @Override
  public TelemetryProperty publishConstStringArray(String key, String[] value) {
    StringArrayPublisher pub = m_networkTable.getStringArrayTopic(key).publish();
    TelemetryProperty prop =
        new TelemetryProperty(pub) {
          final StringArrayPublisher m_pub = pub;
          boolean hasPublished = false;

          {
            m_closeables.add(m_pub);
          }

          @Override
          protected void update(long timestamp) {
            if (!hasPublished) {
              m_pub.set(value, timestamp);
              hasPublished = true;
            }
          }

          @Override
          protected void applyMetadata(TelemetryMetadata metadata) {
            m_pub.getTopic().setProperties(metadata.toString());
          }
        };
    m_props.add(prop);
    return prop;
  }

  @Override
  public TelemetryProperty logStringArray(String key, Supplier<String[]> getter) {
    TelemetryProperty prop =
        new TelemetryProperty(null) {
          final StringArrayLogEntry m_log =
              new StringArrayLogEntry(
                  m_dataLog, m_networkTable.getPath() + NetworkTable.PATH_SEPARATOR + key);
          final Supplier<String[]> m_supplier = getter;

          @Override
          protected void update(long timestamp) {
            m_log.append(m_supplier.get(), timestamp);
          }

          @Override
          protected void applyMetadata(TelemetryMetadata metadata) {
            m_log.setMetadata(metadata.toString());
          }
        };
    m_props.add(prop);
    return prop;
  }

  @Override
  public TelemetryProperty addChild(String key, TelemetryNode child) {
    TelemetryBuilderImpl builder = new TelemetryBuilderImpl(m_networkTable.getSubTable(key));

    child.declareTelemetry(builder);

    TelemetryProperty prop =
        new TelemetryProperty(builder) {
          final TelemetryBuilderImpl m_builder = builder;

          @Override
          protected void update(long timestamp) {
            m_builder.update(timestamp);
          }

          @Override
          protected void applyMetadata(TelemetryMetadata metadata) {
            m_builder.selfMetadata(metadata);
          }
        };
    m_props.add(prop);
    return prop;
  }

  @Override
  public void close() {
    m_props.forEach(TelemetryProperty::close);
    m_props.clear();
  }
}
