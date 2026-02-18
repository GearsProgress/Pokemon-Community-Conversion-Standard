#include "catch_amalgamated.hpp"
#include "pccs_utils.h"

using namespace PCCSUtils;

TEST_CASE("extractLehmerCode4", "[unit][gen3][utils][lehmer]")
{
    u8 out_order[4];

    extractLehmerCode4(0, out_order);
    REQUIRE(out_order[0] == 0);
    REQUIRE(out_order[1] == 1);
    REQUIRE(out_order[2] == 2);
    REQUIRE(out_order[3] == 3);

    extractLehmerCode4(1, out_order);
    REQUIRE(out_order[0] == 0);
    REQUIRE(out_order[1] == 1);
    REQUIRE(out_order[2] == 3);
    REQUIRE(out_order[3] == 2);

    extractLehmerCode4(2, out_order);
    REQUIRE(out_order[0] == 0);
    REQUIRE(out_order[1] == 2);
    REQUIRE(out_order[2] == 1);
    REQUIRE(out_order[3] == 3);

    extractLehmerCode4(3, out_order);
    REQUIRE(out_order[0] == 0);
    REQUIRE(out_order[1] == 2);
    REQUIRE(out_order[2] == 3);
    REQUIRE(out_order[3] == 1);

    extractLehmerCode4(4, out_order);
    REQUIRE(out_order[0] == 0);
    REQUIRE(out_order[1] == 3);
    REQUIRE(out_order[2] == 1);
    REQUIRE(out_order[3] == 2);

    extractLehmerCode4(5, out_order);
    REQUIRE(out_order[0] == 0);
    REQUIRE(out_order[1] == 3);
    REQUIRE(out_order[2] == 2);
    REQUIRE(out_order[3] == 1);

    extractLehmerCode4(6, out_order);
    REQUIRE(out_order[0] == 1);
    REQUIRE(out_order[1] == 0);
    REQUIRE(out_order[2] == 2);
    REQUIRE(out_order[3] == 3);

    extractLehmerCode4(7, out_order);
    REQUIRE(out_order[0] == 1);
    REQUIRE(out_order[1] == 0);
    REQUIRE(out_order[2] == 3);
    REQUIRE(out_order[3] == 2);

    extractLehmerCode4(8, out_order);
    REQUIRE(out_order[0] == 1);
    REQUIRE(out_order[1] == 2);
    REQUIRE(out_order[2] == 0);
    REQUIRE(out_order[3] == 3);

    extractLehmerCode4(9, out_order);
    REQUIRE(out_order[0] == 1);
    REQUIRE(out_order[1] == 2);
    REQUIRE(out_order[2] == 3);
    REQUIRE(out_order[3] == 0);

    extractLehmerCode4(10, out_order);
    REQUIRE(out_order[0] == 1);
    REQUIRE(out_order[1] == 3);
    REQUIRE(out_order[2] == 0);
    REQUIRE(out_order[3] == 2);

    extractLehmerCode4(11, out_order);
    REQUIRE(out_order[0] == 1);
    REQUIRE(out_order[1] == 3);
    REQUIRE(out_order[2] == 2);
    REQUIRE(out_order[3] == 0);

    extractLehmerCode4(12, out_order);
    REQUIRE(out_order[0] == 2);
    REQUIRE(out_order[1] == 0);
    REQUIRE(out_order[2] == 1);
    REQUIRE(out_order[3] == 3);

    extractLehmerCode4(13, out_order);
    REQUIRE(out_order[0] == 2);
    REQUIRE(out_order[1] == 0);
    REQUIRE(out_order[2] == 3);
    REQUIRE(out_order[3] == 1);

    extractLehmerCode4(14, out_order);
    REQUIRE(out_order[0] == 2);
    REQUIRE(out_order[1] == 1);
    REQUIRE(out_order[2] == 0);
    REQUIRE(out_order[3] == 3);

    extractLehmerCode4(15, out_order);
    REQUIRE(out_order[0] == 2);
    REQUIRE(out_order[1] == 1);
    REQUIRE(out_order[2] == 3);
    REQUIRE(out_order[3] == 0);

    extractLehmerCode4(16, out_order);
    REQUIRE(out_order[0] == 2);
    REQUIRE(out_order[1] == 3);
    REQUIRE(out_order[2] == 0);
    REQUIRE(out_order[3] == 1);

    extractLehmerCode4(17, out_order);
    REQUIRE(out_order[0] == 2);
    REQUIRE(out_order[1] == 3);
    REQUIRE(out_order[2] == 1);
    REQUIRE(out_order[3] == 0);

    extractLehmerCode4(18, out_order);
    REQUIRE(out_order[0] == 3);
    REQUIRE(out_order[1] == 0);
    REQUIRE(out_order[2] == 1);
    REQUIRE(out_order[3] == 2);

    extractLehmerCode4(19, out_order);
    REQUIRE(out_order[0] == 3);
    REQUIRE(out_order[1] == 0);
    REQUIRE(out_order[2] == 2);
    REQUIRE(out_order[3] == 1);

    extractLehmerCode4(20, out_order);
    REQUIRE(out_order[0] == 3);
    REQUIRE(out_order[1] == 1);
    REQUIRE(out_order[2] == 0);
    REQUIRE(out_order[3] == 2);

    extractLehmerCode4(21, out_order);
    REQUIRE(out_order[0] == 3);
    REQUIRE(out_order[1] == 1);
    REQUIRE(out_order[2] == 2);
    REQUIRE(out_order[3] == 0);

    extractLehmerCode4(22, out_order);
    REQUIRE(out_order[0] == 3);
    REQUIRE(out_order[1] == 2);
    REQUIRE(out_order[2] == 0);
    REQUIRE(out_order[3] == 1);

    extractLehmerCode4(23, out_order);
    REQUIRE(out_order[0] == 3);
    REQUIRE(out_order[1] == 2);
    REQUIRE(out_order[2] == 1);
    REQUIRE(out_order[3] == 0);
}