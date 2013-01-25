#include <cstdio>
#include <string>
#include <memory>
#include <typeinfo>
#include <cxxabi.h>

template <typename... Types>
struct variadic;

template <>
struct variadic<> {
    using last = void;
};

template <typename Head, typename... Tail>
struct variadic<Head, Tail...> {
    using head = Head;
    using tail = variadic<Tail...>;

    using init = void;
};

//////////////////////////////////////////////////////////////////////////////

template <typename Key, typename Value>
struct pair { using key = Key; using value = Value; };

//////////////////////////////////////////////////////////////////////////////

template <template <typename, typename> class Func, typename Z, typename Variadic, typename Enable = void>
struct foldl;

template <template <typename, typename> class Func, typename Z, typename Variadic>
struct foldl<Func, Z, Variadic, typename Variadic::last> : Z { };

template <template <typename, typename> class Func, typename Z, typename Variadic>
struct foldl<Func, Z, Variadic, typename Variadic::init>
        : foldl< Func
               , typename Func<Z, typename Variadic::head>::type
               , typename Variadic::tail > { };

//////////////////////////////////////////////////////////////////////////////

template <typename Variadic, template <typename, typename> class Predicate>
using count_if = foldl< Predicate
                      , std::integral_constant<unsigned int, 0>
                      , Variadic >;

template <typename T0>
struct equals {
    template <typename I, typename T1>
    using func = std::integral_constant< typename I::value_type
                                       , I::value + std::is_same<T0, T1>::value >;
};

template <typename Key0>
struct key_equals {
    template <typename I, typename Pair>
    using func = std::integral_constant< typename I::value_type
                                       , I::value + std::is_same<Key0, typename Pair::key>::value >;
};

template <typename I, typename>
using size_func = std::integral_constant< typename I::value_type
                                        , I::value + 1 >;

template <typename Variadic>
using size = foldl< size_func
                  , std::integral_constant<unsigned int, 0>
                  , Variadic >;

template <typename Variadic, typename Pair, typename Enable = void>
struct key_does_not_exist;

template <typename Variadic, typename Pair>
struct key_does_not_exist<Variadic, Pair, typename Variadic::last> : std::true_type {
};

template <typename Variadic, typename Pair>
struct key_does_not_exist<Variadic, Pair, typename Variadic::init> {
    using type = typename Variadic::tail;

    template <typename, typename>
    using keyeq = typename key_equals<typename Pair::key>::func;

    static constexpr unsigned int value = count_if<Variadic, keyeq>::value;
    static_assert(!value, "duplicate key in map");
};

template <typename Variadic, typename Pair>
using key_does_not_exist_aux = key_does_not_exist<Variadic, Pair>;

template <typename Variadic>
using is_map = foldl< key_does_not_exist_aux
                    , typename Variadic::tail
                    , Variadic >;

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
    using v = variadic<int, char, double>;

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
    printf("%d long doubles\n", count_if<v, equals<long double>::func>::value);

    using m = variadic<pair<bool, char>, pair<wchar_t, char16_t>, pair<char32_t, signed char>>;

    is_map<m> i;

    printf("m = %s\n", type_name<m>().c_str());
    printf("%d bool keys\n", count_if<m, key_equals<bool>::func>::value);
    printf("%d wchar_t keys\n", count_if<m, key_equals<wchar_t>::func>::value);
    printf("%d char32_t keys\n", count_if<m, key_equals<char32_t>::func>::value);
    printf("%d unsigned char keys\n", count_if<m, key_equals<unsigned char>::func>::value);
}
