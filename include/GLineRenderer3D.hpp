#pragma once

#include <memory>
#include <stddef.h>
#include <stdint.h>

class GLineRenderer3D { public: ~GLineRenderer3D(); explicit GLineRenderer3D(); private: struct _impl; std::unique_ptr<_impl> _; public:
        float camera_pos[3] = {0, 0, 0};
        uint16_t camera_yaw = 0;  // 0..65535 = 0..2PI
        uint16_t camera_pitch = (UINT16_MAX/2);  // 16384..49152 = PI/2..PI+PI/2  (max. recommended sens. multiplier: 24 (to avoid going beyond 16384 buffer on either side in a single count (for correct clamping)))
        float camera_fov = 90.0; float camera_near = 0.01f; float camera_far = 1000.0f;

        struct LineInstance { void* user_data = nullptr; uint64_t mesh_id; GLineRenderer3D* _renderer_instance = nullptr;
                float start[3] = {0, 0, 0};
                float end[3] = {0, 0, 0};
                uint32_t color_RGBA = 0xFFFFFFFF;
                void Remove() noexcept;
        };
        LineInstance* AddLine() noexcept;

        int DrawFrame(int framebuffer_width, int framebuffer_height) noexcept;  // returns non-0 on error
};