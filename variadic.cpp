#include <cstdio>
#include <string>
#include <memory>
#include <limits>
#include <iostream>
#include <typeinfo>
#include <cxxabi.h>

template <typename... Types>
struct variadic;

template <>
struct variadic<> {
    template <typename NewHead>
    using push = variadic<NewHead>;

    static constexpr unsigned int size = 0;
    using empty = void;
};

template <typename Head, typename... Tail>
struct variadic<Head, Tail...> {
    using head = Head;
    using tail = variadic<Tail...>;

    template <typename NewHead>
    using push = variadic<NewHead, Head, Tail...>;

    static constexpr unsigned int size = sizeof...(Tail) + 1;
    using nonempty = void;
};

//////////////////////////////////////////////////////////////////////////////

template <typename Key, typename Value>
struct pair { using key = Key; using value = Value; };

//////////////////////////////////////////////////////////////////////////////

template <template <typename, typename> class Func, typename Z, typename Variadic, typename Enable = void>
struct foldl_aux;

template <template <typename, typename> class Func, typename Z, typename Variadic>
struct foldl_aux<Func, Z, Variadic, typename Variadic::empty> {
    using type = Z;
};

template <template <typename, typename> class Func, typename Z, typename Variadic>
struct foldl_aux<Func, Z, Variadic, typename Variadic::nonempty>
        : foldl_aux< Func
                   , typename Func<Z, typename Variadic::head>::type
                   , typename Variadic::tail > { };

/* By making foldl a template alias, foldl<Func, Z, Variadic> /is/ the type of
 * the result. This makes metafunctions that rely on foldl easier to use:
 * using type = typename at<Variadic, Key>::type;
 * becomes
 * using type = at<Variadic, Key>;
 */
template <template <typename, typename> class Func, typename Z, typename Variadic>
using foldl = typename foldl_aux<Func, Z, Variadic>::type;

//////////////////////////////////////////////////////////////////////////////

template <typename Variadic, template <typename, typename> class Predicate>
using count_if = foldl< Predicate
                      , std::integral_constant<unsigned int, 0>
                      , Variadic >;

/* equals and key_equals are predicates for use with count_if */
template <typename T0>
struct equals {
    template <typename I, typename T1>
    using func = std::integral_constant< typename I::value_type
                                       , I::value + std::is_same<T0, T1>::value >;
};

template <typename Key>
struct key_equals {
    template <typename I, typename Pair>
    using func = std::integral_constant< typename I::value_type
                                       , I::value + std::is_same<Key, typename Pair::key>::value >;
};

//////////////////////////////////////////////////////////////////////////////

/* Exists solely to force an assertion with key_not_found<>. If this were a
 * non-template struct, and just had a static_assert(false, ...), it would
 * always fail at compile-time, which is not what we want. The only way I can
 * think of to force a conditional compile-time assertion failure involves
 * template parameters. */
template <typename... Types>
struct key_not_found {
    static_assert(sizeof...(Types), "key not found");
};

template <typename Key>
struct at_key {
    template <typename Default, typename Pair>
    using func = typename std::conditional< std::is_same<Key, typename Pair::key>::value
                                          , typename Pair::value
                                          , Default>;
};

template <typename Variadic, typename Key, typename Default = key_not_found<>>
using at = foldl< at_key<Key>::template func
                , Default
                , Variadic >;

//////////////////////////////////////////////////////////////////////////////

template <typename Variadic>
using size = std::integral_constant< unsigned int
                                   , Variadic::size >;

//////////////////////////////////////////////////////////////////////////////

template <typename Tuple, typename Pair>
struct is_key_unique {
    using tail = typename Tuple::key::tail;
    using result = typename Tuple::value;

    using key = typename Pair::key;

    template <typename I, typename P>
    using func = typename key_equals<key>::template func<I, P>;

    static constexpr bool value = !count_if<tail, func>::value;

    using new_result = typename result::template push<typename std::conditional<value, std::true_type, std::false_type>::type>;

    using type = pair<tail, new_result>;
};

template <typename Variadic>
struct has_unique_keys {
    using unique = foldl< is_key_unique
                        , pair<Variadic, variadic<>>
                        , Variadic >;

    static constexpr bool value = !count_if<typename unique::value, equals<std::false_type>::func>::value;
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

template <typename Casts, typename T>
void write (T t) {
    using to_type = at<Casts, T, T>;

    printf("static_cast<%s>(%s)\n", type_name<to_type>().c_str(), type_name<T>().c_str());

    using lim_to = std::numeric_limits<to_type>;

    if (std::is_integral<to_type>::value
            && std::is_integral<T>::value
            && !std::is_same<bool, to_type>::value
            && !std::is_same<bool, T>::value
            && !std::is_same<T, to_type>::value) {
        if (t < lim_to::lowest()) {
            printf("** t < std::numeric_limits<%s>::lowest\n", type_name<to_type>().c_str());
        }
        else if (t > lim_to::max()) {
            printf("** t > std::numeric_limits<%s>::max\n", type_name<to_type>().c_str());
        }
    }

    std::cout << static_cast<to_type>(t) << " << " << t << '\n';
}

int main () {
    using v = variadic<int, char, double>;

    {
        using h0 = typename v::head;
        using t0 = typename v::tail;
        using h1 = typename t0::head;
        using t1 = typename t0::tail;
        using h2 = typename t1::head;
        using t2 = typename t1::tail;

        printf("v = %s\n", type_name<v>().c_str());
        printf("%s : %s\n", type_name<h0>().c_str(), type_name<t0>().c_str());
        printf("%s : %s\n", type_name<h1>().c_str(), type_name<t1>().c_str());
        printf("%s : %s\n", type_name<h2>().c_str(), type_name<t2>().c_str());

        printf("%d\n", size<v>::value);

        printf("%d ints\n", count_if<v, equals<int>::func>::value);
        printf("%d char\n", count_if<v, equals<char>::func>::value);
        printf("%d doubles\n", count_if<v, equals<double>::func>::value);
        printf("%d bools\n", count_if<v, equals<bool>::func>::value);
    }

    using vv = v::push<bool>;

    {
        using h0 = typename vv::head;
        using t0 = typename vv::tail;
        using h1 = typename t0::head;
        using t1 = typename t0::tail;
        using h2 = typename t1::head;
        using t2 = typename t1::tail;
        using h3 = typename t2::head;
        using t3 = typename t2::tail;

        printf("vv = %s\n", type_name<vv>().c_str());
        printf("%s : %s\n", type_name<h0>().c_str(), type_name<t0>().c_str());
        printf("%s : %s\n", type_name<h1>().c_str(), type_name<t1>().c_str());
        printf("%s : %s\n", type_name<h2>().c_str(), type_name<t2>().c_str());
        printf("%s : %s\n", type_name<h3>().c_str(), type_name<t3>().c_str());

        printf("%d\n", size<vv>::value);

        printf("%d ints\n", count_if<vv, equals<int>::func>::value);
        printf("%d char\n", count_if<vv, equals<char>::func>::value);
        printf("%d doubles\n", count_if<vv, equals<double>::func>::value);
        printf("%d bools\n", count_if<vv, equals<bool>::func>::value);
    }

    using m = variadic<pair<bool, char>, pair<wchar_t, char16_t>, pair<char32_t, signed char>>;

    static_assert(has_unique_keys<m>::value, "m has duplicate keys");

    printf("m = %s\n", type_name<m>().c_str());
    printf("%d bool keys\n", count_if<m, key_equals<bool>::func>::value);
    printf("%d wchar_t keys\n", count_if<m, key_equals<wchar_t>::func>::value);
    printf("%d char32_t keys\n", count_if<m, key_equals<char32_t>::func>::value);
    printf("%d unsigned char keys\n", count_if<m, key_equals<unsigned char>::func>::value);

    printf("m[bool] ==\t%s\n", type_name<at<m, bool>>().c_str());
    printf("m[wchar_t] ==\t%s\n", type_name<at<m, wchar_t>>().c_str());
    printf("m[char32_t] ==\t%s\n", type_name<at<m, char32_t>>().c_str());

    using casting_policy = variadic< pair<bool, char>
                                   , pair<int, int16_t>
                                   , pair<unsigned int, uint16_t>
                                   , pair<double, float>>;

    write<casting_policy>(true);
    write<casting_policy>('d');
    write<casting_policy>(1000000);
    write<casting_policy>(1U);
    write<casting_policy>(1000000L);
    write<casting_policy>(3.14159678243234);
}
