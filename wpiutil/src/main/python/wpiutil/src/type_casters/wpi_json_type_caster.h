/***************************************************************************
* Copyright (c) 2019, Martin Renou                                         *
*                                                                          *
Copyright (c) 2019,
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

****************************************************************************/

#pragma once

#include <string>
#include <vector>

#include "wpi/json.h"

#include "pybind11/pybind11.h"

namespace py = pybind11;

namespace pyjson
{
    using number_unsigned_t = uint64_t;
    using number_integer_t = int64_t;

    inline py::object from_json(const wpi::json& j)
    {
        if (j.is_null())
        {
            return py::none();
        }
        else if (j.is_boolean())
        {
            return py::bool_(j.get<bool>());
        }
        else if (j.is_number_unsigned())
        {
            return py::int_(j.get<number_unsigned_t>());
        }
        else if (j.is_number_integer())
        {
            return py::int_(j.get<number_integer_t>());
        }
        else if (j.is_number_float())
        {
            return py::float_(j.get<double>());
        }
        else if (j.is_string())
        {
            return py::str(j.get<std::string>());
        }
        else if (j.is_array())
        {
            py::list obj(j.size());
            for (std::size_t i = 0; i < j.size(); i++)
            {
                obj[i] = from_json(j[i]);
            }
            return std::move(obj);
        }
        else // Object
        {
            py::dict obj;
            for (wpi::json::const_iterator it = j.cbegin(); it != j.cend(); ++it)
            {
                obj[py::str(it.key())] = from_json(it.value());
            }
            return std::move(obj);
        }
    }

    inline wpi::json to_json(const py::handle& obj)
    {
        if (obj.ptr() == nullptr || obj.is_none())
        {
            return nullptr;
        }
        if (py::isinstance<py::bool_>(obj))
        {
            return obj.cast<bool>();
        }
        if (py::isinstance<py::int_>(obj))
        {
            try
            {
                number_integer_t s = obj.cast<number_integer_t>();
                if (py::int_(s).equal(obj))
                {
                    return s;
                }
            }
            catch (...)
            {
            }
            try
            {
                number_unsigned_t u = obj.cast<number_unsigned_t>();
                if (py::int_(u).equal(obj))
                {
                    return u;
                }
            }
            catch (...)
            {
            }
            throw py::value_error("to_json received an integer out of range for both number_integer_t and number_unsigned_t type: " + py::repr(obj).cast<std::string>());
        }
        if (py::isinstance<py::float_>(obj))
        {
            return obj.cast<double>();
        }
        // if (py::isinstance<py::bytes>(obj))
        // {
        //     py::module base64 = py::module::import("base64");
        //     return base64.attr("b64encode")(obj).attr("decode")("utf-8").cast<std::string>();
        // }
        if (py::isinstance<py::str>(obj))
        {
            return obj.cast<std::string>();
        }
        if (py::isinstance<py::tuple>(obj) || py::isinstance<py::list>(obj))
        {
            auto out = wpi::json::array();
            for (const py::handle value : obj)
            {
                out.push_back(to_json(value));
            }
            return out;
        }
        if (py::isinstance<py::dict>(obj))
        {
            auto out = wpi::json::object();
            for (const py::handle key : obj)
            {
                if (py::isinstance<py::str>(key)) {
                    out[key.cast<std::string>()] = to_json(obj[key]);

                } else if (py::isinstance<py::int_>(key) || py::isinstance<py::float_>(key) ||
                           py::isinstance<py::bool_>(key) || py::isinstance<py::none>(key)) {
                    // only allow the same implicit conversions python allows
                    out[py::str(key).cast<std::string>()] = to_json(obj[key]);
                } else {
                    throw py::type_error("JSON keys must be str, int, float, bool, or None, not " + py::repr(key).cast<std::string>());
                }
            }
            return out;
        }
        throw py::type_error("Object of type " + py::type::of(obj).attr("__name__").cast<std::string>() + " is not JSON serializable");
    }
}

// nlohmann_json serializers
namespace wpi
{
    #define MAKE_NLJSON_SERIALIZER_DESERIALIZER(T)         \
    template <>                                            \
    struct adl_serializer<T>                               \
    {                                                      \
        inline static void to_json(json& j, const T& obj)  \
        {                                                  \
            j = pyjson::to_json(obj);                      \
        }                                                  \
                                                           \
        inline static T from_json(const json& j)           \
        {                                                  \
            return pyjson::from_json(j);                   \
        }                                                  \
    }

    #define MAKE_NLJSON_SERIALIZER_ONLY(T)                 \
    template <>                                            \
    struct adl_serializer<T>                               \
    {                                                      \
        inline static void to_json(json& j, const T& obj)  \
        {                                                  \
            j = pyjson::to_json(obj);                      \
        }                                                  \
    }

    MAKE_NLJSON_SERIALIZER_DESERIALIZER(py::object);

    MAKE_NLJSON_SERIALIZER_DESERIALIZER(py::bool_);
    MAKE_NLJSON_SERIALIZER_DESERIALIZER(py::int_);
    MAKE_NLJSON_SERIALIZER_DESERIALIZER(py::float_);
    MAKE_NLJSON_SERIALIZER_DESERIALIZER(py::str);

    MAKE_NLJSON_SERIALIZER_DESERIALIZER(py::list);
    MAKE_NLJSON_SERIALIZER_DESERIALIZER(py::tuple);
    MAKE_NLJSON_SERIALIZER_DESERIALIZER(py::dict);

    MAKE_NLJSON_SERIALIZER_ONLY(py::handle);
    MAKE_NLJSON_SERIALIZER_ONLY(py::detail::item_accessor);
    MAKE_NLJSON_SERIALIZER_ONLY(py::detail::list_accessor);
    MAKE_NLJSON_SERIALIZER_ONLY(py::detail::tuple_accessor);
    MAKE_NLJSON_SERIALIZER_ONLY(py::detail::sequence_accessor);
    MAKE_NLJSON_SERIALIZER_ONLY(py::detail::str_attr_accessor);
    MAKE_NLJSON_SERIALIZER_ONLY(py::detail::obj_attr_accessor);

    #undef MAKE_NLJSON_SERIALIZER
    #undef MAKE_NLJSON_SERIALIZER_ONLY
}

// pybind11 caster
namespace pybind11
{
    namespace detail
    {
        template <> struct type_caster<wpi::json>
        {
        public:
            PYBIND11_TYPE_CASTER(wpi::json, _("wpiutil.json"));

            bool load(handle src, bool convert)
            {
                // TODO: raising errors gives the user informative error messages,
                //       but at the expense of proper argument parsing..
                // try
                // {
                    value = pyjson::to_json(src);
                    return true;
                // }
                // catch (...)
                // {
                //     return false;
                // }
            }

            static handle cast(wpi::json src, return_value_policy /* policy */, handle /* parent */)
            {
                object obj = pyjson::from_json(src);
                return obj.release();
            }
        };
    }
}
