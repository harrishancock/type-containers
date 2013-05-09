/* DEPRECATED */

#ifndef TYPE_CONTAINERS_POLYNOMIAL_HPP
#define TYPE_CONTAINERS_POLYNOMIAL_HPP

#include "pack.hpp"

#include <ratio>

//////////////////////////////////////////////////////////////////////////////

template <typename LHS, typename RHS>
using min = std::integral_constant< typename LHS::value_type
                                  , (LHS::value < RHS::value) ? LHS::value : RHS::value>;

template <typename LHS, typename RHS>
using sum_func = typename std::ratio_add<LHS, RHS>::type;

template <typename List>
using sum = foldl<sum_func, std::ratio<0>, List>;

//////////////////////////////////////////////////////////////////////////////

namespace polynomial {

template <typename Poly>
using degree = std::integral_constant<typename length<Poly>::value_type,
      0 == length<Poly>::value ? 0 : length<Poly>::value - 1>;

template <typename LHS, typename RHS>
struct add_aux {
    using z = zip_with<std::ratio_add, LHS, RHS>;
    using zlen = min<length<LHS>, length<RHS>>;
    using t = append<drop<zlen, LHS>, drop<zlen, RHS>>;
    using type = append<z, t>;
};

template <typename LHS, typename RHS>
using add = typename add_aux<LHS, RHS>::type;

template <typename LHS, typename RHS>
struct multiply_aux {
    template <typename R>
    struct mult_by_head : std::ratio_multiply<head<LHS>, R> { };

    using current = map<mult_by_head, RHS>;
    using next = typename multiply_aux<tail<LHS>, RHS>::type;
    using type = add<current, cons<std::ratio<0>, next>>;
};

template <typename RHS>
struct multiply_aux<list<>, RHS> {
    using type = list<>;
};

template <typename LHS, typename RHS>
using multiply = typename multiply_aux<LHS, RHS>::type;

template <typename P, typename S>
struct quotient_aux {
    using srev = reverse<S>;
    using next = typename quotient_aux<tail<P>, S>::type;
    using ip = sum<zip_with<std::ratio_multiply, tail<srev>, next>>;
    using current = typename std::ratio_divide<
        typename std::ratio_subtract<head<P>, ip>::type, head<srev>>::type;
    using type = cons<current, next>;
};

template <typename S>
struct quotient_aux<list<>, S> {
    using type = list<>;
};

template <typename P, typename S>
using quotient = typename quotient_aux<drop<degree<S>, P>, S>::type;

template <typename P, typename SVecs, typename Quo>
struct remainder_aux2 {
    using next = typename remainder_aux2<tail<P>, tail<SVecs>, Quo>::type;
    using ip = sum<zip_with<std::ratio_multiply, head<SVecs>, Quo>>;
    using current = typename std::ratio_subtract<head<P>, ip>::type;
    using type = cons<current, next>;
};

template <typename P, typename Quo>
struct remainder_aux2<P, list<>, Quo> {
    using type = list<>;
};

template <typename P, typename S>
struct remainder_aux {
    using quo = quotient<P, S>;
    using svecs = tail<reverse<tails<tail<reverse<S>>>>>;   // hehe
    using type = typename remainder_aux2<P, svecs, quo>::type;
};

template <typename P, typename S>
using remainder = typename remainder_aux<P, S>::type;

template <typename P, typename S>
struct quo_rem_aux {
    using result = remainder_aux<P, S>;
    using type = pair<typename result::quo, typename result::type>;
};

template <typename P, typename S>
using quo_rem = typename quo_rem_aux<P, S>::type;

template <typename P, typename X>
using evaluate = head<remainder<P, list<typename std::ratio_subtract<std::ratio<0>, X>::type, std::ratio<1>>>>;



} // namespace polynomial

#endif
