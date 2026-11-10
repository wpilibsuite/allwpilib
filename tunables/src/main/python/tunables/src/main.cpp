#include "rpy/TunableStorage.h"
#include "semiwrap_init.tunables._tunables.hpp"

SEMIWRAP_PYBIND11_MODULE(m) {
  initWrapper(m);
  wpi::tunables::python::InitializeTunablePython(m);
}
