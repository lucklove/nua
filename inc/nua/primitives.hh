#pragma once
#include <lua.hpp>
#include <type_traits>

namespace nua
{
    template <typename T>
    struct is_primitive
    {
        enum { value = std::is_integral<T>::value || std::is_floating_point<T>::value };
    };

    template <>
    struct is_primitive<std::string>
    {
        enum { value = true };
    };

    template <>
    struct is_primitive<std::nullptr_t>
    {
        enum { value = true };
    };
}       /**< nua */
