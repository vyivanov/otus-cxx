#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

namespace {

constexpr uint8_t IP_OCTETS_NUM = 4;

using ip_cnt_t = uint16_t;
using ip_idx_t = uint32_t;

using ip_octets_t = std::array<uint16_t, IP_OCTETS_NUM>;
using ip_record_t = std::pair<ip_octets_t, ip_cnt_t>;
using ip_mapper_t = std::unordered_map<ip_idx_t, ip_record_t>;
using ip_idxrec_t = std::pair<ip_idx_t, ip_record_t>;
using ip_splits_t = std::vector<std::string>;
using ip_idxvec_t = std::vector<ip_idx_t>;
using ip_string_t = std::string;

using stdinp_input_t = std::vector<std::string>;
using stdout_print_t = std::function<bool(const ip_octets_t&)>;

// T(1), S(1)
[[nodiscard]] ip_octets_t ip_into_octets(const ip_string_t& ip_string) noexcept
{
    ip_splits_t ip_splits = {};
    boost::split(ip_splits, ip_string, [](const char symb) { return symb == '.'; });

    ip_octets_t ip_octets = {};
    assert(ip_octets.size() == ip_splits.size());

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
    return (ip_octets.at(0) << 24) +
           (ip_octets.at(1) << 16) +
           (ip_octets.at(2) <<  8) +
           (ip_octets.at(3) <<  0);
}

// T(N), S(N)
[[nodiscard]] ip_mapper_t parse_stdin() noexcept
{
    ip_mapper_t ip_mapper = {};

    for (std::string line; std::getline(std::cin, line);)
    {
        stdinp_input_t stdinp_input = {};
        boost::split(stdinp_input, line, [](const char symb) { return symb == '\t'; });

        const ip_string_t ip_string = stdinp_input.at(0);
        const ip_octets_t ip_octets = ip_into_octets(ip_string);
        const ip_idx_t    ip_idx    = octets_to_idx(ip_octets);

        if (ip_mapper.find(ip_idx) == ip_mapper.end())
        {
            const auto ip_record = ip_record_t{ip_octets, 1};
            ip_mapper.insert({ip_idx, ip_record});
        }
        else
        {
            ip_record_t& ip_record = ip_mapper.at(ip_idx);
            auto& [__, ip_cnt]     = ip_record;
            ip_cnt                 = ip_cnt + 1;
        }
    }

    return ip_mapper;
}

// T(N), S(1)
void print_stdout(const ip_mapper_t& ip_mapper,
                  const ip_idxvec_t& ip_idxvec, const stdout_print_t& is_print) noexcept
{
    for (const ip_idx_t ip_idx: ip_idxvec)
    {
        const ip_record_t& ip_record    = ip_mapper.at(ip_idx);
        const auto& [ip_octets, ip_cnt] = ip_record;

        if (is_print(ip_octets)) {
            for (ip_cnt_t iter{0}; iter < ip_cnt; ++iter) {
                std::cout << ip_from_octets(ip_octets) << std::endl;
            }
        }
    }
}

}

int main()
{
    assert(ip_from_octets(ip_into_octets("127.0.0.1")) == "127.0.0.1");
    assert(octets_to_idx (ip_into_octets("255.255.255.255")) == UINT32_MAX);

    ip_mapper_t const ip_mapper = parse_stdin();
    ip_idxvec_t ip_idxvec(ip_mapper.size());
    assert(ip_idxvec.size() == ip_mapper.size());

    auto ret_idx = [](const ip_idxrec_t& ip_idxrec) -> ip_idx_t {
        const auto [ip_idx, __] = ip_idxrec;
        return ip_idx;
    };

    std::transform(ip_mapper.cbegin(), ip_mapper.cend(), ip_idxvec.begin(), ret_idx);

    // T(N*logN), S(N)
    std::sort(ip_idxvec.begin(), ip_idxvec.end(), std::greater<ip_idx_t>());

    print_stdout(ip_mapper, ip_idxvec, [](const ip_octets_t& ip_octets) -> bool {
        (void) ip_octets;
        return true;
    });

    print_stdout(ip_mapper, ip_idxvec, [](const ip_octets_t& ip_octets) -> bool {
        return ip_octets.at(0) == 1;
    });

    print_stdout(ip_mapper, ip_idxvec, [](const ip_octets_t& ip_octets) -> bool {
        return ip_octets.at(0) == 46 and
               ip_octets.at(1) == 70;
    });

    print_stdout(ip_mapper, ip_idxvec, [](const ip_octets_t& ip_octets) -> bool {
        return ip_octets.at(0) == 46 or
               ip_octets.at(1) == 46 or
               ip_octets.at(2) == 46 or
               ip_octets.at(3) == 46;
    });
}
