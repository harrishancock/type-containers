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
};

template <typename Head, typename... Tail>
struct variadic<Head, Tail...> {
    using head = Head;
    using tail = variadic<Tail...>;

    template <typename NewHead>
    using push = variadic<NewHead, Head, Tail...>;

    static constexpr unsigned int size = sizeof...(Tail) + 1;
};

//////////////////////////////////////////////////////////////////////////////

template <typename Variadic>
using length = std::integral_constant<unsigned int, Variadic::size>;

//////////////////////////////////////////////////////////////////////////////

template <template <typename, typename> class Func, typename Z, typename Variadic>
struct foldl_aux : foldl_aux< Func
                            , typename Func<Z, typename Variadic::head>::type
                            , typename Variadic::tail > { };


template <template <typename, typename> class Func, typename Z>
struct foldl_aux<Func, Z, variadic<>> {
    using type = Z;
};

template <template <typename, typename> class Func, typename Z, typename Variadic>
using foldl = typename foldl_aux<Func, Z, Variadic>::type;

//////////////////////////////////////////////////////////////////////////////

template <template <typename> class Func, typename Variadic>
struct map_aux {
    using head = typename Variadic::head;
    using tail = typename Variadic::tail;
    using result = typename Func<head>::type;
    using type = typename map_aux<Func, tail>::type::template push<result>;
};

template <template <typename> class Func>
struct map_aux<Func, variadic<>> {
    using type = variadic<>;
};

template <template <typename> class Func, typename Variadic>
using map = typename map_aux<Func, Variadic>::type;

//////////////////////////////////////////////////////////////////////////////

template <template <typename> class Predicate, typename Variadic>
struct filter_aux {
    using head = typename Variadic::head;
    using tail = typename Variadic::tail;
    using next = typename filter_aux<Predicate, tail>::type;
    using type = typename std::conditional<Predicate<head>::value, typename next::template push<head>, next>::type;
};

template <template <typename> class Predicate>
struct filter_aux<Predicate, variadic<>> {
    using type = variadic<>;
};

template <template <typename> class Predicate, typename Variadic>
using filter = typename filter_aux<Predicate, Variadic>::type;

//////////////////////////////////////////////////////////////////////////////

template <typename Variadic>
struct tails_aux {
    using tail = typename Variadic::tail;
    using type = typename tails_aux<tail>::type::template push<Variadic>;
};

template <>
struct tails_aux<variadic<>> {
    using type = variadic<variadic<>>;
};

template <typename Variadic>
using tails = typename tails_aux<Variadic>::type;

//////////////////////////////////////////////////////////////////////////////

template <typename Z, typename Bool>
using or_aux = std::conditional<Z::value && Bool::value, std::true_type, std::false_type>;

template <typename Variadic>
using or_ = foldl<or_aux, std::false_type, Variadic>;

template <template <typename> class Predicate, typename Variadic>
using any = or_<map<Predicate, Variadic>>;

template <typename Z, typename Bool>
using and_aux = std::conditional<Z::value && Bool::value, std::true_type, std::false_type>;

template <typename Variadic>
using and_ = foldl<and_aux, std::true_type, Variadic>;

template <template <typename> class Predicate, typename Variadic>
using all = and_<map<Predicate, Variadic>>;

//////////////////////////////////////////////////////////////////////////////

template <typename Key, typename Value>
struct pair {
    using key = Key;
    using value = Value;
};

//////////////////////////////////////////////////////////////////////////////

template <typename T>
struct is_pair : std::false_type { };

template <typename Key, typename Value>
struct is_pair<pair<Key, Value>> : std::true_type { };

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

template <typename Variadic, template <typename> class Predicate>
using count_if = length<filter<Predicate, Variadic>>;

template <typename T0>
struct equals {
    template <typename T1>
    using func = std::is_same<T0, T1>;
};

template <typename Key>
struct key_equals {
    template <typename Pair>
    using func = std::is_same<Key, typename Pair::key>;
};

//////////////////////////////////////////////////////////////////////////////

template <typename Variadic>
struct has_unique_key_head {
    using tail = typename Variadic::tail;
    using head = typename Variadic::head;

    template <typename P>
    using func = typename key_equals<typename head::key>::template func<P>;

    static constexpr bool value = !count_if<tail, func>::value;
    using type = typename std::conditional< value
                                          , std::true_type
                                          , std::false_type>::type;
};

template <>
struct has_unique_key_head<variadic<>> {
    using type = std::true_type;
};

template <typename Variadic>
using has_unique_keys = all<has_unique_key_head, tails<Variadic>>;

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

        printf("%d\n", v::size);

        printf("%d ints\n", count_if<v, equals<int>::func>::value);
        printf("%d char\n", count_if<v, equals<char>::func>::value);
        printf("%d doubles\n", count_if<v, equals<double>::func>::value);
        printf("%d bools\n", count_if<v, equals<bool>::func>::value);
    }

    using vv = v::push<void>;

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

        printf("%d\n", vv::size);

        printf("%d ints\n", count_if<vv, equals<int>::func>::value);
        printf("%d char\n", count_if<vv, equals<char>::func>::value);
        printf("%d doubles\n", count_if<vv, equals<double>::func>::value);
        printf("%d bools\n", count_if<vv, equals<bool>::func>::value);

        printf("map<is_void, vv> = %s\n", type_name<map<std::is_void, vv>>().c_str());

        using ts = tails<vv>;

        using tail0 = typename ts::head;
        using tail1 = typename ts::tail::head;
        using tail2 = typename ts::tail::tail::head;
        using tail3 = typename ts::tail::tail::tail::head;
        using tail4 = typename ts::tail::tail::tail::tail::head;

        printf("tail0 = %s\n", type_name<tail0>().c_str());
        printf("tail1 = %s\n", type_name<tail1>().c_str());
        printf("tail2 = %s\n", type_name<tail2>().c_str());
        printf("tail3 = %s\n", type_name<tail3>().c_str());
        printf("tail4 = %s\n", type_name<tail4>().c_str());
    }

    using m = variadic<pair<bool, char>, pair<wchar_t, char16_t>, pair<char32_t, signed char>>;

    //static_assert(has_unique_keys<m>::value, "m has duplicate keys");

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
