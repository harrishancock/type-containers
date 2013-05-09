/* DEPRECATED */

#ifndef TYPE_CONTAINERS_TRIVIAL_HPP
#define TYPE_CONTAINERS_TRIVIAL_HPP

#include "pack.hpp"

//////////////////////////////////////////////////////////////////////////////

template <typename T>
struct id {
    using type = T;
};

//////////////////////////////////////////////////////////////////////////////

template <template <typename...> class T>
struct has_apply_aux : std::true_type { };

/**
 * True if parameter T has a nested class template apply, false otherwise.
 * TODO this is ugly, find a better way
 */
template <typename T, typename Enable = void>
struct has_apply : std::false_type { };

template <typename T>
struct has_apply<T,
        typename std::enable_if<
            has_apply_aux<T::template apply>::value
        >::type> : std::true_type { };

template <typename MFC, typename ListArgs, typename Enable = void>
struct apply_impl : id<MFC> { };

template <typename MFC, typename... Args>
struct apply_impl<MFC, list<Args...>,
        typename std::enable_if<
            has_apply<MFC>::value
        >::type> {
    using type = typename MFC::template apply<Args...>::type;
};

/**
 * Invoke a given metafunction class MFC with the given Args.
 */
template <typename MFC, typename... Args>
struct apply {
    using type = typename apply_impl<MFC, list<Args...>>::type;
};

//////////////////////////////////////////////////////////////////////////////

#endif
