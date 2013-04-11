#ifndef TYPE_CONTAINERS_TYPE_NAME_HPP
#define TYPE_CONTAINERS_TYPE_NAME_HPP

#include <string>
#include <memory>
#include <typeinfo>
#include <cxxabi.h>

//////////////////////////////////////////////////////////////////////////////

template <typename T>
static std::string type_name () {
    int status;
    std::unique_ptr<char> realname {
        abi::__cxa_demangle(typeid(T).name(), 0, 0, &status)
    };
    return { realname.get() };
}

#endif
