#include <cassert>
#include <iostream>
#include <list>
#include <string>
#include <vector>

#include <print-ip.hpp>

int main()
{
    using printer::print_ip;

    const auto a = print_ip(int8_t{-1});
    const auto b = print_ip(int16_t{0});
    const auto c = print_ip(int32_t{2130706433});
    const auto d = print_ip(int64_t{8875824491850138409});
    const auto e = print_ip(std::string{"127.0.0.1"});
    const auto f = print_ip(std::vector<int>{100, 200, 300, 400});
    const auto g = print_ip(std::list  <int>{400, 300, 200, 100});

    assert(a == "255");
    assert(b == "0.0");
    assert(c == "127.0.0.1");
    assert(d == "123.45.67.89.101.112.131.41");
    assert(e == "127.0.0.1");
    assert(f == "100.200.300.400");
    assert(g == "400.300.200.100");

    std::cout << a + '\n'
              << b + '\n'
              << c + '\n'
              << d + '\n'
              << e + '\n'
              << f + '\n'
              << g + '\n';
}
