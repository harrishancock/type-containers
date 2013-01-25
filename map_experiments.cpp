
template <typename Head, typename...>
struct variadic_head { using type = Head; };

template <template <typename...> class Container, typename, typename... Tail>
struct variadic_tail { using type = Container<Tail...>; };

//////////////////////////////////////////////////////////////////////////////

template <typename... List>
struct list {
    using head = typename variadic_head<List...>::type;
    using tail = typename variadic_tail<list, List...>::type;
    static constexpr unsigned int size = sizeof...(List);
};

template <>
struct list<> {
    static constexpr unsigned int size = 0;
};

/* TODO: turn count_if into a fold, then implement at and count in terms of folds? */
template <typename List, template <typename> class Predicate, unsigned int C = 0>
struct count_if : count_if<typename List::tail, Func, C + Predicate<typename List::head>::value> { };

template <template <typename> class Predicate, unsigned int C>
struct count_if<list<>, Predicate, C> {
    static constexpr unsigned int value = C;
};

template <typename... Pairs>
struct map {
    using list_type = list<Pairs...>;
    static_assert(count_if<typename list_type::tail, keymatch<typename list_type::head>::predicate>::value,
            "duplicate key inserted");

private:
    template <typename Key>
    struct keymatch {
        template <typename Pair>
        struct predicate : std::is_same<Key, typename Pair::key> { };
    };
};

//////////////////////////////////////////////////////////////////////////////

template <typename Key, typename Value>
struct pair { using key = Key; using value = Value; };

