#ifndef TYPE_CONTAINERS_NUM_HPP
#define TYPE_CONTAINERS_NUM_HPP

template <typename Fst, typename Snd>
struct tuple2 {
    using fst = Fst;
    using snd = Snd;
    using type = tuple2<Fst, Snd>;
};

template <typename Tuple>
struct fst {
    using type = typename Tuple::fst;
};

template <typename Tuple>
struct snd {
    using type = typename Tuple::snd;
};

//////////////////////////////////////////////////////////////////////////////

template <typename Tag>
struct value_ctor;

#define DIGIT(TAG, D) value_ctor<TAG>::template apply<D>::type;

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

/* Num tag class */

template <typename LHS, typename RHS, typename Tag = LHS::tag>
struct add;

template <typename LHS, typename RHS, typename Tag = LHS::tag>
struct multiply;

template <typename LHS, typename RHS, typename Tag = LHS::tag>
struct subtract;

template <typename T, typename Tag = T::tag>
struct negate;

template <typename T, typename Tag = T::tag>
struct abs;

template <typename T, typename Tag = T::tag>
struct signum;

/* Default implementations */

template <typename LHS, typename RHS, typename Tag>
struct subtract : add<LHS, typename negate<RHS>::type> { };

template <typename T, typename Tag>
struct negate : subtract<typename subtract<T, T>::type, T> { };

//////////////////////////////////////////////////////////////////////////////

/* Eq tag class */

template <typename LHS, typename RHS, typename Tag = LHS::tag>
struct eq;

template <typename LHS, typename RHS, typename Tag = LHS::tag>
struct neq;

/* Default implementations */

template <typename LHS, typename RHS, typename Tag>
struct eq : not_<typename neq<LHS, RHS>::type> { };

template <typename LHS, typename RHS, typename Tag>
struct neq : not_<typename eq<LHS, RHS>::type> { };

//////////////////////////////////////////////////////////////////////////////

/* Integral tag class */

template <typename LHS, typename RHS, typename Tag = LHS::tag>
struct quot;

template <typename LHS, typename RHS, typename Tag = LHS::tag>
struct rem;

template <typename LHS, typename RHS, typename Tag = LHS::tag>
struct div;

template <typename LHS, typename RHS, typename Tag = LHS::tag>
struct mod;

template <typename LHS, typename RHS, typename Tag = LHS::tag>
struct quot_rem;

template <typename LHS, typename RHS, typename Tag = LHS::tag>
struct div_mod;

/* Default implementations */

template <typename LHS, typename RHS, typename Tag>
struct quot : fst<typename quot_rem<LHS, RHS>::type> { };

template <typename LHS, typename RHS, typename Tag>
struct rem : snd<typename quot_rem<LHS, RHS>::type> { };

template <typename LHS, typename RHS, typename Tag>
struct div : fst<typename div_mod<LHS, RHS>::type> { };

template <typename LHS, typename RHS, typename Tag>
struct mod : snd<typename div_mod<LHS, RHS>::type> { };

template <typename LHS, typename RHS, typename Tag>
struct div_mod {
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

//////////////////////////////////////////////////////////////////////////////

/* Int */

struct int_tag;

template <>
struct value_ctor<int_tag> {
    template <int I>
    using apply = int_<I>;
};

template <int I>
struct int_ : ::std::integral_constant<int, I> {
    using tag = int_tag;
    using type = int_<I>;
};

/* Num instance */

template <typename LHS, typename RHS>
struct add<LHS, RHS, int_tag> : int_<LHS::value + RHS::value> { };

template <typename LHS, typename RHS>
struct multiply<LHS, RHS, int_tag> : int_<LHS::value * RHS::value> { };

template <typename T>
struct negate<LHS, RHS, int_tag> : int_<-T::value> { };

template <typename T>
struct abs<T, int_tag> : int_<(T::value < 0 ? -T::value : T::value)> { };

template <typename T>
struct signum<T, int_tag> : int_<(T::value / abs<T>::value)> { };

/* Eq instance */

template <typename LHS, typename RHS>
struct eq<LHS, RHS, int_tag> : bool_<(LHS::value == RHS::value)> { };

/* Integral instance */

template <typename LHS, typename RHS>
struct quot_rem<LHS, RHS, int_tag> {
    using q = LHS::value / RHS::value;
    using r = LHS::value % abs<RHS>::value;
    using type = tuple<q, r>;
};

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
