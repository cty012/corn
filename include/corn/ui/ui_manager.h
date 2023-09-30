#pragma once

#include <corn/ecs/entity_manager.h>
#include <corn/ui/ui_element.h>
#include <corn/util/config.h>

namespace corn {
    template <typename T>
    concept ElementType = std::derived_from<T, UIElement>;

    /**
     * @class UIManager
     * @brief Manages all UI elements.
     */
    class UIManager {
    public:
        friend class UIElement;

        explicit UIManager(EntityManager* entityManager);
        ~UIManager();

        [[nodiscard]] EntityManager* getEntityManager() const;

        /**
         * @brief Create a UI element and attach it to the UI manager.
         * @tparam T Type of the UI element, must derive from UIElement class.
         * @param args Arguments for constructing the UI element (excluding the first argument UIManager& uiManager).
         * @return Pointer to the UI element created.
         */
        template <ElementType T, typename... Args>
        T* createElement(Args&&... args);

    private:
        void destroyElement(UIElement& element);  // TODO: Implement this

        EntityManager* entityManager;
        // TODO: tree structure
    };
}

#include <corn/ui/ui_manager_template.cpp>
