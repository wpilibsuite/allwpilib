
#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <wpi/SmallVector.h>
#include <span>

namespace pybind11 {
namespace detail {

template <size_t N>
struct span_name_maker {
  template <typename T>
  static constexpr auto make(const T &t) {
    return concat(t, span_name_maker<N-1>::make(t));
  }
};

template <>
struct span_name_maker<1> {
  template <typename T>
  static constexpr auto make(const T &t) {
    return t;
  }
};

// span with fixed size converts to a tuple
template <typename Type, size_t Extent> struct type_caster<std::span<Type, Extent>> {
  using span_type = typename std::span<Type, Extent>;
  using value_conv = make_caster<Type>;
  using value_type = typename std::remove_cv<Type>::type;

  value_type backing_array[Extent] = {};

  PYBIND11_TYPE_CASTER(span_type, _("Tuple[") + span_name_maker<Extent>::make(value_conv::name) + _("]"));

  type_caster() : value(backing_array) {}
  
  bool load(handle src, bool convert) {
    if (!isinstance<sequence>(src) || isinstance<str>(src))
      return false;
    auto s = reinterpret_borrow<sequence>(src);
    if (s.size() != Extent)
      return false;
    size_t i = 0;
    for (auto it : s) {
      value_conv conv;
      if (!conv.load(it, convert))
        return false;
      backing_array[i] = cast_op<Type &&>(std::move(conv));
      i++;
    }
    return true;
  }

public:
  template <typename T>
  static handle cast(T &&src, return_value_policy policy, handle parent) {
    if (!std::is_lvalue_reference<T>::value)
      policy = return_value_policy_override<Type>::policy(policy);
    tuple l(Extent);
    size_t index = 0;
    for (auto &&value : src) {
      auto value_ = reinterpret_steal<object>(
          value_conv::cast(forward_like<T>(value), policy, parent));
      if (!value_)
        return handle();
      PyTuple_SET_ITEM(l.ptr(), (ssize_t)index++,
                      value_.release().ptr()); // steals a reference
    }
    return l.release();
  }
};


// span with dynamic extent
template <typename Type> struct type_caster<std::span<Type, std::dynamic_extent>> {
  using span_type = typename  std::span<Type, std::dynamic_extent>;
  using value_conv = make_caster<Type>;
  using value_type = typename std::remove_cv<Type>::type;
  PYBIND11_TYPE_CASTER(span_type, _("List[") + value_conv::name + _("]"));

  wpi::SmallVector<value_type, 32> vec;
  bool load(handle src, bool convert) {
    if (!isinstance<sequence>(src) || isinstance<str>(src))
      return false;
    auto s = reinterpret_borrow<sequence>(src);
    vec.reserve(s.size());
    for (auto it : s) {
      value_conv conv;
      if (!conv.load(it, convert))
        return false;
      vec.push_back(cast_op<Type &&>(std::move(conv)));
    }
    value = span_type(std::data(vec), std::size(vec));
    return true;
  }

public:
  template <typename T>
  static handle cast(T &&src, return_value_policy policy, handle parent) {
    if (!std::is_lvalue_reference<T>::value)
      policy = return_value_policy_override<Type>::policy(policy);
    list l(src.size());
    size_t index = 0;
    for (auto &&value : src) {
      auto value_ = reinterpret_steal<object>(
          value_conv::cast(forward_like<T>(value), policy, parent));
      if (!value_)
        return handle();
      PyList_SET_ITEM(l.ptr(), (ssize_t)index++,
                      value_.release().ptr()); // steals a reference
    }
    return l.release();
  }
};

// span specialization: accepts any readonly buffers
template <> struct type_caster<std::span<const uint8_t, std::dynamic_extent>> {
  using span_type = typename  std::span<const uint8_t, std::dynamic_extent>;
  PYBIND11_TYPE_CASTER(span_type, _("Buffer"));

  bool load(handle src, bool convert) {
    if (!isinstance<buffer>(src))
      return false;
    auto buf = reinterpret_borrow<buffer>(src);
    auto req = buf.request();
    if (req.ndim != 1) {
      return false;
    }

    value = span_type((const uint8_t*)req.ptr, req.size*req.itemsize);
    return true;
  }

public:
  template <typename T>
  static handle cast(T &&src, return_value_policy policy, handle parent) {
    return bytes((char*)src.data(), src.size()).release();
  }
};

// span specialization: writeable buffer
template <> struct type_caster<std::span<uint8_t, std::dynamic_extent>> {
  using span_type = typename  std::span<const uint8_t, std::dynamic_extent>;
  PYBIND11_TYPE_CASTER(std::span<uint8_t>, _("Buffer"));

  bool load(handle src, bool convert) {
    if (!isinstance<buffer>(src))
      return false;
    auto buf = reinterpret_borrow<buffer>(src);
    auto req = buf.request(true); // buffer must be writeable
    if (req.ndim != 1) {
      return false;
    }

    value = std::span<uint8_t>((uint8_t*)req.ptr, req.size*req.itemsize);
    return true;
  }

public:
  template <typename T>
  static handle cast(T &&src, return_value_policy policy, handle parent) {
    // TODO: should this be a memoryview instead?
    return bytes((char*)src.data(), src.size()).release();
  }
};

} // namespace detail
} // namespace pybind11