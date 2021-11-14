#include "cereal/cereal.hpp"
#include <cereal/archives/json.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/unordered_map.hpp>

#include <boost/test/unit_test.hpp>

#include <memory>
#include <ostream>
#include <sstream>
#include <tuple>
#include <unordered_map>

namespace
{
struct simple_data
{
    template <class Archive>
    void serialize(Archive& ar)
    {
        ar(cereal::make_nvp("x", x), cereal::make_nvp("y", y), cereal::make_nvp("z", z));
    }

    uint8_t x{};
    uint8_t y{};
    float z{};
};

bool operator==(const simple_data& a, const simple_data& b) noexcept
{
    return std::tie(a.x, a.y, a.z) == std::tie(b.x, b.y, b.z);
}

bool operator!=(const simple_data& a, const simple_data& b) noexcept
{
    return !(a == b);
}

std::ostream& operator<<(std::ostream& str, const simple_data& a)
{
    str << "x=" << a.x << " y=" << a.y << " z=" << a.z;

    return str;
}

struct complex_data
{
    template <class Archive>
    void save(Archive& ar) const
    {
        ar(cereal::make_nvp("data", data));
    }

    template <class Archive>
    void load(Archive& ar)
    {
        static int32_t idGen = 0;
        id = idGen++;
        ar(data);
    }

    int32_t id{};
    std::shared_ptr<std::unordered_map<uint32_t, simple_data>> data{};
};

bool operator==(const complex_data& a, const complex_data& b) noexcept
{
    return std::tie(a.id, a.data) == std::tie(b.id, b.data);
}

bool operator!=(const complex_data& a, const complex_data& b) noexcept
{
    return !(a == b);
}

std::ostream& operator<<(std::ostream& str, const complex_data& a)
{
    str << "id=" << a.id << '{';
    if (a.data)
    {
        for (const auto& [i, d] : (*a.data))
        {
            str << '(' << i << ',' << d << ')';
        }
    }
    str << '}';

    return str;
}

BOOST_AUTO_TEST_SUITE(lib_tests)

BOOST_AUTO_TEST_CASE(load_save_simple_data)
{
    std::stringstream ss{};
    const simple_data a{};
    {
        cereal::JSONOutputArchive archive{ss};

        archive(cereal::make_nvp("simple_data", a));
    }
    BOOST_CHECK(!ss.str().empty());
    BOOST_CHECK_EQUAL(ss.str(), R"({
    "simple_data": {
        "x": 0,
        "y": 0,
        "z": 0.0
    }
})");
    {
        cereal::JSONInputArchive iarchive(ss);

        simple_data b{};
        iarchive(b);

        BOOST_CHECK_EQUAL(a, b);
    }
}

BOOST_AUTO_TEST_CASE(load_save_complex_data)
{
    std::stringstream ss{};
    const complex_data a{};
    {
        cereal::JSONOutputArchive archive{ss};

        archive(cereal::make_nvp("complex_data", a));
    }
    BOOST_CHECK(!ss.str().empty());
    BOOST_CHECK_EQUAL(ss.str(), R"({
    "complex_data": {
        "data": {
            "ptr_wrapper": {
                "id": 0
            }
        }
    }
})");
    {
        cereal::JSONInputArchive iarchive(ss);

        complex_data b{};
        iarchive(b);

        BOOST_CHECK_EQUAL(a, b);
    }
}

BOOST_AUTO_TEST_SUITE_END()
} // namespace
