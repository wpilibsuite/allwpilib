package edu.wpi.first.util.datalog;

import java.util.Arrays;

/** Lazy log array of double values. This will only write if the value has been changed since last append call */
public class LazyDoubleArrayLogEntry extends DoubleArrayLogEntry {
    private double[] lastValue;
    private boolean firstCall = true;

    public LazyDoubleArrayLogEntry(DataLog log, String name, String metadata, long timestamp) {
        super(log, name, metadata, timestamp);
    }

    public LazyDoubleArrayLogEntry(DataLog log, String name, String metadata) {
        super(log, name, metadata);
    }

    public LazyDoubleArrayLogEntry(DataLog log, String name, long timestamp) {
        super(log, name, timestamp);
    }

    public LazyDoubleArrayLogEntry(DataLog log, String name) {
        super(log, name);
    }

    @Override
    public void append(double[] value, long timestamp) {
        if (firstCall || !Arrays.equals(value, lastValue)) {
            firstCall = false;
            lastValue = value;
            super.append(value, timestamp);
        }
    }
}
