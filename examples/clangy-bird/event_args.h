#pragma once

#include <corn/event.h>

class EventArgsLanguageChange : public corn::EventArgs {
    [[nodiscard]] constexpr std::string type() const noexcept override { return "clangy-bird::langchange"; }
};
