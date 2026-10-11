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


const size_t MAX_LINES = 65536;


struct Line {
        float x1, y1, z1;
        uint32_t color_RGBA;
        float x2, y2, z2;
        uint32_t color_RGBA_2;  // duplicate
};

namespace {
const GLchar* VERTEX_SHADER = {"#version 460 core\n"
        "layout (location = 0) in vec3 pos;"
        "layout (location = 1) in uint color_packed;"
        "uniform mat4 viewProjection;"

        "out vec4 color;"

        "void main() {"
                "gl_Position = viewProjection * vec4(pos, 1.0f);"
                "color = unpackUnorm4x8(color_packed).abgr;"
        "}"
""};
const GLchar* FRAGMENT_SHADER = {"#version 460 core\n"
        "in vec4 color;"

        "out vec4 FragColor;"

        "void main() {"
                "FragColor = color;"
        "}"
""};
}

struct GLineRenderer3D::_impl { uint64_t _last_uid = 0; uint64_t NewUID() { return _last_uid++; }
        GLuint shader_id;
        GLint shader_viewProjection_location;
        GLuint vao, vbo;
        std::vector<Line> lines;
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

        this->_->shader_viewProjection_location = glGetUniformLocation(this->_->shader_id, "viewProjection");

        glGenVertexArrays(1, &this->_->vao);
        glBindVertexArray(this->_->vao);
        glGenBuffers(1, &this->_->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, this->_->vbo);
        glBufferData(GL_ARRAY_BUFFER, MAX_LINES*sizeof(Line), nullptr, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Line)/2, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(Line)/2, (void*)offsetof(Line, color_RGBA));

        this->_->lines.reserve(MAX_LINES);
}

int GLineRenderer3D::DrawLine(
        float x1, float y1, float z1,
        float x2, float y2, float z2,
        uint32_t color_RGBA
) noexcept {
        if (this->_->lines.size() > MAX_LINES) return __LINE__;
        Line& line = this->_->lines.emplace_back();
        line.x1 = x1; line.y1 = y1; line.z1 = z1;
        line.x2 = x2; line.y2 = y2; line.z2 = z2;
        line.color_RGBA = color_RGBA; line.color_RGBA_2 = color_RGBA;
        return 0;
}

int GLineRenderer3D::DrawFrame(int framebuffer_width, int framebuffer_height) noexcept { glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE); glEnable(GL_DEPTH_TEST);
        glUseProgram(this->_->shader_id);

        // Set view and projection matrices (Camera)
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
        glUniformMatrix4fv(
                this->_->shader_viewProjection_location, 1, GL_FALSE,
                (const GLfloat*)&viewProjection
        );

        // Draw lines
        glBindBuffer(GL_ARRAY_BUFFER, this->_->vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, this->_->lines.size()*sizeof(Line), this->_->lines.data());
        glBindVertexArray(this->_->vao);
        glDrawArrays(GL_LINES, 0, this->_->lines.size()*2);

        this->_->lines.clear();

        return 0;
}

GLineRenderer3D::~GLineRenderer3D() { glDeleteProgram(this->_->shader_id); }