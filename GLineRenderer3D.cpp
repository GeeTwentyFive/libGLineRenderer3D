#include "include/GLineRenderer3D.hpp"

#include <string>
#include <stdexcept>
#include <vector>
#include <memory>
#include <math.h>
#ifndef M_PI  // (Windows fix)
#define M_PI 3.14159265358979323846
#endif

#include <glad/glad.h>
#include <linalg/linalg.h>
using namespace linalg::aliases;


#define ERROR(msg) throw std::runtime_error(std::string("[ERROR] ") + __FILE__ + "@" + std::to_string(__LINE__) + " (" + __func__ + "): " + (msg))


namespace {
const GLchar* VERTEX_SHADER = {"#version 460 core\n"
        "layout (location = 0) in vec3 pos;"
        "uniform mat4 viewProjection;"

        "void main() {"
                "gl_Position = viewProjection * vec4(pos, 1.0f);"
        "}"
""};
const GLchar* FRAGMENT_SHADER = {"#version 460 core\n"
        "uniform vec4 color;"

        "out vec4 FragColor;"

        "void main() {"
                "FragColor = color;"
        "}"
""};
}

struct GLineRenderer3D::_impl { uint64_t _last_uid = 0; uint64_t NewUID() { return _last_uid++; }
        GLuint shader_id;
        GLint shader_viewProjection_location;

        std::vector<std::unique_ptr<GLineRenderer3D::LineInstance>> line_instances;
};

GLineRenderer3D::GLineRenderer3D() { this->_ = std::make_unique<GLineRenderer3D::_impl>(); GLint status; //glEnable(GL_DEBUG_OUTPUT); glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam){ERROR(message);}, nullptr)
        if (!gladLoadGL()) ERROR("Failed to load OpenGL functions");

        this->_->shader_id = glCreateProgram(); if (!this->_->shader_id) ERROR("Failed to create shader program");
        GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER); if (!vertex_shader_id) ERROR("Failed to create vertex shader object");
        glShaderSource(vertex_shader_id, 1, &VERTEX_SHADER, NULL); glCompileShader(vertex_shader_id); glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &status); if (status == GL_FALSE) ERROR("Failed to compile vertex shader");
        glAttachShader(this->_->shader_id, vertex_shader_id);
        GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER); if (!fragment_shader_id) ERROR("Failed to create fragment shader object");
        glShaderSource(fragment_shader_id, 1, &FRAGMENT_SHADER, NULL); glCompileShader(fragment_shader_id); glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &status); if (status == GL_FALSE) ERROR("Failed to compile fragment shader");
        glAttachShader(this->_->shader_id, fragment_shader_id);
        glLinkProgram(this->_->shader_id); glGetProgramiv(this->_->shader_id, GL_LINK_STATUS, &status); if (status == GL_FALSE) ERROR("Failed to link shader program"); glDeleteShader(vertex_shader_id); glDeleteShader(fragment_shader_id);

        this->_->shader_viewProjection_location = glGetUniformLocation(this->_->shader_id, "modelViewProjection");
}

GLineRenderer3D::LineInstance* GLineRenderer3D::AddLine() noexcept {
        std::unique_ptr<GLineRenderer3D::LineInstance>& line_instance = this->_->line_instances.emplace_back(std::make_unique<GLineRenderer3D::LineInstance>());
        line_instance->_renderer_instance = this;
        return line_instance.get();
}

void GLineRenderer3D::LineInstance::Remove() noexcept {
        std::vector<std::unique_ptr<GLineRenderer3D::LineInstance>>& line_instances = this->_renderer_instance->_->line_instances;
        for (size_t i = 0; i < line_instances.size(); i++) {
                if (line_instances[i].get() == this) {
                        line_instances[i] = std::move(line_instances.back());
                        break;
                }
        }
        line_instances.pop_back();
}

int GLineRenderer3D::DrawFrame(int framebuffer_width, int framebuffer_height) noexcept { glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE); glEnable(GL_DEPTH_TEST);
        glUseProgram(this->_->shader_id);

        // Calculate view and projection matrices (Camera)
        float3 _camera_pos = {camera_pos[0], camera_pos[1], camera_pos[2]};
        if (camera_pitch < 16385) camera_pitch = 16385; if (camera_pitch > 49151) camera_pitch = 49151;  // 16384..49152 = PI/2..PI+PI/2
        float4x4 view = linalg::lookat_matrix(
                _camera_pos,
                _camera_pos + linalg::qrot(
                        linalg::qmul(
                                linalg::rotation_quat(float3{0, 1, 0}, -(float)(camera_yaw*(2*M_PI/65536))),
                                linalg::rotation_quat(float3{1, 0, 0}, (float)(camera_pitch*(2*M_PI/65536)))
                        ),
                        float3{0, 0, -1}
                ),
                float3{0, 1, 0}
        );
        float4x4 proj = linalg::perspective_matrix(camera_fov, ((float)framebuffer_width)/((float)framebuffer_height), camera_near, camera_far, linalg::fwd_axis::neg_z, linalg::z_range::zero_to_one);
        float4x4 viewProjection = linalg::mul(proj, view);  // NOTE: Flip multiplication order in not-OpenGL

        // Draw lines
        for (const std::unique_ptr<GLineRenderer3D::LineInstance>& line_instance : this->_->line_instances) {
                // TODO
        }

        return 0;
}

GLineRenderer3D::~GLineRenderer3D() { glDeleteProgram(this->_->shader_id); }