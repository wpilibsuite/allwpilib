
#include "HLUsageReporting.h"

HLUsageReportingInterface* HLUsageReporting::impl = nullptr;

void HLUsageReporting::SetImplementation(HLUsageReportingInterface* i) {
  impl = i;
}

void HLUsageReporting::ReportScheduler() {
  if (impl != nullptr) {
    impl->ReportScheduler();
  }
}

void HLUsageReporting::ReportSmartDashboard() {
  if (impl != nullptr) {
    impl->ReportSmartDashboard();
  }
}
