#include <algorithm>
#include <ranges>
#include <stack>
#include <corn/core/game.h>
#include <corn/core/scene.h>
#include <corn/ecs/component.h>
#include <corn/ecs/entity_manager.h>
#include "../event/event_args_extend.h"

namespace corn {
    EntityManager::Node::Node(Entity* ent, Node* parent) noexcept : ent(ent), parent(parent), children(), dirty(false) {}

    EntityManager::EntityManager(Scene& scene) noexcept : scene_(scene), root_(nullptr, nullptr), nodes_() {
        // Listen to zorder change events
        this->eventScope_.addListener(
                this->scene_.getEventManager(),
                "corn::game::ecs::zorder",
                [this](const EventArgs& args) {
                    const auto& _args = dynamic_cast<const EventArgsEntityZOrderChange&>(args);
                    if (!_args.entity) return;
                    Node* node = this->getNodeFromEntity(_args.entity);
                    node->parent->dirty = true;
                });

        // Listen to add/remove camera events
        this->eventScope_.addListener(
                this->scene_.getEventManager(),
                "corn::game::ecs::camera",
                [this](const EventArgs& args) {
                    const auto& _args = dynamic_cast<const EventArgsCamera&>(args);
                    if (!_args.camera || &_args.camera->getEntityManager() != this) return;
                    switch (_args.eventType) {
                        case CameraEventType::ADD:
                            this->cameras_.push_back(_args.camera);
                            break;
                        case CameraEventType::REMOVE:
                            std::erase(this->cameras_, _args.camera);
                            break;
                    }
                });
    }

    EntityManager::~EntityManager() {
        // Delete entities
        for (auto& [id, node] : this->nodes_) {
            delete node.ent;
        }
    }

    Scene& EntityManager::getScene() const noexcept {
        return this->scene_;
    }

    const EntityManager::Node* EntityManager::getRoot() const noexcept {
        return &this->root_;
    }

    Entity& EntityManager::createEntity(const std::string& name, const Entity* parent) {
        // Verify parent
        Node* parentNode = this->getNodeFromEntity(parent);

        // Create the entity
        static Entity::EntityID entID = 1;
        while (entID == 0 || this->nodes_.contains(entID)) {  // Avoid ID 0
            entID++;
        }
        auto* entity = new Entity(entID++, name, *this);

        // Create the node
        this->nodes_.emplace(entity->id_, Node(entity, parentNode));
        parentNode->children.push_back(&this->nodes_.at(entity->id_));
        parentNode->dirty = true;

        return *entity;
    }

    Entity* EntityManager::getEntityByID(Entity::EntityID id) const noexcept {
        if (!this->nodes_.contains(id)) return nullptr;
        return this->nodes_.at(id).ent;
    }

    Entity* EntityManager::getEntityByName(const std::string& name, const Entity* parent, bool recurse) const noexcept {
        std::vector<Entity*> result = getEntitiesHelper([&name](const Entity* entity) {
                return entity->name_ == name;
            }, false, 1, parent, recurse);
        return result.empty() ? nullptr : result[0];
    }

    std::vector<Entity*> EntityManager::getEntitiesByName(
            const std::string& name, const Entity* parent, bool recurse) const noexcept {

        return getEntitiesHelper([&name](const Entity* entity) {
                return entity->name_ == name;
            }, false, 0, parent, recurse);
    }

    Entity* EntityManager::getEntityThat(
            const std::function<bool(const Entity*)>& pred, const Entity* parent, bool recurse) const {

        std::vector<Entity*> result = this->getEntitiesHelper(pred, false, 1, parent, recurse);
        return result.empty() ? nullptr : result[0];
    }

    std::vector<Entity*> EntityManager::getEntitiesThat(
            const std::function<bool(const Entity*)>& pred, const Entity* parent, bool recurse) const {

        return this->getEntitiesHelper(pred, false, 0, parent, recurse);
    }

    std::vector<Entity*> EntityManager::getAllEntities(const Entity* parent, bool recurse) const noexcept {
        return this->getEntitiesHelper(nullptr, false, 0, parent, recurse);
    }

    std::vector<Entity*> EntityManager::getAllActiveEntities(const Entity* parent, bool recurse) const noexcept {
        return this->getEntitiesHelper(nullptr, true, 0, parent, recurse);
    }

    void EntityManager::clear() noexcept {
        // Delete child nodes
        for (auto& [id, node] : this->nodes_) {
            delete node.ent;
        }
        this->nodes_.clear();
        // Reset root node
        this->root_.children.clear();
        this->root_.dirty = false;
    }

    void EntityManager::tidy() noexcept {
        if (this->root_.dirty) {
            this->root_.dirty = false;
            std::stable_sort(this->root_.children.begin(), this->root_.children.end(),
                             [](Node* left, Node* right) {
                                 if (left == right) return false;
                                 auto lTrans = left->ent->getComponent<CTransform2D>();
                                 auto rTrans = right->ent->getComponent<CTransform2D>();
                                 if (lTrans == nullptr || rTrans == nullptr) return false;
                                 else return lTrans->getZOrder() < rTrans->getZOrder();
                             });
        }

        for (auto& [id, node] : this->nodes_) {
            if (!node.dirty) continue;
            node.dirty = false;
            std::stable_sort(node.children.begin(), node.children.end(),
                             [](Node* left, Node* right) {
                                 if (left == right) return false;
                                 auto lTrans = left->ent->getComponent<CTransform2D>();
                                 auto rTrans = right->ent->getComponent<CTransform2D>();
                                 if (lTrans == nullptr || rTrans == nullptr) return false;
                                 else return lTrans->getZOrder() < rTrans->getZOrder();
                             });
        }
    }

    void EntityManager::destroyNode(Node* node) noexcept {  // NOLINT
        if (node == nullptr) return;
        // Destroy all children
        for (Node* child : node->children) {
            this->destroyNode(child);
        }
        // Destroy self
        Entity::EntityID entID = node->ent->id_;
        delete node->ent;
        this->nodes_.erase(entID);
    }

    void EntityManager::destroyEntity(Entity& entity) noexcept {
        Node* node = &this->nodes_.at(entity.id_);
        Node* parent = node->parent;
        // Removes relation (parent --> node)
        std::erase(parent->children, node);
        // Destroy node itself
        this->destroyNode(node);
    }

    const EntityManager::Node* EntityManager::getNodeFromEntity(const Entity* entity) const {
        if (!entity) {
            return &this->root_;
        } else if (&entity->entityManager_ == this) {
            return &this->nodes_.at(entity->id_);
        } else {
            throw std::invalid_argument("Parent Entity must be created by the same Entity Manager.");
        }
    }

    EntityManager::Node* EntityManager::getNodeFromEntity(const Entity* entity) {
        if (!entity) {
            return &this->root_;
        } else if (&entity->entityManager_ == this) {
            return &this->nodes_.at(entity->id_);
        } else {
            throw std::invalid_argument("Parent Entity must be created by the same Entity Manager.");
        }
    }

    std::vector<Entity*> EntityManager::getEntitiesHelper(
            const std::function<bool(Entity*)>& pred, bool onlyActive,
            size_t limit, const Entity* parent, bool recurse) const {

        auto entities = std::vector<Entity*>();
        auto nodeStack = std::stack<const Node*>();
        const Node* parentNode = this->getNodeFromEntity(parent);
        std::for_each(parentNode->children.rbegin(), parentNode->children.rend(), [&nodeStack](Node *child) {
            nodeStack.push(child);
        });
        while (!nodeStack.empty()) {
            // Retrieve the next node
            const Node* next = nodeStack.top();
            nodeStack.pop();

            // Skip if not active
            if (onlyActive && (next != &this->root_) && !next->ent->active_) continue;

            // Add Entity pointer to vector if current Entity satisfy conditions
            if (!pred || pred(next->ent)) {
                entities.push_back(next->ent);
                if ((--limit) == 0) break;
            }

            // Add children to stack
            if (recurse) {
                std::for_each(next->children.rbegin(), next->children.rend(), [&nodeStack](Node *child) {
                    nodeStack.push(child);
                });
            }
        }
        return entities;
    }

    const Game* EntityManager::getGame() const noexcept {
        return this->scene_.getGame();
    }

    const std::vector<const CCamera*>& EntityManager::getCameras() const noexcept {
        return this->cameras_;
    }
#ifdef _MSC_VER  // MSVC
#pragma warning(push)
#pragma warning(disable : 4702)  // Unreachable code
#endif

    bool EntityManager::screenToWorldPosition(const Vec2f& screenPosition, Vec2f& worldPosition) const noexcept {
        if (this->cameras_.empty()) return false;

        Vec2f percentWindowSize = this->getGame()->windowSize() * 0.01f;

        // Calculate the viewport and fov of each camera, in reverse order
        for (const CCamera* camera : this->cameras_ | std::views::reverse) {  // MSVC falsely detects unreachable code
            Vec2f viewportPos(camera->viewport.x.calc(1.0f, percentWindowSize.x, percentWindowSize.y),
                             camera->viewport.y.calc(1.0f, percentWindowSize.x, percentWindowSize.y));
            Vec2f viewportSize(camera->viewport.w.calc(1.0f, percentWindowSize.x, percentWindowSize.y),
                              camera->viewport.h.calc(1.0f, percentWindowSize.x, percentWindowSize.y));

            // Check if the screen position is within the camera's viewport
            if (screenPosition.x < viewportPos.x || screenPosition.x > viewportPos.x + viewportSize.x ||
                screenPosition.y < viewportPos.y || screenPosition.y > viewportPos.y + viewportSize.y) {
                return false;
            }

            // Calculate the world position
            Vec2f cameraCenter =
                    camera->getEntity().getComponent<CTransform2D>()->getWorldTransform().getTranslationComponent() +
                    camera->anchor.to<2>();
            Vec2f fovSize(camera->fovW.calc(1.0f, viewportSize.x / 100, viewportSize.y / 100) * (1 / camera->scale),
                         camera->fovH.calc(1.0f, viewportSize.x / 100, viewportSize.y / 100) * (1 / camera->scale));
            Vec2f scale(fovSize.x / viewportSize.x, fovSize.y / viewportSize.y);
            worldPosition = cameraCenter - fovSize * 0.5 + (screenPosition - viewportPos) * scale;
            return true;
        }

        // No camera contains the screen position
        return false;
    }

#ifdef _MSC_VER  // MSVC
#pragma warning(pop)
#endif
}
