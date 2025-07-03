def fixup_root_package_name(name):
    if name == "wpihal":
        return "hal"
    if name == "wpilib":
        return "wpilibc"
    if name == "wpilog":
        return "datalog"
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
    return name


def fixup_python_dep_name(name):
    if name == "robotpy-datalog":
        return "robotpy-wpilog"
    if name == "robotpy-ntcore":
        return "pyntcore"
    return name
