#ifndef TYPE_CONTAINERS_NUM_HPP
#define TYPE_CONTAINERS_NUM_HPP

//////////////////////////////////////////////////////////////////////////////

/* Num tag class */

template <typename LHS, typename RHS>
struct add;

template <typename LHS, typename RHS>
struct multiply;

template <typename LHS, typename RHS>
struct subtract;

template <typename T>
struct negate;

template <typename T>
struct abs;

template <typename T>
struct signum;

//////////////////////////////////////////////////////////////////////////////

template <typename Tag>
struct add_impl;

template <typename Tag>
struct multiply_impl;

template <typename Tag>
struct subtract_impl {
    template <typename LHS, typename RHS>
    struct apply : add<LHS, typename negate<RHS>::type> { };
};

template <typename Tag>
struct negate_impl {
    template <typename T>
    struct apply : subtract<typename subtract<T, T>::type, T> { };
};

template <typename Tag>
struct abs_impl;

template <typename Tag>
struct signum_impl;

//////////////////////////////////////////////////////////////////////////////

template <typename LHS, typename RHS>
struct add : apply<add_impl<typename LHS::tag>, LHS, RHS> {
    static_assert(::std::is_same<typename LHS::tag, typename RHS::tag>::value,
            "add argument tag mismatch");
};

template <typename LHS, typename RHS>
struct multiply : apply<multiply_impl<typename LHS::tag>, LHS, RHS> {
    static_assert(::std::is_same<typename LHS::tag, typename RHS::tag>::value,
            "multiply argument tag mismatch");
};

template <typename LHS, typename RHS>
struct subtract : apply<subtract_impl<typename LHS::tag>, LHS, RHS> { };
    static_assert(::std::is_same<typename LHS::tag, typename RHS::tag>::value,
            "subtract argument tag mismatch");
};

template <typename T>
struct negate : apply<negate_impl<typename T::tag>, T> { };

template <typename T>
struct abs : apply<abs_impl<typename T::tag>, T> { };

template <typename T>
struct signum : apply<signum_impl<typename T::tag>, T> { };

//////////////////////////////////////////////////////////////////////////////

struct bool_tag;

template <bool X>
struct bool_ : ::std::integral_constant<bool, X> {
    using tag = bool_tag;
    using type = bool_<X>;
};

using true_ = bool_<true>;
using false_ = bool_<false>;

template <typename Bool>
struct not_ : bool_<!Bool::value> { };

//////////////////////////////////////////////////////////////////////////////

/* Eq tag class */

template <typename LHS, typename RHS>
struct eq;

template <typename LHs, typename RHS>
struct neq;

template <typename Tag>
struct eq_impl {
    template <typename LHS, typename RHS>
    struct apply : not_<typename neq<LHS, RHS>::type> { };
};

template <typename Tag>
struct neq_impl {
    template <typename LHS, typename RHS>
    struct apply : not_<typename eq<LHS, RHS>::type> { };
};

template <typename LHS, typename RHS>
struct eq : apply<eq_impl<typename LHS::tag>, LHS, RHS> {
    static_assert(::std::is_same<typename LHS::tag, typename RHS::tag>::value,
            "eq argument tag mismatch");
};

template <typename LHS, typename RHS>
struct neq : apply<neq_impl<typename LHS::tag>, LHS, RHS> {
    static_assert(::std::is_same<typename LHS::tag, typename RHS::tag>::value,
            "neq argument tag mismatch");
};

//////////////////////////////////////////////////////////////////////////////

/* Enum tag class */

template <typename T>
struct succ;

template <typename T>
struct pred;

template <typename Tag>
struct succ_impl;

template <typename Tag>
struct pred_impl;

template <typename T>
struct succ : apply<succ_impl<typename T::tag>, T> { };

template <typename T>
struct pred : apply<pred_impl<typename T::tag>, T> { };

//////////////////////////////////////////////////////////////////////////////

/* Integral tag class */

template <typename LHS, typename RHS>
struct quot;

template <typename LHS, typename RHS>
struct rem;

template <typename LHS, typename RHS>
struct div;

template <typename LHS, typename RHS>
struct mod;

template <typename LHS, typename RHS>
struct quot_rem;

template <typename LHS, typename RHS>
struct div_mod;

template <typename Tag>
struct quot_impl {
    template <typename LHS, typename RHS>
    struct apply : fst<typename quot_rem<LHS, RHS>::type> { };
};

template <typename Tag>
struct rem_impl {
    template <typename LHS, typename RHS>
    struct apply : snd<typename quot_rem<LHS, RHS>::type> { };
};

template <typename Tag>
struct div_impl {
    template <typename LHS, typename RHS>
    struct apply : fst<typename div_mod<LHS, RHS>::type> { };
};

template <typename Tag>
struct mod_impl {
    template <typename LHS, typename RHS>
    struct apply : snd<typename div_mod<LHS, RHS>::type> { };
};

template <typename Tag>
struct quot_rem_impl;

template <typename Tag>
struct div_mod_impl {
    template <typename LHS, typename RHS>
    struct apply {
        using qr = typename quot_rem<LHS, RHS>::type;
        using q = typename fst<qr>::type;
        using r = typename snd<qr>::type;
        using type = typename ::std::conditional<
            typename eq<
                    typename signum<LHS>::type,
                    typename negate<typename signum<RHS>::type>::type
            >::type::value,
            tuple<typename pred<q>::type, typename add<r, RHS>::type>,
            qr
        >::type;
    };
};

template <typename LHS, typename RHS>
struct quot : apply<quot_impl<typename LHS::tag>, LHS, RHS> {
    static_assert(::std::is_same<typename LHS::tag, typename RHS::tag>::value,
            "quot argument tag mismatch");
};

template <typename LHS, typename RHS>
struct rem : apply<rem_impl<typename LHS::tag>, LHS, RHS> {
    static_assert(::std::is_same<typename LHS::tag, typename RHS::tag>::value,
            "rem argument tag mismatch");
};

template <typename LHS, typename RHS>
struct div : apply<div_impl<typename LHS::tag>, LHS, RHS> {
    static_assert(::std::is_same<typename LHS::tag, typename RHS::tag>::value,
            "div argument tag mismatch");
};

template <typename LHS, typename RHS>
struct mod : apply<mod_impl<typename LHS::tag>, LHS, RHS> {
    static_assert(::std::is_same<typename LHS::tag, typename RHS::tag>::value,
            "mod argument tag mismatch");
};

template <typename LHS, typename RHS>
struct quot_rem : apply<quot_rem_impl<typename LHS::tag>, LHS, RHS> {
    static_assert(::std::is_same<typename LHS::tag, typename RHS::tag>::value,
            "quot_rem argument tag mismatch");
};

template <typename LHS, typename RHS>
struct div_mod : apply<div_mod_impl<typename LHS::tag>, LHS, RHS> {
    static_assert(::std::is_same<typename LHS::tag, typename RHS::tag>::value,
            "div_mod argument tag mismatch");
};

//////////////////////////////////////////////////////////////////////////////

struct int_tag;

template <int I>
struct int_ : ::std::integral_constant<int, I> {
    using tag = int_tag;
    using type = int_<I>;
};

template <>
struct add_impl<int_tag> {
    template <typename LHS, typename RHS>
    struct apply : int_<LHS::value + RHS::value> { };
};

template <>
struct multiply_impl<int_tag> {
    template <typename LHS, typename RHS>
    struct apply : int_<LHS::value * RHS::value> { };
};

template <>
struct negate_impl<int_tag> {
    template <typename T>
    struct apply : int_<-T::value> { };
};

template <>
struct abs_impl<int_tag> {
    template <typename T>
    struct apply : int_<(T::value < 0 ? -T::value : T::value)> { };
};

template <>
struct signum_impl<int_tag> {
    template <typename T>
    struct apply : int_<(T::value / (T::value < 0 ? -T::value : T::value))> { };
};

template <>
struct eq_impl<int_tag> {
    template <typename LHS, typename RHS>
    struct apply : bool_<(LHS::value == RHS::value)> { };
};

template <>
struct succ_impl<int_tag> {
    template <typename T>
    struct apply : int_<T::value + 1> { };
};

template <>
struct pred_impl<int_tag> {
    template <typename T>
    struct apply : int_<T::value - 1> { };
};

template <>
struct quot_rem_impl<int_tag> {
    template <typename LHS, typename RHS>
    struct apply {
        using type = tuple<

    };
};

//////////////////////////////////////////////////////////////////////////////

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
