def fixup_root_package_name(name):
    if name == "wpihal":
        return "hal"
    if name == "wpilib":
        return "wpilibc"
    if name == "wpilog":
        return "datalog"
    if name == "xrp":
        return "xrpVendordep"
    if name == "romi":
        return "romiVendordep"
    if name == "pyntcore":
        return "ntcore"
    if name == "halsim-ws":
        return "simulation/halsim_ws_core"
    return name


def fixup_native_lib_name(name):
    return name


def fixup_shared_lib_name(name):
    if name == "wpihal":
        return "wpiHal"
    if name == "hal":
        return "wpiHal"
    if name == "wpilib":
        return "wpilibc"
    if name == "xrp":
        return "xrpVendordep"
    if name == "romi":
        return "romiVendordep"
    return name


def fixup_python_dep_name(name):
    if name == "robotpy-datalog":
        return "robotpy-wpilog"
    if name == "robotpy-ntcore":
        return "pyntcore"
    if name == "wpilib":
        return "robotpy-wpilib"
    return name
