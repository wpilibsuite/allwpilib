package edu.wpi.first.wpilibj.telemetry;

import edu.wpi.first.networktables.BooleanPublisher;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.util.datalog.BooleanLogEntry;
import edu.wpi.first.util.datalog.DataLog;

import java.util.ArrayList;
import java.util.Collection;
import java.util.function.BooleanSupplier;

public class TelemetryBuilder {
  private final NetworkTable m_networkTable;
  // TODO: this would be started before the match!
  private DataLog m_dataLog;

  public TelemetryBuilder(NetworkTable table) {
    m_networkTable = table;
  }

  private final Collection<TelemetryProperty> m_props = new ArrayList<>();

  private void update(long timestamp) {

  }

  public void selfMetadata(TelemetryMetadata metadata) {

  }
  public TelemetryProperty publishBoolean(String key, BooleanSupplier getter) {
    return new TelemetryProperty() {
      final BooleanPublisher m_pub = m_networkTable.getBooleanTopic(key).publish();
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
  }

  public TelemetryProperty publishConstBoolean(String key, boolean value) {
    return new TelemetryProperty() {
      final BooleanPublisher m_pub = m_networkTable.getBooleanTopic(key).publish();
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
  }

  public TelemetryProperty logBoolean(String key, BooleanSupplier getter) {
    return new TelemetryProperty() {
      final BooleanLogEntry m_log = new BooleanLogEntry(m_dataLog, m_networkTable.getPath() + NetworkTable.PATH_SEPARATOR + key);
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
  }

  public TelemetryProperty addChild(String key, TelemetryNode child) {
    TelemetryBuilder builder = new TelemetryBuilder(m_networkTable.getSubTable(key));
    return new TelemetryProperty() {
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
  }
}
