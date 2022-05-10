#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

namespace {

constexpr uint8_t IP_OCTETS_NUM = 4U;

using ip_idx_t = uint32_t;

using ip_octets_t = std::array<uint16_t, IP_OCTETS_NUM>;
using ip_mapper_t = std::multimap<ip_idx_t, ip_octets_t, std::greater<ip_idx_t>>;
using ip_splits_t = std::vector<std::string>;
using ip_string_t = std::string;

using stdinp_split_t = std::vector<std::string>;
using stdout_print_t = std::function<bool(const ip_octets_t&)>;

// T(1), S(1)
[[nodiscard]] ip_octets_t ip_into_octets(const ip_string_t& ip_string) noexcept
{
    ip_splits_t ip_splits = {};
    ip_octets_t ip_octets = {};

    boost::split(ip_splits, ip_string, [](const char symb) { return symb == '.'; });
    assert(ip_splits.size() == ip_octets.size());

    auto str_to_int = [](const std::string& octet) -> uint16_t {
        return boost::lexical_cast<uint16_t>(octet);
    };

    std::transform(ip_splits.cbegin(), ip_splits.cend(), ip_octets.begin(), str_to_int);

    return ip_octets;
}

// T(1), S(1)
[[nodiscard]] ip_string_t ip_from_octets(const ip_octets_t& ip_octets) noexcept
{
    assert(ip_octets.size() == IP_OCTETS_NUM);

    const auto ip_string = boost::format("%1%.%2%.%3%.%4%")
        % ip_octets.at(0)
        % ip_octets.at(1)
        % ip_octets.at(2)
        % ip_octets.at(3);

    return ip_string.str();
}

// T(1), S(1)
[[nodiscard]] ip_idx_t octets_to_idx(const ip_octets_t& ip_octets) noexcept
{
    assert(ip_octets.size() == IP_OCTETS_NUM);

    const ip_idx_t ip_idx = (ip_octets.at(0) << 24) +
                            (ip_octets.at(1) << 16) +
                            (ip_octets.at(2) <<  8) +
                            (ip_octets.at(3) <<  0);

    return ip_idx;
}

// T(N*logN), S(N)
[[nodiscard]] ip_mapper_t parse_stdin() noexcept
{
    ip_mapper_t    ip_mapper    = {};
    stdinp_split_t stdinp_split = {};

    for (std::string line; std::getline(std::cin, line);)
    {
        boost::split(stdinp_split, line, [](const char symb) { return symb == '\t'; });
        assert(stdinp_split.size() == 3);

        const ip_string_t ip_string = stdinp_split.at(0);
        const ip_octets_t ip_octets = ip_into_octets(ip_string);
        const ip_idx_t    ip_idx    = octets_to_idx (ip_octets);

        ip_mapper.emplace(std::make_pair(ip_idx, ip_octets));
    }

    return ip_mapper;
}

// T(N), S(1)
void print_stdout(const ip_mapper_t& ip_mapper, const stdout_print_t& is_print) noexcept
{
    for (const auto& [__, ip_octets]: ip_mapper)
    {
        if (is_print(ip_octets))
        {
            std::cout << ip_from_octets(ip_octets) << std::endl;
        }
    }
}

}

int main()
{
    assert(ip_from_octets(ip_into_octets("127.0.0.1")) == "127.0.0.1");
    assert(octets_to_idx (ip_into_octets("255.255.255.255")) == UINT32_MAX);

    const ip_mapper_t ip_mapper = parse_stdin();

    print_stdout(ip_mapper, [](const ip_octets_t& ip_octets) -> bool {
        (void) ip_octets;
        return true;
    });

    print_stdout(ip_mapper, [](const ip_octets_t& ip_octets) -> bool {
        return ip_octets.at(0) == 1;
    });

    print_stdout(ip_mapper, [](const ip_octets_t& ip_octets) -> bool {
        return ip_octets.at(0) == 46 and
               ip_octets.at(1) == 70;
    });

    print_stdout(ip_mapper, [](const ip_octets_t& ip_octets) -> bool {
        return ip_octets.at(0) == 46 or
               ip_octets.at(1) == 46 or
               ip_octets.at(2) == 46 or
               ip_octets.at(3) == 46;
    });
}
