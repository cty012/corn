#pragma once

#include <corn/ecs/entity_manager.h>
#include <corn/util/stopwatch.h>

namespace corn {
    /**
     * @class System
     * @brief Base class for all Systems.
     *
     * @todo Write detailed description.
     */
    class System {
    public:
        /// @brief The update function will only be called if the system is active.
        bool active;

        System();
        ~System() = default;

        /// @brief If active, will be called repeatedly during game loop.
        virtual void update(EntityManager& entityManager, unsigned long long int millis) = 0;
    };

    /**
     * @class SGravity
     * @brief Applies gravity to the Entities in both 2D and 3D world.
     */
    class SGravity : public System {
    public:
        static constexpr double g = 5;
        double scale;
        explicit SGravity(double scale = 1.0);
        void update(EntityManager& entityManager, unsigned long long int millis) override;
    };

    /**
     * @class SMovement2D
     * @brief Moves Entities in the 2D world.
     */
    class SMovement2D : public System {
    public:
        void update(EntityManager& entityManager, unsigned long long int millis) override;
    };

    /**
     * @class SCollisionDetection
     * @brief Calculates and resolves collision.
     *
     * @todo Implement this class.
     * @todo Write detailed description.
     */
    class SCollisionDetection : public System {
    public:
        void update(EntityManager& entityManager, unsigned long long int millis) override;
    };
}
