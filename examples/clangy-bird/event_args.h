#pragma once

#include <corn/event.h>

class EventArgsLanguageChange : public corn::EventArgs {
    [[nodiscard]] constexpr const char* type() const override { return "clangy-bird::langchange"; }
};
