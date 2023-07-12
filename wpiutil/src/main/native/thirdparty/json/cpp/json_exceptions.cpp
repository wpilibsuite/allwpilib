#define WPI_JSON_IMPLEMENTATION
#include "wpi/detail/exceptions.h"

#include "fmt/format.h"

namespace wpi {
namespace detail {

exception::exception(int id_, std::string_view what_arg)
    : id(id_), m(std::string{what_arg}) {}

parse_error parse_error::create(int id_, std::size_t byte_, std::string_view what_arg)
{
    if (byte_ != 0)
        return parse_error(id_, byte_, fmt::format("[json.exception.parse_error.{}] parse error at {}: {}", id_, byte_, what_arg));
    else
        return parse_error(id_, byte_, fmt::format("[json.exception.parse_error.{}] parse error: {}", id_, what_arg));
}

invalid_iterator invalid_iterator::create(int id_, std::string_view what_arg)
{
    return invalid_iterator(id_, fmt::format("[json.exception.invalid_iterator.{}] {}", id_, what_arg));
}

invalid_iterator invalid_iterator::create(int id_, std::string_view what_arg, std::string_view type_info)
{
    return invalid_iterator(id_, fmt::format("[json.exception.invalid_iterator.{}] {} {}", id_, what_arg, type_info));
}

type_error type_error::create(int id_, std::string_view what_arg)
{
    return type_error(id_, fmt::format("[json.exception.type_error.{}] {}", id_, what_arg));
}

type_error type_error::create(int id_, std::string_view what_arg, std::string_view type_info)
{
    return type_error(id_, fmt::format("[json.exception.type_error.{}] {} {}", id_, what_arg, type_info));
}

out_of_range out_of_range::create(int id_, std::string_view what_arg)
{
    return out_of_range(id_, fmt::format("[json.exception.out_of_range.{}] {}", id_, what_arg));
}

other_error other_error::create(int id_, std::string_view what_arg)
{
    return other_error(id_, fmt::format("[json.exception.other_error.{}] {}", id_, what_arg));
}

}  // namespace detail
} // namespace wpi