package edu.wpi.first.util.datalog;

/** Lazy log boolean values. This will only write if the value has been changed since last append call */
public class LazyBooleanLogEntry extends BooleanLogEntry {
    private boolean lastValue;
    private boolean firstAppend = true;

    public LazyBooleanLogEntry(DataLog log, String name, String metadata, long timestamp) {
        super(log, name, metadata, timestamp);
    }

    public LazyBooleanLogEntry(DataLog log, String name, String metadata) {
        super(log, name, metadata);
    }

    public LazyBooleanLogEntry(DataLog log, String name, long timestamp) {
        super(log, name, timestamp);
    }

    public LazyBooleanLogEntry(DataLog log, String name) {
        super(log, name);
    }

    @Override
    public void append(boolean value, long timestamp) {
        if (firstAppend || value != lastValue) {
            firstAppend = false;
            lastValue = value;
            super.append(value, timestamp);
        }
    }
}
