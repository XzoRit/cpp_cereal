#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/vector.hpp>

#include <boost/test/unit_test.hpp>

#include <memory>
#include <ostream>
#include <sstream>
#include <tuple>
#include <utility>
#include <vector>

namespace
{
struct simple_data
{
    simple_data() = default;
    simple_data(int aa, int bb, float cc)
        : a{aa}
        , b{bb}
        , c{cc}
    {
    }

    template <class Archive>
    void serialize(Archive& ar)
    {
        ar(cereal::make_nvp("a", a), cereal::make_nvp("b", b), cereal::make_nvp("c", c));
    }

    bool operator==(const simple_data&) const = default;

    int a{};
    int b{};
    float c{};
};

std::ostream& operator<<(std::ostream& str, const simple_data& a)
{
    str << "a=" << a.a << " b=" << a.b << " c=" << a.c;

    return str;
}

struct complex_data
{
    using data_type = std::vector<std::pair<int, simple_data>>;

    complex_data() = default;
    complex_data(data_type d)
        : data{std::move(d)}
    {
    }

    template <class Archive>
    void save(Archive& ar) const
    {
        ar(cereal::make_nvp("data", data));
    }

    template <class Archive>
    void load(Archive& ar)
    {
        ar(data);
    }

    bool operator==(const complex_data&) const = default;

    data_type data{};
};

std::ostream& operator<<(std::ostream& str, const complex_data& a)
{
    for (const auto& [i, d] : a.data)
    {
        str << '(' << i << ',' << d << ')';
    }

    return str;
}

BOOST_AUTO_TEST_SUITE(lib_tests)

BOOST_AUTO_TEST_CASE(load_save_simple_data)
{
    std::stringstream ss{};
    const simple_data a{1, 22, 33.0};
    {
        cereal::JSONOutputArchive archive{ss};

        archive(cereal::make_nvp("simple_data", a));
    }

    BOOST_REQUIRE(!ss.str().empty());
    BOOST_CHECK_EQUAL(ss.str(), R"({
    "simple_data": {
        "a": 1,
        "b": 22,
        "c": 33.0
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
    const complex_data a{{{0, {0, 0, 0.}}, {1, {1, 1, 1.}}, {2, {2, 2, 2.}}}};

    {
        cereal::JSONOutputArchive archive{ss};

        archive(cereal::make_nvp("complex_data", a));
    }

    BOOST_REQUIRE(!ss.str().empty());
    BOOST_CHECK_EQUAL(ss.str(), R"({
    "complex_data": {
        "data": [
            {
                "first": 0,
                "second": {
                    "a": 0,
                    "b": 0,
                    "c": 0.0
                }
            },
            {
                "first": 1,
                "second": {
                    "a": 1,
                    "b": 1,
                    "c": 1.0
                }
            },
            {
                "first": 2,
                "second": {
                    "a": 2,
                    "b": 2,
                    "c": 2.0
                }
            }
        ]
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
