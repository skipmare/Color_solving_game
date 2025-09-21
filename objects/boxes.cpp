//
// Created by Skip on 17/10/2024.
//

#include "boxes.h"

std::vector<int> activeBoxes;
int selected_chems_quiz_index = -1;
int selected_function_index = -1;

// objects/boxes.cpp

void drawButton(float x, float y, float width, float height) {
    // Load the texture (ensure the path is correct)
    GLuint buttonTexture = loadTexture("../images/Restart.png");

    // Check if the texture is valid
    if (buttonTexture == 0) {
        std::cerr << "Failed to load button texture" << std::endl;
        return;
    }

    // Draw the button background (optional)
    glColor3f(0.0f, 0.5f, 0.5f); // Button color
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
    glColor3f(1.0f, 1.0f, 1.0f);

    // Render the texture on the button
    renderTexture(buttonTexture, x, y, width, height);
    glDeleteTextures(1, &buttonTexture);
    // Reset color to white after drawing the button
    glColor3f(1.0f, 1.0f, 1.0f);
}




void drawSelectedToolBorder(float x, float y, float width, float height, float borderWidth) {
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow color for the border
    glLineWidth(borderWidth);

    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();

    // Reset color to white after drawing the border
    glColor3f(1.0f, 1.0f, 1.0f);
}


bool isPointInsideBox(float px, float py, float x, float y, float width, float height) {
    return (px >= x && px <= x + width && py >= y && py <= y + height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods, chemsynth& game) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        // Convert from window coordinates to OpenGL coordinates
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        xpos = (xpos / width) * 2 - 1;
        ypos = 1 - (ypos / height) * 2;

        // Check if the restart button was clicked
        if (isPointInsideBox(xpos, ypos, buttonX, buttonY, buttonWidth, buttonHeight)) {
            game.restartGame();
            return;
        }

        // Check which box was clicked
        float boxWidth = 0.15f;
        float boxHeight = 0.15f;
        float gap = 0.02f;
        float totalWidth = 10 * boxWidth + 9 * gap;
        float startX = -totalWidth / 2.0f;
        float startY = 0.3f;

        for (int i = 0; i < 10; ++i) {
            if (isPointInsideBox(xpos, ypos, startX + i * (boxWidth + gap), startY - (boxHeight + gap), boxWidth, boxHeight)) {
                selected_chems_quiz_index = i;
                if (selected_function_index != -1) {
                    switch (selected_function_index) {
                        case 0: game.c_solvent(selected_chems_quiz_index); break;
                        case 1: game.c_centrifuge(selected_chems_quiz_index); break;
                        case 2: game.c_replicator(selected_chems_quiz_index); break;
                        case 3: game.c_stirer(selected_chems_quiz_index); break;
                        case 4: game.c_catalyst(selected_chems_quiz_index); break;
                    }
                }
                return;
            }
        }

        // Check the big boxes
        float bigBoxWidth = 0.3f;
        float bigBoxHeight = 0.3f;
        float bigBoxGap = 0.05f;
        float bigBoxStartX = -((5 * bigBoxWidth + 4 * bigBoxGap) / 2.0f);
        float bigBoxStartY = startY - 2 * (boxHeight + gap) - bigBoxGap - 0.4f;

        for (int i = 0; i < 5; ++i) {
            if (isPointInsideBox(xpos, ypos, bigBoxStartX + i * (bigBoxWidth + bigBoxGap), bigBoxStartY, bigBoxWidth, bigBoxHeight)) {
                selected_function_index = i;
                return;
            }
        }
    }
std::this_thread::sleep_for(std::chrono::milliseconds(16));
}

void drawRoundedBox(float x, float y, float width, float height, float radius, const std::string& color) {
    if (color == "red") glColor3f(0.906f, 0.173f, 0.157f); // Red
    else if (color == "green") glColor3f(0.278f, 0.847f, 0.078f); // Green
    else if (color == "blue") glColor3f(0.012f, 0.620f, 1.0f); // Blue
    else if (color == "yellow") glColor3f(0.937f, 0.761f, 0.024f); // Yellow
    else if (color == "pink") glColor3f(0.996f, 0.486f, 0.925f); // Purple
    else if (color == "white") glColor3f(1.0f, 1.0f, 1.0f); // White
    int num_segments = 20;
    float theta = 2.0f * 3.1415926f / float(num_segments);
    // Draw the rounded corners
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x + radius, y + radius);
    for (int i = 0; i <= num_segments; i++) {
        float angle = i * theta;
        glVertex2f(x + radius + cosf(angle) * radius, y + radius + sinf(angle) * radius);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x + width - radius, y + radius);
    for (int i = 0; i <= num_segments; i++) {
        float angle = i * theta;
        glVertex2f(x + width - radius + cosf(angle) * radius, y + radius + sinf(angle) * radius);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x + width - radius, y + height - radius);
    for (int i = 0; i <= num_segments; i++) {
        float angle = i * theta;
        glVertex2f(x + width - radius + cosf(angle) * radius, y + height - radius + sinf(angle) * radius);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x + radius, y + height - radius);
    for (int i = 0; i <= num_segments; i++) {
        float angle = i * theta;
        glVertex2f(x + radius + cosf(angle) * radius, y + height - radius + sinf(angle) * radius);
    }
    glEnd();

    // Draw the sides
    glBegin(GL_QUADS);
    glVertex2f(x + radius, y);
    glVertex2f(x + width - radius, y);
    glVertex2f(x + width - radius, y + height);
    glVertex2f(x + radius, y + height);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(x, y + radius);
    glVertex2f(x + width, y + radius);
    glVertex2f(x + width, y + height - radius);
    glVertex2f(x, y + height - radius);
    glEnd();
}
// Modify the drawBoxes function to highlight the selected tool

void NeverChangeBoxes(chemsynth &game) {
    float boxWidth = 0.15f;
    float boxHeight = 0.15f;
    float gap = 0.02f;
    float totalWidth = 10 * boxWidth + 9 * gap;
    float startX = -totalWidth / 2.0f;
    float startY = 0.3f;
    float radius = 0.02f;

    // Draw boxes for chems
    for (int i = 0; i < 10; ++i) {
        drawRoundedBox(startX + i * (boxWidth + gap), startY, boxWidth, boxHeight, radius, game.get_chems()[i]);
    }

    // Draw 5 big boxes
    float bigBoxWidth = 0.3f;
    float bigBoxHeight = 0.3f;
    float bigBoxGap = 0.05f;
    float bigBoxStartX = -((5 * bigBoxWidth + 4 * bigBoxGap) / 2.0f);
    float bigBoxStartY = startY - 2 * (boxHeight + gap) - bigBoxGap - 0.4f; // Adjusted to move the boxes further down

    for (int i = 0; i < 5; ++i) {
        drawRoundedBox(bigBoxStartX + i * (bigBoxWidth + bigBoxGap), bigBoxStartY, bigBoxWidth, bigBoxHeight, radius, "white");
    }

    const char* textures[] = {
        "../images/Solvent.png",
        "../images/Centrifuge.png",
        "../images/Replicator.png",
        "../images/StiRrer.png",
        "../images/Catalyst.png"
    };

    for (int i = 0; i < 5; ++i) {
        GLuint myTexture = loadTexture(textures[i]);
        renderTexture(myTexture, bigBoxStartX + i * (bigBoxWidth + bigBoxGap), bigBoxStartY, bigBoxWidth, bigBoxHeight);
        glDeleteTextures(1, &myTexture);
        // Draw border around the selected tool
        if (i == selected_function_index) {
            drawSelectedToolBorder(bigBoxStartX + i * (bigBoxWidth + bigBoxGap), bigBoxStartY, bigBoxWidth, bigBoxHeight, 5.0f);
        }
    }
}

void changes_drawborder(int &selected_function_index) {
    float boxHeight = 0.15f;
    float gap = 0.02f;
    float startY = 0.3f;

    float bigBoxWidth = 0.3f;
    float bigBoxHeight = 0.3f;
    float bigBoxGap = 0.05f;
    float bigBoxStartX = -((5 * bigBoxWidth + 4 * bigBoxGap) / 2.0f);
    float bigBoxStartY = startY - 2 * (boxHeight + gap) - bigBoxGap - 0.4f;
for (int i = 0; i < 5; ++i) {
    if (i == selected_function_index) {
        drawSelectedToolBorder(bigBoxStartX + i * (bigBoxWidth + bigBoxGap), bigBoxStartY, bigBoxWidth, bigBoxHeight, 5.0f);
    }
}
}
void drawBoxes(chemsynth& game) {
    float boxWidth = 0.15f;
    float boxHeight = 0.15f;
    float gap = 0.02f;
    float totalWidth = 10 * boxWidth + 9 * gap;
    float startX = -totalWidth / 2.0f;
    float startY = 0.3f;
    float radius = 0.02f;

    // Draw boxes for chems_quiz
    for (int i = 0; i < 10; ++i) {
        drawRoundedBox(startX + i * (boxWidth + gap), startY - (boxHeight + gap), boxWidth, boxHeight, radius, game.get_chems_quiz()[i]);
    }

}

void drawarrowunderactive(const std::vector<int>& activeBoxes) {
    GLuint myTexture = loadTexture("../images/arrow.png");
    float boxWidth = 0.15f;
    float gap = 0.02f;
    float totalWidth = 10 * boxWidth + 9 * gap;
    float startX = -totalWidth / 2.0f;
    float startY = 0.2f - (boxWidth + gap) - 0.1f; // Adjust Y position to be under the boxes

    for (int index : activeBoxes) {
        float x = startX + index * (boxWidth + gap);
        renderTexture(myTexture, x, startY, boxWidth, boxWidth);
    }
    glDeleteTextures(1, &myTexture);
}