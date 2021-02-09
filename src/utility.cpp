#include "utility.h"

/*------------------------------------------------------------------*/
// doctest:

#include <doctest/doctest.h>
#include <vector>
#include <set>

TEST_CASE("testing utility functions")
{
    SUBCASE("testing container functions")
    {
        std::set<int> digits_set = { 1, 2, 3 };
        CHECK(contains(digits_set, 1));
        CHECK(contains(digits_set, 2));
        CHECK(contains(digits_set, 3));
        CHECK_FALSE(contains(digits_set, 42));

        std::vector<int> digits_vec = { 1, 2, 3 };
        CHECK(contains(digits_vec, 1));
        CHECK(contains(digits_vec, 2));
        CHECK(contains(digits_vec, 3));
        CHECK_FALSE(contains(digits_vec, 1337));

        std::vector<const char*> cstrs_vec = { "foo", "bar" };
        const char* foo = "foo";
        const char* bar = "bar";
        const char* foobar = "foobar";
        CHECK(contains(cstrs_vec, foo));
        CHECK(contains(cstrs_vec, bar));
        CHECK_FALSE(contains(cstrs_vec, foobar));
    }
}