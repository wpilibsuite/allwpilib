
#pragma once

#include "wpi/nt/ntcore.h"
#include "wpi/nt/NetworkTableInstance.hpp"

namespace pyntcore {

void onInstanceStart(nt::NetworkTableInstance *instance);
void onInstancePreReset(nt::NetworkTableInstance *instance);
void onInstancePostReset(nt::NetworkTableInstance *instance);
void onInstanceDestroy(nt::NetworkTableInstance *instance);

void resetAllInstances();

}; // namespace pyntcore
