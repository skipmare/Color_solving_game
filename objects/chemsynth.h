//
// Created by Skip on 12/10/2024.
//

#ifndef CHEMSYNTH_H
#define CHEMSYNTH_H

#include <vector>
#include <string>
#include <iostream>
#include <thread>
#include <windows.h>
#include <atomic>
using namespace std;

/**
 * @class chemsynth
 * @brief Game class that simulates chemical synthesis using colors and tools.
 *
 * The class manages:
 *  - A target sequence of chemicals (`chems`)
 *  - A puzzle sequence that the player manipulates (`chems_quiz`)
 *  - Tools that transform or shift chemicals
 *  - A background thread that cycles "active boxes" for interaction
 *  - Game state such as win/loss and rendering
 */
class chemsynth {
    // Enum representing chemical types (for clarity, though strings are stored).
    enum ChemType {
        b, // blue
        y, // yellow
        r, // red
        g, // green
        p, // pink/purple
    };

    // Target sequence of chemicals the player must match.
    std::vector<string> chems[10];

    // Current puzzle sequence that the player manipulates with tools.
    std::vector<string> chems_quiz[10];

    // Stores chemicals temporarily removed (used by solvent/replicator).
    std::vector<string> replicated_chems;

    // Game state flags.
    bool game_over = false;       // True if game ended
    bool message_shown = false;   // True if win message has been displayed

    // Background thread state.
    std::atomic<bool> running;    // Controls update loop
    std::thread updateThread;     // Thread cycling active boxes

    // Rendering state flag.
    bool already_render_once = false;

public:
    // === Core Game Functions ===

    // Starts the game: initializes data, starts update thread, plays sound.
    void start();

    // Checks for win condition and marks game as finished if solved.
    void won();

    // Returns true if the game has ended.
    bool is_game_over();

    // === Display Functions ===

    // Prints the target sequence of chemicals.
    void print_chems();

    // Prints the current puzzle sequence.
    void print_chems_quiz();

    // === Chemical Tool Functions ===
    // Each tool manipulates the puzzle sequence in a different way.

    // Solvent: shifts chemicals and inserts new or replicated one.
    void c_solvent(int row);

    // Centrifuge: swaps chemicals around a row based on distance.
    void c_centrifuge(int row);

    // Replicator: duplicates chemical at a row and shifts sequence down.
    void c_replicator(int row);

    // Stirrer: swaps neighbors around a row.
    void c_stirer(int row);

    // Catalyst: cycles chemical color and contiguous matching neighbors.
    void c_catalyst(int row);

    // === Accessors ===

    // Returns the target sequence.
    std::vector<string> get_chems();

    // Returns the current puzzle sequence.
    std::vector<string> get_chems_quiz();

    // === Game Management ===

    // Background update loop: cycles which boxes are active.
    void updateActiveBoxes();

    // Restarts the game: resets state and restarts thread.
    void restartGame();

    // Checks if the puzzle has been rendered at least once.
    bool getRendered();

    // Sets render flag.
    void setRendered(bool rendered);

    // Attempts to automatically solve the puzzle by trying tools.
    void solve();
};

#endif //CHEMSYNTH_H
