
#include "TunablePython.h"
#include "semiwrap_init.tunable._tunable.hpp"

SEMIWRAP_PYBIND11_MODULE(m) {
  initWrapper(m);
  wpi::InitTunablePython(m);
}
