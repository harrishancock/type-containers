/* DEPRECATED */

#ifndef TYPE_CONTAINERS_LAMBDA2_HPP
#define TYPE_CONTAINERS_LAMBDA2_HPP

#include "trivial.hpp"

#include <type_traits>

template <int N>
struct arg;

template <typename MF, typename... Vars>
struct pexpr;

using _1 = arg<1>;
using _2 = arg<2>;

//////////////////////////////////////////////////////////////////////////////

template <unsigned int N, typename... List>
struct get;

template <unsigned int N, typename Head, typename... Tail>
struct get<N, Head, Tail...> : get<N-1, Tail...> { };

template <typename Head, typename... List>
struct get<0, Head, List...> : id<Head> { };

template <typename PExpr>
struct nonlocal_pexpr : pexpr<PExpr> { };

template <typename T>
struct is_pexpr : std::false_type { };

template <int N>
struct is_pexpr<arg<N>> : std::true_type { };

template <typename PExpr>
struct is_pexpr<pexpr<PExpr>> : std::true_type { };

template <typename... Bools>
struct or2;

template <typename Head, typename... Tail>
struct or2<Head, Tail...> : std::conditional<Head::value,
    std::true_type, or2<Tail...>>::type { };

template <>
struct or2<> : std::false_type { };

template <typename T>
struct lambda : id<T> { };

template <typename PExpr>
struct lambda<pexpr<PExpr>> {
    using type = nonlocal_pexpr<PExpr>;
};

template <typename PExpr>
struct lambda<nonlocal_pexpr<PExpr>> {
    using type = nonlocal_pexpr<PExpr>;
};

template <template <typename...> class MF, typename... Args>
struct lambda<MF<Args...>> {
    using recursion = MF<typename lambda<Args>::type...>;
    using type = typename std::conditional<
        or2<typename is_pexpr<typename lambda<Args>::type>::type...>::value,
        pexpr<recursion>,
        recursion>::type;
};

//////////////////////////////////////////////////////////////////////////////

template <typename MFC, typename... Args>
struct apply_local : apply<MFC, Args...> { };

template <typename PExpr, typename... Args>
struct apply_local<nonlocal_pexpr<PExpr>, Args...> {
    using type = nonlocal_pexpr<PExpr>;
};

template <int N>
struct arg : id<arg<N>> {
    template <typename... Args>
    struct apply {
        using type = typename lambda<
            typename std::conditional<(static_cast<int>(sizeof...(Args)) < N),
              arg<N - static_cast<int>(sizeof...(Args))>,
              get<N-1, Args...>>::type::type
                  >::type;
    };
};

template <typename... List>
struct count_vars;

template <typename Head, typename... Tail>
struct count_vars<Head, Tail...> : std::integral_constant<int, count_vars<Tail...>::value> { };

template <int N, typename... Tail>
struct count_vars<arg<N>, Tail...> : std::integral_constant<int, 1 + count_vars<Tail...>::value> { };

template <typename PExpr, typename... Tail>
struct count_vars<pexpr<PExpr>, Tail...> : std::integral_constant<int, pexpr<PExpr>::value + count_vars<Tail...>::value> { };

template <>
struct count_vars<> : std::integral_constant<int, 0> { };

template <typename MFC, int N, typename... Args>
struct dropapply;

template <typename MFC, int N, typename Head, typename... Tail>
struct dropapply<MFC, N, Head, Tail...> : dropapply<MFC, N-1, Tail...> { };

template <typename MFC, typename Head, typename... Tail>
struct dropapply<MFC, 0, Head, Tail...> : apply<MFC, Head, Tail...> { };

template <typename MFC, int N>
struct dropapply<MFC, N> : apply<MFC> { };

template <typename MFC>
struct dropapply<MFC, 0> : apply<MFC> { };

template <template <typename...> class MF, typename... Exprs>
struct pexpr<MF<Exprs...>> : id<pexpr<MF<Exprs...>>> {
    static constexpr unsigned int numvars = count_vars<Exprs...>::value;

    template <typename... Args>
    struct apply {
        /* Three cases:
         * sizeof...(Args) < numvars -> pexpr<recursion>
         * sizeof...(Args) = numvars -> typename recursion::type
         * sizeof...(Args) > numvars -> typename dropapply<apply, typename recursion::type, numvars, Args...>::type
         */
        using recursion = MF<typename apply_local<Exprs, Args...>::type...>;
        using transformation = typename std::conditional<
            (sizeof...(Args) < numvars),
            pexpr<recursion>,
            recursion>::type::type;

        using type = typename std::conditional<
            (sizeof...(Args) > numvars),
            dropapply<transformation, numvars, Args...>,
            id<transformation>>::type::type;

        static_assert(static_cast<int>(sizeof...(Args)) <= numvars
                || has_apply<transformation>::value, "too many args to pexpr::apply");

    };
};

#endif
