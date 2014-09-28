#include "Stdafx.hpp"

Color g_White(255, 255, 255, 255);
Color g_Black(0, 0, 0, 255);
Color g_Cyan(0, 255, 255, 255);
Color g_Red(255, 0, 0, 255);
Color g_Green(0, 255, 0, 255);
Color g_Blue(0, 0, 255, 255);
Color g_Yellow(255, 255, 0, 255);
Color g_Gray40(100, 100, 100, 255);
Color g_Gray25(64, 64, 64, 255);
Color g_Gray65(166, 166, 166, 255);
Color g_Transparent(255, 0, 255, 0);

//NO
// Note! If you change these direction vectors around to suit yourself
// Make sure you change it so it retains a left handed
// coordinate system 
//END NO
glm::vec3 g_Right(1.0f, 0.0f, 0.0f);
glm::vec3 g_Up(0.0f, 1.0f, 0.0f);
glm::vec3 g_Forward(0.0f, 0.0f, -1.0f);

glm::vec3 g_XAxis(1.0f, 0.0f, 0.0f);
glm::vec3 g_YAxis(0.0f, 1.0f, 0.0f);
glm::vec3 g_ZAxis(0.0f, 0.0f, 1.0f);

glm::vec4 g_Up4(g_Up.x, g_Up.y, g_Up.z, 0.0f);
glm::vec4 g_Right4(g_Right.x, g_Right.y, g_Right.z, 0.0f);
glm::vec4 g_Forward4(g_Forward.x, g_Forward.y, g_Forward.z, 0.0f);

const glm::u8  u8OPAQUE = 255;
const glm::u8  u8TRANSPARENT = 0;

const int MEGABYTE = 1024 * 1024;
const float SIXTY_HERTZ = 16.66f;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;