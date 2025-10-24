
#include "semiwrap_init.wpilib.simulation._simulation.hpp"

#ifndef __FRC_SYSTEMCORE__

namespace frc::impl {
void ResetSmartDashboardInstance();
void ResetMotorSafety();
} // namespace frc::impl

namespace wpi::impl {
void ResetSendableRegistry();
} // namespace wpi::impl

void resetWpilibSimulationData() {
  frc::impl::ResetSmartDashboardInstance();
  frc::impl::ResetMotorSafety();
  wpi::impl::ResetSendableRegistry();
}

void resetMotorSafety() {
  frc::impl::ResetMotorSafety();
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
