#ifndef TYPE_CONTAINERS_VARIADIC_HPP
#define TYPE_CONTAINERS_VARIADIC_HPP

#include <type_traits>

namespace variadic {

//////////////////////////////////////////////////////////////////////////////

template <typename Fst, typename Snd>
struct pair {
    using fst = Fst;
    using snd = Snd;
    using type = pair<Fst, Snd>;
};

template <typename Pair>
struct fst {
    using type = typename Pair::fst;
};

template <typename Pair>
struct snd {
    using type = typename Pair::snd;
};

//////////////////////////////////////////////////////////////////////////////

template <typename... Xs>
struct pack;

template <>
struct pack<> {
    using length = std::integral_constant<unsigned int, 0>;
    using type = pack<>;
};

template <typename Head, typename... Tail>
struct pack<Head, Tail...> {
    using head = Head;
    using tail = pack<Tail...>;

    using length = std::integral_constant<unsigned int, sizeof...(Tail) + 1>;
    using type = pack<Head, Tail...>;
};

//////////////////////////////////////////////////////////////////////////////

template <typename Pack>
struct null : std::is_same<pack<>, Pack> { };

template <typename Pack>
struct head {
    using type = typename Pack::head;
};

template <typename Pack>
struct tail {
    using type = typename Pack::tail;
};

template <typename Element, typename Pack>
struct cons;

template <typename Element, typename... Ps>
struct cons<Element, pack<Ps...>> {
    using type = pack<Element, Ps...>;
};

template <typename Pack>
struct length : Pack::length { };

//////////////////////////////////////////////////////////////////////////////

template <typename Func, typename Z, typename Pack>
struct foldl : foldl< Func
                    , typename Func::template apply<Z, typename head<Pack>::type>::type
                    , typename tail<Pack>::type > { };

template <typename Func, typename Z>
struct foldl<Func, Z, pack<>> {
    using type = Z;
};

//////////////////////////////////////////////////////////////////////////////

template <typename Func, typename Z, typename Pack>
struct foldr {
    using next = typename foldr<Func, Z, typename tail<Pack>::type>::type;
    using type = typename Func::template apply<typename head<Pack>::type, next>::type;
};

template <typename Func, typename Z>
struct foldr<Func, Z, pack<>> {
    using type = Z;
};

//////////////////////////////////////////////////////////////////////////////

/* In Haskell:
 * reverse = foldl (flip (:)) []
 */

struct reverse_cons {
    template <typename Pack, typename Element>
    struct apply : cons<Element, Pack> { };
};

template <typename Pack>
struct reverse : foldl< reverse_cons
                      , pack<>
                      , Pack > { };

//////////////////////////////////////////////////////////////////////////////

template <typename Pack>
struct last : head<typename reverse<Pack>::type> { };

template <typename Pack>
struct init : reverse<typename tail<typename reverse<Pack>::type>::type> { };

//////////////////////////////////////////////////////////////////////////////

template <typename Func, typename Pack>
struct map {
    using result = typename Func::template apply<typename head<Pack>::type>::type;
    using type = typename cons<result, typename map<Func, typename tail<Pack>::type>::type>::type;
};

template <typename Func>
struct map<Func, pack<>> {
    using type = pack<>;
};

//////////////////////////////////////////////////////////////////////////////

template <typename Predicate, typename Pack>
struct filter {
    using next = typename filter<Predicate, typename tail<Pack>::type>::type;
    using type = typename std::conditional< Predicate::template apply<typename head<Pack>::type>::type::value
                                          , typename cons<typename head<Pack>::type, next>::type
                                          , next >::type;
};

template <typename Predicate>
struct filter<Predicate, pack<>> {
    using type = pack<>;
};

//////////////////////////////////////////////////////////////////////////////

template <typename Pack>
struct tails {
    using type = typename cons<Pack, typename tails<typename tail<Pack>::type>::type>::type;
};

template <>
struct tails<pack<>> {
    using type = pack<pack<>>;
};

//////////////////////////////////////////////////////////////////////////////

struct reverse_func {
    template <typename T>
    struct apply : reverse<T> { };
};

template <typename Pack>
struct inits : reverse<typename map<reverse_func, typename tails<typename reverse<Pack>::type>::type>::type> { };

//////////////////////////////////////////////////////////////////////////////

/* In Haskell:
 * append = flip $ foldr (:)
 */

struct cons_func {
    template <typename Element, typename Pack>
    struct apply : cons<Element, Pack> { };
};

template <typename Pack0, typename Pack1>
struct append : foldr<cons_func, Pack1, Pack0> { };

//////////////////////////////////////////////////////////////////////////////

struct or_aux {
    template <typename Z, typename Bool>
    struct apply : std::conditional< Z::value || Bool::value
                                   , std::true_type
                                   , std::false_type > { };
};

template <typename Pack>
struct or_ : foldl<or_aux, std::false_type, Pack> { };

struct and_aux {
    template <typename Z, typename Bool>
    struct apply : std::conditional< Z::value && Bool::value
                                   , std::true_type
                                   , std::false_type > { };
};

template <typename Pack>
struct and_ : foldl<and_aux, std::true_type, Pack> { };

template <typename Predicate, typename Pack>
struct any : or_<typename map<Predicate, Pack>::type> { };

template <typename Predicate, typename Pack>
struct all : and_<typename map<Predicate, Pack>::type> { };

//////////////////////////////////////////////////////////////////////////////

#if 0
/* need maybe monad */
template <typename Predicate, typename Pack>
using find = head<filter<Predicate, Pack>>;
#endif

//////////////////////////////////////////////////////////////////////////////

template <typename Op, typename L0, typename L1>
struct zip_with {
    using type = typename cons< typename Op::template apply<head<L0>::type, typename head<L1>::type>
                     , typename zip_with<Op, typename tail<L0>::type, typename tail<L1>::type>::type>::type;
};

template <typename Op, typename L0>
struct zip_with<Op, L0, pack<>> {
    using type = pack<>;
};

template <typename Op, typename L1>
struct zip_with<Op, pack<>, L1> {
    using type = pack<>;
};

template <typename Op>
struct zip_with<Op, pack<>, pack<>> {
    using type = pack<>;
};

//////////////////////////////////////////////////////////////////////////////

struct unzip_aux {
    template <typename Pair, typename Z>
    struct apply {
        using new_a = typename cons<typename fst<Pair>::type, typename fst<Z>::type>::type;
        using new_b = typename cons<typename snd<Pair>::type, typename snd<Z>::type>::type;
        using type = pair<new_a, new_b>;
    };
};

template <typename Pack>
struct unzip : foldr<unzip_aux, pair<pack<>, pack<>>, Pack> { };

//////////////////////////////////////////////////////////////////////////////

template <unsigned int N, typename Pack>
struct drop : drop<N-1, typename tail<Pack>::type> { };

template <typename Pack>
struct drop<0, Pack> {
    using type = Pack;
};

template <unsigned int N>
struct drop<N, pack<>> {
    using type = pack<>;
};

template <>
struct drop<0, pack<>> {
    using type = pack<>;
};

//////////////////////////////////////////////////////////////////////////////

template <unsigned int N, typename T>
struct replicate : cons<T, typename replicate<N-1, T>::type> { };

template <typename T>
struct replicate<0, T> {
    using type = pack<>;
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
    struct apply : std::conditional< std::is_same<Key, typename fst<Pair>::type>::value
                                          , typename snd<Pair>::type
                                          , Default > { };
};

template <typename Pack, typename Key, typename Default = key_not_found<>>
struct at : foldl< at_key<Key>
                , Default
                , Pack > { };

//////////////////////////////////////////////////////////////////////////////

template <typename Pack, typename Predicate>
struct count_if : length<typename filter<Predicate, Pack>::type> { };

template <typename T0>
struct equals {
    template <typename T1>
    struct apply : std::is_same<T0, T1> { };
};

template <typename Key>
struct key_equals {
    template <typename Pair>
    struct apply : std::is_same<Key, typename fst<Pair>::type> { };
};

//////////////////////////////////////////////////////////////////////////////

struct has_unique_key_head {
    template <typename Pack>
    struct apply {
        using key = typename head<Pack>::type::key;

        using func = key_equals<key>;

        static constexpr bool value = !count_if<typename tail<Pack>::type, key_equals<key>>::value;
        using type = typename std::conditional< value
                                              , std::true_type
                                              , std::false_type >::type;
    };
};

template <>
struct has_unique_key_head::apply<pack<>> {
    using type = std::true_type;
};

template <typename Pack>
struct has_unique_keys : all<has_unique_key_head, typename tails<Pack>::type> { };

//////////////////////////////////////////////////////////////////////////////

#if 0
/* NOT YET REVISED */
template <typename Pack>
struct has_pairs : all<is_pair, Pack>;

//////////////////////////////////////////////////////////////////////////////

template <typename Pack, typename Enable = void>
struct is_assocpack_aux;

template <typename Pack>
struct is_assocpack_aux<Pack, typename std::enable_if<has_pairs<Pack>::value>::type>
        : has_unique_keys<Pack> { };

template <typename Pack>
struct is_assocpack_aux<Pack, typename std::enable_if<!has_pairs<Pack>::value>::type>
        : std::false_type { };

template <typename Pack>
using is_association_pack = typename is_assocpack_aux<Pack>::type;
#endif

} // namespace variadic

#endif
