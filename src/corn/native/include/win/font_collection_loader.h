#pragma once

// This is because of a bug in CMake.
// Failing to include this before all windows headers will result in the symbol "==" being exported into exports.def.
#include <guiddef.h>

#include <vector>
#include <memory>
#include <dwrite.h>
#include <wrl.h>

namespace corn {
    class SingleFontFileEnumerator : public IDWriteFontFileEnumerator {
    public:
        SingleFontFileEnumerator(IDWriteFontFile* fontFile);
        virtual ~SingleFontFileEnumerator();

        // IUnknown
        ULONG STDMETHODCALLTYPE AddRef() override;
        ULONG STDMETHODCALLTYPE Release() override;
        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** ppv) override;

        // IDWriteFontFileEnumerator
        HRESULT STDMETHODCALLTYPE MoveNext(BOOL* hasCurrentFile) override;
        HRESULT STDMETHODCALLTYPE GetCurrentFontFile(IDWriteFontFile** fontFile) override;

    private:
        ULONG refCount_;
        IDWriteFontFile* fontFile_;
        bool hasEnumerated_;
    };

    class SingleFontCollectionLoader : public IDWriteFontCollectionLoader {
    public:
        SingleFontCollectionLoader();
        virtual ~SingleFontCollectionLoader();

        // IUnknown
        ULONG STDMETHODCALLTYPE AddRef() override;
        ULONG STDMETHODCALLTYPE Release() override;
        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override;

        // IDWriteFontCollectionLoader
        HRESULT STDMETHODCALLTYPE CreateEnumeratorFromKey(
                IDWriteFactory* factory,
                void const* collectionKey, UINT32 collectionKeySize,
                IDWriteFontFileEnumerator** fontFileEnumerator) override;

    private:
        ULONG refCount_ = 1;
    };
}
