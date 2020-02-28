#pragma once

#include <string_view>
#include <iostream>
#include <string.h>

#define CHECK(res) Checker(res, __FILE__, __LINE__)
#define CHECK_ERROR(res) Checker<decltype(res), -1, 0, true>(res, __FILE__, __LINE__)
#define CHECK_FAIL_STR(res, FAIL_CODE, msg, SUCCESS) Checker<decltype(FAIL_CODE), FAIL_CODE, SUCCESS>(res, msg, __FILE__, __LINE__)
#define CHECK_STR(res, VALUE, msg) ValueChecker<decltype(res), VALUE>(res, msg, __FILE__, __LINE__)

template<typename T, T ERROR = T(-1), T SUCCESS = T(0), bool DEF_RES = false>
static inline bool Checker(T res, std::string_view file, int line)
{
    switch (res)
    {
    case ERROR:
        std::cerr << "Error:[" << file << ":" << line << "]" << ":" << strerror(errno) << std::endl;
        return false;
    case SUCCESS:
        return true;
    default:
        return DEF_RES;
    }
}

template<typename T, T FAIL_CODE, T SUCCESS = T(0), T ERROR = T(-1), bool DEF_RES = false>
static inline bool Checker(T res, std::string_view message, std::string_view file, int line)
{
    switch (res)
    {
    case ERROR:
        std::cerr << "[" << file << ":" << line << ":" << "]" << ":" << strerror(errno) << std::endl;
        return false;
    case FAIL_CODE:
        std::cerr << "[" << file << ":" << line << "]" << ":" << message << std::endl;
        return false;
    case SUCCESS:
        return true;

    default:
        return DEF_RES;
    }
}

template<typename T, T FAIL_CODE>
static inline bool ValueChecker(T res, std::string_view message, std::string_view file, int line)
{
    if (res == FAIL_CODE)
    {
        std::cerr << "[" << file << ":" << line << "]" << ":" << message << std::endl;
        return true;
    }
    return false;
}
