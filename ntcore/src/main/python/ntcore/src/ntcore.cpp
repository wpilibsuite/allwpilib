
#include <semiwrap_init.ntcore._ntcore.hpp>
#include "nt_instance.h"

SEMIWRAP_PYBIND11_MODULE(m) {
  initWrapper(m);

  static int unused;
  py::capsule cleanup(&unused, [](void *) {
    pyntcore::resetAllInstances();
  });

  m.add_object("_st_cleanup", cleanup);
}