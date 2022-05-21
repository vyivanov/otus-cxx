#include <cassert>
#include <list>
#include <string>
#include <vector>

#include <print-ip.hpp>

int main()
{
    using printer::print_ip;

    assert(print_ip(int8_t{-1})                           == "255");
    assert(print_ip(int16_t{0})                           == "0.0");
    assert(print_ip(int32_t{2130706433})                  == "127.0.0.1");
    assert(print_ip(int64_t{8875824491850138409})         == "123.45.67.89.101.112.131.41");
    assert(print_ip(std::string{"127.0.0.1"})             == "127.0.0.1");
    assert(print_ip(std::vector<int>{100, 200, 300, 400}) == "100.200.300.400");
    assert(print_ip(std::list  <int>{400, 300, 200, 100}) == "400.300.200.100");
}
