package edu.wpi.first.hal;

import java.util.ArrayList;
import java.util.List;

public class HALReporter {
    static class ReportStore {
        public int resource;
        public int instanceNumber;
        public int context;
        public String feature;

        public ReportStore(int resource, int instanceNumber, int context, String feature) {
            this.resource = resource;
            this.instanceNumber = instanceNumber;
            this.context = context;
            this.feature = feature;
        }
    }

    private static final List<ReportStore> storeList = new ArrayList<>();

    private static final Object storeLock = new Object();
    private static boolean allowDirectWrite = false;

    public static void writeReports() {
        synchronized (storeLock) {
            if (allowDirectWrite) {
                return;
            }
            allowDirectWrite = true;
        }
        for (ReportStore r : storeList) {
            HALReporter.report(r.resource, r.instanceNumber, r.context, r.feature);
        }
    }

    public static void report(int resource, int instanceNumber) {
        report(resource, instanceNumber, 0, "");
    }

    public static void report(int resource, int instanceNumber, int context) {
        report(resource, instanceNumber, context, "");
    }

    public static int report(int resource, int instanceNumber, int context, String feature) {
        synchronized(storeLock) {
            if (!allowDirectWrite) {
                storeList.add(new ReportStore(resource, instanceNumber, context, feature));
                return 0;
            }
        }
        return HALReporter.report(resource, instanceNumber, context, feature);
    }
}