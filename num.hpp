#ifndef TYPE_CONTAINERS_NUM_HPP
#define TYPE_CONTAINERS_NUM_HPP

//////////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////////

#endif
