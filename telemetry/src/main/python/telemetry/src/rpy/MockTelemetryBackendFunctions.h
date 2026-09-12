#pragma once

#include <pybind11/pybind11.h>
#include <wpi/telemetry/MockTelemetryBackend.hpp>

namespace wpi::telemetry::python {

pybind11::object ActionValueToPython(
    const wpi::telemetry::MockTelemetryBackend::Action& action);
pybind11::object SchemaToPython(
    const wpi::telemetry::MockTelemetryBackend::Schema* schema);

}  // namespace wpi::telemetry::python
