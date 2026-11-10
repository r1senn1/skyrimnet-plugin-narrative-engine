#pragma once

#include <cstdint>
#include <optional>
#include <string_view>

namespace NarrativeEngine::VisitRange
{
    // VisitRange — how far away a visit sender may be from the player.
    //
    // A visit warps the sender to a marker near the player, so without a
    // limit a Markarth smith can "drop by" in Windhelm the same evening.
    // Two INI knobs pick the range, one for the player's potential
    // followers (who travel the province anyway) and one for everyone
    // else:
    //
    //   World — anywhere (stock behaviour).
    //   Hold  — only from the hold the player is in right now.
    //
    // Far senders are not lost: NPCLetterBeat still offers them, so the
    // Director's "this person wants to talk" turns into a letter.
    //
    // This file holds the engine-free decision only, so it builds into
    // NarrativeEngineCore and is tested by src/VisitRange.test.cpp. Finding
    // the holds and the follower flag is VisitComposer::InVisitRange's job.
    enum class Range : std::uint8_t
    {
        World,
        Hold,
    };

    // "world" / "hold", case-insensitive, surrounding blanks ignored.
    // Anything else is nullopt so the caller can keep its default and say so.
    std::optional<Range> Parse(std::string_view text);

    const char* Name(Range range);

    // Whether a sender may visit. Hold FormIDs are 0 when the hold could
    // not be resolved (a cell with no location chain up to a hold); an
    // unknown hold on either side admits the sender, because guessing
    // "far" would silently starve the beat in places the grid never mapped.
    bool Admits(Range range, std::uint32_t playerHoldFormID, std::uint32_t senderHoldFormID);
} // namespace NarrativeEngine::VisitRange
