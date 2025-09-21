#ifndef TEXTURES_H
#define TEXTURES_H

#include <GLFW/glfw3.h>

// Load a texture from file
GLuint loadTexture(const char* path);

// Render an OpenGL texture at (x,y) with size (width,height)
void renderTexture(GLuint texture, float x, float y, float width, float height);

#endif // TEXTURES_H
