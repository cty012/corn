#include <corn/ui/ui_element.h>
#include <corn/ui/ui_manager.h>

namespace corn {
    UIElement::UIElement(UIManager &uiManager) : uiManager(uiManager) {}

    void UIElement::destroy() {
        uiManager.destroyElement(*this);
    }
}
