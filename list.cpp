#include <cstdio>
#include <string>
#include <memory>
#include <limits>
#include <iostream>
#include <typeinfo>
#include <cxxabi.h>

//////////////////////////////////////////////////////////////////////////////

template <typename... Types>
struct list;

template <>
struct list<> {
    template <typename NewHead>
    using cons = list<NewHead>;

    using length = std::integral_constant<unsigned int, 0>;
};

template <typename Head, typename... Tail>
struct list<Head, Tail...> {
    using head = Head;
    using tail = list<Tail...>;

    template <typename NewHead>
    using cons = list<NewHead, Head, Tail...>;

    using length = std::integral_constant<unsigned int, sizeof...(Tail) + 1>;
};

//////////////////////////////////////////////////////////////////////////////

template <typename List>
using null = typename std::is_same<list<>, List>::type;

template <typename List>
using head = typename List::head;

template <typename List>
using tail = typename List::tail;

template <typename Element, typename List>
using cons = typename List::template cons<Element>;

template <typename List>
using length = typename List::length;

//////////////////////////////////////////////////////////////////////////////

template <template <typename, typename> class Func, typename Z, typename List>
struct foldl_aux : foldl_aux< Func
                            , Func<Z, head<List>>
                            , tail<List> > { };

template <template <typename, typename> class Func, typename Z>
struct foldl_aux<Func, Z, list<>> {
    using type = Z;
};

template <template <typename, typename> class Func, typename Z, typename List>
using foldl = typename foldl_aux<Func, Z, List>::type;

//////////////////////////////////////////////////////////////////////////////

template <template <typename, typename> class Func, typename Z, typename List>
struct foldr_aux {
    using next = typename foldr_aux<Func, Z, tail<List>>::type;
    using type = Func<head<List>, next>;
};

template <template <typename, typename> class Func, typename Z>
struct foldr_aux<Func, Z, list<>> {
    using type = Z;
};

template <template <typename, typename> class Func, typename Z, typename List>
using foldr = typename foldr_aux<Func, Z, List>::type;

//////////////////////////////////////////////////////////////////////////////

template <typename List, typename Element>
using reverse_cons = cons<Element, List>;

template <typename List>
using reverse = foldl< reverse_cons
                     , list<>
                     , List >;

//////////////////////////////////////////////////////////////////////////////

template <typename List>
using last = head<reverse<List>>;

template <typename List>
using init = reverse<tail<reverse<List>>>;

//////////////////////////////////////////////////////////////////////////////

template <template <typename> class Func, typename List>
struct map_aux {
    using result = typename Func<head<List>>::type;
    using type = cons<result, typename map_aux<Func, tail<List>>::type>;
};

template <template <typename> class Func>
struct map_aux<Func, list<>> {
    using type = list<>;
};

template <template <typename> class Func, typename List>
using map = typename map_aux<Func, List>::type;

//////////////////////////////////////////////////////////////////////////////

template <template <typename> class Predicate, typename List>
struct filter_aux {
    using next = typename filter_aux<Predicate, tail<List>>::type;
    using type = typename std::conditional< Predicate<head<List>>::value
                                          , cons<head<List>, next>
                                          , next >::type;
};

template <template <typename> class Predicate>
struct filter_aux<Predicate, list<>> {
    using type = list<>;
};

template <template <typename> class Predicate, typename List>
using filter = typename filter_aux<Predicate, List>::type;

//////////////////////////////////////////////////////////////////////////////

template <typename List>
struct tails_aux {
    using type = cons<List, typename tails_aux<tail<List>>::type>;
};

template <>
struct tails_aux<list<>> {
    using type = list<list<>>;
};

template <typename List>
using tails = typename tails_aux<List>::type;

//////////////////////////////////////////////////////////////////////////////

template <typename List0, typename List1>
using append = foldr<cons, List1, List0>;

//////////////////////////////////////////////////////////////////////////////

template <typename Z, typename Bool>
using or_aux = typename std::conditional< Z::value || Bool::value
                                        , std::true_type
                                        , std::false_type >::type;

template <typename List>
using or_ = foldl<or_aux, std::false_type, List>;

template <typename Z, typename Bool>
using and_aux = typename std::conditional< Z::value && Bool::value
                                         , std::true_type
                                         , std::false_type >::type;

template <typename List>
using and_ = foldl<and_aux, std::true_type, List>;

template <template <typename> class Predicate, typename List>
using any = or_<map<Predicate, List>>;

template <template <typename> class Predicate, typename List>
using all = and_<map<Predicate, List>>;

//////////////////////////////////////////////////////////////////////////////

#if 0
template <typename Predicate, typename List>
using find = head<filter<Predicate, List>>;
#endif

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
                                          , Default>::type;
};

template <typename List, typename Key, typename Default = key_not_found<>>
using at = foldl< at_key<Key>::template func
                , Default
                , List >;

//////////////////////////////////////////////////////////////////////////////

template <typename List, template <typename> class Predicate>
using count_if = length<filter<Predicate, List>>;

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

template <typename List>
struct has_unique_key_head {
    using key = typename head<List>::key;

    template <typename P>
    using func = typename key_equals<key>::template func<P>;

    static constexpr bool value = !count_if<tail<List>, func>::value;
    using type = typename std::conditional< value
                                          , std::true_type
                                          , std::false_type >::type;
};

template <>
struct has_unique_key_head<list<>> {
    using type = std::true_type;
};

template <typename List>
using has_unique_keys = all<has_unique_key_head, tails<List>>;

//////////////////////////////////////////////////////////////////////////////

template <typename List>
using has_pairs = all<is_pair, List>;

//////////////////////////////////////////////////////////////////////////////

template <typename List, typename Enable = void>
struct is_assoclist_aux;

template <typename List>
struct is_assoclist_aux<List, typename std::enable_if<has_pairs<List>::value>::type>
        : has_unique_keys<List> { };

template <typename List>
struct is_assoclist_aux<List, typename std::enable_if<!has_pairs<List>::value>::type>
        : std::false_type { };

template <typename List>
using is_association_list = typename is_assoclist_aux<List>::type;

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
    using v = list<int, char, double>;

    {
        using h0 = head<v>;
        using t0 = tail<v>;
        using h1 = head<t0>;
        using t1 = tail<t0>;
        using h2 = head<t1>;
        using t2 = tail<t1>;

        printf("v = %s\n", type_name<v>().c_str());
        printf("%s : %s\n", type_name<h0>().c_str(), type_name<t0>().c_str());
        printf("%s : %s\n", type_name<h1>().c_str(), type_name<t1>().c_str());
        printf("%s : %s\n", type_name<h2>().c_str(), type_name<t2>().c_str());

        printf("%d\n", length<v>::value);

        printf("%d ints\n", count_if<v, equals<int>::func>::value);
        printf("%d char\n", count_if<v, equals<char>::func>::value);
        printf("%d doubles\n", count_if<v, equals<double>::func>::value);
        printf("%d bools\n", count_if<v, equals<bool>::func>::value);
    }

    using vv = cons<void, v>;

    {
        using h0 = head<vv>;
        using t0 = tail<vv>;
        using h1 = head<t0>;
        using t1 = tail<t0>;
        using h2 = head<t1>;
        using t2 = tail<t1>;
        using h3 = head<t2>;
        using t3 = tail<t2>;

        printf("vv = %s\n", type_name<vv>().c_str());
        printf("%s : %s\n", type_name<h0>().c_str(), type_name<t0>().c_str());
        printf("%s : %s\n", type_name<h1>().c_str(), type_name<t1>().c_str());
        printf("%s : %s\n", type_name<h2>().c_str(), type_name<t2>().c_str());
        printf("%s : %s\n", type_name<h3>().c_str(), type_name<t3>().c_str());

        printf("%d\n", length<vv>::value);

        printf("%d ints\n", count_if<vv, equals<int>::func>::value);
        printf("%d char\n", count_if<vv, equals<char>::func>::value);
        printf("%d doubles\n", count_if<vv, equals<double>::func>::value);
        printf("%d bools\n", count_if<vv, equals<bool>::func>::value);

        printf("map<is_void, vv> = %s\n", type_name<map<std::is_void, vv>>().c_str());

        using ts = tails<vv>;

        using tail0 = head<ts>;
        using tail1 = head<tail<ts>>;
        using tail2 = head<tail<tail<ts>>>;
        using tail3 = head<tail<tail<tail<ts>>>>;
        using tail4 = head<tail<tail<tail<tail<ts>>>>>;

        printf("tail0 = %s\n", type_name<tail0>().c_str());
        printf("tail1 = %s\n", type_name<tail1>().c_str());
        printf("tail2 = %s\n", type_name<tail2>().c_str());
        printf("tail3 = %s\n", type_name<tail3>().c_str());
        printf("tail4 = %s\n", type_name<tail4>().c_str());
    }

    using vvv = append<v, vv>;

    printf("vvv = %s\n", type_name<vvv>().c_str());
    printf("reverse<vvv> = %s\n", type_name<reverse<vvv>>().c_str());
    printf("head<vvv> = %s\n", type_name<head<vvv>>().c_str());
    printf("last<vvv> = %s\n", type_name<last<vvv>>().c_str());
    printf("tail<vvv> = %s\n", type_name<tail<vvv>>().c_str());
    printf("init<vvv> = %s\n", type_name<init<vvv>>().c_str());

    using m = list<pair<bool, char>, pair<wchar_t, char16_t>, pair<char32_t, signed char>>;

    static_assert(is_association_list<m>::value, "m is not an association list");

    using mm = cons<void, m>;

    //static_assert(is_association_list<mm>::value, "mm is not an association list");

    printf("m = %s\n", type_name<m>().c_str());
    printf("%d bool keys\n", count_if<m, key_equals<bool>::func>::value);
    printf("%d wchar_t keys\n", count_if<m, key_equals<wchar_t>::func>::value);
    printf("%d char32_t keys\n", count_if<m, key_equals<char32_t>::func>::value);
    printf("%d unsigned char keys\n", count_if<m, key_equals<unsigned char>::func>::value);

    printf("m[bool] ==\t%s\n", type_name<at<m, bool>>().c_str());
    printf("m[wchar_t] ==\t%s\n", type_name<at<m, wchar_t>>().c_str());
    printf("m[char32_t] ==\t%s\n", type_name<at<m, char32_t>>().c_str());

    using casting_policy = list< pair<bool, char>
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
