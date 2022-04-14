package edu.wpi.first.util.datalog;

/** Lazy log float values. This will only write if the value has been changed since last append call */
public class LazyFloatLogEntry extends FloatLogEntry {
    private float lastValue;
    private boolean firstAppend = true;

    public LazyFloatLogEntry(DataLog log, String name, String metadata, long timestamp) {
        super(log, name, metadata, timestamp);
    }

    public LazyFloatLogEntry(DataLog log, String name, String metadata) {
        super(log, name, metadata);
    }

    public LazyFloatLogEntry(DataLog log, String name, long timestamp) {
        super(log, name, timestamp);
    }

    public LazyFloatLogEntry(DataLog log, String name) {
        super(log, name);
    }

    @Override
    public void append(float value, long timestamp) {
        if (firstAppend || value != lastValue) {
            firstAppend = false;
            lastValue = value;
            super.append(value, timestamp);
        }
    }
}
