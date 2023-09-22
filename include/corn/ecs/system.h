#pragma once

#include <corn/ecs/entity_manager.h>

namespace corn {
    /**
     * @class System
     * @brief Base class for all Systems.
     *
     * @todo Write detailed description.
     */
    class System {
    public:
        /// @brief The update() function will only be called if the system is active.
        bool active;

        explicit System(EntityManager& entityManager);
        ~System() = default;

        /**
         * @return Number of milliseconds since the last update.
         * If there are no previous updates then the object's creation time is used. Only count the milliseconds
         * during which active is true.
         */
        [[nodiscard]] double milsec() const;
        /// @brief If active, will be called repeatedly during game loop.
        virtual void update() = 0;

    protected:
        EntityManager& entityManager;
    };

    /**
     * @class SPhysics
     * @brief Calculates and resolves collision.
     *
     * @todo Implement this class.
     * @todo Write detailed description.
     */
    class SPhysics : public System {
    public:
        void update() override;
    };
}
