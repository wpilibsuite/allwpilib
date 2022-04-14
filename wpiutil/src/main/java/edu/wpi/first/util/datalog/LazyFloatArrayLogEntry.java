package edu.wpi.first.util.datalog;

import java.util.Arrays;

/** Lazy log array of float values. This will only write if the value has been changed since last append call */
public class LazyFloatArrayLogEntry extends FloatArrayLogEntry {
    private float[] lastValue;
    private boolean firstAppend = true;

    public LazyFloatArrayLogEntry(DataLog log, String name, String metadata, long timestamp) {
        super(log, name, metadata, timestamp);
    }

    public LazyFloatArrayLogEntry(DataLog log, String name, String metadata) {
        super(log, name, metadata);
    }

    public LazyFloatArrayLogEntry(DataLog log, String name, long timestamp) {
        super(log, name, timestamp);
    }

    public LazyFloatArrayLogEntry(DataLog log, String name) {
        super(log, name);
    }

    @Override
    public void append(float[] value, long timestamp) {
        if (firstAppend || !Arrays.equals(value, lastValue)) {
            firstAppend = false;
            lastValue = value;
            super.append(value, timestamp);
        }
    }
}
