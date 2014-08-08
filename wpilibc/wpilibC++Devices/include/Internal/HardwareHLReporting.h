
#include "HLUsageReporting.h"

class HardwareHLReporting : public HLUsageReportingInterface
{
public:
    virtual void ReportScheduler();
    virtual void ReportSmartDashboard();
};
