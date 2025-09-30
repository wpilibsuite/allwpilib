
#pragma once

#include <wpi/sendable/Sendable.h>
#include <semiwrap.h>

namespace rpy {

//
// These functions must be called with the GIL held
//

void addSmartDashboardData(py::str &key, std::shared_ptr<wpi::Sendable> data);
void clearSmartDashboardData();
void destroySmartDashboardData();

} // namespace rpy