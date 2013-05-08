#include "num.hpp"
#include "lambda2.hpp"
#include "list.hpp"
#include "type_name.hpp"
#include "polynomial.hpp"

#include <cstdio>
#include <limits>
#include <iostream>

//////////////////////////////////////////////////////////////////////////////

template <typename Casts, typename T>
void write (T t) {
    using to_type = at<Casts, T, T>;

    printf("static_cast<%s>(%s)\n", type_name<to_type>().c_str(), type_name<T>().c_str());

    using lim_to = std::numeric_limits<to_type>;

    if (std::is_integral<to_type>::value
            && std::is_integral<T>::value
            && !std::is_same<bool, to_type>::value
            && !std::is_same<bool, T>::value
            && !std::is_same<T, to_type>::value) {
        if (t < lim_to::lowest()) {
            printf("** t < std::numeric_limits<%s>::lowest\n", type_name<to_type>().c_str());
        }
        else if (t > lim_to::max()) {
            printf("** t > std::numeric_limits<%s>::max\n", type_name<to_type>().c_str());
        }
    }

    std::cout << static_cast<to_type>(t) << " << " << t << '\n';
}

template <typename A, typename B>
struct plus {
    using type = std::ratio_add<A, B>;
};

template <typename A, typename B, typename C>
struct foo {
    using type = typename lambda<plus<A, _1>>::type;
};

int main () {
    using v = list<int, char, double>;

    {
        using h0 = head<v>;
        using t0 = tail<v>;
        using h1 = head<t0>;
        using t1 = tail<t0>;
        using h2 = head<t1>;
        using t2 = tail<t1>;

        printf("v = %s\n", type_name<v>().c_str());
        printf("%s : %s\n", type_name<h0>().c_str(), type_name<t0>().c_str());
        printf("%s : %s\n", type_name<h1>().c_str(), type_name<t1>().c_str());
        printf("%s : %s\n", type_name<h2>().c_str(), type_name<t2>().c_str());

        printf("%d\n", length<v>::value);

        printf("%d ints\n", count_if<v, equals<int>::func>::value);
        printf("%d char\n", count_if<v, equals<char>::func>::value);
        printf("%d doubles\n", count_if<v, equals<double>::func>::value);
        printf("%d bools\n", count_if<v, equals<bool>::func>::value);
    }

    using vv = cons<void, v>;

    {
        using h0 = head<vv>;
        using t0 = tail<vv>;
        using h1 = head<t0>;
        using t1 = tail<t0>;
        using h2 = head<t1>;
        using t2 = tail<t1>;
        using h3 = head<t2>;
        using t3 = tail<t2>;

        printf("vv = %s\n", type_name<vv>().c_str());
        printf("%s : %s\n", type_name<h0>().c_str(), type_name<t0>().c_str());
        printf("%s : %s\n", type_name<h1>().c_str(), type_name<t1>().c_str());
        printf("%s : %s\n", type_name<h2>().c_str(), type_name<t2>().c_str());
        printf("%s : %s\n", type_name<h3>().c_str(), type_name<t3>().c_str());

        printf("%d\n", length<vv>::value);

        printf("%d ints\n", count_if<vv, equals<int>::func>::value);
        printf("%d char\n", count_if<vv, equals<char>::func>::value);
        printf("%d doubles\n", count_if<vv, equals<double>::func>::value);
        printf("%d bools\n", count_if<vv, equals<bool>::func>::value);

        printf("map<is_void, vv> = %s\n", type_name<map<std::is_void, vv>>().c_str());

        using ts = tails<vv>;

        using tail0 = head<ts>;
        using tail1 = head<tail<ts>>;
        using tail2 = head<tail<tail<ts>>>;
        using tail3 = head<tail<tail<tail<ts>>>>;
        using tail4 = head<tail<tail<tail<tail<ts>>>>>;

        printf("tail0 = %s\n", type_name<tail0>().c_str());
        printf("tail1 = %s\n", type_name<tail1>().c_str());
        printf("tail2 = %s\n", type_name<tail2>().c_str());
        printf("tail3 = %s\n", type_name<tail3>().c_str());
        printf("tail4 = %s\n", type_name<tail4>().c_str());

        using is = inits<vv>;

        using init0 = head<is>;
        using init1 = head<tail<is>>;
        using init2 = head<tail<tail<is>>>;
        using init3 = head<tail<tail<tail<is>>>>;
        using init4 = head<tail<tail<tail<tail<is>>>>>;

        printf("init0 = %s\n", type_name<init0>().c_str());
        printf("init1 = %s\n", type_name<init1>().c_str());
        printf("init2 = %s\n", type_name<init2>().c_str());
        printf("init3 = %s\n", type_name<init3>().c_str());
        printf("init4 = %s\n", type_name<init4>().c_str());
    }

    using vvv = append<v, vv>;

    printf("vvv = %s\n", type_name<vvv>().c_str());
    printf("reverse<vvv> = %s\n", type_name<reverse<vvv>>().c_str());
    printf("head<vvv> = %s\n", type_name<head<vvv>>().c_str());
    printf("last<vvv> = %s\n", type_name<last<vvv>>().c_str());
    printf("tail<vvv> = %s\n", type_name<tail<vvv>>().c_str());
    printf("init<vvv> = %s\n", type_name<init<vvv>>().c_str());

    using m = list<pair<bool, char>, pair<wchar_t, char16_t>, pair<char32_t, signed char>>;

    static_assert(is_association_list<m>::value, "m is not an association list");

    using mm = cons<void, m>;

    //static_assert(is_association_list<mm>::value, "mm is not an association list");

    printf("m = %s\n", type_name<m>().c_str());
    printf("%d bool keys\n", count_if<m, key_equals<bool>::func>::value);
    printf("%d wchar_t keys\n", count_if<m, key_equals<wchar_t>::func>::value);
    printf("%d char32_t keys\n", count_if<m, key_equals<char32_t>::func>::value);
    printf("%d unsigned char keys\n", count_if<m, key_equals<unsigned char>::func>::value);

    printf("m[bool] ==\t%s\n", type_name<at<m, bool>>().c_str());
    printf("m[wchar_t] ==\t%s\n", type_name<at<m, wchar_t>>().c_str());
    printf("m[char32_t] ==\t%s\n", type_name<at<m, char32_t>>().c_str());

    using casting_policy = list< pair<bool, char>
                                   , pair<int, int16_t>
                                   , pair<unsigned int, uint16_t>
                                   , pair<double, float>>;

    write<casting_policy>(true);
    write<casting_policy>('d');
    write<casting_policy>(1000000);
    write<casting_policy>(1U);
    write<casting_policy>(1000000L);
    write<casting_policy>(3.14159678243234);

    printf("length<casting_policy> == %d\n", length<casting_policy>::value);
    printf("degree<casting_policy> == %d\n", polynomial::degree<casting_policy>::value);

    using zero_length = list<>;
    printf("length<zero_length> == %d\n", length<zero_length>::value);
    printf("degree<zero_length> == %d\n", polynomial::degree<zero_length>::value);

    using p0 = list<std::ratio<1>, std::ratio<2>, std::ratio<3>>;
    using p1 = reverse<p0>;
    printf("p0 + p1 = %s\n", type_name<polynomial::add<p0, p1>>().c_str());

    using p2 = list<std::ratio<3, 2>>;
    using p3 = list<std::ratio<1, 2>, std::ratio<9>, std::ratio<79>, std::ratio<123,123>, std::ratio<0>, std::ratio<-1>>;

    printf("p2 + p3 = %s\n", type_name<polynomial::add<p2, p3>>().c_str());
    printf("p3 + p2 = %s\n", type_name<polynomial::add<p3, p2>>().c_str());
    
    using p4 = list<std::ratio<-42>, std::ratio<0>, std::ratio<-12>, std::ratio<1>>;
    using p5 = list<std::ratio<-3>, std::ratio<1>>;
    using p6 = list<std::ratio<-3>, std::ratio<1>, std::ratio<1>>;
    using p7 = list<std::ratio<-7>, std::ratio<0>, std::ratio<5>, std::ratio<6>>;
    using p8 = list<std::ratio<-1>, std::ratio<-2>, std::ratio<3>>;

    printf("p4 = %s\n", type_name<p4>().c_str());
    printf("p5 = %s\n", type_name<p5>().c_str());
    printf("p6 = %s\n", type_name<p6>().c_str());
    printf("p7 = %s\n", type_name<p7>().c_str());
    printf("p8 = %s\n", type_name<p8>().c_str());

    printf("p4 * p5 == %s\n", type_name<polynomial::multiply<p4, p5>>().c_str());
    printf("p4 * p6 == %s\n", type_name<polynomial::multiply<p4, p6>>().c_str());
    printf("p4 * p7 == %s\n", type_name<polynomial::multiply<p4, p7>>().c_str());

    printf("replicate<std::integral_constant<3>, std::ratio<1,2>> = %s\n", type_name<replicate<std::integral_constant<unsigned int, 3>, std::ratio<1,2>>>().c_str());

    {
        using qr = polynomial::quo_rem<p4, p5>;
        printf("p4 / p5 == %s\n", type_name<qr>().c_str());
    }

    {
        using qr = polynomial::quo_rem<p4, p6>;
        printf("p4 / p6 == %s\n", type_name<qr>().c_str());
    }

    {
        using qr = polynomial::quo_rem<p7, p8>;
        printf("p7 / p8 == %s\n", type_name<qr>().c_str());
    }

    printf("p4(0) = %s\n", type_name<polynomial::evaluate<p4, std::ratio<0>>>().c_str());
    printf("p4(1/2) = %s\n", type_name<polynomial::evaluate<p4, std::ratio<1,2>>>().c_str());
    printf("p4(1) = %s\n", type_name<polynomial::evaluate<p4, std::ratio<1>>>().c_str());
    printf("p4(3/2) = %s\n", type_name<polynomial::evaluate<p4, std::ratio<3,2>>>().c_str());

#ifdef TYPE_CONTAINERS_LAMBDA_HPP
    using f = lambda<foo<_2, wchar_t, _1>>;

    printf("apply<f, void, int> == %s\n", type_name<apply<f, void, int>>().c_str());

    using plus_one = lambda<plus<_1, std::ratio<1>>>;

    printf("apply<plus_one, std::ratio<2>>::type == %s\n",
            type_name<typename apply<plus_one, std::ratio<2>>::type>().c_str());

#elif defined(TYPE_CONTAINERS_LAMBDA2_HPP)
    using f = typename lambda<foo<_2, wchar_t, _1>>::type;

    printf("      f             == %s\n", type_name<f>().c_str());
    printf("apply<f>            == %s\n", type_name<typename apply<f>::type>().c_str());
    printf("apply<f, void>      == %s\n", type_name<typename apply<f, void>::type>().c_str());
    printf("apply<f, void, std::ratio<2>>                == %s\n", type_name<typename apply<f, void, std::ratio<2>>::type>().c_str());
    printf("apply<f, void, std::ratio<2>, std::ratio<3>> == %s\n", type_name<typename apply<f, void, std::ratio<2>, std::ratio<3>>::type>().c_str());
    printf("apply<apply<f, void, std::ratio<2>>::type, std::ratio<3>> == %s\n", type_name<typename apply<typename apply<f, void, std::ratio<2>>::type, std::ratio<3>>::type>().c_str());
#endif

    using i5 = int_<5>;
    using im5 = typename num::negate<i5>::type;
    using i4 = int_<4>;
    using im4 = typename num::negate<i4>::type;
    using i0 = int_<0>;

    printf("i5  = %s\n", type_name<i5>().c_str());
    printf("im5 = %s\n", type_name<im5>().c_str());
    printf("i4  = %s\n", type_name<i4>().c_str());
    printf("im4 = %s\n", type_name<im4>().c_str());

    printf("i5  + i0  = %s\n", type_name<typename num::add<i5, i0>::type>().c_str());
    printf("i5  + i4  = %s\n", type_name<typename num::add<i5, i4>::type>().c_str());
    printf("im5 + i4  = %s\n", type_name<typename num::add<im5, i4>::type>().c_str());
    printf("i5  + im4 = %s\n", type_name<typename num::add<i5, im4>::type>().c_str());
    printf("im5 + im4 = %s\n", type_name<typename num::add<im5, im4>::type>().c_str());

    printf("i5  * i0  = %s\n", type_name<typename num::add<i5, i0>::type>().c_str());
    printf("i5  * i4  = %s\n", type_name<typename num::multiply<i5, i4>::type>().c_str());
    printf("im5 * i4  = %s\n", type_name<typename num::multiply<im5, i4>::type>().c_str());
    printf("i5  * im4 = %s\n", type_name<typename num::multiply<i5, im4>::type>().c_str());
    printf("im5 * im4 = %s\n", type_name<typename num::multiply<im5, im4>::type>().c_str());

    printf("i5  - i0  = %s\n", type_name<typename num::add<i5, i0>::type>().c_str());
    printf("i5  - i4  = %s\n", type_name<typename num::subtract<i5, i4>::type>().c_str());
    printf("im5 - i4  = %s\n", type_name<typename num::subtract<im5, i4>::type>().c_str());
    printf("i5  - im4 = %s\n", type_name<typename num::subtract<i5, im4>::type>().c_str());
    printf("im5 - im4 = %s\n", type_name<typename num::subtract<im5, im4>::type>().c_str());

    printf("abs(i0)  = %s\n", type_name<typename num::abs<i0>::type>().c_str());
    printf("abs(i5)  = %s\n", type_name<typename num::abs<i5>::type>().c_str());
    printf("abs(im5) = %s\n", type_name<typename num::abs<i5>::type>().c_str());

    printf("signum(i0)  = %s\n", type_name<typename num::signum<i0>::type>().c_str());
    printf("signum(i5)  = %s\n", type_name<typename num::signum<i5>::type>().c_str());
    printf("signum(im5) = %s\n", type_name<typename num::signum<im5>::type>().c_str());

    printf("i5 == i5 = %s\n", type_name<typename eq::eq<i5, i5>::type>().c_str());
    printf("i5 == i0 = %s\n", type_name<typename eq::eq<i5, i0>::type>().c_str());
    printf("i5 != i5 = %s\n", type_name<typename eq::neq<i5, i5>::type>().c_str());
    printf("i5 != i0 = %s\n", type_name<typename eq::neq<i5, i0>::type>().c_str());

    printf("quot(i5, i4)   = %s\n", type_name<typename integral::quot<i5, i4>::type>().c_str());
    printf("quot(im5, i4)  = %s\n", type_name<typename integral::quot<im5, i4>::type>().c_str());
    printf("quot(i5, im4)  = %s\n", type_name<typename integral::quot<i5, im4>::type>().c_str());
    printf("quot(im5, im4) = %s\n", type_name<typename integral::quot<im5, im4>::type>().c_str());

    printf("rem(i5, i4)   = %s\n", type_name<typename integral::rem<i5, i4>::type>().c_str());
    printf("rem(im5, i4)  = %s\n", type_name<typename integral::rem<im5, i4>::type>().c_str());
    printf("rem(i5, im4)  = %s\n", type_name<typename integral::rem<i5, im4>::type>().c_str());
    printf("rem(im5, im4) = %s\n", type_name<typename integral::rem<im5, im4>::type>().c_str());

    printf("div(i5, i4)   = %s\n", type_name<typename integral::div<i5, i4>::type>().c_str());
#if 0
    printf("div(im5, i4)  = %s\n", type_name<typename integral::div<im5, i4>::type>().c_str());
    printf("div(i5, im4)  = %s\n", type_name<typename integral::div<i5, im4>::type>().c_str());
    printf("div(im5, im4) = %s\n", type_name<typename integral::div<im5, im4>::type>().c_str());

    printf("mod(i5, i4)   = %s\n", type_name<typename integral::mod<i5, i4>::type>().c_str());
    printf("mod(im5, i4)  = %s\n", type_name<typename integral::mod<im5, i4>::type>().c_str());
    printf("mod(i5, im4)  = %s\n", type_name<typename integral::mod<i5, im4>::type>().c_str());
    printf("mod(im5, im4) = %s\n", type_name<typename integral::mod<im5, im4>::type>().c_str());
#endif
}
