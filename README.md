
C++ code snippet
=============

1. [cons.cpp](https://github.com/luozhaohui/cpp/blob/master/cons.cpp)  
    基于<code>meta programming</code>模拟<code>Scheme</code>中的<code>list</code>以及相关操作，如<code>cons</code>、car、cdr、length、is_empty、reverse、equal、map、transform、enumerate、print等.    
    
    文档说明：[基于C++11模板元编程实现Scheme中的list及相关函数式编程接口](http://blog.csdn.net/kesalin/article/details/62229505)
    使用示例:  
    
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
    
        // equal
        //
        std::cout << "\n>equal" << std::endl;
        using e1 = list<int, 1, 2, 3>;
        using e2 = list<int, 1, 2, 3>;
        using e3 = list<int, 1, 2, 1>;
        std::cout << "equal<e1, e2> : " << equal<e1, e2>::value << std::endl;   // 1
        std::cout << "equal<e1, e3> : " << equal<e1, e3>::value << std::endl;   // 0
        std::cout << "equal<e1, list<int>> : " << equal<e1, list<int>>::value << std::endl; // 0
        std::cout << "equal<list<int>, e1> : " << equal<list<int>, e1>::value << std::endl; // 0
        std::cout << "equal<list<int>, list<int>> : " << equal<list<int>, list<int>>::value << std::endl;   // 1
    
        // enumerate
        //
        std::cout << "\n>enumerate" << std::endl;
        using value_type = typename car<e1>::value_type;
        auto sqr_print = [](value_type val) { std::cout << val * val << " "; };
        enumerate<decltype(sqr_print), e1>(sqr_print);      // 1 4 9
    
        std::cout << "\n\n>> end <<" << std::endl;
    
