#pragma once

// This is because of a bug in CMake.
// Failing to include this before all Windows headers will result in the symbol "==" being exported into exports.def.
#include <guiddef.h>

#include <string>
#include <d2d1.h>
#include <dwrite_3.h>
#include <wincodec.h>
#include <corn/geometry/transform.h>

namespace corn {
    /**
     * @brief Converts a UTF-8 string to a UTF-16 string.
     * @param str The UTF-8 string to convert.
     * @return The converted UTF-16 string.
     */
    std::wstring utf8To16(const std::string& str);

    /**
     * @brief Converts a float weight to a DWRITE_FONT_WEIGHT.
     * @param weight The float weight to convert.
     * @return The converted DWRITE_FONT_WEIGHT.
     */
    DWRITE_FONT_WEIGHT toDWriteFontWeight(float weight);

    /**
     * @brief Converts a Transform2D object to a D2D1_MATRIX_3X2_F object.
     * @param transform The Transform2D object to convert.
     * @return The converted D2D1_MATRIX_3X2_F object.
     */
    D2D1_MATRIX_3X2_F getD2D1Matrix(const Transform2D& transform);

    /**
     * @brief Calculates the bounding box after applying a transform to a rectangle with the given size.
     * @param transform The transform to apply.
     * @param size The size of the rectangle.
     * @return The bounding box [top-left-x, top-left-y, width, height].
     *
     * The four corners of the rectangle are:
     *   - top-left: (0, 0)
     *   - top-right: (size.x, 0)
     *   - bottom-left: (0, size.y)
     *   - bottom-right: (size.x, size.y)
     * The bounding box is the minimum rectangle (with integer coordinates) that contains all four corners after
     * applying the transform to the corners.
     */
    Vec<int16_t, 4> calcBoundingBox(const Transform2D& transform, const Vec2f& size);

    /**
     * @return A pointer to the IDWriteFactory5 object.
     *
     * DO NOT release the IDWriteFactory5 object. The DWrite library manages the shared factory's lifetime.
     */
    IDWriteFactory5* getDWriteFactory5();

    ID2D1Factory* getD2D1Factory();

    ID2D1RenderTarget* createRenderTarget(IWICBitmap* wicBitmap);

    IWICBitmap* createWICBitmap(UINT width, UINT height);

    /**
     * @brief Retrieves the font file's family name from a given IDWriteFontFile.
     * @param fontFile The IDWriteFontFile to retrieve the family name from.
     * @param familyName Pointer to a std::wstring to store the family name found.
     * @return HRESULT indicating success or failure.
     */
    HRESULT getFontFileFamilyName(IDWriteFontFile* fontFile, std::wstring* familyName);
}
