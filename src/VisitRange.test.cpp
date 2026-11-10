#include <VisitRange.h>

#include <catch2/catch_test_macros.hpp>

#include <cstdint>

using NarrativeEngine::VisitRange::Admits;
using NarrativeEngine::VisitRange::Name;
using NarrativeEngine::VisitRange::Parse;
using NarrativeEngine::VisitRange::Range;

namespace
{
    constexpr std::uint32_t kWhiterunHold = 0x00016772u;
    constexpr std::uint32_t kReachHold = 0x00016769u;
} // namespace

TEST_CASE("VisitRange reads the INI words", "[VisitRange]")
{
    SECTION("when the value names a range")
    {
        SECTION("should accept it in any case and with blanks around it")
        {
            REQUIRE(Parse("world") == Range::World);
            REQUIRE(Parse("HOLD") == Range::Hold);
            REQUIRE(Parse("  Hold\t") == Range::Hold);
        }
    }

    SECTION("when the value is anything else")
    {
        SECTION("should refuse it, so the caller keeps its default")
        {
            REQUIRE_FALSE(Parse("").has_value());
            REQUIRE_FALSE(Parse("town").has_value());
            REQUIRE_FALSE(Parse("holds").has_value());
        }
    }

    SECTION("should write back what it reads")
    {
        REQUIRE(Parse(Name(Range::World)) == Range::World);
        REQUIRE(Parse(Name(Range::Hold)) == Range::Hold);
    }
}

TEST_CASE("VisitRange decides who may call", "[VisitRange]")
{
    SECTION("when the range is the whole world")
    {
        SECTION("should admit a sender from another hold")
        {
            REQUIRE(Admits(Range::World, kWhiterunHold, kReachHold));
        }
    }

    SECTION("when the range is the player's hold")
    {
        SECTION("should admit a neighbour")
        {
            REQUIRE(Admits(Range::Hold, kWhiterunHold, kWhiterunHold));
        }

        SECTION("should turn away a sender from another hold")
        {
            // They are not lost: the letter beat still offers them.
            REQUIRE_FALSE(Admits(Range::Hold, kWhiterunHold, kReachHold));
        }

        SECTION("should admit anyone when either hold is unknown")
        {
            // Solstheim or an unmapped cave must not quietly end all visits.
            REQUIRE(Admits(Range::Hold, 0, kReachHold));
            REQUIRE(Admits(Range::Hold, kWhiterunHold, 0));
        }
    }
}
