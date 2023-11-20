#pragma once

#include <vector>
#include <corn/ecs/entity.h>
#include <corn/ecs/system.h>

namespace corn {
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

    class Game;

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

        Scene();
        virtual ~Scene();
        Scene(const Scene& other) = delete;
        Scene& operator=(const Scene& other) = delete;

        /**
         * @return The unique ID of the scene.
         *
         * The unique ID starts from 0 and adds one for each new scene created. After it reaches the max value of
         * SceneID, it goes back to 0.
         */
        [[nodiscard]] SceneID getID() const;

        /// @return The game that owns this scene.
        [[nodiscard]] const Game* getGame() const;

        /// @return The EntityManager owned by this scene.
        [[nodiscard]] EntityManager& getEntityManager() const;

        /// @return The EventManager room corresponding to this scene.
        [[nodiscard]] EventManager& getEventManager() const;

        /**
         * @brief Update all Entities, Components, and Systems in the scene.
         * @param millis Number of milliseconds elapsed.
         */
        void update(float millis);


    protected:
        /// @brief The unique ID of the scene.
        SceneID id;
        std::string room;

        /// @brief The game that owns this scene.
        const Game* game;

        /// @brief Manages the lifetime of all Entities in this scene.
        EntityManager* entityManager;

        /// @brief List of all Systems in this scene.
        std::vector<System*> systems;
    };
}
