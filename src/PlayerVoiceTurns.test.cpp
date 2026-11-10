#include <PlayerVoiceTurns.h>

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>

using NarrativeEngine::PlayerVoiceTurns::IsDuplicate;
using NarrativeEngine::PlayerVoiceTurns::Parse;

namespace
{
    // Shape copied from a real PublicGetRecentEvents row (2026-10-29 log).
    constexpr const char* kEvents = R"([
        {"type":"dialogue_player_stt","gameTime":7728999.1,
         "data":{"dialogue":"So why you have long talk to me before...forward?","listener":"Larkspur","player_name":"Urzog","speaker":"Urzog"}},
        {"type":"dialogue_player_stt","gameTime":7728900.0,"targetActorName":"Larkspur",
         "data":{"dialogue":"And remember me, who are you?","player_name":"Urzog","speaker":"Urzog"}},
        {"type":"dialogue_player_stt","gameTime":7728950.0,
         "data":{"dialogue":"Said to no one.","speaker":"Urzog"}},
        {"type":"dialogue_player_stt","gameTime":7729050.0,
         "data":{"dialogue":"Lydia, wait outside.","listener":"Lydia","speaker":"Urzog"}},
        {"type":"dialogue_player_stt","gameTime":7700000.0,
         "data":{"dialogue":"Before the visit.","listener":"Larkspur","speaker":"Urzog"}},
        {"type":"dialogue","gameTime":7729000.0,
         "data":{"dialogue":"An NPC line.","listener":"Urzog","speaker":"Larkspur"}},
        {"type":"dialogue_player_stt","gameTime":7729060.0,"data":{"dialogue":"   ","listener":"Larkspur"}},
        {"type":"dialogue_player_stt","gameTime":7729070.0,"data":null},
        "not an object"
    ])";
} // namespace

TEST_CASE("PlayerVoiceTurns picks the player's spoken lines to the visitor", "[PlayerVoiceTurns]")
{
    SECTION("when the events hold voice lines to several people")
    {
        const auto turns = Parse(kEvents, "Larkspur", 7727333.2);

        SECTION("should keep the visitor's (by listener, by target, or not yet addressed), oldest first")
        {
            REQUIRE(turns.size() == 3);
            REQUIRE(turns[0].text == "And remember me, who are you?");
            REQUIRE(turns[1].text == "Said to no one.");
            REQUIRE(turns[2].text == "So why you have long talk to me before...forward?");
            REQUIRE(turns[0].gameTime < turns[1].gameTime);
        }
    }

    SECTION("when the listener differs only in case and blanks")
    {
        SECTION("should still match the visitor")
        {
            REQUIRE(Parse(kEvents, "  larkspur ", 7727333.2).size() == 3);
        }
    }

    SECTION("when the visit started after every line")
    {
        SECTION("should return nothing")
        {
            REQUIRE(Parse(kEvents, "Larkspur", 7800000.0).empty());
        }
    }

    SECTION("when the payload is not a JSON array")
    {
        SECTION("should return nothing instead of throwing")
        {
            REQUIRE(Parse("", "Larkspur", 0.0).empty());
            REQUIRE(Parse("{oops", "Larkspur", 0.0).empty());
            REQUIRE(Parse(R"({"type":"dialogue_player_stt"})", "Larkspur", 0.0).empty());
        }
    }
}

TEST_CASE("PlayerVoiceTurns spots a line the dialogue endpoint already returned", "[PlayerVoiceTurns]")
{
    const std::vector<std::string> existing{"what do you want from me?", "Larkspur. Just Larkspur."};

    SECTION("when the same words come back re-cased and re-spaced")
    {
        SECTION("should call it a duplicate")
        {
            REQUIRE(IsDuplicate("What do you  want from me?", existing));
        }
    }

    SECTION("when the words differ")
    {
        SECTION("should not")
        {
            REQUIRE_FALSE(IsDuplicate("so remember who are you?", existing));
        }
    }
}
