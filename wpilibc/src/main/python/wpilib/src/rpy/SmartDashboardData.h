
#pragma once

#include <memory>

#include <semiwrap.h>

#include "wpi/util/sendable/Sendable.hpp"

namespace rpy {

//
// These functions must be called with the GIL held
//

void addSmartDashboardData(py::str& key,
                           std::shared_ptr<wpi::util::Sendable> data);
void clearSmartDashboardData();
void destroySmartDashboardData();

}  // namespace rpy
