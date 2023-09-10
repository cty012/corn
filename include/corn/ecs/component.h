#pragma once

#include <corn/util/geometry.h>

namespace corn {
    // TODO: all
    class Component {
    public:
        Component();
    };

    class CTransform : public Component {
    public:
        CTransform();
        ~CTransform();

    private:
        Vec2 position;
    };
}
