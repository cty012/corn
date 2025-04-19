#include "win/utils.h"

namespace corn {
    std::wstring utf8To16(const std::string& str) {
        int requiredSize = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
        if (requiredSize == 0) {
            // Handle error (e.g., GetLastError())
            return L"";
        }
        std::wstring wstr(requiredSize, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], requiredSize);
        wstr.pop_back(); // Remove null terminator
        return wstr;
    }

    DWRITE_FONT_WEIGHT toDWriteFontWeight(float weight) {
        if (weight < 150.0f) {
            return DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_THIN;  // 100
        } else if (weight < 250.0f) {
            return DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_EXTRA_LIGHT;  // 200
        } else if (weight < 325.0f) {
            return DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_LIGHT;  // 300
        } else if (weight < 375.0f) {
            return DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_SEMI_LIGHT;  // 350
        } else if (weight < 450.0f) {
            return DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_REGULAR;  // 400
        } else if (weight < 550.0f) {
            return DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_MEDIUM;  // 500
        } else if (weight < 650.0f) {
            return DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_SEMI_BOLD;  // 600
        } else if (weight < 750.0f) {
            return DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_BOLD;  // 700
        } else if (weight < 850.0f) {
            return DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_EXTRA_BOLD;  // 800
        } else if (weight < 925.0f) {
            return DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_BLACK;  // 900
        } else {
            return DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_EXTRA_BLACK;  // 950
        }
    }

    IDWriteFactory5* getDWriteFactory5() {
        static IDWriteFactory5* factory = []() {
            IDWriteFactory5* f = nullptr;
            HRESULT hr = DWriteCreateFactory(
                DWRITE_FACTORY_TYPE_SHARED,
                __uuidof(IDWriteFactory),
                reinterpret_cast<IUnknown**>(&f)
            );
            return SUCCEEDED(hr) ? f : nullptr;
        }();
        return factory;
    }

    ID2D1Factory* getD2D1Factory() {
        ID2D1Factory* d2d1Factory = nullptr;
        D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2d1Factory);
        return d2d1Factory;
    }

    IWICImagingFactory* getWICFactory() {
        IWICImagingFactory* wicFactory = nullptr;
        CoCreateInstance(
            CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&wicFactory));
        return wicFactory;
    }

    HRESULT getFontFileFamilyName(IDWriteFontFile* fontFile, std::wstring* familyName) {
        IDWriteFactory5* factory = getDWriteFactory5();

        // Get font file reference key and loader
        IDWriteFontSetBuilder1* builder;
        factory->CreateFontSetBuilder(&builder);

        // Add to builder
        builder->AddFontFile(fontFile);

        // Create a font set and font collection
        IDWriteFontSet* fontSet;
        IDWriteFontCollection1* fontCollection;
        builder->CreateFontSet(&fontSet);
        factory->CreateFontCollectionFromFontSet(fontSet, &fontCollection);

        // Now get the family name
        IDWriteFontFamily1* fontFamily;
        fontCollection->GetFontFamily(0, &fontFamily);

        IDWriteLocalizedStrings* familyNames;
        fontFamily->GetFamilyNames(&familyNames);

        // Get the English name (or fallback)
        UINT32 index = 0;
        BOOL exists = false;
        familyNames->FindLocaleName(L"en-us", &index, &exists);
        if (!exists) index = 0;

        UINT32 length = 0;
        familyNames->GetStringLength(index, &length);

        auto* familyNameBuffer = new WCHAR[length + 1];
        familyNames->GetString(index, familyNameBuffer, length + 1);
        *familyName = familyNameBuffer;

        // Release resources
        delete[] familyNameBuffer;
        familyNames->Release();
        fontFamily->Release();
        fontCollection->Release();
        fontSet->Release();
        builder->Release();

        return S_OK;
    }
}
