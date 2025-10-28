
#include "semiwrap_init.wpilib.simulation._simulation.hpp"

#ifndef __FRC_SYSTEMCORE__

namespace wpi::impl {
void ResetSmartDashboardInstance();
void ResetMotorSafety();
} // namespace wpi::impl

namespace wpi::util::impl {
void ResetSendableRegistry();
} // namespace wpi::impl

void resetWpilibSimulationData() {
  wpi::impl::ResetSmartDashboardInstance();
  wpi::impl::ResetMotorSafety();
  wpi::util::impl::ResetSendableRegistry();
}

void resetMotorSafety() {
  wpi::impl::ResetMotorSafety();
}

#else
void resetWpilibSimulationData() {}
void resetMotorSafety() {}
#endif

SEMIWRAP_PYBIND11_MODULE(m) {
  initWrapper(m);

  m.def("_resetWpilibSimulationData", &resetWpilibSimulationData,
        release_gil());
  m.def("_resetMotorSafety", &resetMotorSafety, release_gil());
}
