#include <semiwrap.h>
#include "nt_instance.h"
#include "wpi/nt/ntcore_cpp.hpp"

#include <set>

// only accessed under GIL
static std::set<NT_Inst> g_known_instances;

namespace pyntcore {

void onInstanceStart(wpi::nt::NetworkTableInstance *instance) {
    g_known_instances.emplace(instance->GetHandle());

    py::module::import("ntcore._logutil")
        .attr("NtLogForwarder").attr("onInstanceStart")(instance);
}

void onInstancePreReset(wpi::nt::NetworkTableInstance *instance) {
    py::module::import("ntcore._logutil")
        .attr("NtLogForwarder").attr("onInstanceDestroy")(instance);
}

void onInstancePostReset(wpi::nt::NetworkTableInstance *instance) {
    py::module::import("ntcore.util")
        .attr("_NtProperty").attr("onInstancePostReset")(instance);
}

void onInstanceDestroy(wpi::nt::NetworkTableInstance *instance) {
    py::module::import("ntcore._logutil")
        .attr("NtLogForwarder").attr("onInstanceDestroy")(instance);
    py::module::import("ntcore.util")
        .attr("_NtProperty").attr("onInstanceDestroy")(instance);

    g_known_instances.erase(instance->GetHandle());
}

// reset all instances to clear out any potential python references that
// might be hanging around in a callback or something
void resetAllInstances()
{
    std::set<NT_Inst> known_instances;
    known_instances.swap(g_known_instances);

    // always reset the default instance
    known_instances.emplace(wpi::nt::GetDefaultInstance());

    py::gil_scoped_release unlock;

    for (auto &inst: known_instances) {
        wpi::nt::ResetInstance(inst);
    }
}


}; // namespace pyntcore