#ifndef BOXES_H
#define BOXES_H

#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include "chemsynth.h"
#include "textures.h"
#include <math.h>

// Shared state
extern std::vector<int> activeBoxes;
extern int selected_function_index;
extern int selected_chems_quiz_index;

// Input
bool isPointInsideBox(float px, float py, float x, float y, float width, float height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods, chemsynth& game);

// Rendering
void drawRoundedBox(float x, float y, float width, float height, float radius, const std::string& color);
void drawBoxes(chemsynth &game);
void NeverChangeBoxes(chemsynth &game);
void changes_drawborder(int &selected_function_index);
void drawarrowunderactive(const std::vector<int>& activeBoxes);

// UI
void drawButton(float x, float y, float width, float height);
void drawSelectedToolBorder(float x, float y, float width, float height, float borderWidth);

// Button constants
const float buttonX = 0.8f;
const float buttonY = 0.8f;
const float buttonWidth = 0.15f;
const float buttonHeight = 0.1f;

#endif // BOXES_H
