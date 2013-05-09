/* DEPRECATED */

#ifndef TYPE_CONTAINERS_LAMBDA_HPP
#define TYPE_CONTAINERS_LAMBDA_HPP

template <unsigned int N>
struct arg;

struct wildcard_arg;

using _ = wildcard_arg;
using _1 = arg<1>;
using _2 = arg<2>;
using _3 = arg<3>;
using _4 = arg<4>;
using _5 = arg<5>;
using _6 = arg<6>;
using _7 = arg<7>;
using _8 = arg<8>;
using _9 = arg<9>;

template <typename...>
struct pack;

template <template <typename...> class MF, typename PlaceholderPack, typename ProcessedPack, typename... Args>
struct fill_wildcards;

template <template <typename...> class MF,
         typename PHead, typename... Placeholders,
         typename... Processed,
         typename... Args>
struct fill_wildcards<MF, pack<PHead, Placeholders...>, pack<Processed...>, Args...>
    : fill_wildcards<MF, pack<Placeholders...>, pack<Processed..., PHead>, Args...> { };

template <template <typename...> class MF,
         typename... Placeholders,
         typename... Processed, 
         typename AHead, typename... Args>
struct fill_wildcards<MF, pack<wildcard_arg, Placeholders...>, pack<Processed...>, AHead, Args...>
    : fill_wildcards<MF, pack<Placeholders...>, pack<Processed..., AHead>, Args...> { };

template <template <typename...> class MF,
         typename... Processed, typename... Args>
struct fill_wildcards<MF, pack<>, pack<Processed...>, Args...>
    : MF<Processed...> { };

template <unsigned int N, typename... Args>
struct get;

template <typename Head, typename... Args>
struct get<0, Head, Args...> {
    using type = Head;
};

template <unsigned int N, typename Head, typename... Args>
struct get<N, Head, Args...> : get<N-1, Args...> { };

template <typename T, typename...>
struct fill_indexes {
    using type = T;
};

template <unsigned int N, typename... Args>
struct fill_indexes<arg<N>, Args...> {
    using type = typename get<N-1, Args...>::type;
};

template <typename PlaceholderExpression>
struct lambda;

template <template <typename...> class MF, typename... Placeholders>
struct lambda<MF<Placeholders...>> {
    template <typename... Args>
    using apply = fill_wildcards<pack<typename fill_indexes<Placeholders, Args...>::type...>, pack<>, Args...>;
};

template <typename PE, typename... Args>
struct apply : apply<lambda<PE>, Args...> { };

template <typename PE, typename... Args>
struct apply<lambda<PE>, Args...> : lambda<PE>::template apply<Args...> { };

#endif
