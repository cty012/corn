#include <corn/ui/ui_manager.h>
#include <corn/ui/ui_widget.h>

#include <utility>

namespace corn {
    UIWidget::UIWidget(UIType type, WidgetID id, std::string name, UIManager& uiManager)
        : active(true), uiManager(uiManager), type(type), id(id), name(std::move(name)), opacity(255) {

        this->setX("0px");
        this->setY("0px");
        this->setW("100%w");
        this->setH("100%h");
    }

    UIWidget::UIWidget(UIWidget::WidgetID id, std::string name, UIManager& uiManager)
        : UIWidget(UIType::PANEL, id, std::move(name), uiManager) {}

    UIWidget::~UIWidget() = default;

    void UIWidget::destroy() {
        uiManager.destroyWidget(*this);
    }

    bool UIWidget::isActive() const {
        const UIWidget* current = this;
        while (current) {
            if (!this->active) return false;
            current = current->getParent();
        }
        return true;
    }

    UIWidget* UIWidget::getParent() const {
        UIManager::Node* parent = this->uiManager.nodes.at(this->id).parent;
        return parent ? parent->widget : nullptr;
    }

    const Expression<3>& UIWidget::getX() const {
        return this->x;
    }

    const Expression<3>& UIWidget::getY() const {
        return this->y;
    }

    const Expression<3>& UIWidget::getW() const {
        return this->w;
    }

    const Expression<3>& UIWidget::getH() const {
        return this->h;
    }

    void UIWidget::setX(const std::string& expression) {
        static const std::array<std::string, 3> units = {"px", "%w", "%h"};
        this->x = Expression(expression, units);
    }

    void UIWidget::setY(const std::string& expression) {
        static const std::array<std::string, 3> units = {"px", "%w", "%h"};
        this->y = Expression(expression, units);
    }

    void UIWidget::setW(const std::string& expression) {
        static const std::array<std::string, 3> units = {"px", "%w", "%h"};
        this->w = Expression(expression, units);
    }

    void UIWidget::setH(const std::string& expression) {
        static const std::array<std::string, 3> units = {"px", "%w", "%h"};
        this->h = Expression(expression, units);
    }
}
