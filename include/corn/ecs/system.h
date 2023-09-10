#pragma once

#include <corn/ecs/entity_manager.h>

namespace corn {
    class System {
    public:
        bool active = true;

        explicit System(EntityManager& entityManager);
        ~System() = default;
        /**
         * Returns the number of milliseconds since the last update. If there are no previous updates then the object's
         * creation time is used. Only count the milliseconds during which active is true
         * @return Number of milliseconds
         */
        [[nodiscard]] double milsec() const;
        /**
         * If active, will be called repeatedly during game loop
         */
        virtual void update() = 0;

    private:
        EntityManager& entityManager;
    };

    /**
     * Calculates and resolves collision.
     */
    class SPhysics : public System {
    public:
        void update() override;
    };
}
