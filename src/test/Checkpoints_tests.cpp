//
// Unit tests for block-chain checkpoints
//
#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/test/unit_test.hpp>
#include <boost/foreach.hpp>

#include "../checkpoints.h"
#include "../util.h"

using namespace std;

BOOST_AUTO_TEST_SUITE(Checkpoints_tests)

BOOST_AUTO_TEST_CASE(sanity)
{
    uint256 p19080 = uint256("0x00000000000000afe37b4935607631decac4bf3fb0e008f9bb700d073a6d868e");
    uint256 p30583 = uint256("0x00000000000000b6a1c931d793d07ec77fbf8da6c280c092bfaddf04412cfaa3");
    BOOST_CHECK(Checkpoints::CheckHardened(19080, p19080));
    BOOST_CHECK(Checkpoints::CheckHardened(30583, p30583));

    
    // Wrong hashes at checkpoints should fail:
    BOOST_CHECK(!Checkpoints::CheckHardened(19080, p30583));
    BOOST_CHECK(!Checkpoints::CheckHardened(30583, p19080));

    // ... but any hash not at a checkpoint should succeed:
    BOOST_CHECK(Checkpoints::CheckHardened(19080+1, p30583));
    BOOST_CHECK(Checkpoints::CheckHardened(30583+1, p19080));

    BOOST_CHECK(Checkpoints::GetTotalBlocksEstimate() >= 30583);
}

BOOST_AUTO_TEST_SUITE_END()
