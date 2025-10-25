
#include "pyentry.h"
#include "py2value.h"

#include <pybind11/stl.h>
#include <wpi_span_type_caster.h>

namespace pyntcore {

py::object GetBooleanEntry(const nt::NetworkTableEntry &entry, py::object defaultValue) {
    nt::Value value;
    {
        py::gil_scoped_release release;
        value = nt::GetEntryValue(entry.GetHandle());
    }
    if (!value || value.type() != NT_BOOLEAN) return defaultValue;
    return py::cast(value.GetBoolean());
}

py::object GetDoubleEntry(const nt::NetworkTableEntry &entry, py::object defaultValue) {
    nt::Value value;
    {
        py::gil_scoped_release release;
        value = nt::GetEntryValue(entry.GetHandle());
    }
    if (!value || value.type() != NT_DOUBLE) return defaultValue;
    return py::cast(value.GetDouble());
}

py::object GetFloatEntry(const nt::NetworkTableEntry &entry, py::object defaultValue) {
    nt::Value value;
    {
        py::gil_scoped_release release;
        value = nt::GetEntryValue(entry.GetHandle());
    }
    if (!value || value.type() != NT_FLOAT) return defaultValue;
    return py::cast(value.GetFloat());
}

py::object GetIntegerEntry(const nt::NetworkTableEntry &entry, py::object defaultValue) {
    nt::Value value;
    {
        py::gil_scoped_release release;
        value = nt::GetEntryValue(entry.GetHandle());
    }
    if (!value || value.type() != NT_INTEGER) return defaultValue;
    return py::cast(value.GetInteger());
}


py::object GetStringEntry(const nt::NetworkTableEntry &entry, py::object defaultValue) {
    nt::Value value;
    {
        py::gil_scoped_release release;
        value = nt::GetEntryValue(entry.GetHandle());
    }
    if (!value || value.type() != NT_STRING) return defaultValue;
    auto s = value.GetString();
    return py::str(s.data(), s.size());
}

py::object GetRawEntry(const nt::NetworkTableEntry &entry, py::object defaultValue) {
    nt::Value value;
    {
        py::gil_scoped_release release;
        value = nt::GetEntryValue(entry.GetHandle());
    }
    if (!value || value.type() != NT_RAW) return defaultValue;
    return py::cast(value.GetRaw());
}

py::object GetBooleanArrayEntry(const nt::NetworkTableEntry &entry, py::object defaultValue) {
    nt::Value value;
    {
        py::gil_scoped_release release;
        value = nt::GetEntryValue(entry.GetHandle());
    }
    if (!value || value.type() != NT_BOOLEAN_ARRAY) return defaultValue;
    // ntcore will return bit vector by default. Convert to List[bool]
    auto v = value.value();
    py::list l(v.data.arr_boolean.size);
    for (size_t i = 0; i < v.data.arr_boolean.size; i++) {
        auto b = py::bool_(v.data.arr_boolean.arr[i]);
        PyList_SET_ITEM(l.ptr(), i, b.release().ptr());
    }
    return std::move(l);
}

py::object GetDoubleArrayEntry(const nt::NetworkTableEntry &entry, py::object defaultValue) {
    nt::Value value;
    {
        py::gil_scoped_release release;
        value = nt::GetEntryValue(entry.GetHandle());
    }
    if (!value || value.type() != NT_DOUBLE_ARRAY) return defaultValue;
    return py::cast(value.GetDoubleArray());
}

py::object GetFloatArrayEntry(const nt::NetworkTableEntry &entry, py::object defaultValue) {
    nt::Value value;
    {
        py::gil_scoped_release release;
        value = nt::GetEntryValue(entry.GetHandle());
    }
    if (!value || value.type() != NT_FLOAT_ARRAY) return defaultValue;
    return py::cast(value.GetFloatArray());
}

py::object GetIntegerArrayEntry(const nt::NetworkTableEntry &entry, py::object defaultValue) {
    nt::Value value;
    {
        py::gil_scoped_release release;
        value = nt::GetEntryValue(entry.GetHandle());
    }
    if (!value || value.type() != NT_INTEGER_ARRAY) return defaultValue;
    return py::cast(value.GetIntegerArray());
}

py::object GetStringArrayEntry(const nt::NetworkTableEntry &entry, py::object defaultValue) {
    nt::Value value;
    {
        py::gil_scoped_release release;
        value = nt::GetEntryValue(entry.GetHandle());
    }
    if (!value || value.type() != NT_STRING_ARRAY) return defaultValue;
    std::span<const std::string> rval = value.GetStringArray();
    return py::cast(rval);
}

py::object GetValueEntry(const nt::NetworkTableEntry &entry, py::object defaultValue) {
    nt::Value value;
    {
        py::gil_scoped_release release;
        value = nt::GetEntryValue(entry.GetHandle());
    }
    if (!value) return defaultValue;
    return ntvalue2py(value);
}


}; // pyntcore
