#include <corn/ui/ui_manager.h>
#include <corn/ui/ui_widget.h>

namespace corn {
    UIWidget::UIWidget(UIType type, WidgetID id, std::string name, UIManager& uiManager)
        : active(true), uiManager(uiManager), type(type), id(id), name(std::move(name)),
          background(Color::rgb(255, 255, 255, 0)), opacity(255) {

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

    const Expression<5>& UIWidget::getX() const {
        return this->x;
    }

    const Expression<5>& UIWidget::getY() const {
        return this->y;
    }

    const Expression<5>& UIWidget::getW() const {
        return this->w;
    }

    const Expression<5>& UIWidget::getH() const {
        return this->h;
    }

    void UIWidget::setX(const std::string& expression) {
        static const std::array<std::string, 5> units = {"px", "%pw", "%ph", "%w", "%h"};
        this->x = Expression(expression, units);
    }

    void UIWidget::setY(const std::string& expression) {
        static const std::array<std::string, 5> units = {"px", "%pw", "%ph", "%w", "%h"};
        this->y = Expression(expression, units);
    }

    void UIWidget::setW(const std::string& expression) {
        static const std::array<std::string, 5> units = {"px", "%pw", "%ph", "%w", "%h"};
        this->w = Expression(expression, units);
    }

    void UIWidget::setH(const std::string& expression) {
        static const std::array<std::string, 5> units = {"px", "%pw", "%ph", "%w", "%h"};
        this->h = Expression(expression, units);
    }

    float UIWidget::getNaturalWidth() const {
        return 0.0f;
    }

    float UIWidget::getNaturalHeight() const {
        return 0.0f;
    }
}
