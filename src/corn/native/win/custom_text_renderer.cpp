#include "win/custom_text_renderer.h"

namespace corn {
    CustomTextRenderer::CustomTextRenderer(ID2D1RenderTarget* renderTarget) : refCount_(1), renderTarget_(renderTarget) {
        this->renderTarget_->AddRef();
    }

    CustomTextRenderer::~CustomTextRenderer() {
        this->renderTarget_->Release();
    }

    HRESULT CustomTextRenderer::QueryInterface(REFIID riid, void** ppv) {
        if (riid == __uuidof(IUnknown) || riid == __uuidof(IDWriteTextRenderer)) {
            *ppv = this;
            this->AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }

    ULONG CustomTextRenderer::AddRef() {
        return InterlockedIncrement(&this->refCount_);
    }

    ULONG CustomTextRenderer::Release() {
        ULONG count = InterlockedDecrement(&this->refCount_);
        if (count == 0) delete this;
        return count;
    }

    HRESULT CustomTextRenderer::DrawGlyphRun(
            void* clientDrawingContext,
            FLOAT baselineOriginX, FLOAT baselineOriginY,
            DWRITE_MEASURING_MODE measuringMode,
            const DWRITE_GLYPH_RUN* glyphRun,
            const DWRITE_GLYPH_RUN_DESCRIPTION* glyphRunDescription,
            IUnknown* clientDrawingEffect) {

        (void)clientDrawingContext; (void)glyphRunDescription;

        DrawingEffect* effect = clientDrawingEffect ? dynamic_cast<DrawingEffect*>(clientDrawingEffect) : nullptr;
        ID2D1SolidColorBrush* brush = effect ? effect->brush : nullptr;
        FLOAT offsetY = effect ? effect->baselineOffset : 0.0f;

        // Fallback if no custom brush
        if (brush) {
            brush->AddRef();
        } else {
            renderTarget_->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &brush);
        }

        renderTarget_->DrawGlyphRun(
            D2D1::Point2F(baselineOriginX, baselineOriginY + offsetY),
            glyphRun,
            brush,
            measuringMode
        );

        // Release resources
        if (brush) {
            brush->Release();
        }

        return S_OK;
    }


    HRESULT CustomTextRenderer::DrawUnderline(
            void* clientDrawingContext,
            FLOAT baselineOriginX, FLOAT baselineOriginY,
            const DWRITE_UNDERLINE* underline,
            IUnknown* clientDrawingEffect) {

        (void)clientDrawingContext;

        DrawingEffect* effect = clientDrawingEffect ? dynamic_cast<DrawingEffect*>(clientDrawingEffect) : nullptr;
        ID2D1SolidColorBrush* brush = effect ? effect->brush : nullptr;

        // Fallback if no custom brush
        if (brush) {
            brush->AddRef();
        } else {
            renderTarget_->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &brush);
        }

        // Create underline rectangle
        D2D1_RECT_F rect = D2D1::RectF(
            baselineOriginX,
            baselineOriginY + underline->offset,
            baselineOriginX + underline->width,
            baselineOriginY + underline->offset + underline->thickness
        );

        renderTarget_->FillRectangle(&rect, brush);

        // Release resources
        if (brush) {
            brush->Release();
        }

        return S_OK;
    }


    HRESULT CustomTextRenderer::DrawStrikethrough(void*, FLOAT, FLOAT, const DWRITE_STRIKETHROUGH*, IUnknown*) {
        return S_OK;
    }

    HRESULT CustomTextRenderer::DrawInlineObject(void*, FLOAT, FLOAT, IDWriteInlineObject*, BOOL, BOOL, IUnknown*) {
        return S_OK;
    }

    HRESULT CustomTextRenderer::IsPixelSnappingDisabled(void*, BOOL* isDisabled) {
        *isDisabled = FALSE;
        return S_OK;
    }

    HRESULT CustomTextRenderer::GetCurrentTransform(void*, DWRITE_MATRIX* transform) {
        this->renderTarget_->GetTransform(reinterpret_cast<D2D1_MATRIX_3X2_F*>(transform));
        return S_OK;
    }

    HRESULT CustomTextRenderer::GetPixelsPerDip(void*, FLOAT* pixelsPerDip) {
        *pixelsPerDip = 1.0f;
        return S_OK;
    }
}
