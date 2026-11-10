#include <VisitRange.h>

#include <cctype>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>

namespace NarrativeEngine::VisitRange
{
    namespace
    {
        bool EqualsIgnoreCase(std::string_view a, std::string_view b)
        {
            if (a.size() != b.size())
                return false;
            for (std::size_t i = 0; i < a.size(); ++i) {
                if (std::tolower(static_cast<unsigned char>(a[i])) != std::tolower(static_cast<unsigned char>(b[i])))
                    return false;
            }
            return true;
        }
    } // namespace

    std::optional<Range> Parse(std::string_view text)
    {
        while (!text.empty() && std::isspace(static_cast<unsigned char>(text.front())))
            text.remove_prefix(1);
        while (!text.empty() && std::isspace(static_cast<unsigned char>(text.back())))
            text.remove_suffix(1);

        if (EqualsIgnoreCase(text, "world"))
            return Range::World;
        if (EqualsIgnoreCase(text, "hold"))
            return Range::Hold;
        return std::nullopt;
    }

    const char* Name(Range range)
    {
        switch (range) {
        case Range::World:
            return "world";
        case Range::Hold:
            return "hold";
        }
        return "world";
    }

    bool Admits(Range range, std::uint32_t playerHoldFormID, std::uint32_t senderHoldFormID)
    {
        if (range == Range::World)
            return true;
        if (playerHoldFormID == 0 || senderHoldFormID == 0)
            return true;
        return playerHoldFormID == senderHoldFormID;
    }
} // namespace NarrativeEngine::VisitRange
