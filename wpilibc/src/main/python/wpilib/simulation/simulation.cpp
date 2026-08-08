
#include "semiwrap_init.wpilib.simulation._simulation.hpp"

#ifndef __FIRST_SYSTEMCORE__

namespace wpi::impl {
void ResetMotorSafety();
}  // namespace wpi::impl

void resetWpilibSimulationData() {
  wpi::impl::ResetMotorSafety();
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

  m.def("_reset_wpilib_simulation_data", &resetWpilibSimulationData,
        release_gil());
  m.def("_reset_motor_safety", &resetMotorSafety, release_gil());
}
