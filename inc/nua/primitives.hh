#pragma once
#include <lua.hpp>

namespace nua
{
    template <typename>
    struct is_primitive
    {
        enum { value = false };
    };

    template <>
    struct is_primitive<int>
    {
        enum { value = true };
    };

    template <>
    struct is_primitive<bool>
    {
        enum { value = true };
    };

    template <>
    struct is_primitive<lua_Number>
    {
        enum { value = true };
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
