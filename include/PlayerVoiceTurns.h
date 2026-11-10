#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace NarrativeEngine::PlayerVoiceTurns
{
    // PlayerVoiceTurns — the player's spoken (speech-to-text) lines to a
    // visitor.
    //
    // The visit's conclusion poll and its speech-turn sampler read
    // SkyrimNet's PublicGetRecentDialogue, which carries typed player
    // lines (`dialogue_player_text`) but not spoken ones: a player who
    // talks by voice looks silent to both, so the silence gate fires
    // nudges and the visit is force-ended mid-conversation. The spoken
    // lines do reach the event stream as `dialogue_player_stt`, with the
    // transcription under data.dialogue and the addressee under
    // data.listener (and targetActorName). This module picks them out of
    // a PublicGetRecentEvents payload so both readers can merge them back.
    //
    // Engine-free, so it builds into NarrativeEngineCore and is tested by
    // src/PlayerVoiceTurns.test.cpp. Fetching is the callers' job.

    inline constexpr const char* kEventType = "dialogue_player_stt";

    struct Turn
    {
        std::string text;
        double gameTime = 0.0;
    };

    // Voice lines in `eventsJson` said at or after `sinceGameTime` and
    // addressed to `senderName` (data.listener, else the documented
    // targetActorName) or not yet addressed: SkyrimNet fills the addressee
    // minutes late, if at all. Oldest first.
    // Malformed JSON or rows yield nothing rather than throwing.
    std::vector<Turn> Parse(std::string_view eventsJson, std::string_view senderName, double sinceGameTime);

    // True when `text` is already among `existing` (case- and
    // blank-insensitive), so a voice line SkyrimNet one day also returns
    // from the dialogue endpoint is not counted twice.
    bool IsDuplicate(std::string_view text, const std::vector<std::string>& existing);
} // namespace NarrativeEngine::PlayerVoiceTurns
