#pragma once

#include <string>
#include <vector>

namespace corn {
    class EntityManager;
    class EventManager;
    class Game;
    class System;
    class UIManager;

    template <typename T>
    concept SystemType = std::derived_from<T, System>;

    /**
     * @enum SceneOperation
     * @brief Possible operations on a scene stack
     */
    enum class SceneOperation {
        PUSH,                         // Push the new scene to the top of the stack
        POP,                          // Pop the top scene from the stack (if applicable)
        REPLACE,                      // Replaces the top scene with the new scene (original scene is destroyed)
                                      // If scene stack is empty then same with PUSH
        REPLACE_ALL,                  // Empties the scene stack before pushing the new scene onto the stack
    };

    /**
     * @class Scene
     * @brief Contains a set of ECS architecture for one single scene.
     *
     * Base scene class. To define your own scene, extend this class and add the entities and systems in the
     * constructor. Make sure to remember deallocation by overriding the destructor.
     *
     * The update function will be called by the Game class repeatedly. You should not override this method. Instead,
     * define your own Systems and their update() functions.
     *
     * @see Game
     * @see System
     * @see EntityManager
     */
    class Scene {
    public:
        using SceneID = unsigned long long int;

        friend class Game;

        /// @brief Constructor.
        Scene() noexcept;

        /// @brief Destructor.
        virtual ~Scene();

        Scene(const Scene& other) = delete;
        Scene& operator=(const Scene& other) = delete;

        /**
         * @return The unique ID of the scene.
         *
         * The unique ID starts from 0 and adds one for each new scene created. After it reaches the max value of
         * SceneID, it goes back to 0.
         */
        [[nodiscard]] SceneID getID() const noexcept;

        /// @return The EntityManager owned by this scene.
        [[nodiscard]] EntityManager& getEntityManager() const noexcept;

        /// @return The UIManager owned by this scene.
        [[nodiscard]] UIManager& getUIManager() const noexcept;

        /// @return The EventManager room corresponding to this scene.
        [[nodiscard]] EventManager& getEventManager() const noexcept;

        /// @return The game that owns this scene.
        [[nodiscard]] const Game* getGame() const noexcept;

        /**
         * @brief Creates a system and attach it to the scene.
         * @tparam T Type of the system. Must derive from System class.
         * @param args Arguments for constructing the system (excluding the first argument Scene& scene).
         * @return Pointer to the system created.
         *
         * Multiple systems of the same type CAN coexist.
         */
        template <SystemType T, typename... Args>
        T* addSystem(Args&&... args);

        /**
         * @brief Calls the update methods of all ACTIVE systems added to the scene.
         * @param millis Number of milliseconds elapsed.
         *
         * Systems' update methods will be called in the order they are added. This order cannot be changed.
         */
        void update(float millis);

    private:
        /// @brief The unique ID of the scene.
        SceneID id_;

        /// @brief The room ID of the scene.
        std::string room_;

        /// @brief The game that owns this scene.
        const Game* game_;

        /// @brief List of all Systems in this scene.
        std::vector<System*> systems_;

        /// @brief Manages the lifetime of all entities in this scene.
        EntityManager* entityManager_;

        /// @brief Manages the lifetime of all UI widgets in this scene.
        UIManager* uiManager_;
    };

    template<SystemType T, typename... Args>
    T* Scene::addSystem(Args&&... args) {
        T* system = new T(*this, std::forward<Args>(args)...);
        this->systems_.push_back(system);
        return system;
    }
}
