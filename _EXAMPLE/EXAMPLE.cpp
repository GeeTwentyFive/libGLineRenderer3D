#include <GWindower.hpp>
#include <glad/glad.h>
// #include "../include/GLineRenderer3D.hpp"
// #include <linalg/linalg.h>
// using namespace linalg::aliases;
#include <GFramePacer/GFramePacer.hpp>

#include <stdexcept>
#include <string>
#include <math.h>
#ifndef M_PI  // (Windows fix)
#define M_PI 3.14159265358979323846
#endif
#include <iostream>


const double TARGET_FRAMETIME_MS = 2.0;  // Should be a power-of-2 (like 1.0, 2.0, 4.0, 8.0) (to align as fraction/multiple of USB input report rate)

const int CAMERA_SENSITIVITY = 16;
const int CAMERA_MOVE_SPEED = 8;


#define ERROR(msg) throw std::runtime_error(std::string("[ERROR] ") + __FILE__ + "@" + std::to_string(__LINE__) + " (" + __func__ + "): " + (msg))


int main() { try {
        GWindower gw{0, 0, 4, 6};
        //if (!gladLoadGL()) ERROR("Failed to load OpenGL");

        //GLineRenderer3D glr3d;

        //glClearColor(0.0, 0.0, 0.0, 1.0);
        //GFramePacer gfp; gfp.target_frametime_ms = TARGET_FRAMETIME_MS;
        while (gw.Update()) { if(gw.key_states[GWindower::KEY_ESCAPE]) break;
                //double delta_time = gfp.Wait();

                // glr3d.camera_yaw += gw.mouse_x * CAMERA_SENSITIVITY;
                // glr3d.camera_pitch += gw.mouse_y * CAMERA_SENSITIVITY;
                // glr3d.camera_pos[1] += (-gw.key_states[GWindower::KEY_LEFT_CONTROL] + gw.key_states[GWindower::KEY_SPACE]) * CAMERA_MOVE_SPEED * delta_time;
                // float2 move_dir = float2{
                //         (float)(-gw.key_states[GWindower::KEY_A] + gw.key_states[GWindower::KEY_D]),
                //         (float)(-gw.key_states[GWindower::KEY_W] + gw.key_states[GWindower::KEY_S])
                // };
                // if (linalg::length2(move_dir) != 0) move_dir = linalg::normalize(move_dir);
                // move_dir = linalg::rot((float)(glr3d.camera_yaw*(2*M_PI/65536)), move_dir);
                // glr3d.camera_pos[0] += -move_dir.x * CAMERA_MOVE_SPEED * delta_time;
                // glr3d.camera_pos[2] += -move_dir.y * CAMERA_MOVE_SPEED * delta_time;

                // static float line_end[3] = {0, 0, 0};
                // if (gw.key_states[GWindower::KEY_RIGHT]) line_end[0] += delta_time;
                // if (gw.key_states[GWindower::KEY_LEFT]) line_end[0] -= delta_time;
                // if (gw.key_states[GWindower::KEY_UP]) line_end[1] += delta_time;
                // if (gw.key_states[GWindower::KEY_DOWN]) line_end[1] -= delta_time;
                // glr3d.DrawLine(0, 0, 0, line_end[0], line_end[1], line_end[2], 0x0000FFFF);

                //glViewport(0, 0, gw.window_width, gw.window_height);
                //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                // glr3d.DrawFrame(gw.window_width, gw.window_height);
        }

        return 0;
} catch (const std::exception& e) { std::cout << e.what() << std::endl; return 1; } }