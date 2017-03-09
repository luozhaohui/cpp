
// purpose : simulate list and list's operators in Scheme
// compile : g++ -std=c++11 -o cons cons.cpp
// run     : ./cons
// date    : 2017.03.08

#include <iostream>

// type_
//
template <typename T, T N>
struct type_ {
    using type = type_<T, N>;
    using value_type = T;
    static constexpr T value = N;
};

// int_
//
template <int N>
struct int_ {
    using type = int_<N>;
    using value_type = int;
    static constexpr int value = N;
};

// uint_
//
template <unsigned int N>
struct uint_ {
    using type = uint_<N>;
    using value_type = unsigned int;
    static constexpr unsigned int value = N;
};

template <>
struct uint_<0> {
    using type = uint_<0>;
    using value_type = unsigned int;
    static constexpr unsigned int value = 0;
};

// bool_
template <bool N>
struct bool_ {
    using type = bool_<N>;
    using value_type = bool;
    static constexpr bool value = N;
};

// empty
//
struct empty {
    using type = empty;
    using value = empty;
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

// inc_t
//
struct inc_t {
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
struct car_t {
    template <typename cons>
    struct apply {
        using type = typename cons::type::head;
    };
};

template <typename cons>
struct car : car_t::template apply<cons>::type {};

// cdr
//
struct cdr_t {
    template <typename cons>
    struct apply {
        using type = typename cons::type::tail::type;
    };
};

template <typename cons>
struct cdr : cdr_t::template apply<cons>::type {};

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

// list
//
template <typename T, T ...elements>
struct list : list_t<type_<T, elements>...> {};

// length
//
template <typename list>
struct length_t
{
    static constexpr unsigned int value =
        1 + length_t<typename cdr<list>::type>::value;
};

template <>
struct length_t<empty>
{
    static constexpr unsigned int value = 0;
};

template <typename list>
struct length
{
    static constexpr unsigned int value = length_t<typename list::type>::value;
};

// is_empty
//
template <typename list>
struct is_empty_t
{
    static constexpr bool value = false;
};

template <>
struct is_empty_t<empty>
{
    static constexpr bool value = true;
};

template <typename list>
struct is_empty
{
    static constexpr bool value = is_empty_t<typename list::type>::value;
};

// another implementation
//
// template <typename list>
// struct is_empty
// {
//     static constexpr bool value = (0 == length<list>::value);
// };

// append
//
struct append_t {
    template <typename list1, typename list2>
    struct apply : cons<
        typename car<list1>::type,
        typename append_t::template apply<typename cdr<list1>::type, list2>::type>
    {};

    template<typename list2>
    struct apply <empty, list2>: list2
    {};
};

template <typename list1, typename list2>
struct append : std::conditional <
    is_empty<list1>::value,
    list2,
    append_t::template apply<list1, list2>
    >::type
{};

// reverse
//
struct reverse_t {
    template <typename reset, typename ready>
    struct apply : reverse_t::template apply<
            typename cdr<reset>::type,
            cons<typename car<reset>::type, ready>>
    {};

    template<typename ready>
    struct apply <empty, ready> : ready
    {};
};

template <typename list>
struct reverse : std::conditional <
    is_empty<list>::value,
    list,
    reverse_t::template apply<typename list::type, empty>
    >::type
{};

// map
//
struct map_t {
    template <typename fn, typename list>
    struct apply : cons <
        typename fn::template apply<typename car<list>::type>,
        map_t::template apply<fn, typename cdr<list>::type>
    >{};

    template <typename fn>
    struct apply <fn, empty>: empty{};
};

template <typename fn, typename list>
struct map : std::conditional <
    is_empty<list>::value,
    list,
    map_t::template apply<fn, list>
    >::type
{};

// apply
//
struct apply_t {
    template <template <typename...> class F, typename ...args>
    struct apply : F<typename args::type...> {};

    template <template <typename...> class F>
    struct apply <F, empty> : empty {};
};

template <template <typename...> class F, typename ...args>
struct apply : apply_t::template apply<F, args...> {};

// lambda
//
template <template <typename...> class F>
struct lambda {
    template <typename ...args>
    struct apply : apply_t::template apply<F, args...> {};
};

// transform
//
struct transform_t {
    template <typename list1, typename list2, typename fn>
    struct apply : cons <
        typename fn::template apply<
            typename car<list1>::type, typename car<list2>::type>::type,
        typename transform_t::template apply <
            typename cdr<list1>::type, typename cdr<list2>::type, fn>::type
    > {};

    template <typename list1, typename fn>
    struct apply<list1, empty, fn> : cons <
        typename fn::template apply<typename car<list1>::type, empty>,
        typename transform_t::template apply <typename cdr<list1>::type, empty, fn>::type
    > {};

    template <typename list2, typename fn>
    struct apply<empty, list2, fn> : cons <
        typename fn::template apply<empty, typename car<list2>::type>::type,
        typename transform_t::template apply <empty, typename cdr<list2>::type, fn>::type
    > {};

    template <typename fn>
    struct apply<empty, empty, fn> : empty {};
};

template <typename list1, typename list2, typename fn>
struct transform : std::conditional <
    is_empty<list1>::value,
    list1,
    transform_t::template apply<list1, list2, fn>
    >::type
{
    static_assert(length<list1>::value == length<list2>::value, "transform: length of lists mismatch!");
};

// equal
//
struct equal_t {
    template <typename list1, typename list2, typename pred = lambda<std::is_same>>
    struct apply : std::conditional <
        !pred::template apply <
            typename car<list1>::type,
            typename car<list2>::type
            >::type::value,
        bool_<false>,
        typename equal_t::template apply <
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
struct equal : equal_t::template apply<list1, list2, pred> {};

// print
//
template <typename list, bool is_empty>
struct print_t;

template <typename list>
void print()
{
    print_t<list, is_empty<list>::value> impl;
    impl();
}

template <typename list, bool is_empty = true>
struct print_t
{
    void operator()() {
        std::cout << std::endl;
    }
};

template <typename list>
struct print_t<list, false>
{
    void operator()() {
        std::cout << car<list>::value;
        using rest = typename cdr<list>::type;
        if (false == is_empty<rest>::value) {
            std::cout << ", ";
            print<rest>();
        }
        else {
            std::cout << std::endl;
        }
    }
};

int main()
{
    // plus & minus
    //
    std::cout << "\n>plus & minus" << std::endl;
    std::cout << plus<int_<1>, int_<1>>::value << std::endl;    // 2

    // binary
    //
    std::cout << "\n>binary" << std::endl;
    std::cout << binary<101>::value << std::endl;               // 5

    // cons
    //
    std::cout << "\n>cons" << std::endl;
    std::cout << cons<int_<1>, int_<2>>::head::value << std::endl;  // 1
    std::cout << cons<int_<1>, cons<int_<2>, int_<3>>>::tail::tail::value << std::endl; // 3

    // car & cdr
    //
    std::cout << "\n>car & cdr" << std::endl;
    std::cout << car_t::template apply<cons<int_<1>, int_<2>>>::type::value << std::endl;   // 1
    using c1 = cons<int_<1>, cons<int_<2>, int_<3>>>;
    std::cout << car<c1>::value << ", " << cdr<c1>::head::value << std::endl; // 1, 2

    // list
    //
    using l1 = list<int, 1, 2, 3>;
    using l2 = list<int, 4, 5, 6, 7>;
    using l3 = list_t<int_<1>, int_<2>, int_<3>>;

    std::cout << "\n>list" << std::endl;
    print<l1>();    // 1, 2, 3
    print<l3>();    // 1, 2, 3
    std::cout << car<l1>::value << ", " << cdr<l1>::head::value << std::endl;   // 1, 2

    // length & is_empty
    //
    std::cout << "\n>list length" << std::endl;
    std::cout << "is_empty<empty> : " << is_empty<empty>::value << std::endl;            // 1
    std::cout << "is_empty<list<int>> : " << is_empty<list<int>>::value << std::endl;    // 1
    std::cout << "is_empty<list<int, 1, 2, 3>> : " << is_empty<l1>::value << std::endl;  // 0
    std::cout << "length<empty> : " << length<empty>::value << std::endl;                // 0
    std::cout << "length<list<int>> : " << length<list<int>>::value << std::endl;        // 0
    std::cout << "length<list<int, 1, 2, 3>> : " << length<l1>::value << std::endl;      // 3

    // reverse
    //
    std::cout << "\n>reverse" << std::endl;
    using r1 = reverse<l1>;
    using r2 = reverse<list<int>>;
    print<r1>();    // 3, 2, 1
    print<r2>();

    // append
    //
    std::cout << "\n>list append" << std::endl;
    using a1 = append<l1, l2>;
    using a2 = append<l1, list<int>>;
    using a3 = append<list<int>, l1>;
    print<a1>();    // 1, 2, 3, 4, 5, 6, 7
    print<a2>();    // 1, 2, 3
    print<a3>();    // 1, 2, 3

    // map
    //
    std::cout << "\n>map" << std::endl;
    using m1 = map<inc_t, list<int, 1, 2, 3>>;
    using m2 = map<inc_t, list<int>>;
    print<m1>();    // 2, 3, 4
    print<m2>();

    // lambda
    //
    std::cout << "\n>lambda" << std::endl;
    std::cout << lambda<inc>::template apply<int_<0>>::value << std::endl;  // 1
    using ml1 = map<lambda<inc>, list<int, 1, 2, 3>>;
    print<ml1>();  // 2, 3, 4

    // transform
    //
    std::cout << "\n>transform" << std::endl;
    using t1 = list<int, 1, 2, 3>;
    using t2 = list<int, 3, 2, 1>;
    using ml = transform<t1, t2, lambda<minus>>;
    using pl = transform<t1, t2, lambda<plus>>;
    using te = transform<list<int>, list<int>, lambda<plus>>;
    using el = transform<t1, list<int>, lambda<plus>>;
    print<ml>();    // -2, 0, 2
    print<pl>();    // 4, 4, 4
    print<te>();
    // print<el>(); // assertion: length mismatch!

    return 0;
}
