#pragma once
#include <exception>

struct ExceptionHolder
{
private:
    static std::exception_ptr& exception_storage()
    {
        thread_local std::exception_ptr eptr = nullptr;
        return eptr;
    }

public:
    static void set()
    {
        exception_storage() = std::current_exception();
    }

    static void check()
    {
        std::exception_ptr eptr = exception_storage();
        if(eptr)
        {
            exception_storage() = nullptr;
            std::rethrow_exception(eptr);
        }
    }
};
