#include <array>
#include <functional>
#include <memory>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <wpi_span_type_caster.h>

namespace py = pybind11;

namespace {

// Model a custom element caster that owns the memory its converted view uses.
struct CasterOwnedView {
  std::string_view text;
  std::weak_ptr<int> lifetime;
};

struct SpanElement {
  int value;

  explicit SpanElement(int value) : value(value) {}
  SpanElement(const SpanElement&) = default;
  SpanElement(SpanElement&& other) noexcept
      : value(std::exchange(other.value, -1)) {}
};

struct SpanOwner {
  std::array<SpanElement, 2> values{SpanElement{11}, SpanElement{22}};
};

template <typename T, size_t Extent>
void BindOwnerSpan(py::class_<SpanOwner>& cls, const char* name,
                   py::return_value_policy policy) {
  cls.def(
      name, [](SpanOwner& owner) { return std::span<T, Extent>(owner.values); },
      policy);
}

}  // namespace

namespace pybind11::detail {

template <>
struct type_caster<CasterOwnedView> {
  std::string backing;
  std::shared_ptr<int> lifetime = std::make_shared<int>(0);
  PYBIND11_TYPE_CASTER(CasterOwnedView, _("str"));

  type_caster() = default;
  type_caster(const type_caster&) = delete;
  type_caster(type_caster&&) = delete;

  bool load(handle src, bool convert) {
    make_caster<std::string> conv;
    if (!conv.load(src, convert)) {
      return false;
    }
    backing = cast_op<std::string&&>(std::move(conv));
    value = {backing, lifetime};
    return true;
  }

  static handle cast(const CasterOwnedView& src, return_value_policy, handle) {
    if (src.lifetime.expired()) {
      throw std::runtime_error(
          "element caster destroyed before return conversion");
    }
    return py::str(src.text.data(), src.text.size()).release();
  }
};

}  // namespace pybind11::detail

void span_safety_test(py::module_& m) {
  m.def("span_zero", [](std::span<const int, 0> values) { return values; });
  m.def("load_span_dynamic_double",
        [](std::span<const double> values) { return values; });
  m.def("span_string_views_with_callback",
        [](std::span<std::string_view> values,
           const std::function<void()>& callback) {
          callback();
          return values;
        });
  m.def("fixed_span_string_views_with_callback",
        [](std::span<std::string_view, 3> values,
           const std::function<void()>& callback) {
          callback();
          return values;
        });
  m.def("span_caster_owned_views",
        [](std::span<CasterOwnedView> values) { return values; });
  m.def("fixed_span_caster_owned_views",
        [](std::span<CasterOwnedView, 3> values) { return values; });
  m.def("span_bytes_with_callback", [](std::span<const uint8_t> values,
                                       const std::function<void()>& callback) {
    callback();
    return values;
  });
  m.def("writable_span_bytes_with_callback",
        [](std::span<uint8_t> values, const std::function<void()>& callback) {
          callback();
          return values;
        });

  py::class_<SpanElement>(m, "SpanElement")
      .def_readwrite("value", &SpanElement::value);
  py::class_<SpanOwner> owner(m, "SpanOwner");
  owner.def(py::init<>()).def("values", [](const SpanOwner& self) {
    return py::make_tuple(self.values[0].value, self.values[1].value);
  });

  owner.def("cast_span_copy", [](SpanOwner& self) {
    return py::cast(std::span<SpanElement>(self.values),
                    py::return_value_policy::copy);
  });
  owner.def("cast_fixed_span_copy", [](SpanOwner& self) {
    return py::cast(std::span<SpanElement, 2>(self.values),
                    py::return_value_policy::copy);
  });

  for (auto [suffix, policy] :
       {std::pair{"", py::return_value_policy::automatic},
        std::pair{"_copy", py::return_value_policy::copy},
        std::pair{"_reference_internal",
                  py::return_value_policy::reference_internal},
        std::pair{"_reference", py::return_value_policy::reference},
        std::pair{"_move", py::return_value_policy::move}}) {
    BindOwnerSpan<SpanElement, std::dynamic_extent>(
        owner, (std::string("span") + suffix).c_str(), policy);
    BindOwnerSpan<SpanElement, 2>(
        owner, (std::string("fixed_span") + suffix).c_str(), policy);
    BindOwnerSpan<const SpanElement, std::dynamic_extent>(
        owner, (std::string("const_span") + suffix).c_str(), policy);
    BindOwnerSpan<const SpanElement, 2>(
        owner, (std::string("const_fixed_span") + suffix).c_str(), policy);
  }
}
