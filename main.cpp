// main.cpp

#include "objects/boxes.h"
#include "objects/chemsynth.h"
#include <thread>
#include <windows.h>
#include <mmsystem.h>

chemsynth game;


int main() {
    std::cout<<"Welcome to Chemsynth by Karan"<<std::endl;
    cout<<"Made to practice your chemistry skills"<<endl;
    cout<<"Enjoy!"<<endl;

    game.start();  // Start the game logic (includes update thread)

    // 💡 Start the solver in the background
    std::thread solverThread([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // optional short delay
        game.solve();
    });
    solverThread.detach();

    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(1200, 600, "Chemsynth by staff", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glClearColor(0.4f, 0.3f, 0.4f, 0.0f);

    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
        mouse_button_callback(window, button, action, mods, game);
    });

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        drawBoxes(game);
        changes_drawborder(selected_function_index);
        NeverChangeBoxes(game);
        drawButton(buttonX, buttonY, buttonWidth, buttonHeight);
        drawarrowunderactive(activeBoxes);

        glfwSwapBuffers(window);

        // 🏁 Check if user solved the puzzle (just in case)
        if (game.get_chems_quiz() == game.get_chems()) {
            game.won();
        }

        glfwWaitEventsTimeout(0.16);
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    glfwTerminate();
    return 0;
}
