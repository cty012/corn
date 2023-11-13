#include <corn/ui/ui_manager.h>
#include <corn/ui/ui_widget.h>

#include <utility>

namespace corn {
    UIWidget::UIWidget(UIType type, WidgetID id, std::string name, UIManager &uiManager)
        : active(true), uiManager(uiManager), type(type), id(id), name(std::move(name)), opacity(255) {}

    UIWidget::UIWidget(UIWidget::WidgetID id, std::string name, UIManager& uiManager)
        : UIWidget(UIType::LABEL, id, std::move(name), uiManager) {}

    void UIWidget::destroy() {
        uiManager.destroyWidget(*this);
    }
}
