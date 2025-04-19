#include <ranges>
#include "win/font_collection_loader.h"
#include "win/utils.h"

namespace corn {
    /// SingleFontFileEnumerator

    SingleFontFileEnumerator::SingleFontFileEnumerator(IDWriteFontFile* fontFile)
            : refCount_(1), fontFile_(fontFile), hasEnumerated_(false) {

        this->fontFile_->AddRef();
    }

    SingleFontFileEnumerator::~SingleFontFileEnumerator() {
        this->fontFile_->Release();
    }

    ULONG STDMETHODCALLTYPE SingleFontFileEnumerator::AddRef() {
        return InterlockedIncrement(&this->refCount_);
    }

    ULONG STDMETHODCALLTYPE SingleFontFileEnumerator::Release() {
        ULONG newCount = InterlockedDecrement(&this->refCount_);
        if (newCount == 0) delete this;
        return newCount;
    }

    HRESULT STDMETHODCALLTYPE SingleFontFileEnumerator::QueryInterface(REFIID iid, void** ppv) {
        if (iid == __uuidof(IUnknown) || iid == __uuidof(IDWriteFontFileEnumerator)) {
            *ppv = this;
            this->AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }

    HRESULT STDMETHODCALLTYPE SingleFontFileEnumerator::MoveNext(BOOL* hasCurrentFile) {
        if (this->hasEnumerated_) {
            *hasCurrentFile = FALSE;
        } else {
            this->hasEnumerated_ = true;
            *hasCurrentFile = TRUE;
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE SingleFontFileEnumerator::GetCurrentFontFile(IDWriteFontFile** fontFile) {
        this->fontFile_->AddRef();
        *fontFile = this->fontFile_;
        return S_OK;
    }

    /// CustomFontCollectionLoader

    SingleFontCollectionLoader::SingleFontCollectionLoader() = default;

    SingleFontCollectionLoader::~SingleFontCollectionLoader() = default;

    ULONG SingleFontCollectionLoader::AddRef() {
        return InterlockedIncrement(&this->refCount_);
    }

    ULONG SingleFontCollectionLoader::Release() {
        ULONG newCount = InterlockedDecrement(&this->refCount_);
        if (newCount == 0) delete this;
        return newCount;
    }
    HRESULT SingleFontCollectionLoader::QueryInterface(REFIID riid, void** ppv) {
        if (riid == __uuidof(IUnknown) || riid == __uuidof(IDWriteFontCollectionLoader)) {
            *ppv = this;
            this->AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }

    HRESULT SingleFontCollectionLoader::CreateEnumeratorFromKey(
            IDWriteFactory* factory,
            void const* collectionKey, UINT32 collectionKeySize,
            IDWriteFontFileEnumerator** fontFileEnumerator) {

        (void)factory; (void)collectionKeySize;
        IDWriteFontFile* fontFile = *static_cast<IDWriteFontFile* const*>(collectionKey);
        *fontFileEnumerator = new SingleFontFileEnumerator(fontFile);
        return S_OK;
    }
}
