#pragma once


namespace corn {
    class Scene;


    class System {
    private:
        Scene& scene;
    public:
        bool active = true;

        System(Scene& scene);
        ~System() = default;
        /**
         * If active, will be called repeatedly during game loop
         */
        virtual void update(double milsec);
    };


    /**
     * Detects user input (keyboard & mouse only)
     */
    class SUserInput : public System {};


    /**
     * Renders all sprites on the canvas. Only one camera will be used.
     */
    class SRender : public System {
        void update(double milsec) override;
    };


    /**
     * Calculates and resolves collision.
     */
    class SPhysics : public System {};
}