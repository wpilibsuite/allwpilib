
#pragma once

#include "wpi/nt/ntcore.h"
#include "wpi/nt/NetworkTableInstance.hpp"

namespace pyntcore {

void onInstanceStart(wpi::nt::NetworkTableInstance *instance);
void onInstancePreReset(wpi::nt::NetworkTableInstance *instance);
void onInstancePostReset(wpi::nt::NetworkTableInstance *instance);
void onInstanceDestroy(wpi::nt::NetworkTableInstance *instance);

void resetAllInstances();

}; // namespace pyntcore
