#pragma once

#include <memory>
#include <stddef.h>
#include <stdint.h>

class GLineRenderer3D { public: ~GLineRenderer3D(); explicit GLineRenderer3D(); private: struct _impl; std::unique_ptr<_impl> _; public:
        float camera_pos[3] = {0, 0, 0};
        uint16_t camera_yaw = 0;  // 0..65535 = 0..2PI
        uint16_t camera_pitch = (UINT16_MAX/2);  // 16384..49152 = PI/2..PI+PI/2  (max. recommended sens. multiplier: 24 (to avoid going beyond 16384 buffer on either side in a single count (for correct clamping)))
        float camera_fov = 90.0; float camera_near = 0.01f; float camera_far = 1000.0f;

        int DrawLine(
                float x1, float y1, float z1,
                float x2, float y2, float z2,
                uint32_t color_RGBA = 0xFFFFFFFF
        ) noexcept;  // returns (non-0) source line number on error

        int DrawBox(
                float x_min, float y_min, float z_min,
                float x_max, float y_max, float z_max,
                uint32_t color_RGBA = 0xFFFFFFFF
        ) noexcept;

        int DrawFrame(int framebuffer_width, int framebuffer_height) noexcept;  // returns (non-0) source line number on error
};