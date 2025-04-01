#include <gtest/gtest.h>
#include <corn/ui/ui_manager.h>
#include "dummy_scene.h"
#include "utility.h"

namespace corn::test::ui_manager {
    std::array<UIWidget*, 4> setupUI(UIManager& uiManager) {
        auto* widget1 = &uiManager.createWidget<UIWidget>("widget1", nullptr);
        auto* widget2 = &uiManager.createWidget<UIWidget>("widget2", widget1);
        auto* widget3 = &uiManager.createWidget<UIWidget>("widget3", widget1);
        auto* widget4 = &uiManager.createWidget<UIWidget>("widget4", widget2);

        // Geometry
        widget1->setW("100%pw"); widget1->setH("100%pw");
        widget2->setW("50%pw"); widget2->setH("50%ph");
        widget3->setX("50%pw"); widget3->setY("50%ph"); widget3->setW("50%pw"); widget3->setH("50%ph");
        widget4->setX("50px"); widget4->setY("50px"); widget4->setW("100%pw - 100px"); widget4->setH("100%ph - 100px");

        return { widget1, widget2, widget3, widget4 };
    }

    void addListeners(UIWidget* widget, std::unordered_map<UIWidget*, std::unordered_map<std::string, bool>>& status) {
        widget->getEventManager().addListener(
                "corn::ui::keyboard",
                [widget, &status](const EventArgs&) {
                    status[widget]["onkey"] = true;
                });
        widget->getEventManager().addListener(
                "corn::ui::text",
                [widget, &status](const EventArgs&) {
                    status[widget]["ontext"] = true;
                });
        widget->getEventManager().addListener(
                "corn::ui::onclick",
                [widget, &status](const EventArgs&) {
                    status[widget]["onclick"] = true;
                });
        widget->getEventManager().addListener(
                "corn::ui::onhover",
                [widget, &status](const EventArgs&) {
                    status[widget]["onhover"] = true;
                });
        widget->getEventManager().addListener(
                "corn::ui::onenter",
                [widget, &status](const EventArgs&) {
                    status[widget]["onenter"] = true;
                });
        widget->getEventManager().addListener(
                "corn::ui::onexit",
                [widget, &status](const EventArgs&) {
                    status[widget]["onexit"] = true;
                });
        widget->getEventManager().addListener(
                "corn::ui::onscroll",
                [widget, &status](const EventArgs&) {
                    status[widget]["onscroll"] = true;
                });
        widget->getEventManager().addListener(
                "corn::ui::onfocus",
                [widget, &status](const EventArgs&) {
                    status[widget]["onfocus"] = true;
                });
        widget->getEventManager().addListener(
                "corn::ui::onunfocus",
                [widget, &status](const EventArgs&) {
                    status[widget]["onunfocus"] = true;
                });
    }

    TEST(UIManager, widget_management) {
        DummyScene scene;
        auto [widget1, widget2, widget3, widget4] = setupUI(scene.getUIManager());
        UIWidget* result;
        std::vector<UIWidget*> results, ans;

        // Find widget1
        result = scene.getUIManager().getWidgetByName("widget1");
        EXPECT_EQ(result, widget1);

        // Find widget3 as descendant of widget2
        result = scene.getUIManager().getWidgetByName("widget3", widget2);
        EXPECT_EQ(result, nullptr);

        // Find widget4 as descendant of widget1
        result = scene.getUIManager().getWidgetByName("widget4", widget1);
        EXPECT_EQ(result, widget4);

        // Find widget4 as child of widget1
        result = scene.getUIManager().getWidgetByName("widget4", widget1, false);
        EXPECT_EQ(result, nullptr);

        // Find all descendants of widget3
        results = scene.getUIManager().getAllWidgets(widget3);
        ans = std::vector<UIWidget*>{};
        EXPECT_EQ(results, ans);

        // Find all descendants of widget1
        results = scene.getUIManager().getAllWidgets(widget1);
        ans = std::vector<UIWidget*>{ widget2, widget4, widget3 };
        EXPECT_EQ(results, ans);

        // Find all children of widget1
        results = scene.getUIManager().getAllWidgets(widget1, false);
        ans = std::vector<UIWidget*>{ widget2, widget3 };
        EXPECT_EQ(results, ans);

        // Destroy widget2
        widget2->destroy();
        result = scene.getUIManager().getWidgetByName("widget2");
        EXPECT_EQ(result, nullptr);
        result = scene.getUIManager().getWidgetByName("widget4");
        EXPECT_EQ(result, nullptr);
        result = scene.getUIManager().getWidgetByName("widget1");
        EXPECT_EQ(result, widget1);
        result = scene.getUIManager().getWidgetByName("widget3");
        EXPECT_EQ(result, widget3);
    }

    TEST(UIManager, layout) {
        DummyScene scene;
        auto [widget1, widget2, widget3, widget4] = setupUI(scene.getUIManager());

        scene.getUIManager().calcGeometry(corn::Vec2f(1000, 1000));
        EXPECT_VEC4_EQ(scene.getUIManager().getCachedGeometry(widget1), Vec4f(0, 0, 1000, 1000));
        EXPECT_VEC4_EQ(scene.getUIManager().getCachedGeometry(widget2), Vec4f(0, 0, 500, 500));
        EXPECT_VEC4_EQ(scene.getUIManager().getCachedGeometry(widget3), Vec4f(500, 500, 500, 500));
        EXPECT_VEC4_EQ(scene.getUIManager().getCachedGeometry(widget4), Vec4f(50, 50, 400, 400));
    }

    TEST(UIManager, ui_event) {
        DummyScene scene;
        UIManager& uiManager = scene.getUIManager();
        auto [widget1, widget2, widget3, widget4] = setupUI(uiManager);

        // Interactions
        widget2->setMouseInteractable(true);
        widget3->setKeyboardInteractable(true);
        widget3->setMouseInteractable(true);
        widget4->setMouseInteractable(true);

        // Listeners
        std::unordered_map<UIWidget*, std::unordered_map<std::string, bool>> status;
        std::function<void(void)> clearStatus = [&status]() { status.clear(); };
        addListeners(widget1, status);
        addListeners(widget2, status);
        addListeners(widget3, status);
        addListeners(widget4, status);

        // Click & focus
        clearStatus();
        EXPECT_EQ(uiManager.getFocusedWidget(), nullptr);
        uiManager.calcGeometry(Vec2f(1000, 1000));
        uiManager.onClick(EventArgsMouseButton(Mouse::LEFT, ButtonEvent::DOWN, Vec2f(250, 250)));
        EXPECT_EQ(uiManager.getFocusedWidget(), widget4);
        UnorderedMapsEqual(status[widget1], {});
        UnorderedMapsEqual(status[widget2], {{ "onclick", true }});
        UnorderedMapsEqual(status[widget3], {});
        UnorderedMapsEqual(status[widget4], {{ "onclick", true }, { "onfocus", true }});

        clearStatus();
        uiManager.onClick(EventArgsMouseButton(Mouse::LEFT, ButtonEvent::DOWN, Vec2f(750, 750)));
        EXPECT_EQ(uiManager.getFocusedWidget(), widget3);
        UnorderedMapsEqual(status[widget1], {});
        UnorderedMapsEqual(status[widget2], {});
        UnorderedMapsEqual(status[widget3], {{ "onclick", true }, { "onfocus", true }});
        UnorderedMapsEqual(status[widget4], {{ "onunfocus", true }});
    }
}
