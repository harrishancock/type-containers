#ifndef TYPE_CONTAINERS_PRIMITIVES_HPP
#define TYPE_CONTAINERS_PRIMITIVES_HPP

namespace primitives {

template <typename... Pack>
struct head {
    static_assert(sizeof...(Pack), "head called with empty parameter pack");
};

template <typename Head, typename... Tail>
struct head<Head, Tail...> : id<Head> { };

template <typename Continue, typename... Pack>
struct tail : Continue::template apply<> { };

template <typename Continue, typename Head, typename... Tail>
struct tail<Continue, Head, Tail...> : Continue::template apply<Tail...> { };

template <typename Continue, int N, typename... Pack>
struct drop {
    struct drop_cont {
        template <typename... PackA>
        struct apply : drop<Continue, N-1, PackA...> { };
    };

    using type = typename std::conditional<
        N,
        tail<drop_cont, Pack...>,
        typename Continue::template apply<Pack...>
    >::type::type;
};

template <int N, typename... Pack>
struct index {
    static_assert(N < sizeof...(Pack), "index out of range");

    struct index_cont {
        template <typename... Pack>
        struct apply : head<Pack...> { };
    };

    using type = drop<index_cont, N, Pack...>;
};

} // namespace primitives

#endif
