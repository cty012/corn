#pragma once

// This is because of a bug in CMake.
// Failing to include this before all Windows headers will result in the symbol "==" being exported into exports.def.
#include <guiddef.h>

#include <d2d1.h>
#include <corn/media/font_manager.h>
#include <corn/util/color.h>

namespace corn {
    class DrawingEffect : public IUnknown {
    public:
        ID2D1SolidColorBrush* brush;
        Color color;
        FLOAT baselineOffset;

        DrawingEffect(Color color, FontPosition position, float baseFontSize);
        virtual ~DrawingEffect();

        void createBrush(ID2D1RenderTarget* renderTarget);
        void destroyBrush();

        // IUnknown methods
        ULONG STDMETHODCALLTYPE AddRef() override;
        ULONG STDMETHODCALLTYPE Release() override;
        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override;

    private:
        ULONG refCount_;
    };
}
