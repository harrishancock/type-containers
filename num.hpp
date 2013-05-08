#ifndef TYPE_CONTAINERS_NUM_HPP
#define TYPE_CONTAINERS_NUM_HPP

#include <type_traits>

struct bool_tag;

template <bool X>
struct bool_ {
    static constexpr bool value = X;
    using tag = bool_tag;
    using type = bool_<X>;
};

using true_ = bool_<true>;
using false_ = bool_<false>;

template <typename Bool>
struct not_ : bool_<!Bool::value> { };

//////////////////////////////////////////////////////////////////////////////

namespace tag {
    struct dispatch;
    struct impl;
    struct any;
} // namespace tag

template <typename...>
struct pack;

template <template <typename...> class Thunk, typename Pack, typename Tag, typename... Args>
struct coerce_impl;

template <template <typename...> class Thunk, typename... Params, typename Tag, typename Head, typename... Tail>
struct coerce_impl<Thunk, pack<Params...>, Tag, Head, Tail...> : coerce_impl<Thunk, pack<Params..., typename std::conditional<sizeof...(Tail) == 0 && std::is_same<tag::any, Head>::value,
    Tag,
    Head>::type>, Tag, Tail...> { };

template <template <typename...> class Thunk, typename... Params, typename Tag>
struct coerce_impl<Thunk, pack<Params...>, Tag> {
    using type = typename Thunk<Params...>::type;
};

template <typename Thunk, typename Tag>
struct coerce;

template <template <typename...> class Thunk, typename... Args, typename Tag>
struct coerce<Thunk<Args...>, Tag> {
    using type = typename coerce_impl<Thunk, pack<>, Tag, Args...>::type;
};

struct tuple2_tag;

template <typename Fst, typename Snd>
struct tuple2 {
    using tag = tuple2_tag;
    using type = tuple2<Fst, Snd>;

    using fst = typename Fst::type;
    using snd = typename Snd::type;
};

template <typename Tuple, typename Tag = tag::signature>
struct fst;

template <typename Tuple>
struct fst<Tuple, tag::signature> {
    /* Put this inside each signed function for help debugging. Maybe move this
     * outside when a better debugging system is found */
    template <typename Thunk, typename Tag>
    struct coercion {
        using type = typename coerce<Thunk, Tag>::type;
        static_assert(std::is_same<Tag, typename type::tag>::value,
                "type mismatch in arguments to fst");
    };

    using type = typename fst<coercion<Tuple, tuple2_tag>, tag::impl>::type;
};

template <typename Tuple, typename Tag>
struct fst {
    using type = typename Tuple::type::fst;
};

template <typename Tuple, typename Tag = tag::signature>
struct snd;

template <typename Tuple>
struct snd<Tuple, tag::signature> {
    template <typename Thunk, typename Tag>
    struct coercion {
        using type = typename coerce<Thunk, Tag>::type;
        static_assert(std::is_same<Tag, typename type::tag>::value,
                "type mismatch in arguments to snd");
    };

    using type = typename snd<coercion<Tuple, tuple2_tag>, tag::impl>::type;
};

template <typename Tuple, typename Tag>
struct snd {
    using type = typename Tuple::type::snd;
};

//////////////////////////////////////////////////////////////////////////////

template <typename Bool, typename Then, typename Else, typename Tag = tag::signature>
struct if_;

template <typename Bool, typename Then, typename Else>
struct if_<Bool, Then, Else, tag::signature> {
    template <typename Thunk, typename Tag>
    struct coercion {
        using type = typename coerce<Thunk, Tag>::type;
        static_assert(std::is_same<Tag, typename type::tag>::value,
                "type mismatch in arguments to if_");
    };

    using type = typename if_<coercion<Bool, bool_tag>, Then, Else, tag::impl>::type;
};

template <typename Bool, typename Then, typename Else, typename Tag>
struct if_ {
    using type = typename std::conditional<Bool::type::value, Then, Else>::type::type;
};

//////////////////////////////////////////////////////////////////////////////

template <typename... Thunks>
struct tag_of;

template <typename Head, typename... Tail>
struct tag_of<Head, Tail...> {
    using type = typename std::conditional<!std::is_same<tag::any, Head>::value,
          Head,
          typename tag_of<Tail...>::type>::type;
};

template <>
struct tag_of<> {
    using type = tag::any;
};

template <template <typename...> class Thunk, typename... Args>
struct inferred {
    using tag = tag::any;

    template <typename Tag>
    struct apply : Thunk<Args..., Tag> { };
};

namespace num {

/* Num tag class */

template <typename LHS, typename RHS, typename Tag = tag::dispatch>
struct add;

template <typename LHS, typename RHS>
struct add<LHS, RHS, tag::dispatch> : add<LHS, RHS, typename tag_of<LHS, RHS>::type> { };

template <typename LHs, typename RHS>
struct add<LHS, RHS, tag::any> : inferred<add, LHS, RHS> { };

template <typename LHS, typename RHS, typename Tag = tag::dispatch>
struct multiply;

template <typename LHS, typename RHS>
struct multiply<LHS, RHS, tag::dispatch> : multiply<LHS, RHS, typename tag_of<LHS, RHS>::type> { };

template <typename LHs, typename RHS>
struct multiply<LHS, RHs, tag::any> : inferred<multiply, LHS, RHS> { };

template <typename LHS, typename RHS, typename Tag = tag::dispatch>
struct subtract;

template <typename LHS, typename RHS>
struct subtract<LHS, RHS, tag::dispatch> : subtract<LHS, RHS, typename tag_of<LHS, RHS>::type> { };

template <typename LHS, typename RHS>
struct subtract<LHS, RHS tag::any> : inferred<subtract, LHS, RHS> { };

template <typename T, typename Tag = tag::dispatch>
struct negate;

template <typename T>
struct negate<T, tag::dispatch> : negate<T, typename tag_of<T>::type> { };

template <typename T>
struct negate<T, tag::any> : inferred<negate, T> { };

template <typename T, typename Tag = tag::dispatch>
struct abs;

template <typename T>
struct abs<T, tag::dispatch> : abs<T, typename tag_of<T>::type> { };

template <typename T>
struct abs<T, tag::any> : inferred<abs, T> { };

template <typename T, typename Tag = tag::dispatch>
struct signum;

template <typename T>
struct signum<T, tag::dispatch> : signum<T, typename tag_of<T>::type> { };

template <typename T>
struct signum<T, tag::any> : inferred<signum, T> { };

template <typename T, typename Tag = tag::dispatch>
struct from_integer;

template <typename T>
struct from_integer<T, tag::dispatch> : inferred<from_integer, T> { };

/* Default implementations */

template <typename LHS, typename RHS, typename Tag>
struct subtract : add<LHS, negate<RHS>> { };

template <typename T, typename Tag>
struct negate : subtract<subtract<T, T>, T> { };

} // namespace num

//////////////////////////////////////////////////////////////////////////////

namespace eq {

/* Eq tag class */

template <typename LHS, typename RHS, typename Tag = tag::dispatch>
struct eq;

template <typename LHS, typename RHS>
struct eq<LHS, RHS, tag::dispatch> : eq<LHS, RHS, typename tag_of<LHS, RHS>::type> { };

template <typename LHS, typename RHS, typename Tag = tag::dispatch>
struct neq;

template <typename LHS, typename RHS>
struct neq<LHS, RHS, tag::dispatch> : neq<LHS, RHS, typename tag_of<LHS, RHS>::type> { };

/* Default implementations */

template <typename LHS, typename RHS, typename Tag>
struct eq : not_<neq<LHS, RHS>> { };

template <typename LHS, typename RHS, typename Tag>
struct neq : not_<eq<LHS, RHS>> { };

} // namespace eq

//////////////////////////////////////////////////////////////////////////////

namespace integral {

/* Integral tag class */

template <typename LHS, typename RHS, typename Tag = tag::dispatch>
struct quot;

template <typename LHS, typename RHS>
struct quot<LHS, RHS, tag::dispatch> : quot<LHS, RHS, typename tag_of<LHS, RHS>::type> { };

template <typename LHS, typename RHS>
struct quot<LHS, RHS, tag::any> : inferred<quot, LHS, RHS> { };

template <typename LHS, typename RHS, typename Tag = tag::dispatch>
struct rem;

template <typename LHS, typename RHS>
struct rem<LHS, RHS, tag::dispatch> : rem<LHS, RHS, typename tag_of<LHS, RHS>::type> { };

template <typename LHS, typename RHS>
struct rem<LHS, RHS, tag::any> : inferred<rem, LHS, RHS> { };

template <typename LHS, typename RHS, typename Tag = tag::dispatch>
struct div;

template <typename LHS, typename RHS>
struct div<LHS, RHS, tag::dispatch> : div<LHS, RHS, typename tag_of<LHS, RHS>::type> { };

template <typename LHS, typename RHS>
struct div<LHS, RHS, tag::any> : inferred<div, LHS, RHS> { };

template <typename LHS, typename RHS, typename Tag = tag::dispatch>
struct mod;

template <typename LHS, typename RHS>
struct mod<LHS, RHS, tag::dispatch> : mod<LHS, RHS, typename tag_of<LHS, RHS>::type> { };

template <typename LHS, typename RHS>
struct mod<LHS, RHS, tag::any> : inferred<mod, LHS, RHS> { };

/* FIXME These two metafunctions would be tuple2_tag<tag::any, tag::any>... need to enable
 * this capability. */

template <typename LHS, typename RHS, typename Tag = tag::dispatch>
struct quot_rem;

template <typename LHS, typename RHS>
struct quot_rem<LHS, RHS, tag::dispatch> : quot_rem<LHS, RHS, typename tag_of<LHS, RHS>::type> { };

template <typename LHS, typename RHS, typename Tag = tag::dispatch>
struct div_mod;

template <typename LHS, typename RHS>
struct div_mod<LHS, RHS, tag::dispatch> : div_mod<LHS, RHS, typename tag_of<LHS, RHS>::type> { };

template <typename T, typename Tag = tag::dispatch>
struct to_integer;

template <typename T>
struct to_integer<T, tag::dispatch> : to_integer<T, typename tag_of<T>::type> { };

/* Default implementations */

template <typename LHS, typename RHS, typename Tag>
struct quot : fst<quot_rem<LHS, RHS>> { };

template <typename LHS, typename RHS, typename Tag>
struct rem : snd<quot_rem<LHS, RHS>> { };

template <typename LHS, typename RHS, typename Tag>
struct div : fst<div_mod<LHS, RHS>> { };

template <typename LHS, typename RHS, typename Tag>
struct mod : snd<div_mod<LHS, RHS>> { };

template <typename LHS, typename RHS, typename Tag>
struct div_mod {
    using qr = quot_rem<LHS, RHS>;
    using q = fst<qr>;
    using r = snd<qr>;
    using type = typename if_<
        
        eq::eq< num::signum<LHS>, num::negate< num::signum<RHS> > >,

        tuple2< num::subtract<q, from_integral<int_<1>>>,
                num::add<r, RHS> >,
        qr
    >::type;
};

} // namespace integral

//////////////////////////////////////////////////////////////////////////////

/* Int */

namespace fuglyhack {
    template <typename T>
    constexpr T abs (T x) {
        return x < 0 ? -x : x;
    }
} // namespace fuglyhack

struct int_tag;

template <int I>
struct int_ {
    static constexpr int value = I;
    using tag = int_tag;
    using type = int_<I>;
};

namespace num {

/* Num instance */

template <typename LHS, typename RHS>
struct add<LHS, RHS, int_tag> : int_<LHS::type::value + RHS::type::value> { };

template <typename LHS, typename RHS>
struct multiply<LHS, RHS, int_tag> : int_<LHS::type::value * RHS::type::value> { };

template <typename T>
struct negate<T, int_tag> : int_<-T::type::value> { };

template <typename T>
struct abs<T, int_tag> : int_<fuglyhack::abs(T::type::value)> { };

template <typename T>
struct signum<T, int_tag> : int_<(T::value / (T::value ? fuglyhack::abs(T::type::value) : 1))> { };

} // namespace num

namespace eq {

/* Eq instance */

template <typename LHS, typename RHS>
struct eq<LHS, RHS, int_tag> : bool_<(LHS::value == RHS::value)> { };

} // namespace eq

namespace integral {

/* Integral instance */

template <typename LHS, typename RHS>
struct quot_rem<LHS, RHS, int_tag> {
    using q = int_<LHS::type::value / RHS::type::value>;
    using r = int_<LHS::type::value % fuglyhack::abs(RHS::type::value)>;
    using type = tuple2<q, r>;
};

} // namespace integral

//////////////////////////////////////////////////////////////////////////////

#if 0
struct ratio_tag;

template <typename Num, typename Den>
struct ratio {
    using tag = ratio_tag;
    static_assert(::std::is_same<typename Num::tag, typename Den::tag>::value,
            "ratio argument tag mismatch");
    using num = typename multiply<
        typename signum<typename multiply<Num, Den>::type>::type,
        typename abs<Num>::type
    >::type;
    using den = typename abs<Den>::type;
    using divisor = typename gcd<Num, Den>::type;
    using type = ratio<typename divide<Num, divisor>::type, typename divide<Den, divisor>::type>;
};
#endif

#endif
