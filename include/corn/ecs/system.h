#pragma once

#include <corn/ecs/entity_manager.h>
#include <corn/util/stopwatch.h>

namespace corn {
    /**
     * @class System
     * @brief System in the ECS architecture. Base class for all Systems.
     *
     * All systems must implement the update function, which will be called once every game loop.
     *
     * @see Entity
     * @see EntityManager
     * @see Component
     */
    class System {
    public:
        /// @brief The update function will only be called if the system is active.
        bool active;

        explicit System(Scene& scene);
        virtual ~System();
        System(const System&) = delete;
        System& operator=(const System&) = delete;

        /// @return The Scene that owns this system.
        [[nodiscard]] Scene& getScene() const;

        /// @return The Game that contains this system.
        [[nodiscard]] const Game* getGame() const;

        /// @brief If active, will be called repeatedly during game loop.
        virtual void update(float millis) = 0;

    private:
        /// @brief The Scene that owns this system.
        Scene& scene;
    };

    /**
     * @class SMovement2D
     * @brief Moves Entities in the 2D world.
     *
     * @see System
     * @see CMovement2D
     * @see CTransform2D
     */
    class SMovement2D : public System {
    public:
        explicit SMovement2D(Scene& scene);
        void update(float millis) override;
    };

    /**
     * @class SGravity
     * @brief Applies gravity to the Entities in both 2D and 3D world.
     *
     * Unit: pixels/second^2
     *
     * @see System
     * @see CMovement2D
     * @see CGravity2D
     */
    class SGravity : public System {
    public:
        static constexpr float g = 2000.0;
        float scale;
        explicit SGravity(Scene& scene, float scale = 1.0);
        void update(float millis) override;
    };

    /**
     * @class SCollisionDetection
     * @brief Detects and resolves collision.
     *
     * The system retrieves all Entities with a position and collision detection component and detects any collision
     * between them. If detected, it will invoke any collision resolvers attached to either Entity.
     *
     * @see System
     * @see CAABB
     * @see CCollisionResolve
     */
    class SCollisionDetection : public System {
    public:
        explicit SCollisionDetection(Scene& scene);
        void update(float millis) override;
    };
}
