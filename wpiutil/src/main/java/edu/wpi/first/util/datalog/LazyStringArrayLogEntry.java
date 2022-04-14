package edu.wpi.first.util.datalog;

import java.util.Arrays;

/** Lazy log array of string values. This will only write if the value has been changed since last append call */
public class LazyStringArrayLogEntry extends StringArrayLogEntry {
    private String[] lastValue;
    private boolean firstAppend = true;

    public LazyStringArrayLogEntry(DataLog log, String name, String metadata, long timestamp) {
        super(log, name, metadata, timestamp);
    }

    public LazyStringArrayLogEntry(DataLog log, String name, String metadata) {
        super(log, name, metadata);
    }

    public LazyStringArrayLogEntry(DataLog log, String name, long timestamp) {
        super(log, name, timestamp);
    }

    public LazyStringArrayLogEntry(DataLog log, String name) {
        super(log, name);
    }

    @Override
    public void append(String[] value, long timestamp) {
        if (firstAppend || !Arrays.equals(value, lastValue)) {
            firstAppend = false;
            lastValue = value;
            super.append(value, timestamp);
        }
    }
}
