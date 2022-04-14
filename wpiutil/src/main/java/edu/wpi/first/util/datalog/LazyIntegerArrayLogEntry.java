package edu.wpi.first.util.datalog;

import java.util.Arrays;

/** Lazy log array of integer values. This will only write if the value has been changed since last append call */
public class LazyIntegerArrayLogEntry extends IntegerArrayLogEntry {
    private long[] lastValue;
    private boolean firstAppend = true;

    public LazyIntegerArrayLogEntry(DataLog log, String name, String metadata, long timestamp) {
        super(log, name, metadata, timestamp);
    }

    public LazyIntegerArrayLogEntry(DataLog log, String name, String metadata) {
        super(log, name, metadata);
    }

    public LazyIntegerArrayLogEntry(DataLog log, String name, long timestamp) {
        super(log, name, timestamp);
    }

    public LazyIntegerArrayLogEntry(DataLog log, String name) {
        super(log, name);
    }

    @Override
    public void append(long[] value, long timestamp) {
        if (firstAppend || !Arrays.equals(value, lastValue)) {
            firstAppend = false;
            lastValue = value;
            super.append(value, timestamp);
        }
    }
}
