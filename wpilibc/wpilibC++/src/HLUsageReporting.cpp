
#include "HLUsageReporting.h"

HLUsageReportingInterface* HLUsageReporting::impl = 0;

void HLUsageReporting::SetImplementation(HLUsageReportingInterface* i) {
    impl = i;
}

void HLUsageReporting::ReportScheduler() {
    if (impl != 0) {
        impl->ReportScheduler();
    }
}

void HLUsageReporting::ReportSmartDashboard() {
    if (impl != 0) {
        impl->ReportSmartDashboard();
    }
}
