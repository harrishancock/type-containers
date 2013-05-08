#ifndef TYPE_CONTAINERS_METAFUNC_HPP
#define TYPE_CONTAINERS_METAFUNC_HPP

template <typename...>
struct pack;

template <typename Head, typename... Tail>
struct head;

template <typename Head, typename... Tail>
struct head<pack
template <typename X, typename Pack>
struct cons;

template <typename X, typename... Ps>
struct cons<X, pack<Ps>> {
    using type = pack<X, Ps>;
};

template <typename As, typename Bs>
struct zip;

template <typename A, typename... As, typename B, typename... Bs>
struct zip<pack<A, As...>, pack<B, Bs...>> {
    using type = typename cons<pack<A, B>, typename zip<pack<As...>, pack<Bs...>>::type>::type;
};

template <typename A, typename... As>
struct zip<pack<A, As...>, pack<>> {
    using type = pack<>;
};

template <typename B, typename... Bs>
struct zip<pack<>, pack<B, Bs...>> {
    using type = pack<>;
};

template <>
struct zip<pack<>, pack<>> {
    using type = pack<>;
};

template <typename Arg, typename Tag>
struct inference;

template <typename... Tags>
struct same_as;

template <typename Lookup, typename Tag>
struct get_same_as;

template <typename A, typename T, typename 

template <typename Lookup, typename Arg, typename Tag, typename Enable = void>
struct infer {
    using type = inference<Arg, Tag>;
};

template <typename Lookup, typename Arg, typename Tag>
struct infer<Lookup, Arg, Tag, typename std::enable_if<is_var<Tag>::value>::type> {
    using type = inference<Arg, typename get_same_as<Lookup, Tag>::type>;
};

template <typename Lookup, typename Args, typename Tags>
struct infer_args;

template <typename Lookup, typename A, typename... As, typename T, typename... T>
struct infer_args<Lookup, pack<A, As...>, pack<T, Ts...>> {
    using arg = typename infer<Lookup, A, T>::type;
    using type = typename cons<arg, typename infer_args<Lookup, pack<As...>, pack<Ts...>>::type>::type;
};

template <typename Lookup, typename T, typename... Ts>
struct infer_args<Lookup, pack<>, pack<T, Ts...>> {
    static_assert(!sizeof...(Ts), "partial function application not yet supported");
    // TODO return a result signature
    using type = pack<>;
};

template <typename... Tags>
struct signature;

template <template <typename...> class Impl, typename Signature, typename... Args>
struct metafunc;

template <template <typename...> class Impl, typename... Tags, typename... Args> 
struct metafunc<Impl, signature<Tags...>, Args...> {
    // TODO if any of the args are placeholders, return a lambda
    static_assert(sizeof...(Tags) == sizeof...(Args) + 1, "partial function application not yet supported");
    using lookup = typename zip<pack<Args...>, pack<Tags...>>::type;
    using inferred_args = typename infer_args<lookup, pack<Args...>, pack<Tags...>>::type;
    using type = typename call_with_pack<Impl, inferred_args>::type;
};

#endif
