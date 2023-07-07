package edu.wpi.first.wpilibj.telemetry;

import java.util.Collection;
import java.util.HashSet;

public abstract class TelemetryProperty implements AutoCloseable {
  protected Collection<AutoCloseable> m_closeables = new HashSet<>(1);

  protected TelemetryProperty(AutoCloseable closeable) {
    if (closeable != null) {
      m_closeables.add(closeable);
    }
  }

  protected abstract void update(long timestamp);

  protected abstract void applyMetadata(TelemetryMetadata metadata);

  public TelemetryProperty withMetadata(TelemetryMetadata metadata) {
    applyMetadata(metadata);
    return this;
  }

  public TelemetryProperty withWidget(TelemetryWidget widget) {
    return this.withMetadata(widget);
  }

  @Override
  public void close() {
    for (AutoCloseable m_closeable : m_closeables) {
      try {
        m_closeable.close();
      } catch (Exception ignored) {
        // ignored
      }
    }
  }
}
