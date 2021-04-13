#pragma once

namespace nua
{
    struct NuaPrimitiveTypeError
    {
        int expected_type;
        int index;
    };

    struct NuaUserDataTypeError
    {
        std::string metatable_name;
        int index;
    };
}
