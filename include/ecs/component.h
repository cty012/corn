#pragma once

#include <util/geometry.h>


namespace corn {
    class Component {
    private:
    public:
        Component();
    };


    class CTransform : public Component {
    private:
        Vec2 position;

    public:
        CTransform();
        ~CTransform();
    };
}
