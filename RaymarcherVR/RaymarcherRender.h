#ifndef RAYMARCHER_H
#define RAYMARCHER_H 1

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shader.h"

#undef APIENTRY

using namespace std;

GLFWwindow* setup_raymarcher(int width, int height, int pos_x, int pos_y, const char * title, Shader s);

void render_raymarcher(GLFWwindow* window, Shader s, int width, int height);

void terminate_raymarcher(GLFWwindow* window);

#endif