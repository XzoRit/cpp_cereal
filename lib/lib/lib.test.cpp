#include <boost/utility/identity_type.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/vector.hpp>

#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/utility/identity_type.hpp>

#include <libs/test/include/boost/test/tools/old/interface.hpp>
#include <libs/test/include/boost/test/unit_test_log.hpp>
#include <ostream>
#include <sstream>
#include <tuple>
#include <utility>
#include <vector>

namespace v1
{
struct simple_data
{
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
} // namespace v1

namespace v2
{
struct simple_data
{
    bool operator==(const simple_data&) const = default;

    int a{};
    int b{};
    float c{};
};

template <class Archive>
void serialize(Archive& ar, simple_data& a)
{
    ar(cereal::make_nvp("a", a.a), cereal::make_nvp("b", a.b), cereal::make_nvp("c", a.c));
}

std::ostream& operator<<(std::ostream& str, const simple_data& a)
{
    str << "a=" << a.a << " b=" << a.b << " c=" << a.c;

    return str;
}
} // namespace v2

namespace v3
{
struct simple_data
{
    template <class Archive>
    void save(Archive& archive) const
    {
        archive(a, b, c);
    }

    template <class Archive>
    void load(Archive& archive)
    {
        archive(a, b, c);
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
} // namespace v3

namespace v4
{
struct simple_data
{
    bool operator==(const simple_data&) const = default;

    int a{};
    int b{};
    float c{};
};

template <class Archive>
void save(Archive& archive, const simple_data& a)
{
    archive(a.a, a.b, a.c);
}

template <class Archive>
void load(Archive& archive, simple_data& a)
{
    archive(a.a, a.b, a.c);
}

std::ostream& operator<<(std::ostream& str, const simple_data& a)
{
    str << "a=" << a.a << " b=" << a.b << " c=" << a.c;

    return str;
}
} // namespace v4

BOOST_AUTO_TEST_SUITE(cereal_tests)

using test_types = boost::mpl::list<v1::simple_data, v2::simple_data, v3::simple_data, v4::simple_data>;

BOOST_AUTO_TEST_CASE_TEMPLATE(serialize_free_func, A, test_types)
{
    std::stringstream ss{};
    const A a{};
    {
        cereal::JSONOutputArchive archive{ss};

        archive(cereal::make_nvp("serialize_free_func", a));
    }

    BOOST_REQUIRE(!ss.str().empty());
    BOOST_TEST_CHECKPOINT("archive content: " << ss.str());

    {
        A b{};
        {
            cereal::JSONInputArchive iarchive(ss);

            iarchive(b);
        }

        BOOST_CHECK_EQUAL(a, b);
    }
}

BOOST_AUTO_TEST_SUITE_END()
