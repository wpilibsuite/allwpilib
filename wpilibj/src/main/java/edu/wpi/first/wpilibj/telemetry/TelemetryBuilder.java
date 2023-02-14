package edu.wpi.first.wpilibj.telemetry;

import com.fasterxml.jackson.databind.JsonNode;
import edu.wpi.first.networktables.BooleanPublisher;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.util.datalog.BooleanLogEntry;
import edu.wpi.first.util.datalog.DataLog;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.Map;
import java.util.function.BooleanSupplier;

public class TelemetryBuilder implements AutoCloseable {
  private final NetworkTable m_networkTable;
  // TODO: this would be started before the match!
  private DataLog m_dataLog;

  private final Collection<TelemetryProperty> m_props = new ArrayList<>();

  public TelemetryBuilder(NetworkTable table) {
    m_networkTable = table;
  }

  private void update(long timestamp) {
    m_props.forEach(it -> it.update(timestamp));
  }

  public void selfMetadata(TelemetryMetadata metadata) {
    // TODO think how we want to do this
    JsonNode json = metadata.toJson();
    assert json.isObject();
    for (Iterator<Map.Entry<String, JsonNode>> iter = json.fields(); iter.hasNext(); ) {
      var field = iter.next();
      switch (field.getValue().getNodeType()) {
        case STRING:
          break;
        case BOOLEAN:
          publishConstBoolean("." + field.getKey().toLowerCase(), field.getValue().asBoolean());
          break;
        case NUMBER:
          break;
        case BINARY:
          break;
        case MISSING:
          break;
        case NULL:
          break;
        case ARRAY:
          break;
        case OBJECT:
          break;
        case POJO:
          break;
      }
    }
  }

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
            m_pub.getTopic().setProperties(metadata.toString());
          }
        };
    m_props.add(prop);
    return prop;
  }

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

  public TelemetryProperty addChild(String key, TelemetryNode child) {
    TelemetryBuilder builder = new TelemetryBuilder(m_networkTable.getSubTable(key));

    child.declareTelemetry(builder);

    TelemetryProperty prop =
        new TelemetryProperty(builder) {
          final TelemetryBuilder m_builder = builder;

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
