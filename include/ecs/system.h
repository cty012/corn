#pragma once

#include <main/scene.h>


namespace corn {
    class System {
    private:
        Scene& scene;
    public:
        bool active = true;

        System(Scene& scene);
        ~System() = default;
        /**
         * If active, will be called once per fixed number of frames
         */
        virtual void fixedUpdate();
    };


    /**
     * Detects user input (keyboard & mouse only)
     */
    class SUserInput : public System {};


    /**
     * Renders all sprites on the canvas. Only one camera will be used.
     */
    class SRender : public System {};


    /**
     * Calculates and resolves collision.
     */
    class SPhysics : public System {};
}
