package edu.wpi.first.util.datalog;

/** Lazy log string values. This will only write if the value has been changed since last append call */
public class LazyStringLogEntry extends StringLogEntry {
    private String lastValue;
    private boolean firstAppend = true;

    public LazyStringLogEntry(DataLog log, String name, String metadata, String type, long timestamp) {
        super(log, name, metadata, type, timestamp);
    }

    public LazyStringLogEntry(DataLog log, String name, String metadata, String type) {
        super(log, name, metadata, type);
    }

    public LazyStringLogEntry(DataLog log, String name, String metadata, long timestamp) {
        super(log, name, metadata, timestamp);
    }

    public LazyStringLogEntry(DataLog log, String name, String metadata) {
        super(log, name, metadata);
    }

    public LazyStringLogEntry(DataLog log, String name, long timestamp) {
        super(log, name, timestamp);
    }

    public LazyStringLogEntry(DataLog log, String name) {
        super(log, name);
    }

    @Override
    public void append(String value, long timestamp) {
        if (firstAppend || !value.equals(lastValue)) {
            firstAppend = false;
            lastValue = value;
            super.append(value, timestamp);
        }
    }
}
