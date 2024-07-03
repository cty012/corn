#pragma once

#include <corn/event/event_args.h>

namespace corn::test {
    class EventArgsDummy : public EventArgs {
        [[nodiscard]] std::string type() const noexcept override { return "corn::test::dummy"; }
    };
}
