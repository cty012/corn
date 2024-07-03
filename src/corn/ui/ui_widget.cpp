#include <corn/core/scene.h>
#include <corn/ui/ui_manager.h>
#include <corn/ui/ui_widget.h>
#include "../event/event_args_extend.h"

namespace corn {
    UIWidget::UIWidget(UIType type, WidgetID id, std::string name, UIManager& uiManager) noexcept
            : type_(type), id_(id), name_(std::move(name)), active_(true), uiManager_(uiManager),
            geometry_(UIGeometry::DEFAULT), x_(), y_(), w_(), h_(), independent_(), overflow_(UIOverflow::DISPLAY),
            background_(Color::rgb(255, 255, 255, 0)), opacity_(255), zOrder_(0),
            keyboardInteractable_(false), mouseInteractable_(false) {

        this->setX("0px");
        this->setY("0px");
        this->setW("100%nw");
        this->setH("100%nh");
        this->eventManager_ = EventManager::addPrivateRoom();
    }

    UIWidget::UIWidget(UIWidget::WidgetID id, std::string name, UIManager& uiManager) noexcept
            : UIWidget(UIType::PANEL, id, std::move(name), uiManager) {}

    UIWidget::~UIWidget() {
        EventManager::removePrivateRoom(this->eventManager_);
    }

    UIType UIWidget::getType() const noexcept {
        return this->type_;
    }

    UIWidget::WidgetID UIWidget::getID() const noexcept {
        return this->id_;
    }

    const std::string& UIWidget::getName() const noexcept {
        return this->name_;
    }

    void UIWidget::setName(std::string name) noexcept {
        this->name_ = std::move(name);
    }

    bool UIWidget::isActive() const noexcept {
        return this->active_;
    }

    void UIWidget::setActive(bool active) noexcept {
        this->active_ = active;
    }

    bool UIWidget::isActiveInWorld() const noexcept {
        const UIWidget* current = this;
        while (current) {
            if (!current->active_) return false;
            current = current->getParent();
        }
        return true;
    }

    UIManager& UIWidget::getUIManager() const noexcept {
        return this->uiManager_;
    }

    EventManager& UIWidget::getEventManager() const noexcept {
        return *this->eventManager_;
    }

    Scene& UIWidget::getScene() const noexcept {
        return this->uiManager_.getScene();
    }

    const Game* UIWidget::getGame() const noexcept {
        return this->uiManager_.getGame();
    }

    void UIWidget::destroy() noexcept {
        uiManager_.destroyWidget(*this);
    }

    UIWidget* UIWidget::getParent() const noexcept {
        UIManager::Node* parent = this->uiManager_.nodes_.at(this->id_).parent;
        return parent ? parent->widget : nullptr;
    }

    UIGeometry UIWidget::getActualGeometry() const noexcept {
        switch (this->geometry_) {
            case UIGeometry::DEFAULT:
                return this->independent_[0] && this->independent_[1] && this->independent_[2] && this->independent_[3] ?
                    UIGeometry::INDEPENDENT : UIGeometry::DEPENDENT;
            default:
                return this->geometry_;
        }
    }

    int UIWidget::getZOrder() const noexcept {
        return this->zOrder_;
    }

    void UIWidget::setZOrder(int zOrder) noexcept {
        this->zOrder_ = zOrder;
        this->getScene().getEventManager().emit(EventArgsWidgetZOrderChange(this));
    }

    const Expression<5>& UIWidget::getX() const noexcept {
        return this->x_;
    }

    void UIWidget::setX(const std::string& expression) {
        static const std::array<std::string, 5> units = { "px", "%pw", "%ph", "%nw", "%nh" };
        this->x_ = Expression(expression, units);
        this->independent_[0] = expression.find("%p") == std::string::npos;
    }

    const Expression<5>& UIWidget::getY() const noexcept {
        return this->y_;
    }

    void UIWidget::setY(const std::string& expression) {
        static const std::array<std::string, 5> units = { "px", "%pw", "%ph", "%nw", "%nh" };
        this->y_ = Expression(expression, units);
        this->independent_[1] = expression.find("%p") == std::string::npos;
    }

    const Expression<5>& UIWidget::getW() const noexcept {
        return this->w_;
    }

    void UIWidget::setW(const std::string& expression) {
        static const std::array<std::string, 5> units = { "px", "%pw", "%ph", "%nw", "%nh" };
        this->w_ = Expression(expression, units);
        this->independent_[2] = expression.find("%p") == std::string::npos;
    }

    const Expression<5>& UIWidget::getH() const noexcept {
        return this->h_;
    }

    void UIWidget::setH(const std::string& expression) {
        static const std::array<std::string, 5> units = { "px", "%pw", "%ph", "%nw", "%nh" };
        this->h_ = Expression(expression, units);
        this->independent_[3] = expression.find("%p") == std::string::npos;
    }

    UIOverflow UIWidget::getOverflow() const noexcept {
        return this->overflow_;
    }

    void UIWidget::setOverflow(UIOverflow overflow) noexcept {
        this->overflow_ = overflow;
    }

    const Color& UIWidget::getBackground() const noexcept {
        return this->background_;
    }

    unsigned char UIWidget::getOpacity() const noexcept {
        return this->opacity_;
    }

    void UIWidget::setBackground(Color background) noexcept {
        this->background_ = background;
    }

    void UIWidget::setOpacity(unsigned char opacity) noexcept {
        this->opacity_ = opacity;
    }

    bool UIWidget::isKeyboardInteractable() const noexcept {
        return this->keyboardInteractable_;
    }

    void UIWidget::setKeyboardInteractable(bool interactable) noexcept {
        this->keyboardInteractable_ = interactable;
    }

    bool UIWidget::isMouseInteractable() const noexcept {
        return this->mouseInteractable_;
    }

    void UIWidget::setMouseInteractable(bool interactable) noexcept {
        this->mouseInteractable_ = interactable;
    }
}
