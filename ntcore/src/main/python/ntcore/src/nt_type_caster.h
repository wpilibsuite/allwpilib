#pragma once

#include <pybind11/pybind11.h>

namespace pybind11 {
namespace detail {

// ntcore uses std::vector<uint8_t> anytime there is a raw value, so
// add this specialization to convert to/from bytes directly

template<>
struct type_caster<std::vector<uint8_t>> {
    using vector_type = std::vector<uint8_t>;
    PYBIND11_TYPE_CASTER(vector_type, const_name("bytes"));

    bool load(handle src, bool convert) {
        if (!isinstance<buffer>(src)) {
            return false;
        }
        auto buf = reinterpret_borrow<buffer>(src);
        auto req = buf.request();
        if (req.ndim != 1) {
            return false;
        }

        auto begin = (const uint8_t*)req.ptr;
        auto end = begin + req.size*req.itemsize;

        value = std::vector<uint8_t>(begin, end);
        return true;
    }

    static handle cast(const std::vector<uint8_t> &src, return_value_policy policy, handle parent) {
        return py::bytes((char*)src.data(), src.size()).release();
    }
};

}
}