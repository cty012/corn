#include <bgfx/bgfx.h>
#include <corn/geometry/vec.h>
#include <corn/media/rich_text_frame.h>
#include <corn/util/color.h>
#include "../../render/interface_helper.h"
#include "../../render/polygon_renderer.h"
#include "win/rich_text_renderer.h"

namespace corn {
    void draw(
            bgfx::ViewId viewID,
            const CTransform2D& cTransform, const CText& cText,
            const Transform2D& worldToCameraTransform, const Shader& bitmapShader) {

        Transform2D transform =
                worldToCameraTransform *
                cTransform.getWorldTransform() *
                Transform2D::translate(Vec2f(cText.getX(), cText.getY()));
        RichTextRenderer* richTextRenderer = cText.getRichTextFrame().getRichTextRenderer();
        richTextRenderer->setTransform(transform);
        richTextRenderer->draw(viewID, bitmapShader);
    }

    void drawUI(
            bgfx::ViewId viewID,
            UILabel& uiLabel, float maxWidth,
            const Transform2D& transform, const Shader& bitmapShader) {

        RichTextFrame& richTextFrame = uiLabel.getRichTextFrame();
        richTextFrame.setMaxWidth(maxWidth);
        RichTextRenderer* richTextRenderer = richTextFrame.getRichTextRenderer();
        richTextRenderer->setTransform(transform);
        richTextRenderer->draw(viewID, bitmapShader);
    }

    void drawDebug(
            bgfx::ViewId viewID,
            const RichTextFrame& debugText, float xPad, float yPad, float dpiScale,
            const Shader& polygonShader, const Shader& bitmapShader) {

        Transform2D transform = Transform2D::dilate(Vec2f(dpiScale, dpiScale)) * Transform2D::translate(Vec2f(xPad, yPad));

        // Render the background
        const Vec2f& bgSize = (debugText.getSize() + Vec2f(xPad, yPad) * 2.0f) * dpiScale;
        std::vector<Vertex2D> vertices = {
            { 0, 0 },
            { bgSize.x, 0 },
            { bgSize.x, bgSize.y },
            { 0, bgSize.y },
    };
        std::vector<uint16_t> indices = { 0, 1, 2, 0, 2, 3 };
        TransientPolygonRenderer::draw(viewID, polygonShader, vertices, indices, Color::rgb(0, 0, 0, 200), Transform2D::I());

        // Render the text
        RichTextRenderer* richTextRenderer = debugText.getRichTextRenderer();
        richTextRenderer->setTransform(transform);
        richTextRenderer->draw(viewID, bitmapShader);
    }
}
