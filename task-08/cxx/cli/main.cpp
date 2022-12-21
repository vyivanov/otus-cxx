#include <boost/program_options.hpp>

#include <core/inc/data-hasher/boost-data-hasher.hpp>
#include <core/inc/dup-detector/dup-detector-impl.hpp>
#include <core/inc/item-reader/local-file-reader.hpp>

#include <cassert>
#include <iostream>

struct config_t {
    using optional = std::optional<config_t>;

    enum struct hash_type_t {
        MURMUR_HASH
    };

    ddt::DataSize chunk_size;
    ddt::FileList file_list;
    hash_type_t hash_type;
};

namespace po = boost::program_options;
auto convert(std::string_view hash_func) -> config_t::hash_type_t;

auto transform(const po::variables_map& inputs) -> config_t::optional {
    if (inputs.count("help")) {
        return {};
    }

    const auto chunk_size = inputs["chunk-size"].as<std::size_t>();
    const auto hash_type = convert(inputs["hash-func"].as<std::string>());

    return config_t {
        .chunk_size = chunk_size,
        .hash_type = hash_type,
    };
}

auto convert(const std::string_view hash_func) -> config_t::hash_type_t {
    if (hash_func == "MurmurHash") {
        return config_t::hash_type_t::MURMUR_HASH;
    }

    throw std::invalid_argument{""};
}

int main(int argc, char* argv[])
{
    auto header =
        "Tool to find file duplicates by it's content\n\n"
        "Usage:\n"
        "\x20\x20" + std::string{argv[0]} + '\x20' + "[OPTIONS] dir_0 ... dir_N\n\n"
        "Options";

    auto opts = po::options_description{header};
    opts.add_options()
        ("help,h",      "print this message")
        ("recursive,r", "check directories recursively")
        ("chunk-size,s", po::value<std::size_t>()->default_value(1024, "1kB"), "chunk the file to be split on")
        ("hash-func,f", po::value<std::string>()->default_value("MurmurHash"), "hash to be applied to chunk");

    auto dirs = po::options_description{};
    dirs.add_options()
        ("dirs", po::value<std::vector<std::string>>()->required());

    auto cmdline = po::options_description{};
    cmdline.add(opts).add(dirs);

    auto pose = po::positional_options_description{};
    pose.add("dirs", -1);

    auto parser = po::command_line_parser(argc, argv).options(cmdline).positional(pose).run();
    auto inputs = po::variables_map{};

    po::store(parser, inputs);

    if (inputs.count("help"))
    {
        std::cout << opts;
        return 0;
    }

    for (const auto& root: inputs["dirs"].as<std::vector<std::string>>())
    {
        auto it = std::filesystem::recursive_directory_iterator(std::filesystem::path(root));
        for (const auto& entry: it)
        {
            std::cout << std::filesystem::absolute(entry.path()) << '\n';
        }
        std::cout << '\n';
    }

    const auto files = ddt::FileList {
        "/home/vivanov/Dev/git-svn-test/ws-svn/video-2.mkv",
        "/home/vivanov/Dev/git-svn-test/ws-svn/test.txt",
        "/home/vivanov/Dev/git-svn-test/ws-svn/two.json",
        "/home/vivanov/Dev/git-svn-test/ws-svn/empty.txt",
        "/home/vivanov/Dev/git-svn-test/ws-svn/video.mkv",
        "/home/vivanov/Dev/git-svn-test/ws-svn/two-2.json",
    };

    auto reader = ddt::LocalFileReader::create(files, ddt::DataSize{1024});
    auto hasher = ddt::BoostDataHasher::create();

    auto dups = ddt::DupDetectorImpl::create(reader, hasher);
    dups->detect();

    assert(dups->result()->size() == 4);

    for (const auto& group: dups->result().value())
    {
        for (const auto& idx: group)
        {
            std::cout << files.at(idx) << '\n';
        }
        std::cout << '\n';
    }
}
