#include <corn/ecs/system.h>

namespace corn {
    System::System(EntityManager& entityManager) : entityManager(entityManager), active(true) {}

    double System::milsec() const {
        // TODO: Add a timer
        return 0.0;
    }

    void SPhysics::update() {
        // TODO
    }
}
