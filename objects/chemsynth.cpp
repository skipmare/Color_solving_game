//
// Created by Skip on 12/10/2024.
//

#include "chemsynth.h"
#include <random>
#include <chrono>
#include <thread>
#include <functional>  // for std::function
#include <algorithm>   // for std::max or std::reverse if needed
#include "sound.h"
#include "boxes.h"

extern std::vector<int> activeBoxes;

/**
 * @brief Initializes the game:
 *  - Spawns an update thread for active boxes
 *  - Plays sound
 *  - Randomly fills both `chems` (target) and `chems_quiz` (player puzzle)
 */
void chemsynth::start() {
    running = true;
    updateThread = std::thread(&chemsynth::updateActiveBoxes, this);
    playSound();

    std::random_device rd;  // Random hardware seed
    std::mt19937 gen(rd()); // Random generator
    std::uniform_int_distribution<> distr(0, 4); // Colors 0–4

    auto initialize_chems = [&distr, &gen](std::vector<std::string> chems[10]) {
        for (int i = 0; i < 10; ++i) {
            int x = distr(gen);
            switch (x) {
                case 0: chems[i].push_back("blue"); break;
                case 1: chems[i].push_back("yellow"); break;
                case 2: chems[i].push_back("red"); break;
                case 3: chems[i].push_back("green"); break;
                case 4: chems[i].push_back("pink"); break;
            }
        }
    };

    // Clear old state
    for (int i = 0; i < 10; ++i) {
        chems[i].clear();
        chems_quiz[i].clear();
    }

    // Fill both sequences
    initialize_chems(chems);
    initialize_chems(chems_quiz);
}

/**
 * @brief Prints the target sequence (solution) to console.
 */
void chemsynth::print_chems() {
    for (int i = 0; i < 10; i++) {
        cout << chems[i][0] << " ";
    }
    cout << endl;
}

/**
 * @brief Prints the player’s current puzzle sequence.
 */
void chemsynth::print_chems_quiz() {
    for (int i = 0; i < 10; i++) {
        cout << chems_quiz[i][0] << " ";
    }
    cout << endl;
}

/**
 * @brief Solvent tool: replaces a row with a new color (or reuses replicated_chems).
 * Shifts everything upward so the last slot becomes new color.
 */
void chemsynth::c_solvent(int row) {
    if (this->game_over) { cout << "Game is over. You cannot use any tools." << endl; return; }
    if (row > 9) { cout << "You cannot solvent the air." << endl; return; }
    if (row != activeBoxes[0] && row != activeBoxes[1]) return;

    // Pick a color
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, 4);

    string color;
    if (!replicated_chems.empty()) {
        color = replicated_chems.back();
        replicated_chems.pop_back();
    } else {
        switch (distr(gen)) {
            case 0: color = "blue"; break;
            case 1: color = "yellow"; break;
            case 2: color = "red"; break;
            case 3: color = "green"; break;
            case 4: color = "pink"; break;
        }
    }

    // Shift down from row
    for (int i = row; i < 10; i++) {
        if (i == 9) {
            chems_quiz[i][0] = color;
        } else {
            chems_quiz[i][0] = chems_quiz[i + 1][0];
        }
    }
}

/**
 * @brief Catalyst tool: cycles color of row and all contiguous same-colored neighbors.
 */
void chemsynth::c_catalyst(int row) {
    if (this->game_over) { cout << "Game is over. You cannot use any tools." << endl; return; }
    if (row > 9) { cout << "You cannot catalyst the air." << endl; return; }
    if (row != activeBoxes[0] && row != activeBoxes[1]) return;

    auto change_color = [](std::string &color) {
        if (color == "red") color = "yellow";
        else if (color == "yellow") color = "green";
        else if (color == "green") color = "blue";
        else if (color == "blue") color = "pink";
        else if (color == "pink") color = "red";
    };

    std::string current_color = chems_quiz[row][0];
    change_color(chems_quiz[row][0]);

    // Spread left/right until different
    int left = row - 1;
    int right = row + 1;
    while (left >= 0 && chems_quiz[left][0] == current_color) {
        change_color(chems_quiz[left][0]); left--;
    }
    while (right < 10 && chems_quiz[right][0] == current_color) {
        change_color(chems_quiz[right][0]); right++;
    }
}

/**
 * @brief Stirrer tool: swaps neighbors around row.
 */
void chemsynth::c_stirer(int row) {
    if (this->game_over) { cout << "Game is over. You cannot use any tools." << endl; return; }
    if (row != activeBoxes[0] && row != activeBoxes[1]) return;
    if (row > 9 || row == 0 || row == 9) {
        cout << "You cannot stirer the air." << endl;
        return;
    }
    string temp = chems_quiz[row - 1][0];
    chems_quiz[row - 1][0] = chems_quiz[row + 1][0];
    chems_quiz[row + 1][0] = temp;
}

/**
 * @brief Replicator tool: duplicates row into row+1, shifting others down.
 * Saves the last element to replicated_chems.
 */
void chemsynth::c_replicator(int row) {
    if (this->game_over) { cout << "Game is over. You cannot use any tools." << endl; return; }
    if (row != activeBoxes[0] && row != activeBoxes[1]) return;
    if (row > 8) { cout << "You cannot replicate the air." << endl; return; }

    replicated_chems.push_back(chems_quiz[9][0]);
    for (int i = 9; i > row; i--) chems_quiz[i][0] = chems_quiz[i - 1][0];
    chems_quiz[row + 1][0] = chems_quiz[row][0];
}

/**
 * @brief Centrifuge tool: swaps chemicals around row, distance depends on row index.
 */
void chemsynth::c_centrifuge(int row) {
    if (this->game_over) { cout << "Game is over. You cannot use any tools." << endl; return; }
    if (row != activeBoxes[0] && row != activeBoxes[1]) return;
    if (row == 0 || row == 9) { cout << "You cannot centrifuge the air." << endl; return; }

    if (row == 1 || row == 8) {
        string temp = chems_quiz[row - 1][0];
        chems_quiz[row - 1][0] = chems_quiz[row + 1][0];
        chems_quiz[row + 1][0] = temp;
    } else if (row == 2 || row == 7) {
        string temp = chems_quiz[row - 2][0];
        string temp1 = chems_quiz[row - 1][0];
        chems_quiz[row - 2][0] = chems_quiz[row + 2][0];
        chems_quiz[row - 1][0] = chems_quiz[row + 1][0];
        chems_quiz[row + 2][0] = temp;
        chems_quiz[row + 1][0] = temp1;
    } else {
        string temp = chems_quiz[row - 3][0];
        string temp1 = chems_quiz[row - 2][0];
        string temp2 = chems_quiz[row - 1][0];
        chems_quiz[row - 3][0] = chems_quiz[row + 3][0];
        chems_quiz[row - 2][0] = chems_quiz[row + 2][0];
        chems_quiz[row - 1][0] = chems_quiz[row + 1][0];
        chems_quiz[row + 3][0] = temp;
        chems_quiz[row + 2][0] = temp1;
        chems_quiz[row + 1][0] = temp2;
    }
}

/**
 * @brief Checks win condition. If quiz matches target, game ends with popup.
 */
void chemsynth::won() {
    if (this->game_over && this->message_shown) return;

    for (int i = 0; i < 10; ++i) {
        if (chems_quiz[i] != chems[i]) return;
    }

    this->game_over = true;
    MessageBox(NULL, "You won!", "Chemsynth", MB_OK);
    this->message_shown = true;
    running = false;
    updateThread.join();
}

/**
 * @brief Getter for game_over flag.
 */
bool chemsynth::is_game_over() { return this->game_over; }

/**
 * @brief Returns the current target sequence.
 */
std::vector<std::string> chemsynth::get_chems() {
    std::vector<std::string> result;
    for (int i = 0; i < 10; ++i) result.push_back(this->chems[i][0]);
    return result;
}

/**
 * @brief Returns the current quiz sequence.
 */
std::vector<std::string> chemsynth::get_chems_quiz() {
    std::vector<std::string> result;
    for (int i = 0; i < 10; ++i) result.push_back(this->chems_quiz[i][0]);
    return result;
}

/**
 * @brief Thread loop: cycles activeBoxes indices every 2 seconds.
 */
void chemsynth::updateActiveBoxes() {
    int firstBoxIndex = 0;
    int secondBoxIndex = 5;

    while (running) {
        activeBoxes.clear();
        activeBoxes.push_back(firstBoxIndex);
        activeBoxes.push_back(secondBoxIndex);

        firstBoxIndex = (firstBoxIndex + 1) % 10;
        secondBoxIndex = (secondBoxIndex + 1) % 10;

        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }
}

/**
 * @brief Stops update thread and resets state for a fresh game.
 */
void chemsynth::restartGame() {
    running = false;
    if (updateThread.joinable()) updateThread.join();
    this->game_over = false;
    this->message_shown = false;
    this->already_render_once = false;
    start();
}

/**
 * @brief Getter for render flag.
 */
bool chemsynth::getRendered() { return this->already_render_once; }

/**
 * @brief Setter for render flag.
 */
void chemsynth::setRendered(bool rendered) { this->already_render_once = rendered; }

// === Solver helpers ===

enum Direction { LEFT = -1, RIGHT = 1 };
enum Status { SUCCESS = 1, FAIL = -1 };

/**
 * @brief Checks if move is safe (avoids invalid neighbor matches).
 */
bool isSafe(const std::vector<std::string>& list, int index, Direction dir) {
    if ((dir == LEFT && index == 0) || (dir == RIGHT && index == list.size() - 1)) {
        return true;
    }
    return list[index] != list[index + dir];
}

/**
 * @brief Returns indices where quiz != target.
 */
std::vector<int> getNotFinished(const std::vector<std::string>& current,
                                const std::vector<std::string>& target) {
    std::vector<int> indices;
    for (int i = 0; i < current.size(); ++i) {
        if (current[i] != target[i]) indices.push_back(i);
    }
    return indices;
}

/**
 * @brief Scores similarity between quiz and target.
 */
int scoreState(const std::vector<std::string>& quiz,
               const std::vector<std::string>& target) {
    int score = 0;
    for (int i = 0; i < quiz.size(); ++i) {
        if (quiz[i] == target[i]) score++;
    }
    return score;
}

/**
 * @struct ToolAction
 * @brief Represents a reversible tool action for solver search.
 */
struct ToolAction {
    std::string name;
    std::function<void()> apply;
    std::function<void()> revert;
};

/**
 * @brief Simple solver:
 *  - Compares current quiz to target
 *  - Tries tools on active indices
 *  - Chooses tool that improves score
 *  - Reverts unsuccessful attempts
 *  - Repeats until solved
 *  - Doesn't always work, but often does
 */
void chemsynth::solve() {
    while (!is_game_over()) {
        std::vector<std::string> quiz = get_chems_quiz();
        std::vector<std::string> target = get_chems();
        std::vector<int> notFinished = getNotFinished(quiz, target);

        if (notFinished.empty()) { won(); return; }

        for (int index : notFinished) {
            if (!(index == activeBoxes[0] || index == activeBoxes[1])) continue;

            std::cout << "[Solver] Thinking at index " << index << std::endl;

            int bestScore = scoreState(quiz, target);
            std::string bestTool;

            // Save state
            std::vector<std::string> original;
            for (int j = 0; j < 10; ++j) original.push_back(chems_quiz[j][0]);

            // Try each tool
            std::vector<ToolAction> tools = {
                {"catalyst",   [&]() { c_catalyst(index); },   [&]() { for (int j = 0; j < 10; ++j) { chems_quiz[j] = {original[j]}; } }},
                {"stirer",     [&]() { c_stirer(index); },     [&]() { for (int j = 0; j < 10; ++j) { chems_quiz[j] = {original[j]}; } }},
                {"centrifuge", [&]() { c_centrifuge(index); }, [&]() { for (int j = 0; j < 10; ++j) { chems_quiz[j] = {original[j]}; } }},
                {"replicator", [&]() { c_replicator(index); }, [&]() { for (int j = 0; j < 10; ++j) { chems_quiz[j] = {original[j]}; } }},
                {"solvent",    [&]() { c_solvent(index); },    [&]() { for (int j = 0; j < 10; ++j) { chems_quiz[j] = {original[j]}; } }}
            };

            for (const auto& tool : tools) {
                tool.apply();
                int score = scoreState(get_chems_quiz(), target);
                if (score > bestScore) {
                    bestScore = score;
                    bestTool = tool.name;
                }
                tool.revert();
            }

            // Apply best choice
            if (!bestTool.empty()) {
                std::cout << "  → Best tool: " << bestTool << " at index " << index << std::endl;
                if (bestTool == "catalyst")   c_catalyst(index);
                if (bestTool == "stirer")     c_stirer(index);
                if (bestTool == "centrifuge") c_centrifuge(index);
                if (bestTool == "replicator") c_replicator(index);
                if (bestTool == "solvent")    c_solvent(index);
                std::this_thread::sleep_for(std::chrono::milliseconds(60));
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }
}
