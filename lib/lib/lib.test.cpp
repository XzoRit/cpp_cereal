#include <cereal/access.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/vector.hpp>

#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>

#include <ostream>
#include <random>
#include <sstream>
#include <type_traits>
#include <typeinfo>
#include <utility>

namespace
{
std::random_device rd{};
std::mt19937 gen{rd()};

template <class T>
inline typename std::enable_if<std::is_floating_point<T>::value, T>::type random_value(std::mt19937& gen)
{
    return std::uniform_real_distribution<T>(-10000.0, 10000.0)(gen);
}

template <class T>
inline typename std::enable_if<std::is_integral<T>::value && sizeof(T) != sizeof(char), T>::type random_value(
    std::mt19937& gen)
{
    return std::uniform_int_distribution<T>(std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max())(gen);
}

template <class T>
inline typename std::enable_if<std::is_integral<T>::value && sizeof(T) == sizeof(char), T>::type random_value(
    std::mt19937& gen)
{
    return static_cast<T>(
        std::uniform_int_distribution<int64_t>(std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max())(gen));
}

template <class T>
inline typename std::enable_if<std::is_same<T, std::string>::value, std::string>::type random_value(std::mt19937& gen)
{
    std::string s(std::uniform_int_distribution<int>(3, 30)(gen), ' ');
    for (char& c : s)
        c = static_cast<char>(std::uniform_int_distribution<int>('A', 'Z')(gen));
    return s;
}

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

    int a{random_value<int>(gen)};
    int b{random_value<int>(gen)};
    std::string c{random_value<std::string>(gen)};
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

    int a{random_value<int>(gen)};
    int b{random_value<int>(gen)};
    std::string c{random_value<std::string>(gen)};
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

    int a{random_value<int>(gen)};
    int b{random_value<int>(gen)};
    std::string c{random_value<std::string>(gen)};
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

    int a{random_value<int>(gen)};
    int b{random_value<int>(gen)};
    std::string c{random_value<std::string>(gen)};
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

namespace v5
{
class simple_data
{
  public:
    bool operator==(const simple_data&) const = default;

  private:
    friend std::ostream& operator<<(std::ostream& str, const simple_data& a);
    friend class cereal::access;

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(a, b, c);
    }

    int a{random_value<int>(gen)};
    int b{random_value<int>(gen)};
    std::string c{random_value<std::string>(gen)};
};

std::ostream& operator<<(std::ostream& str, const simple_data& a)
{
    str << "a=" << a.a << " b=" << a.b << " c=" << a.c;

    return str;
}
} // namespace v5

namespace v6
{
class simple_data
{
  public:
    bool operator==(const simple_data&) const = default;

  private:
    friend std::ostream& operator<<(std::ostream& str, const simple_data& a);
    friend class cereal::access;

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

    int a{random_value<int>(gen)};
    int b{random_value<int>(gen)};
    std::string c{random_value<std::string>(gen)};
};

std::ostream& operator<<(std::ostream& str, const simple_data& a)
{
    str << "a=" << a.a << " b=" << a.b << " c=" << a.c;

    return str;
}
} // namespace v6

namespace v7
{
class simple_data
{
  public:
    // bool operator==(const simple_data&) const = default;
    bool operator==(const simple_data& other) const
    {
        return other.a == a && other.b == b && other.c == c;
    }

  private:
    friend std::ostream& operator<<(std::ostream& str, const simple_data& a);
    friend class cereal::access;

    template <class Archive>
    std::string save_minimal(const Archive&) const
    {
        std::ostringstream str{};
        str << a << ' ' << b << ' ' << c;
        return str.str();
    }

    template <class Archive>
    void load_minimal(const Archive&, const std::string& from)
    {
        std::istringstream str{from};
        str >> a >> b >> c;
    }

    int a{random_value<int>(gen)};
    int b{random_value<int>(gen)};
    std::string c{random_value<std::string>(gen)};
};

std::ostream& operator<<(std::ostream& str, const simple_data& a)
{
    str << "a=" << a.a << " b=" << a.b << " c=" << a.c;

    return str;
}
} // namespace v7

BOOST_AUTO_TEST_SUITE(cereal_tests)

using test_types = boost::mpl::list<v1::simple_data,
                                    v2::simple_data,
                                    v3::simple_data,
                                    v4::simple_data,
                                    v5::simple_data,
                                    v6::simple_data,
                                    v7::simple_data>;

BOOST_AUTO_TEST_CASE_TEMPLATE(serialize_types, A, test_types)
{
    std::stringstream ss{};
    const A a{};
    {
        cereal::JSONOutputArchive archive{ss};

        archive(cereal::make_nvp(typeid(A).name(), a));
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

} // namespace
