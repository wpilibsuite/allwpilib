#pragma once

#include <pybind11/pybind11.h>

namespace wpi::util::python {

// Other buffer exporters may be numeric sequences; require an explicit
// memoryview to interpret those as raw bytes.
inline bool IsBytesLike(pybind11::handle value) {
  return PyBytes_Check(value.ptr()) || PyByteArray_Check(value.ptr()) ||
         PyMemoryView_Check(value.ptr());
}

// The returned buffer_info keeps the export alive. Borrow C-contiguous storage
// directly; flatten strided or indirect buffers into owned bytes in C order.
inline pybind11::buffer_info RequestContiguousBuffer(pybind11::buffer value) {
  if (PyBytes_Check(value.ptr()) || PyByteArray_Check(value.ptr())) {
    return value.request();
  }

  // Normalize before request(), which does not support indirect buffers.
  auto contiguous = pybind11::reinterpret_steal<pybind11::buffer>(
      PyMemoryView_GetContiguous(value.ptr(), PyBUF_READ, 'C'));
  if (!contiguous) {
    throw pybind11::error_already_set{};
  }
  return contiguous.request();
}

}  // namespace wpi::util::python
