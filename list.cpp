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
    printf("degree<casting_policy> == %d\n", degree<casting_policy>::value);

    using zero_length = list<>;
    printf("length<zero_length> == %d\n", length<zero_length>::value);
    printf("degree<zero_length> == %d\n", degree<zero_length>::value);
}
