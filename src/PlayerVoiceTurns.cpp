#include <PlayerVoiceTurns.h>

#include <JsonUtils.h>
#include <LLMTextSanitizer.h>

#include <nlohmann/json.hpp>

#include <algorithm>
#include <cctype>
#include <string>
#include <string_view>
#include <vector>

namespace NarrativeEngine::PlayerVoiceTurns
{
    namespace
    {
        // Lowercased with runs of blanks collapsed and the ends trimmed, so
        // "Larkspur" matches " larkspur " and a transcription matches the
        // same words re-spaced.
        std::string Normalize(std::string_view s)
        {
            std::string out;
            out.reserve(s.size());
            bool pendingSpace = false;
            for (const char c : s) {
                const auto uc = static_cast<unsigned char>(c);
                if (std::isspace(uc)) {
                    pendingSpace = !out.empty();
                    continue;
                }
                if (pendingSpace) {
                    out.push_back(' ');
                    pendingSpace = false;
                }
                out.push_back(static_cast<char>(std::tolower(uc)));
            }
            return out;
        }
    } // namespace

    std::vector<Turn> Parse(std::string_view eventsJson, std::string_view senderName, double sinceGameTime)
    {
        std::vector<Turn> out;
        auto parsed = nlohmann::json::parse(eventsJson, nullptr, false);
        if (parsed.is_discarded() || !parsed.is_array())
            return out;

        const auto sender = Normalize(senderName);
        for (const auto& e : parsed) {
            if (!e.is_object())
                continue;
            // The caller filters by type already; this guards a payload
            // that ignores the filter.
            if (const auto type = JsonUtils::StringOr(e, "type"); !type.empty() && type != kEventType)
                continue;

            const double gameTime = JsonUtils::NumberOr(e, "gameTime", 0.0);
            if (gameTime < sinceGameTime)
                continue;

            auto dataIt = e.find("data");
            if (dataIt == e.end() || !dataIt->is_object())
                continue;

            // data.listener (listed for this event type in the SkyrimNet
            // devkit's WORKFLOW_TRIGGERS.md) or targetActorName.
            // SkyrimNet often fills both only minutes after the line: in
            // the 2026-10-26..30 logs 191 of 257 voice events were first
            // seen with neither. A line still unaddressed during a visit is
            // credited to the visitor; one addressed to someone else is not.
            auto listener = Normalize(JsonUtils::StringOr(*dataIt, "listener"));
            if (listener.empty())
                listener = Normalize(JsonUtils::StringOr(e, "targetActorName"));
            if (!listener.empty() && listener != sender)
                continue;

            auto text = LLMTextSanitizer::Sanitize(JsonUtils::StringOr(*dataIt, "dialogue"));
            if (text.empty())
                continue;

            out.push_back(Turn{std::move(text), gameTime});
        }

        std::stable_sort(out.begin(), out.end(), [](const Turn& a, const Turn& b) { return a.gameTime < b.gameTime; });
        return out;
    }

    bool IsDuplicate(std::string_view text, const std::vector<std::string>& existing)
    {
        const auto needle = Normalize(text);
        return std::any_of(
            existing.begin(), existing.end(), [&](const std::string& s) { return Normalize(s) == needle; });
    }
} // namespace NarrativeEngine::PlayerVoiceTurns
