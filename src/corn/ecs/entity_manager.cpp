#include <algorithm>
#include <stack>
#include <corn/core/scene.h>
#include <corn/ecs/entity_manager.h>
#include <corn/util/exceptions.h>
#include "../event/event_args_extend.h"

namespace corn {
    EntityManager::Node::Node(Entity* ent, Node* parent) noexcept : ent(ent), parent(parent), children(), dirty(false) {}

    EntityManager::EntityManager(Scene& scene) noexcept : scene_(scene), root_(nullptr, nullptr), nodes_() {
        // Listen to zorder change events
        this->zOrderEventID_ = this->scene_.getEventManager().addListener(
                "corn::game::ecs::zorder", [this](const EventArgs& args) {
                    const auto& _args = dynamic_cast<const EventArgsEntityZOrderChange&>(args);
                    if (!_args.entity) return;
                    Node* node = this->getNodeFromEntity(_args.entity);
                    node->parent->dirty = true;
                });
        // Listen to add/remove camera events
        this->cameraEventID_ = this->scene_.getEventManager().addListener(
                "corn::game::ecs::camera", [this](const EventArgs& args) {
                    const auto& _args = dynamic_cast<const EventArgsCamera&>(args);
                    if (!_args.camera || &_args.camera->getEntityManager() != this) return;
                    switch (_args.eventType) {
                        case CameraEventType::ADD:
                            this->cameras_.push_back(_args.camera);
                            break;
                        case CameraEventType::REMOVE:
                            this->cameras_.erase(
                                    std::remove(this->cameras_.begin(), this->cameras_.end(), _args.camera),
                                    this->cameras_.end());
                            break;
                    }
                });
    }

    EntityManager::~EntityManager() {
        // Unregister event listeners
        this->scene_.getEventManager().removeListener(this->zOrderEventID_);
        this->scene_.getEventManager().removeListener(this->cameraEventID_);

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
        static Entity::EntityID entID = 0;
        while (this->nodes_.contains(entID)) {
            entID++;
        }
        auto* entity = new Entity(entID++, name, *this);

        // Create the node
        this->nodes_.emplace(entity->id_, Node(entity, parentNode));
        parentNode->children.push_back(&this->nodes_.at(entity->id_));
        parentNode->dirty = true;

        return *entity;
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
        this->destroyNode(node);
        // Removes relation (parent --> node)
        parent->children.erase(
                std::remove(parent->children.begin(), parent->children.end(), node),
                parent->children.end());
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

    void EntityManager::tidy() noexcept {
        if (this->root_.dirty) {
            this->root_.dirty = false;
            std::stable_sort(this->root_.children.begin(), this->root_.children.end(),
                             [](Node* left, Node* right) {
                                 auto lTrans = left->ent->getComponent<CTransform2D>();
                                 auto rTrans = right->ent->getComponent<CTransform2D>();
                                 if (lTrans == nullptr) return true;
                                 else if (rTrans == nullptr) return false;
                                 else return lTrans->getZOrder() < rTrans->getZOrder();
                             });
        }

        for (auto& [id, node] : this->nodes_) {
            if (!node.dirty) continue;
            node.dirty = false;
            std::stable_sort(node.children.begin(), node.children.end(),
                             [](Node* left, Node* right) {
                                 auto lTrans = left->ent->getComponent<CTransform2D>();
                                 auto rTrans = right->ent->getComponent<CTransform2D>();
                                 if (lTrans == nullptr) return true;
                                 else if (rTrans == nullptr) return false;
                                 else return lTrans->getZOrder() < rTrans->getZOrder();
                             });
        }
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
}
