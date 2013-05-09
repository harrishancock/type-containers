#ifndef TYPE_CONTAINERS_BOOL_HPP
#define TYPE_CONTAINERS_BOOL_HPP

struct bool_tag;

template <bool X>
struct bool_ {
    static constexpr bool value = X;
    using tag = bool_tag;
    using type = bool_<X>;
};

using true_ = bool_<true>;
using false_ = bool_<false>;

#if 0
/* not_ needs to be a signed metafunction */
template <typename Bool>
struct not_ : bool_<!Bool::value> { };
#endif

#endif
