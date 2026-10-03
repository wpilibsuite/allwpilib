
#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <span>
#include <type_traits>
#include <utility>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "wpi/util/SmallVector.hpp"

namespace pybind11 {
namespace detail {

template <size_t N>
struct span_name_maker {
  template <typename T>
  static constexpr auto make(const T& t) {
    return concat(t, span_name_maker<N - 1>::make(t));
  }
};

template <>
struct span_name_maker<1> {
  template <typename T>
  static constexpr auto make(const T& t) {
    return t;
  }
};

template <>
struct span_name_maker<0> {
  template <typename T>
  static constexpr auto make(const T&) {
    return _("()");
  }
};

// A tuple snapshot stabilizes the length and keeps generated Python elements
// alive. Retain their casters too: custom casters can own storage behind views.
// An array avoids moving casters, which need not be movable and may contain
// pointers into their own inline storage.
template <typename Type>
struct span_sequence_storage {
  using value_conv = make_caster<Type>;
  tuple elements;
  std::unique_ptr<value_conv[]> converters;

  bool load(handle src, bool convert, size_t extent = std::dynamic_extent) {
    if (!isinstance<sequence>(src) || isinstance<str>(src))
      return false;
    auto snapshot = tuple(reinterpret_borrow<sequence>(src));
    if (extent != std::dynamic_extent && snapshot.size() != extent)
      return false;

    converters.reset();
    elements = std::move(snapshot);
    converters = std::make_unique<value_conv[]>(elements.size());
    for (size_t i = 0; i < elements.size(); ++i) {
      if (!converters[i].load(elements[i], convert))
        return false;
    }
    return true;
  }
};

// Sequence inputs are copied into call-local storage; mutations do not write
// back to Python. C++ must not retain the resulting span beyond the call.
// Return conversion honors the element policy. Direct py::cast(rvalue_span)
// resolves its default policy to move before reaching this caster; use an
// explicit copy/reference policy at those call sites to avoid moving elements.
// A span with fixed size converts to a tuple.
template <typename Type, size_t Extent>
struct type_caster<std::span<Type, Extent>> {
  using span_type = std::span<Type, Extent>;
  using value_conv = make_caster<Type>;
  using value_type = std::remove_cv_t<Type>;

  span_sequence_storage<Type> storage;
  std::array<value_type, Extent> backing_array{};

  PYBIND11_TYPE_CASTER(span_type,
                       _("Tuple[") +
                           span_name_maker<Extent>::make(value_conv::name) +
                           _("]"));

  type_caster() : value(backing_array) {}

  bool load(handle src, bool convert) {
    if (!storage.load(src, convert, Extent))
      return false;
    for (size_t i = 0; i < Extent; ++i) {
      backing_array[i] = cast_op<Type&&>(std::move(storage.converters[i]));
    }
    return true;
  }

 public:
  template <typename T>
  static handle cast(T&& src, return_value_policy policy, handle parent) {
    // A span never owns its elements, even when the span itself is an rvalue.
    tuple l(Extent);
    size_t index = 0;
    for (auto& value : src) {
      auto value_ =
          reinterpret_steal<object>(value_conv::cast(value, policy, parent));
      if (!value_)
        return handle();
      PyTuple_SET_ITEM(l.ptr(), (ssize_t)index++,
                       value_.release().ptr());  // steals a reference
    }
    return l.release();
  }
};

// span with dynamic extent
template <typename Type>
struct type_caster<std::span<Type, std::dynamic_extent>> {
  using span_type = std::span<Type>;
  using value_conv = make_caster<Type>;
  using value_type = std::remove_cv_t<Type>;
  PYBIND11_TYPE_CASTER(span_type, _("List[") + value_conv::name + _("]"));

  span_sequence_storage<Type> storage;
  wpi::util::SmallVector<value_type, 32> vec;
  bool load(handle src, bool convert) {
    value = span_type{};
    vec.clear();
    if (!storage.load(src, convert))
      return false;
    vec.reserve(storage.elements.size());
    for (size_t i = 0; i < storage.elements.size(); ++i) {
      vec.push_back(cast_op<Type&&>(std::move(storage.converters[i])));
    }
    value = span_type(vec.data(), vec.size());
    return true;
  }

 public:
  template <typename T>
  static handle cast(T&& src, return_value_policy policy, handle parent) {
    // Preserve the requested element policy without implicitly moving from src.
    list l(src.size());
    size_t index = 0;
    for (auto& value : src) {
      auto value_ =
          reinterpret_steal<object>(value_conv::cast(value, policy, parent));
      if (!value_)
        return handle();
      PyList_SET_ITEM(l.ptr(), (ssize_t)index++,
                      value_.release().ptr());  // steals a reference
    }
    return l.release();
  }
};

// Byte spans borrow contiguous raw bytes, including multi-byte buffer formats.
// Keep the export (not just its Python owner) alive until the call has
// finished.
template <typename Type>
struct span_byte_caster {
  using span_type = std::span<Type>;
  buffer_info buffer_view;
  PYBIND11_TYPE_CASTER(span_type, _("Buffer"));

  bool load(handle src, bool) {
    if (!isinstance<buffer>(src))
      return false;
    auto buf = reinterpret_borrow<buffer>(src);
    auto req = buf.request(!std::is_const_v<Type>);
    if (req.ndim != 1 || !PyBuffer_IsContiguous(req.view(), 'C'))
      return false;

    buffer_view = std::move(req);
    value = span_type(static_cast<Type*>(buffer_view.ptr),
                      buffer_view.size * buffer_view.itemsize);
    return true;
  }

  template <typename T>
  static handle cast(T&& src, return_value_policy, handle) {
    // A memoryview would need an owner that guarantees the span's lifetime.
    return bytes(reinterpret_cast<const char*>(src.data()), src.size())
        .release();
  }
};

template <>
struct type_caster<std::span<const uint8_t, std::dynamic_extent>>
    : span_byte_caster<const uint8_t> {};

template <>
struct type_caster<std::span<uint8_t, std::dynamic_extent>>
    : span_byte_caster<uint8_t> {};

}  // namespace detail
}  // namespace pybind11
