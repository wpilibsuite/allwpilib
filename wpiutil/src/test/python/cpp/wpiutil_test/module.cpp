// clang-format off
#include <pybind11/pybind11.h>

#include <wpi_array_type_caster.h>
#include <wpi_span_type_caster.h>
#include <wpi_smallset_type_caster.h>
#include <wpi_smallvector_type_caster.h>
#include <wpi_smallvectorimpl_type_caster.h>
#include <wpi_string_map_caster.h>
#include <wpi_json_type_caster.h>
#include <wpi_ct_string_type_caster.h>
#include <wpi_string_type_caster.h>

#include <limits>
#include <functional>

#include <pybind11/functional.h>

/*
array tests
*/
wpi::util::array<int, 4> load_array_int(wpi::util::array<int, 4> data) {
    return data;
}

wpi::util::array<int, 1> load_array_int1(wpi::util::array<int, 1> data) {
    return data;
}

/*
span Tests
*/
std::span<const int> load_span_int(std::span<const int> ref) {
    return ref;
}

std::span<const bool> load_span_bool(std::span<const bool> ref) {
    return ref;
}

std::span<std::string> load_span_string(std::span<std::string> ref) {
    return ref;
}

std::span<const std::string> load_span_string_const(std::span<const std::string> ref) {
    return ref;
}

std::span<std::string_view> load_span_string_view(std::span<std::string_view> ref) {
    return ref;
}

std::span<std::vector<std::string>> load_span_vector(std::span<std::vector<std::string>> ref) {
    return ref;
}

std::span<const double, 3> load_span_fixed_double(std::span<const double, 3> ref) {
    return ref;
}


std::span<int> cast_span() {
    static std::vector<int> vec{1, 2, 3};
    return vec;
}

std::span<const std::string> make_string_span() {
    static std::vector<std::string> vec{"hi", "there"};
    return vec;
}

py::object cast_string_span() {
    return py::cast(make_string_span());
}

std::span<const uint8_t> load_span_bytes(std::span<const uint8_t> ref) {
    return ref;
}

void modify_span_buffer(std::span<uint8_t> ref) {
    ref[0] = 0x4;
}

/*
SmallSet tests
*/

wpi::util::SmallSet<int, 4> load_smallset_int(wpi::util::SmallSet<int, 4> ref) {
    return ref;
}

wpi::util::SmallSet<int, 4> cast_smallset() {
    static wpi::util::SmallSet<int, 4> set;
    set.insert(1);
    set.insert(2);
    set.insert(3);
    set.insert(4);
    return set;
}

/*
SmallVector tests
*/

wpi::util::SmallVector<int, 4> load_smallvec_int(wpi::util::SmallVector<int, 4> ref) {
    return ref;
}

wpi::util::SmallVector<int, 4> cast_smallvec() {
    static wpi::util::SmallVector<int, 4> set;
    set.append({1, 2, 3, 4});
    return set;
}

/*
SmallVectorImpl tests

.. seems like references are the only useful things to do with them
*/

wpi::util::SmallVectorImpl<int>&  load_smallvecimpl_int(wpi::util::SmallVectorImpl<int>& ref) {
    static wpi::util::SmallVector<int, 4> set(ref.begin(), ref.end());
    return set;
}

/*
StringMap tests
*/
wpi::util::StringMap<int> load_stringmap_int(wpi::util::StringMap<int> ref) {
    return ref;
}

wpi::util::StringMap<int> cast_stringmap() {
    static wpi::util::StringMap<int> m;
    m["one"] = 1;
    m["two"] = 2;
    return m;
}

/* JSON tests */
wpi::util::json cast_json_arg(const wpi::util::json &j) {
    return j;
}

wpi::util::json cast_json_val(std::function<wpi::util::json()> fn) {
    return fn();
}

constexpr auto const_string() {
    return wpi::util::ct_string<char, std::char_traits<char>, 3>{{'#', '1', '2'}};
}

void sendable_test(py::module &m);
void struct_test(py::module &m);

/* WPI_String tests */
struct StructWithWPI_String {
    int x;
    WPI_String str;
};

WPI_String load_wpi_string(WPI_String str) {
    return str;
}

WPI_String cast_wpi_string() {
    WPI_String str = wpi::util::make_string("Hello WPI_String!");
    return str;
}

StructWithWPI_String cast_struct_with_wpi_string() {
    StructWithWPI_String output{
        .x = 3504,
        .str = wpi::util::make_string("I'm in a struct!")
    };

    return output;
}

PYBIND11_MODULE(module, m) {

    sendable_test(m);
    struct_test(m);

    // array
    m.def("load_array_int", &load_array_int);
    m.def("load_array_int1", &load_array_int1);
    // span
    m.def("load_span_int", &load_span_int);
    m.def("load_span_bool", &load_span_bool);
    m.def("load_span_fixed_double", &load_span_fixed_double);
    m.def("load_span_string", &load_span_string);
    m.def("load_span_string_const", &load_span_string_const);
    m.def("load_span_string_view", &load_span_string_view);
    m.def("load_span_vector", &load_span_vector);
    m.def("cast_span", &cast_span);
    m.def("cast_string_span", &cast_string_span);
    m.def("load_span_bytes", &load_span_bytes);
    m.def("modify_span_buffer", &modify_span_buffer);
    // SmallSet
    m.def("load_smallset_int", &load_smallset_int);
    m.def("cast_smallset", &cast_smallset);
    // SmallVector
    m.def("load_smallvec_int", &load_smallvec_int);
    m.def("cast_smallvec", &cast_smallvec);
    // SmallVectorImpl
    m.def("load_smallvecimpl_int", &load_smallvecimpl_int);
    // StringMap
    m.def("load_stringmap_int", &load_stringmap_int);
    m.def("cast_stringmap", &cast_stringmap);
    // JSON
    m.def("cast_json_arg", &cast_json_arg); 
    m.def("cast_json_val", &cast_json_val);
    m.attr("max_uint64") = std::numeric_limits<uint64_t>::max();
    m.attr("max_int64") = std::numeric_limits<int64_t>::max();
    m.attr("min_int64") = std::numeric_limits<int64_t>::min();
    // ct_string
    m.def("const_string", &const_string);

    // WPI_String
    m.def("load_wpi_string", &load_wpi_string);
    m.def("cast_wpi_string", &cast_wpi_string);
    
    py::class_<StructWithWPI_String> structWithWpiStringCls(m, "StructWithWPI_String");
    structWithWpiStringCls.def_readwrite("x", &StructWithWPI_String::x);
    structWithWpiStringCls.def_readonly("str", &StructWithWPI_String::str);
    m.def("cast_struct_with_wpi_string", &cast_struct_with_wpi_string);
};
