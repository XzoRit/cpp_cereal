#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>

#include <boost/algorithm/hex.hpp>
#include <boost/program_options.hpp>

#include <algorithm>
#include <cctype>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>

namespace po = boost::program_options;

namespace
{
bool only_printable(const std::string& txt)
{
    return std::all_of(txt.cbegin(), txt.cend(), [](unsigned char c) { return std::isprint(c) || std::isspace(c); });
}

std::string to_hex(const std::string& txt)
{
    std::string a{};
    boost::algorithm::hex(txt.cbegin(), txt.cend(), std::back_inserter(a));

    return a;
}

std::string non_ascii_to_hex(const std::string& txt)
{
    if (only_printable(txt))
        return txt;

    return to_hex(txt);
}

template <class Ar>
std::string to_string(const std::string& txt)
{
    std::ostringstream oss{};
    {
        Ar ar{oss};

        ar(cereal::make_nvp("input", txt));
    }

    return non_ascii_to_hex(oss.str());
}

void print_serialized(const std::string& txt)
{
    std::cout << "JSONOutputArchive:\n" << to_string<cereal::JSONOutputArchive>(txt) << '\n';
    std::cout << "XMLOutputArchive:\n" << to_string<cereal::XMLOutputArchive>(txt) << '\n';
    std::cout << "BinaryOutputArchive:\n" << to_string<cereal::BinaryOutputArchive>(txt) << '\n';
    std::cout << "PortableBinaryOutputArchive:\n" << to_string<cereal::PortableBinaryOutputArchive>(txt) << '\n';
}
} // namespace
int main(int ac, char* av[])
{
    std::cout << "Hello app\n";

    try
    {
        po::options_description desc("Allowed options");
        desc.add_options()("help", "help message")("interactive", "serializes input in diffenrent formats");

        po::variables_map vm;
        po::store(po::parse_command_line(ac, av, desc), vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            std::cout << desc << "\n";
        }

        if (vm.count("interactive"))
        {
            std::string txt{};
            std::getline(std::cin, txt);

            print_serialized(txt);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch (...)
    {
        std::cerr << "Exception of unknown type!\n";
    }

    return 0;
}
