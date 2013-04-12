#ifndef TYPE_CONTAINERS_LIST_HPP
#define TYPE_CONTAINERS_LIST_HPP

#include <type_traits>

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

/* In Haskell:
 * reverse = foldl (flip (:)) []
 */

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

template <typename T>
struct reverse_func {
    using type = reverse<T>;
};

template <typename List>
using inits = reverse<map<reverse_func, tails<reverse<List>>>>;

//////////////////////////////////////////////////////////////////////////////

/* In Haskell:
 * append = flip $ foldr (:)
 */

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

template <template <typename, typename> class Op, typename L0, typename L1>
struct zip_with_aux {
    using type = cons< Op<head<L0>, head<L1>>
                     , typename zip_with_aux<Op, tail<L0>, tail<L1>>::type>;
};

template <template <typename, typename> class Op, typename L0>
struct zip_with_aux<Op, L0, list<>> {
    using type = list<>;
};

template <template <typename, typename> class Op, typename L1>
struct zip_with_aux<Op, list<>, L1> {
    using type = list<>;
};

template <template <typename, typename> class Op>
struct zip_with_aux<Op, list<>, list<>> {
    using type = list<>;
};

template <template <typename, typename> class Op, typename L0, typename L1>
using zip_with = typename zip_with_aux<Op, L0, L1>::type;

//////////////////////////////////////////////////////////////////////////////

template <unsigned int N, typename List>
struct drop_aux : drop_aux<N-1, tail<List>> { };

template <typename List>
struct drop_aux<0, List> {
    using type = List;
};

template <unsigned int N>
struct drop_aux<N, list<>> {
    using type = list<>;
};

template <>
struct drop_aux<0, list<>> {
    using type = list<>;
};

template <typename N, typename List>
using drop = typename drop_aux<N::value, List>::type;

//////////////////////////////////////////////////////////////////////////////

template <unsigned int N, typename T>
struct replicate_aux {
    using type = cons<T, typename replicate_aux<N-1, T>::type>;
};

template <typename T>
struct replicate_aux<0, T> {
    using type = list<>;
};

template <typename N, typename T>
using replicate = typename replicate_aux<N::value, T>::type;

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
                                          , Default >::type;
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

#endif
