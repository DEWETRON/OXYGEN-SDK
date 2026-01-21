// Copyright DEWETRON GmbH 2026
//
// Image generator code that creates a RGB color image with an animated ring
// Disclaimer: the following code is based on AI generated content

#include <cstdint>
#include <cmath>
#include <algorithm>
#include <array>
#include <tuple>

namespace
{
    constexpr float PI = 3.1415927f;

    // -----------------------------
    // Point in polygon test
    // Using winding algorithm
    // -----------------------------
    bool pointInPolygon(float px, float py,
        const std::array<std::pair<float, float>, 4>& poly)
    {
        bool inside = false;
        for (size_t i = 0, j = poly.size() - 1; i < poly.size(); j = i++)
        {
            float xi = poly[i].first, yi = poly[i].second;
            float xj = poly[j].first, yj = poly[j].second;

            bool intersect = ((yi > py) != (yj > py)) &&
                (px < (xj - xi) * (py - yi) / (yj - yi + 0.000001f) + xi);
            if (intersect)
                inside = !inside;
        }
        return inside;
    }

    // -----------------------------
    // Rotate a point around center
    // -----------------------------
    inline void rotatePoint(float& x, float& y,
        float cx, float cy, float angle)
    {
        float dx = x - cx;
        float dy = y - cy;
        float c = std::cos(angle);
        float s = std::sin(angle);

        float rx = dx * c - dy * s;
        float ry = dx * s + dy * c;

        x = rx + cx;
        y = ry + cy;
    }
}

// -----------------------------
// Render Full Rotating Logo
// -----------------------------
void render_logo_rgb(void* image,
    uint32_t width, uint32_t height,
    float angle)
{
    uint8_t* const data = reinterpret_cast<uint8_t*>(image);

    // Clear screen (white)
    std::fill(data, data + width * height * 3, 255);

    float cx = width * 0.5f;
    float cy = height * 0.5f;
    float R_outer = std::min(width, height) * 0.42f;
    float R_inner = R_outer * 0.52f;

    // ------------------------------------------------------
    // Build the 9 polygons of the logo:
    //
    // Each side of the triangular loop contains 3 segments.
    // We generate them parametrically using angles.
    //
    // Segment colors (approx):
    // Light Blue:      (0, 180, 255)
    // Medium Blue:     (0, 100, 200)
    // Dark Blue:       (0, 40, 120)
    // Grey Tones:      (30,30,30), (80,80,80), (140,140,140)
    // ------------------------------------------------------

    struct PolySegment {
        std::array<std::pair<float, float>, 4> pts;
        uint8_t r, g, b;
    };

    std::array<PolySegment, 9> segments;

    // Colors for the 9 pieces
    static std::tuple<uint8_t, uint8_t, uint8_t> segmentColors[] = {
        { 0,180,255 }, { 0,120,230 }, { 0, 60,150 },   // blue side 1
        { 0,180,255 }, { 0,120,230 }, { 0, 60,150 },   // blue side 2
        { 30,30,30 },  { 80,80,80 },  {140,140,140}    // gray base
    };

    // Build 3 sides × 3 segments each
    int colorIndex = 0;
    for (int side = 0; side < 3; side++)
    {
        float baseAngle = side * 2.0f * PI / 3.0f;

        for (int seg = 0; seg < 3; seg++)
        {
            float a0 = baseAngle + (seg / 3.0f) * (2.0f * PI / 3.0f);
            float a1 = baseAngle + ((seg + 1) / 3.0f) * (2.0f * PI / 3.0f);

            PolySegment& ps = segments[colorIndex];

            // Outer arc segment
            ps.pts[0] = {cx + R_outer * std::cos(a0),
                         cy + R_outer * std::sin(a0) };
            ps.pts[1] = { cx + R_outer * std::cos(a1),
                          cy + R_outer * std::sin(a1) };

            // Inner arc segment
            ps.pts[2] = {cx + R_inner * std::cos(a1),
                         cy + R_inner * std::sin(a1) };
            ps.pts[3] = {cx + R_inner * std::cos(a0),
                         cy + R_inner * std::sin(a0) };

            // Assign segment color
            std::tie(ps.r, ps.g, ps.b) = segmentColors[colorIndex++];
        }
    }

    // -----------------------------------------
    // Rotate every vertex of every polygon
    // -----------------------------------------
    for (auto& seg : segments)
    {
        for (auto& p : seg.pts)
        {
            rotatePoint(p.first, p.second, cx, cy, angle);
        }
    }

    // -----------------------------------------
    // Rasterize
    // -----------------------------------------
    for (uint32_t y = 0; y < height; y++)
    {
        auto scanline = data + y * width * 3;
        for (uint32_t x = 0; x < width; x++)
        {
            float px = x + 0.5f;
            float py = y + 0.5f;

            for (const auto& seg : segments)
            {
                if (pointInPolygon(px, py, seg.pts))
                {
                    auto pixel = scanline + 3 * x;
                    pixel[0] = seg.r;
                    pixel[1] = seg.g;
                    pixel[2] = seg.b;
                    break;
                }
            }
        }
    }
}
