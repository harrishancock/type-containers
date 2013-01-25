#include <cstdio>
#include <type_traits>
#include <typeinfo>

//////////////////////////////////////////////////////////////////////////////

template <typename Head, typename...>
struct variadic_head { using type = Head; };

template <template <typename...> class Container, typename, typename... Tail>
struct variadic_tail { using type = Container<Tail...>; };

//////////////////////////////////////////////////////////////////////////////

template <typename Key, typename Value>
struct pair { using key = Key; using value = Value; };

template <typename...>
struct map;

template <typename Map, typename Key, unsigned int C = 0>
struct count : count<typename Map::tail, Key, C + std::is_same<Key, typename Map::head::key>::value> { };

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

template <typename Map, typename Key>
struct at_impl<Map, Key, typename std::enable_if<!std::is_same<typename Map::head::key, Key>::value>::type>
        : at_impl<typename Map::tail, Key> { };

template <typename Map, typename Key>
struct at_impl<Map, Key, typename std::enable_if<std::is_same<typename Map::head::key, Key>::value>::type> {
    using type = typename Map::head::value;
};

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

int main () {
#if 0
    using v = list<int, char, float>;
    using h0 = v::head;
    using t0 = v::tail;
    using h1 = t0::head;
    using t1 = t0::tail;
    using h2 = t1::head;
    using t2 = t1::tail;

    printf("%s\n", typeid(v).name());
    printf("%s\n", typeid(h0).name());
    printf("%s\n", typeid(t0).name());
    printf("%s\n", typeid(h1).name());
    printf("%s\n", typeid(t1).name());
    printf("%s\n", typeid(h2).name());
    printf("%s\n", typeid(t2).name());
#endif

    using m = map<pair<int, char>, pair<double, float>, pair<unsigned, bool>>;
    using k0 = m::head::key;
    using v0 = typename at<m, k0>::type;
    using k1 = m::tail::head::key;
    using v1 = typename at<m, k1>::type;
    using k2 = m::tail::tail::head::key;
    using v2 = typename at<m, k2>::type;
    //using vn = typename at<m, long double>::type;

    printf("%s\n", typeid(m).name());
    printf("%s\n", typeid(k0).name());
    printf("%s\n", typeid(v0).name());
    printf("%s\n", typeid(k1).name());
    printf("%s\n", typeid(v1).name());
    printf("%s\n", typeid(k2).name());
    printf("%s\n", typeid(v2).name());

    printf("count(int): %d\n", count<m, int>::value);
    printf("count(double): %d\n", count<m, double>::value);
    printf("count(unsigned): %d\n", count<m, unsigned>::value);

    printf("count(bool): %d\n", count<m, bool>::value);
}
