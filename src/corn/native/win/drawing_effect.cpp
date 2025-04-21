#include "win/drawing_effect.h"

namespace corn {
    DrawingEffect::DrawingEffect(Color color, FontPosition position, float baseFontSize)
            : brush(nullptr), color(std::move(color)), baselineOffset(0.0f), refCount_(1) {

        switch (position) {
            case FontPosition::REGULAR:
                this->baselineOffset = 0.0f;
                break;
            case FontPosition::SUPERSCRIPT:
                this->baselineOffset = 0.4f * baseFontSize;
                break;
            case FontPosition::SUBSCRIPT:
                this->baselineOffset = -0.15f * baseFontSize;
                break;
        }
    }

    DrawingEffect::~DrawingEffect() {
        if (this->brush) {
            this->brush->Release();
        }
    }

    void DrawingEffect::createBrush(ID2D1RenderTarget* renderTarget) {
        if (this->brush) {
            this->brush->Release();
            this->brush = nullptr;
        }

        if (!renderTarget) {
            return;
        }

        const auto [r, g, b, a] = this->color.getRGBA();
        D2D1::ColorF colorF(
            static_cast<float>(r) / 255.0f,
            static_cast<float>(g) / 255.0f,
            static_cast<float>(b) / 255.0f,
            static_cast<float>(a) / 255.0f);
        renderTarget->CreateSolidColorBrush(colorF, &this->brush);
    }

    ULONG STDMETHODCALLTYPE DrawingEffect::AddRef() {
        return InterlockedIncrement(&this->refCount_);
    }

    ULONG STDMETHODCALLTYPE DrawingEffect::Release() {
        ULONG count = InterlockedDecrement(&this->refCount_);
        if (count == 0) delete this;
        return count;
    }

    HRESULT STDMETHODCALLTYPE DrawingEffect::QueryInterface(REFIID riid, void** ppv) {
        if (riid == __uuidof(IUnknown)) {
            *ppv = this;
            this->AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
}
