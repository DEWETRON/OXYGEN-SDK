// Copyright DEWETRON GmbH 2026
//
// Image generator code that creates a Y800 grayscale image with an animated spiral
// Disclaimer: the following code is based on AI generated content

#include <cstdint>
#include <cmath>

void render_spiral_y800(void* dst, uint32_t width, uint32_t height, float t)
{
    uint8_t* buffer = reinterpret_cast<uint8_t*>(dst);
    float cx = 0.5f * width;
    float cy = 0.5f * height;

    for (uint32_t y = 0; y < height; ++y) {
        for (uint32_t x = 0; x < width; ++x) {
            // 1. Translate pixel to be relative to center
            float dx = x - cx;
            float dy = y - cy;

            // 2. Convert to Polar Coordinates
            float r = std::sqrt(dx * dx + dy * dy);
            float theta = std::atan2(dy, dx);

            // 3. The Spiral Math
            // The "twist" comes from adding 'r' to the angle.
            // Multiplying 'r' by a factor controls how tight the spiral is.
            float spiral_factor = 0.1f;

            // 4. Create "Arms"
            // Using sin(theta * N) creates N arms. Let's do a 3-arm spiral.
            float arms = std::sin(3.0f * (theta + (r * spiral_factor) - t));

            // 5. Enhance the center and fade the edges
            float mask = std::exp(-r * 0.005f); // Soft falloff

            // Normalize to 0-255
            // We use (arms + 1.0) / 2.0 to move from range [-1, 1] to [0, 1]
            float final_pixel = ((arms + 1.0f) / 2.0f) * mask * 255.0f;

            buffer[y * width + x] = static_cast<uint8_t>(final_pixel);
        }
    }
}
