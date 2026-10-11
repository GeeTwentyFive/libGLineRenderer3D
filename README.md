# Usage

1) Create native window with OpenGL context (e.g. with [GWindower](https://github.com/GeeTwentyFive/libGWindower) or GLFW)
2) `GLineRenderer3D glr3d;`
3) ...add lines...
4) `glr3d.DrawFrame(FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);` (+ do a `glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)` each frame (lib doesn't do that so that you can compose it with other rendering))


# API

- `.camera_pos` - Set/Get camera position
- `.camera_yaw` - Set/Get camera horizontal (left<->right) rotation
- `.camera_pitch` - Set/Get camera vertical (up<->down) rotation
- `.camera_fov` - Set/Get camera Field of View
- `.AddLine()` - Create instance of line
- ##### AT END OF FRAME: `.DrawFrame()`  (returns 0 on success, source line number on error)

#

#### LineInstance (created with `.AddLine()`):
- `.start` - Set/Get start position
- `.end` - Set/Get end position
- `.color_RGBA` - Set/Get color shift (in RGBA; 0xRRGGBBAA)
- `.Remove()` - Remove from existence.