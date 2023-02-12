package edu.wpi.first.wpilibj.telemetry;

import java.util.Collection;
import java.util.HashSet;

public abstract class TelemetryProperty {
  protected Collection<AutoCloseable> m_closeables = new HashSet<>(1);

  protected abstract void update(long timestamp);
  protected abstract void applyMetadata(TelemetryMetadata metadata);

  public TelemetryProperty withMetadata(TelemetryMetadata metadata) {
    return this;
  }

  public TelemetryProperty withWidget(TelemetryWidget widget) {
    return this;
  }
}
