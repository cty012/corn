#include <utility>
#include <corn/core/scene.h>
#include <corn/ecs/component.h>
#include <corn/ecs/entity.h>
#include <corn/ecs/entity_manager.h>
#include <corn/geometry/operations.h>
#include <corn/media/image.h>
#include "../event/event_args_extend.h"
#include "../render/polygon_renderer.h"

namespace corn {
    Component::Component(Entity& entity) noexcept
            : active(true), entityManager_(entity.getEntityManager()), entityID_(entity.getID()) {}

    Component::~Component() = default;

    Entity& Component::getEntity() const noexcept {
        return *this->entityManager_.getEntityByID(this->entityID_);
    }

    EntityManager& Component::getEntityManager() const noexcept {
        return this->entityManager_;
    }

    Scene& Component::getScene() const noexcept {
        return this->entityManager_.getScene();
    }

    const Game* Component::getGame() const noexcept {
        return this->entityManager_.getGame();
    }

    CTransform2D::CTransform2D(Entity& entity) noexcept : Component(entity), dilation(1.0f, 1.0f), zOrder_(0) {}

    CTransform2D::CTransform2D(Entity& entity, Vec2f translation) noexcept
            : Component(entity), translation(std::move(translation)), dilation(1.0f, 1.0f), zOrder_(0) {}

    CTransform2D::CTransform2D(Entity& entity, Vec2f translation, Deg rotation, Vec2f dilation) noexcept
            : Component(entity), translation(std::move(translation)), rotation(rotation), dilation(std::move(dilation)), zOrder_(0) {}

    Transform2D CTransform2D::getLocalTransform() const noexcept {
        return { this->translation, this->rotation, this->dilation };
    }

    Transform2D CTransform2D::getParentTransform() const noexcept {
        Transform2D parentTransform;
        for (Entity* ancestor = this->getEntity().getParent(); ancestor; ancestor = ancestor->getParent()) {
            auto* ancestorTransform = ancestor->getComponent<CTransform2D>();
            if (ancestorTransform) {
                parentTransform = ancestorTransform->getLocalTransform() * parentTransform;
            }
        }
        return parentTransform;
    }

    Transform2D CTransform2D::getWorldTransform() const noexcept {
        return this->getParentTransform() * this->getLocalTransform();
    }

    void CTransform2D::setWorldTranslation(const Vec2f& worldTranslation) noexcept {
        this->translation = this->getParentTransform().inv().mapPoint(worldTranslation);
    }

    void CTransform2D::addWorldTranslationOffset(const Vec2f& offset) noexcept {
        this->translation += this->getParentTransform().inv().mapVector(offset);
    }

    int CTransform2D::getZOrder() const noexcept {
        return this->zOrder_;
    }

    void CTransform2D::setZOrder(int zOrder) noexcept {
        this->zOrder_ = zOrder;
        this->getScene().getEventManager().emit(EventArgsEntityZOrderChange(&this->getEntity()));
    }

    CBBox::CBBox(Entity& entity, Vec2f tl, Vec2f br) noexcept : Component(entity), tl(std::move(tl)), br(std::move(br)) {}

    bool CBBox::overlapWith(const CBBox& other) const noexcept {
        auto* transform1 = this->getEntity().getComponent<CTransform2D>();
        auto* transform2 = other.getEntity().getComponent<CTransform2D>();
        if (!transform1 || !transform2) return false;
        Vec2f worldTranslation1 = transform1->getWorldTransform().getTranslationComponent();
        Vec2f worldTranslation2 = transform2->getWorldTransform().getTranslationComponent();
        Vec2f tl1 = this->tl + worldTranslation1;
        Vec2f br1 = this->br + worldTranslation1;
        Vec2f tl2 = other.tl + worldTranslation2;
        Vec2f br2 = other.br + worldTranslation2;
        bool xDirection = std::min(br1.x, br2.x) > std::max(tl1.x, tl2.x);
        bool yDirection = std::min(br1.y, br2.y) > std::max(tl1.y, tl2.y);
        return xDirection && yDirection;
    }

    CLines::CLines(Entity& entity, std::vector<Vec2f> vertices, float thickness, Color color, bool closed) noexcept
            : Component(entity), vertices(std::move(vertices)), thickness(thickness), color(std::move(color)), closed(closed) {}

    CPolygon::CPolygon(Entity& entity, Polygon polygon, float thickness, Color color, RenderType renderType) noexcept
            : Component(entity), thickness(thickness), color(std::move(color)), polygon_(std::move(polygon)),
              renderType_(renderType), renderer_(nullptr), rendererDirty_(true) {}

    CPolygon::~CPolygon() {
        delete this->renderer_;
    }

    const Polygon& CPolygon::getPolygon() const noexcept {
        return this->polygon_;
    }

    void CPolygon::setPolygon(Polygon polygon) noexcept {
        this->polygon_ = std::move(polygon);
        this->rendererDirty_ = true;
    }

    CPolygon::RenderType CPolygon::getRenderType() const noexcept {
        return this->renderType_;
    }

    void CPolygon::setRenderType(RenderType renderType) noexcept {
        if (this->renderType_ == renderType) {
            return;
        }
        this->renderType_ = renderType;
        this->rendererDirty_ = true;
    }

    CPolygon::Renderer* CPolygon::getPolygonRenderer() const {
        if (!this->rendererDirty_) {
            return this->renderer_;
        }

        // Create/delete the renderer
        switch (this->renderType_) {
            case RenderType::STATIC:
                if (!this->renderer_) {
                    this->renderer_ = new StaticPolygonRenderer();
                }
                break;
            case RenderType::DYNAMIC:
                if (!this->renderer_) {
                    this->renderer_ = new DynamicPolygonRenderer();
                }
                break;
            case RenderType::TRANSIENT:
                if (this->renderer_) {
                    delete this->renderer_;
                    this->renderer_ = nullptr;
                }
                return nullptr;
        }

        // Variables
        const std::vector<Vec2f>& vertices_ = this->polygon_.getVertices();
        const std::vector<std::vector<Vec2f>>& holes_ = this->polygon_.getHoles();
        const std::vector<Vec2f>& verticesFlat_ = this->polygon_.getVerticesFlat();
        const std::vector<size_t>& triangleIndices_ = this->polygon_.getTriangleIndices();

        // Vertices
        std::vector<Vertex2D> vertices;
        vertices.reserve(vertices.size());
        for (const Vec2f& vertex : verticesFlat_) {
            vertices.emplace_back(vertex.x, vertex.y);
        }

        // Edge indices
        std::vector<uint16_t> edgeIndices;
        std::vector<uint32_t> ringSizes;
        size_t baseIndex = 0;
        for (size_t i = 0; i < holes_.size() + 1; i++) {
            size_t ringSize = (i == 0) ? vertices_.size() : holes_[i - 1].size();
            for (size_t j = 0; j < ringSize; j++) {
                edgeIndices.push_back(static_cast<uint16_t>(baseIndex + j));
            }
            edgeIndices.push_back(static_cast<uint16_t>(baseIndex));
            baseIndex += ringSize;
            ringSizes.push_back(static_cast<uint32_t>(ringSize));
        }

        // Fill indices
        std::vector<uint16_t> fillIndices;
        fillIndices.reserve(triangleIndices_.size());
        for (size_t index : triangleIndices_) {
            fillIndices.push_back(static_cast<uint16_t>(index));
        }

        // Update the renderer
        this->renderer_->update(vertices, edgeIndices, ringSizes, fillIndices);

        // Update the renderer dirty flag
        this->rendererDirty_ = false;

        return this->renderer_;
    }

    CSprite::CSprite(Entity& entity, Image *image, Vec2f location) noexcept
            : Component(entity), image(image), location(std::move(location)) {}

    CSprite::~CSprite() {
        delete this->image;
    }

    CText::CText(Entity& entity, const TextRender& textRender) noexcept : Component(entity), textRender(textRender) {
        this->setX("0px");
        this->setY("0px");
    }

    CText::CText(Entity& entity, const TextRender& textRender, const std::string& x, const std::string& y) noexcept
            : Component(entity), textRender(textRender) {

        this->setX(x);
        this->setY(y);
    }

    float CText::getX() const noexcept {
        Vec2f percSize = this->textRender.getSize() * 0.01f;
        return this->x_.calc(1.0f, percSize.x, percSize.y);
    }

    void CText::setX(const std::string& x) noexcept {
        static const std::array<std::string, 3> units = { "px", "%w", "%h" };
        this->x_ = Expression(x, units);
    }

    float CText::getY() const noexcept {
        Vec2f percSize = this->textRender.getSize() * 0.01f;
        return this->y_.calc(1.0f, percSize.x, percSize.y);
    }

    void CText::setY(const std::string& y) noexcept {
        static const std::array<std::string, 3> units = { "px", "%w", "%h" };
        this->y_ = Expression(y, units);
    }

    CMovement2D::CMovement2D(Entity& entity, Vec2f velocity, float angularVelocity) noexcept
            : Component(entity), velocity(std::move(velocity)), angularVelocity(angularVelocity) {}

    Vec2f CMovement2D::getWorldVelocity() const noexcept {
        auto* cTransform2D = this->getEntity().getComponent<CTransform2D>();
        if (!cTransform2D) {
            return Vec2f::O();
        }

        // Initialize the translation and velocity
        Vec2f worldTranslation = cTransform2D->getLocalTransform().getTranslationComponent();
        Vec2f worldVelocity = this->velocity;

        // Iterate through all ancestors and apply their transforms
        for (Entity* ancestor = this->getEntity().getParent(); ancestor; ancestor = ancestor->getParent()) {
            auto* ancestorCTransform = ancestor->getComponent<CTransform2D>();
            auto* ancestorCMovement = ancestor->getComponent<CMovement2D>();
            if (ancestorCTransform && ancestorCMovement) {
                // Apply the ancestor's transform to the velocity
                Transform2D ancestorTransform = ancestorCTransform->getLocalTransform();
                worldTranslation = ancestorTransform.mapPoint(worldTranslation);
                worldVelocity = ancestorTransform.mapVector(worldVelocity);

                // Add the ancestor's velocity to the world velocity
                worldVelocity += ancestorCMovement->velocity;

                // Add the ancestor's angular velocity to the world velocity
                worldVelocity += Vec2f(-worldTranslation.y, worldTranslation.x) * ancestorCMovement->angularVelocity;
            }
        }

        return worldVelocity;
    }

    void CMovement2D::setWorldVelocity(const Vec2f& worldVelocity) noexcept {
        this->addWorldVelocityOffset(worldVelocity - this->getWorldVelocity());
    }

    void CMovement2D::addWorldVelocityOffset(const Vec2f& offset) noexcept {
        auto* cTransform2D = this->getEntity().getComponent<CTransform2D>();
        if (cTransform2D) {
            this->velocity += cTransform2D->getParentTransform().inv().mapVector(offset);
        }
    }

    CGravity2D::CGravity2D(Entity& entity, float scale) noexcept : Component(entity), scale(scale) {}

    CCamera::CCamera(Entity& entity, CameraType type, Color background) noexcept
            : Component(entity), cameraType(type), background(std::move(background)), opacity(255), scale(1.0f) {

        this->setViewport("0px", "0px", "100%ww", "100%wh");
        this->setFov("100%vw", "100%vh");
        this->getScene().getEventManager().emit(EventArgsCamera(CameraEventType::ADD, this));
    }

    CCamera::~CCamera() {
        this->getScene().getEventManager().emit(EventArgsCamera(CameraEventType::REMOVE, this));
    }

    void CCamera::setViewport(const std::string& x, const std::string& y, const std::string& w, const std::string& h) {
        static const std::array<std::string, 3> units = { "px", "%ww", "%wh" };
        if (!x.empty()) {
            this->viewport.x = Expression(x, units);
        }
        if (!y.empty()) {
            this->viewport.y = Expression(y, units);
        }
        if (!w.empty()) {
            this->viewport.w = Expression(w, units);
        }
        if (!h.empty()) {
            this->viewport.h = Expression(h, units);
        }
    }

    void CCamera::setFov(const std::string& w, const std::string& h) {
        static const std::array<std::string, 3> units = { "px", "%vw", "%vh" };
        if (!w.empty()) {
            this->fovW = Expression(w, units);
        }
        if (!h.empty()) {
            this->fovH = Expression(h, units);
        }
    }
}
