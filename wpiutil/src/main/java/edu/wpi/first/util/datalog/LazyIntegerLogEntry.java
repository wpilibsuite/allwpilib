package edu.wpi.first.util.datalog;

/** Lazy log integer values. This will only write if the value has been changed since last append call */
public class LazyIntegerLogEntry extends IntegerLogEntry {
    private long lastValue;
    private boolean firstAppend = true;

    public LazyIntegerLogEntry(DataLog log, String name, String metadata, long timestamp) {
        super(log, name, metadata, timestamp);
    }

    public LazyIntegerLogEntry(DataLog log, String name, String metadata) {
        super(log, name, metadata);
    }

    public LazyIntegerLogEntry(DataLog log, String name, long timestamp) {
        super(log, name, timestamp);
    }

    public LazyIntegerLogEntry(DataLog log, String name) {
        super(log, name);
    }

    @Override
    public void append(long value, long timestamp) {
        if (firstAppend || value != lastValue) {
            firstAppend = false;
            lastValue = value;
            super.append(value, timestamp);
        }
    }
}
