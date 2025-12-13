
#include "wpystruct_fns.h"

#include "wpystruct.h"

void forEachNested(
    const py::type& t,
    const std::function<void(std::string_view, std::string_view)>& fn) {
  WPyStructInfo info(t);
  wpi::util::ForEachStructSchema<WPyStruct, WPyStructInfo>(fn, info);
}

py::str getTypeName(const py::type& t) {
  WPyStructInfo info(t);
  return wpi::util::GetStructTypeName<WPyStruct, WPyStructInfo>(info);
}

py::str getSchema(const py::type& t) {
  WPyStructInfo info(t);
  return wpi::util::GetStructSchema<WPyStruct, WPyStructInfo>(info);
}

size_t getSize(const py::type& t) {
  WPyStructInfo info(t);
  return wpi::util::GetStructSize<WPyStruct>(info);
}

py::bytes pack(const WPyStruct& v) {
  WPyStructInfo info(v);

  auto sz = wpi::util::GetStructSize<WPyStruct>(info);
  PyObject* b = PyBytes_FromStringAndSize(NULL, sz);
  if (b == NULL) {
    throw py::error_already_set();
  }

  char* pybuf;
  py::ssize_t pysz;
  if (PyBytes_AsStringAndSize(b, &pybuf, &pysz) != 0) {
    Py_DECREF(b);
    throw py::error_already_set();
  }

  auto s = std::span(reinterpret_cast<uint8_t*>(pybuf), pysz);
  wpi::util::PackStruct(s, v, info);

  return py::reinterpret_steal<py::bytes>(b);
}

py::bytes packArray(const py::sequence& seq) {
  auto len = seq.size();
  if (len == 0) {
    return {};
  }

  WPyStructInfo info(py::type::of(seq[0]));
  auto sz = wpi::util::GetStructSize<WPyStruct>(info);
  auto total = sz * len;

  PyObject* b = PyBytes_FromStringAndSize(NULL, total);
  if (b == NULL) {
    throw py::error_already_set();
  }

  char* pybuf;
  py::ssize_t pysz;
  if (PyBytes_AsStringAndSize(b, &pybuf, &pysz) != 0) {
    Py_DECREF(b);
    throw py::error_already_set();
  }

  auto bytes_obj = py::reinterpret_steal<py::bytes>(b);

  for (const auto& v : seq) {
    WPyStruct wv(v);
    auto s = std::span(reinterpret_cast<uint8_t*>(pybuf), sz);
    wpi::util::PackStruct(s, wv, info);
    pybuf += sz;
  }

  return bytes_obj;
}

void packInto(const WPyStruct& v, py::buffer& b) {
  WPyStructInfo info(v);
  py::ssize_t sz = wpi::util::GetStructSize<WPyStruct>(info);

  auto req = b.request();
  if (req.itemsize != 1) {
    throw py::value_error("buffer must only contain bytes");
  } else if (req.ndim != 1) {
    throw py::value_error("buffer must only have a single dimension");
  }

  if (req.size != sz) {
    throw py::value_error("buffer must be " + std::to_string(sz) + " bytes");
  }

  auto s = std::span(reinterpret_cast<uint8_t*>(req.ptr), req.size);
  wpi::util::PackStruct(s, v, info);
}

WPyStruct unpack(const py::type& t, const py::buffer& b) {
  WPyStructInfo info(t);
  py::ssize_t sz = wpi::util::GetStructSize<WPyStruct>(info);

  auto req = b.request();
  if (req.itemsize != 1) {
    throw py::value_error("buffer must only contain bytes");
  } else if (req.ndim != 1) {
    throw py::value_error("buffer must only have a single dimension");
  }

  if (req.size != sz) {
    throw py::value_error("buffer must be " + std::to_string(sz) + " bytes");
  }

  auto s = std::span((const uint8_t*)req.ptr, req.size);
  return wpi::util::UnpackStruct<WPyStruct, WPyStructInfo>(s, info);
}

py::typing::List<WPyStruct> unpackArray(const py::type& t,
                                        const py::buffer& b) {
  WPyStructInfo info(t);
  py::ssize_t sz = wpi::util::GetStructSize<WPyStruct>(info);

  auto req = b.request();
  if (req.itemsize != 1) {
    throw py::value_error("buffer must only contain bytes");
  } else if (req.ndim != 1) {
    throw py::value_error("buffer must only have a single dimension");
  }

  if (req.size % sz != 0) {
    throw py::value_error("buffer must be multiple of " + std::to_string(sz) +
                          " bytes");
  }

  auto items = req.size / sz;
  py::list a(items);
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(req.ptr);
  for (py::ssize_t i = 0; i < items; i++) {
    auto s = std::span(ptr, sz);
    auto v = wpi::util::UnpackStruct<WPyStruct, WPyStructInfo>(s, info);
    // steals a reference
    PyList_SET_ITEM(a.ptr(), i, v.py.inc_ref().ptr());
    ptr += sz;
  }

  return a;
}

// void unpackInto(const py::buffer &b, WPyStruct *v) {
//   WPyStructInfo info(*v);
//   py::ssize_t sz = wpi::util::GetStructSize<WPyStruct>(info);

//   auto req = b.request();
//   if (req.itemsize != 1) {
//     throw py::value_error("buffer must only contain bytes");
//   } else if (req.ndim != 1) {
//     throw py::value_error("buffer must only have a single dimension");
//   }

//   if (req.size != sz) {
//     throw py::value_error("buffer must be " + std::to_string(sz) + " bytes");
//   }

//   auto s = std::span((const uint8_t *)req.ptr, req.size);
//   wpi::util::UnpackStructInto<WPyStruct, WPyStructInfo>(v, s, info);
// }
