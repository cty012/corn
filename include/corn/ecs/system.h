#pragma once

namespace corn {
    class Game;
    class Scene;

    /**
     * @class System
     * @brief System in the ECS architecture. Base class for all systems.
     *
     * All systems must implement the update function, which will be called once every game loop.
     *
     * @see Entity
     * @see EntityManager
     * @see Component
     */
    class System {
    public:
        /**
         * @brief Constructor.
         * @param scene Target scene to attach to.
         */
        explicit System(Scene& scene) noexcept;

        /// @brief Destructor.
        virtual ~System();

        System(const System&) = delete;
        System& operator=(const System&) = delete;

        /// @brief Getter for active.
        [[nodiscard]] bool isActive() const noexcept;

        /// @brief Setter for active.
        void setActive(bool active) noexcept;

        /// @return The scene that owns this system.
        [[nodiscard]] Scene& getScene() const noexcept;

        /// @return The game that contains this system.
        [[nodiscard]] const Game* getGame() const noexcept;

        /**
         * @brief If active, will be called repeatedly during game loop.
         * @param millis Number of milliseconds elapsed.
         */
        virtual void update(float millis) = 0;

    private:
        /// @brief The Scene that owns this system.
        Scene& scene_;

        /// @brief The update function will only be called if the system is active.
        bool active_;
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
        /**
         * @brief Constructor.
         * @param scene Target scene to attach to.
         */
        explicit SMovement2D(Scene& scene) noexcept;

        /**
         * Moves all entities by their velocity.
         * @param millis Number of milliseconds elapsed.
         */
        void update(float millis) override;
    };

    /**
     * @class SGravity
     * @brief Applies gravity to the Entities in both 2D and 3D world.
     *
     * @see System
     * @see CMovement2D
     * @see CGravity2D
     */
    class SGravity : public System {
    public:
        /**
         * @brief Gravitational acceleration.
         *
         * Unit: pixel/second^2
         */
        float g;

        /**
         * @brief Constructor.
         * @param scene Target scene to attach to.
         * @param g Gravitational acceleration. Default value is 2000 pixel/second^2.
         */
        explicit SGravity(Scene& scene, float g = 2000.0f) noexcept;

        /**
         * @brief Calculates amount of velocity change for all entities with the CGravity2D component.
         * @param millis Number of milliseconds elapsed.
         */
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
        /**
         * @brief Constructor.
         * @param scene Target scene to attach to.
         */
        explicit SCollisionDetection(Scene& scene) noexcept;

        /**
         * @brief Detects all collisions and emit events.
         * @param millis Number of milliseconds elapsed.
         */
        void update(float millis) override;
    };
}
