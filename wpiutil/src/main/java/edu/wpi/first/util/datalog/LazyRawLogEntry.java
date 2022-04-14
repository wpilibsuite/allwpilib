package edu.wpi.first.util.datalog;

import java.util.Arrays;

/** Lazy log raw byte array values. This will only write if the value has been changed since last append call */
public class LazyRawLogEntry extends RawLogEntry {
    private byte[] lastValue;
    private boolean firstAppend = true;

    public LazyRawLogEntry(DataLog log, String name, String metadata, String type, long timestamp) {
        super(log, name, metadata, type, timestamp);
    }

    public LazyRawLogEntry(DataLog log, String name, String metadata, String type) {
        super(log, name, metadata, type);
    }

    public LazyRawLogEntry(DataLog log, String name, String metadata, long timestamp) {
        super(log, name, metadata, timestamp);
    }

    public LazyRawLogEntry(DataLog log, String name, String metadata) {
        super(log, name, metadata);
    }

    public LazyRawLogEntry(DataLog log, String name, long timestamp) {
        super(log, name, timestamp);
    }

    public LazyRawLogEntry(DataLog log, String name) {
        super(log, name);
    }

    @Override
    public void append(byte[] value, long timestamp) {
        if (firstAppend || !Arrays.equals(value, lastValue)) {
            firstAppend = false;
            lastValue = value;
            super.append(value, timestamp);
        }
    }
}
