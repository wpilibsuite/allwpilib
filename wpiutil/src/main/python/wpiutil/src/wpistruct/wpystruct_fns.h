
#pragma once

#include "wpystruct.h"

/**
    Call a function to retrieve the (type string, schema) for each nested struct
*/
void forEachNested(
    const py::type &t,
    const std::function<void(std::string_view, std::string_view)> &fn);

/**
    Retrieve the type name for the specified type
*/
py::str getTypeName(const py::type &t);

/**
    Retrieve schema for the specified type
*/
py::str getSchema(const py::type &t);

/**
    Returns the serialized size in bytes
*/
size_t getSize(const py::type &t);

/**
    Serialize object into byte buffer
*/
py::bytes pack(const WPyStruct &v);

/**
    Serialize objects into byte buffer
*/
py::bytes packArray(const py::sequence &seq);

/**
    Serialize object into byte buffer. Buffer must be exact size.
*/
void packInto(const WPyStruct &v, py::buffer &b);

/**
    Convert byte buffer into object of specified type. Buffer must be exact
    size.
*/
WPyStruct unpack(const py::type &t, const py::buffer &b);

/**
    Convert byte buffer into list of objects of specified type. Buffer must be
    exact size.
*/
py::typing::List<WPyStruct> unpackArray(const py::type &t, const py::buffer &b);

// /**
//     Convert byte buffer into passed in object. Buffer must be exact
//     size.
// */
// void unpackInto(const py::buffer &b, WPyStruct *v);
