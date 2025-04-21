#pragma once

// This is because of a bug in CMake.
// Failing to include this before all Windows headers will result in the symbol "==" being exported into exports.def.
#include <guiddef.h>

#include <dwrite.h>
#include <d2d1.h>
#include "drawing_effect.h"

namespace corn {
    class CustomTextRenderer : public IDWriteTextRenderer {
    public:
        CustomTextRenderer(ID2D1RenderTarget* renderTarget);
        virtual ~CustomTextRenderer();

        // IUnknown
        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override;
        ULONG STDMETHODCALLTYPE AddRef() override;
        ULONG STDMETHODCALLTYPE Release() override;

        // IDWriteTextRenderer
        HRESULT STDMETHODCALLTYPE DrawGlyphRun(
                void* clientDrawingContext,
                FLOAT baselineOriginX, FLOAT baselineOriginY,
                DWRITE_MEASURING_MODE measuringMode,
                const DWRITE_GLYPH_RUN* glyphRun,
                const DWRITE_GLYPH_RUN_DESCRIPTION* glyphRunDescription,
                IUnknown* clientDrawingEffect) override;

        HRESULT STDMETHODCALLTYPE CustomTextRenderer::DrawUnderline(
                void* clientDrawingContext,
                FLOAT baselineOriginX, FLOAT baselineOriginY,
                const DWRITE_UNDERLINE* underline,
                IUnknown* clientDrawingEffect) override;

        HRESULT STDMETHODCALLTYPE DrawStrikethrough(void*, FLOAT, FLOAT, const DWRITE_STRIKETHROUGH*, IUnknown*) override;

        HRESULT STDMETHODCALLTYPE DrawInlineObject(void*, FLOAT, FLOAT, IDWriteInlineObject*, BOOL, BOOL, IUnknown*) override;

        HRESULT STDMETHODCALLTYPE IsPixelSnappingDisabled(void*, BOOL* isDisabled) override;

        HRESULT STDMETHODCALLTYPE GetCurrentTransform(void*, DWRITE_MATRIX* transform) override;

        HRESULT STDMETHODCALLTYPE GetPixelsPerDip(void*, FLOAT* pixelsPerDip) override;

    private:
        ULONG refCount_;
        ID2D1RenderTarget* renderTarget_;
    };
}
