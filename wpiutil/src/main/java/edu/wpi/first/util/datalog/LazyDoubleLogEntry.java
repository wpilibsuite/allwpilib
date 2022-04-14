package edu.wpi.first.util.datalog;

/** Lazy log double values. This will only write if the value has been changed since last append call */
public class LazyDoubleLogEntry extends DoubleLogEntry {
    private double lastValue;
    private boolean firstAppend = true;

    public LazyDoubleLogEntry(DataLog log, String name, String metadata, long timestamp) {
        super(log, name, metadata, timestamp);
    }

    public LazyDoubleLogEntry(DataLog log, String name, String metadata) {
        super(log, name, metadata);
    }

    public LazyDoubleLogEntry(DataLog log, String name, long timestamp) {
        super(log, name, timestamp);
    }

    public LazyDoubleLogEntry(DataLog log, String name) {
        super(log, name);
    }

    @Override
    public void append(double value, long timestamp) {
        if (firstAppend || value != lastValue) {
            firstAppend = false;
            lastValue = value;
            super.append(value, timestamp);
        }
    }
}
