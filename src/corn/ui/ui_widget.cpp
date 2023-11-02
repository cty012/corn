#include <corn/ui/ui_manager.h>
#include <corn/ui/ui_widget.h>

namespace corn {
    UIWidget::UIWidget(WidgetID id, std::string name, UIManager &uiManager)
        : active(true), uiManager(uiManager), id(id), name(std::move(name)), opacity(255) {}

    void UIWidget::destroy() {
        uiManager.destroyWidget(*this);
    }
}
