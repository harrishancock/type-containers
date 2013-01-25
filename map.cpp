#include <cstdio>
#include <type_traits>
#include <typeinfo>

#include <string>
#include <memory>
#include <cxxabi.h>

//////////////////////////////////////////////////////////////////////////////

template <typename Head, typename...>
struct variadic_head { using type = Head; };

/* We need the template template parameter Container here, because it's
 * impossible to make a variadic parameter pack typedef--some other templated
 * type /must/ be used to contain the parameter pack. */
template <template <typename...> class Container, typename, typename... Tail>
struct variadic_tail { using type = Container<Tail...>; };

//////////////////////////////////////////////////////////////////////////////

/**
 * A key/value pair for use with map.
 */
template <typename Key, typename Value>
struct pair { using key = Key; using value = Value; };

/**
 * Variadic container of key/value pairs. At most one key may be in the map.
 */
template <typename...>
struct map;

/**
 * Count the number of key/value pairs in Map, where key matches Key. Note
 * that for a regular map, this value will always be either 0 or 1.
 */
/* Recursive case */
template <typename Map, typename Key, unsigned int C = 0>
struct count : count<typename Map::tail, Key, C + std::is_same<Key, typename Map::head::key>::value> { };

/* Base case */
template <typename Key, unsigned int C>
struct count<map<>, Key, C> {
    static constexpr unsigned int value = C;
};

template <typename... Pairs>
struct map {
    using head = typename variadic_head<Pairs...>::type;
    using tail = typename variadic_tail<map, Pairs...>::type;
    static constexpr unsigned int size = sizeof...(Pairs);

    static_assert(!count<tail, typename head::key>::value, "duplicate key inserted");
};

template <>
struct map<> {
    static constexpr unsigned int size = 0;
};

template <typename Map, typename Key, typename Enable = void>
struct at_impl;

/* Recursive case */
template <typename Map, typename Key>
struct at_impl<Map, Key, typename std::enable_if<!std::is_same<typename Map::head::key, Key>::value>::type>
        : at_impl<typename Map::tail, Key> { };

/* Base case */
template <typename Map, typename Key>
struct at_impl<Map, Key, typename std::enable_if<std::is_same<typename Map::head::key, Key>::value>::type> {
    using type = typename Map::head::value;
};

/**
 * Look up the value associated with a given Key in Map. The result will be a
 * member typedef called /type/.
 *
 * at is a wrapper that merely checks for existence of a key, then relies on
 * at_impl to do the lookup. If both the existence check and the lookup code
 * were in the same recursive template, the static_assert which prints the
 * error at compile-time would also be recursively instantiated, leading to n
 * lines of errors.
 */
template <typename Map, typename Key, typename Enable = void>
struct at;

template <typename Map, typename Key>
struct at<Map, Key, typename std::enable_if<count<Map, Key>::value>::type>
        : at_impl<Map, Key> {
};

template <typename Map, typename Key>
struct at<Map, Key, typename std::enable_if<!count<Map, Key>::value>::type> {
    static_assert(count<Map, Key>::value, "key not found");
};

//////////////////////////////////////////////////////////////////////////////

template <typename T>
static std::string type_name () {
    int status;
    std::unique_ptr<char> realname {
        abi::__cxa_demangle(typeid(T).name(), 0, 0, &status)
    };
    return { realname.get() };
}

int main () {
    using m = map< pair<int, char>
                 , pair<double, float>
                 , pair<unsigned, bool> >;

    using v0 = typename at<m, int>::type;
    using v1 = typename at<m, double>::type;
    using v2 = typename at<m, unsigned>::type;

    static_assert(std::is_same<char, v0>::value, "map[int] != char");
    static_assert(std::is_same<float, v1>::value, "map[double] != float");
    static_assert(std::is_same<bool, v2>::value, "map[unsigned] != bool");

    printf("%s\n", type_name<m>().c_str());
    printf("int\t\t-> %s\n", type_name<v0>().c_str());
    printf("double\t\t-> %s\n", type_name<v1>().c_str());
    printf("unsigned\t-> %s\n", type_name<v2>().c_str());

    // compile error, should trip "key not found" static_assert
    //using vn = typename at<m, long double>::type;

    printf("count(int): %d\n", count<m, int>::value);
    printf("count(double): %d\n", count<m, double>::value);
    printf("count(unsigned): %d\n", count<m, unsigned>::value);

    printf("count(bool): %d\n", count<m, bool>::value);
}
