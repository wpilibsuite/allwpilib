#pragma once

#include <pybind11/pybind11.h>

namespace pybind11 {
namespace detail {

/*
  Units type caster assumptions

  When going from C++ to Python (eg, return values), the units that a user
  gets as a float are in whatever the unit was for C++.

    units::foot_t getFeet();    // converted to float, value is feet

  When going from Python to C++, the units a user uses are once again
  whatever the C++ function specifies:

    void setFeet(units::foot_t ft);   // must pass a float, it's in feet
    void setMeters(units::meter_t m); // must pass a float, it's in meters

  Unfortunately, with this type caster and robotpy-build there are mismatch
  issues with implicit conversions when default values are used that don't
  match the actual value:

    foo(units::second_t tm = 10_ms);    // if not careful, pybind11 will 
                                        // store as 10 seconds
*/
template <class U, typename T, template <typename> class S>
struct type_caster<units::unit_t<U, T, S>> {
  using value_type = units::unit_t<U, T, S>;

  // TODO: there should be a way to include the type with this
  PYBIND11_TYPE_CASTER(value_type, handle_type_name<value_type>::name);

  // Python -> C++
  bool load(handle src, bool convert) {
    if (!src)
      return false;
    if (!convert && !PyFloat_Check(src.ptr()))
      return false;
    auto cvted = PyFloat_AsDouble(src.ptr());
    value = value_type(cvted);
    return !(cvted == -1 && PyErr_Occurred());
  }

  // C++ -> Python
  static handle cast(const value_type &src, return_value_policy /* policy */,
                     handle /* parent */) {
    return PyFloat_FromDouble(src.template to<double>());
  }
};

} // namespace detail
} // namespace pybind11