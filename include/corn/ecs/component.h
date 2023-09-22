#pragma once

#include <corn/util/geometry.h>

namespace corn {
    // TODO: all
    class Component {
    public:
        Component() = default;
        virtual ~Component() = 0;
    };

    class CTransform : public Component {
    public:
        CTransform();
        ~CTransform() override;

    private:
        Vec2 position;
    };
}
