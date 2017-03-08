
// purpose : simulate cons & car & cdr & map in Scheme
// compile : g++ -std=c++11 -o cons cons.cpp
// run     : ./cons
// date    : 2017.03.08

#include <iostream>

// int_
//
template <int N>
struct int_ {
    using type = int_<N>;
    using value_type = int;
    static constexpr int value = N;
    using next = int_ < N + 1 >;
    using prev = int_ < N - 1 >;
};

// uint_
//
template <unsigned int N>
struct uint_ {
    using type = uint_<N>;
    using value_type = unsigned int;
    static constexpr unsigned int value = N;
    using next = uint_ < N + 1 >;
    using prev = uint_ < N - 1 >;
};

template <>
struct uint_<0> {
    using type = uint_<0>;
    using value_type = unsigned int;
    static constexpr unsigned int value = 0;
    using next = uint_<1>;
    using prev = uint_<0>;
};

// bool_
template <bool N>
struct bool_ {
    using type = bool_<N>;
    using value_type = bool;
    static constexpr bool value = N;
    using next = bool_ < !N >;
    using prev = bool_ < !N >;
};

// converts a binary string to its base-10 value.
//
template <unsigned int N>
struct binary : uint_ < binary < N / 10 >::type::value * 2 + (N % 10) > {};

template <>
struct binary<0> : uint_<0> {};

// plus
//
template <typename T, typename N>
struct plus : int_ < T::value + N::value > {};

// minus
//
template <typename T, typename N>
struct minus : int_ < T::value - N::value > {};

// inc_f
//
struct inc_f {
    template <typename n>
    struct apply : int_ < n::value + 1 > {};
};

template <typename n>
struct inc : int_ < n::value + 1 > {};

// cons
//
template <typename h, typename t>
struct cons {
    using type = cons<h, t>;
    using head = h;
    using tail = t;
};

// car
//
struct car_f {
    template <typename cons>
    struct apply {
        using type = typename cons::type::head;
    };
};

template <typename cons>
struct car : car_f::template apply<cons>::type {};

// cdr
//
struct cdr_f {
    template <typename cons>
    struct apply {
        using type = typename cons::type::tail::type;
    };
};

template <typename cons>
struct cdr : cdr_f::template apply<cons>::type {};

// empty
//
struct empty {
    using type = empty;
    using value = empty;
};

// list_t
//
template <typename first = empty, typename ...rest>
struct list_t : std::conditional <
    sizeof...(rest) == 0,
    cons<first, empty>,
    cons<first, typename list_t<rest...>::type>>::type
{};

template <>
struct list_t<empty> : empty {};

// type_
//
template <typename T, T N>
struct type_ {
    using type = type_<T, N>;
    using value_type = T;
    static constexpr T value = N;
    using next = type_ < T, N + 1 >;
    using prev = type_ < T, N - 1 >;
};

// list
//
template <typename T, T ...elements>
struct list : list_t<type_<T, elements>...> {};

// length
//
template <typename list>
struct length
{
    static constexpr unsigned int value =
        1 + length<typename cdr<list>::type>::value;
};

template <>
struct length<empty>
{
    static constexpr unsigned int value = 0;
};

// is_empty
//
template <typename list>
struct is_empty
{
    static constexpr bool value = std::is_same<list, empty>::value;
};

// append
//
struct append_f {
    template <typename list1, typename list2>
    struct apply : cons<
        typename car<list1>::type,
        typename append_f::template apply<typename cdr<list1>::type, list2>::type>
    {};

    template<typename list2>
    struct apply <empty, list2>: list2
    {};
};

template <typename list1, typename list2>
struct append : append_f::template apply<list1, list2> {};

// map
//
struct map_f {
    template <typename fn, typename list>
    struct apply : cons <
        typename fn::template apply<typename car<list>::type>,
        map_f::template apply<fn, typename cdr<list>::type>
    >{};
};

template <typename fn, typename list>
struct map : map_f::template apply<fn, list> {};

// apply
//
struct apply_f {
    template <template <typename...> class F, typename ...args>
    struct apply : F<typename args::type...> {};
};

template <template <typename...> class F, typename ...args>
struct apply : apply_f::template apply<F, args...> {};

// lambda
//
template <template <typename...> class F>
struct lambda {
    template <typename ...args>
    struct apply : apply_f::template apply<F, args...> {};
};

// transform
//
struct transform_f {
    template <typename list1, typename list2, typename fn>
    struct apply : cons <
        typename fn::template apply<typename car<list1>::type, typename car<list2>::type>::type,
    typename transform_f::template apply <
        typename cdr<list1>::type,
        typename cdr<list2>::type,
        fn
        >::type
    > {};

    template <typename list1, typename fn>
    struct apply<list1, empty, fn> : cons <
        typename fn::template apply<typename car<list1>::type, empty>,
    typename transform_f::template apply <
        typename cdr<list1>::type,
        empty,
        fn
        >::type
    > {};

    template <typename list2, typename fn>
    struct apply<empty, list2, fn> : cons <
        typename fn::template apply<empty, typename car<list2>::type>::type,
    typename transform_f::template apply <
        empty,
        typename cdr<list2>::type,
        fn
        >::type
    > {};

    template <typename fn>
    struct apply<empty, empty, fn> : empty {};
};

template <typename list1, typename list2, typename fn>
struct transform : transform_f::template apply<list1, list2, fn> {};

// equal
//
struct equal_f {
    template <typename list1, typename list2, typename pred = lambda<std::is_same>>
    struct apply : std::conditional <
        !pred::template apply <
            typename car<list1>::type,
            typename car<list2>::type
            >::type::value,
        bool_<false>,
        typename equal_f::template apply <
            typename cdr<list1>::type,
            typename cdr<list2>::type,
            pred
        >::type
    > {};

    template <typename list, typename pred>
    struct apply<empty, list, pred> : bool_<false> {};

    template <typename list, typename pred>
    struct apply<list, empty, pred> : bool_<false> {};

    template <typename pred>
    struct apply<empty, empty, pred> : bool_<true> {};
};

template <typename list1, typename list2, typename pred = lambda<std::is_same>>
struct equal : equal_f::template apply<list1, list2, pred> {};

// print
//
template <typename list>
void print()
{
    std::cout << car<list>::value;
    using rest = typename cdr<list>::type;
    if (false == is_empty<rest>::value) {
        std::cout << ", ";
        print<rest>();
    }
}

template <>
void print<empty>()
{
    std::cout << std::endl;
}

int main()
{
    // plus & minus
    //
    std::cout << "\n>plus & minus" << std::endl;
    std::cout << plus<int_<1>, int_<1>>::value << std::endl;    // 2

    // binary
    //
    std::cout << "\n>binary" << std::endl;
    std::cout << binary<101>::value << std::endl;         // 5

    // cons
    //
    std::cout << "\n>cons" << std::endl;
    std::cout << cons<int_<1>, int_<2>>::head::value << std::endl;  // 1
    std::cout << cons<int_<1>, cons<int_<2>, int_<3>>>::tail::tail::value << std::endl; // 3

    // car & cdr
    //
    std::cout << "\n>car & cdr" << std::endl;
    using head = car< cons<int_<1>, int_<2> > >;
    std::cout << head::value << std::endl;

    std::cout << car< cons<int_<1>, int_<2>>>::value << std::endl;
    std::cout << car_f::template apply<cons<int_<1>, int_<2>>>::type::value << std::endl;
    std::cout << cdr<cons<int_<1>, cons<int_<2>, int_<3>>>>::head::value << std::endl;

    // list
    //
    std::cout << "\n>list" << std::endl;
    std::cout << car<list_t<int_<1>, int_<2>, int_<3>>>::value << std::endl;
    std::cout << car<list<int, 1, 2, 3>>::value << std::endl;
    std::cout << (std::is_same<list<int>, empty>::value ? "same" : "defferent") << std::endl;
    print<list<int, 1, 2, 3>>();

    // length
    //
    std::cout << "\n>list length" << std::endl;
    std::cout << "is_empty<empty>: " << is_empty<empty>::value << std::endl;
    std::cout << "is_empty<list<int, 1, 2, 3>>: " << is_empty<list<int, 1, 2, 3>>::value << std::endl;
    std::cout << length<list<int, 1, 2, 3>>::value << std::endl;

    // append
    //
    std::cout << "\n>list append" << std::endl;
    using a1 = list<int, 1, 2, 3>;
    using a2 = list<int, 4, 5, 6, 7>;
    using a3 = append<a1, a2>;
    std::cout << length<a3>::value << std::endl;
    print<a3>();

    // map
    //
    std::cout << "\n>map" << std::endl;
    std::cout << map<inc_f, list<int, 1, 2, 3>>::head::value << std::endl;

    // lambda
    //
    std::cout << "\n>lambda" << std::endl;
    std::cout << lambda<inc>::template apply<int_<0>>::value << std::endl;
    std::cout << map<lambda<inc>, list<int, 1, 2, 3>>::head::value << std::endl;

    // transform
    //
    std::cout << "\n>transform" << std::endl;
    using l1 = list<int, 1, 2, 3>;
    using l2 = list<int, 3, 2, 1>;
    using minus_t = transform<l1, l2, lambda<minus>>; // == list<int, -2, 0, 2>
    using plus_t = transform<l1, l2, lambda<plus>>;   // == list<int, 4, 4, 4>
    std::cout << minus_t::head::value << std::endl;
    std::cout << plus_t::head::value << std::endl;

    return 0;
}
